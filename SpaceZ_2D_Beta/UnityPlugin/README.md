# UnityPlugin —— 静态初始化期的堆分配须知

**必读**。本目录下的代码跑在 Unity 进程里，而 `SpaceZ_2D_Beta` 把全局 `operator new` / `operator delete` 换成了自己的实现，这让「静态存储期（全局/静态变量、`static` 局部量、静态注册器）里做堆分配」变成一件会让插件整个加载失败、且几乎查不出原因的事。

---

 **DLL 加载完成到 `UnityPluginLoad()` 被调用之间，以及 `UnityPluginUnload()` 之后，所有走默认 allocator 的堆分配都会抛 `std::bad_alloc`。**

---

## 时序

```time
LoadLibrary
  └─ CRT 静态初始化 _initterm          ← 此处 new 不可用
       ├─ Core/Memory/GlobalMemory.cpp   g_memoryMgr = nullptr（零初始化）
       ├─ UnityPlugin/Memory/UnityMemory.cpp  init：注册 Load/Unload 回调
       └─ UnityPlugin/Debug/Debug.cpp         init：注册 Load/Unload 回调
  └─ 各 TU 的静态对象构造              ← 此处 new 不可用（这就是本文件的主题）
UnityPluginLoad(interfaces)            ← 从此刻起 new 可用
  ├─ ptr->Get<IUnityMemoryManager>() → CreateAllocator
  └─ Core::Memory::EnableGlobalMemoryMgr()
【运行期】                             ← new / Core::Memory::Alloc 均可用
UnityPluginUnload()
  ├─ Core::Memory::DisableGlobalMemoryMgr()   → MemoryMgr 析构，页逐块归还 Unity
  └─ DestroyAllocator
【静态析构】                            ← new 再次不可用，delete 静默泄漏
```

两个"不可用"窗口的成因不同：

- **加载期**：`GlobalMemory.cpp:38` 的 `operator new` 第一句就是 `if(!g_memoryMgr) throw bad_alloc();`，而 `g_memoryMgr` 只在 `EnableGlobalMemoryMgr()` 里赋值，那要等到 `UnityPluginLoad`。
- **卸载期**：`DisableGlobalMemoryMgr()` 把 `g_memoryMgr` 置回 `nullptr`。此时 `operator new` 照旧抛异常，`operator delete` 则**只打一条日志然后 `return`**（`GlobalMemory.cpp:61`），不真正释放。

## 三条硬规则

### 1. 静态存储期禁止使用默认 allocator 的容器和字符串

违规写法（编译零警告，`LoadLibrary` 直接失败）：

```cpp
static std::string g_name = "SpaceZ-2D";        // 静态初始化期 std::string 构造 → operator new
static std::vector<int> g_ids = {1, 2, 3};   // 同上
```

合规写法 —— 用 `UnityPlugin::NativeAllocator`，它在静态初始化期只依赖 CRT 的 `malloc`：

```cpp
#include "UnityPlugin/Memory/NativeAllocator.h"

// 方案 A：容器换 allocator
static std::vector<int, UnityPlugin::NativeAllocator<int>> g_ids = {1, 2, 3};

// 方案 B：改成函数内 static，把构造推迟到 UnityPluginLoad 之后
inline std::vector<int>& Ids() { static std::vector<int> v; return v; }
```

`UnityPlugin/UnityPlugin.h:12` 的 `Action<T>` 就是方案 A 的封装，用于多播回调登记

```cpp
template<typename T>
using Action = std::vector<T, NativeAllocator<T>>;
```

方案 B 只解决"构造时机"，**不解决析构时机**：函数内 static 的析构仍然发生在静态析构期，那时容器里的元素若是含堆指针的类型，`deallocate` 走的是它自己的 allocator，需要逐个确认。

### 2. `NativeAllocator` 里的 `std::malloc` 不可以换成"更统一"的分配器

`UnityPlugin/Memory/NativeAllocator.h:22` 现在是：

```cpp
void* ptr = std::malloc(n * sizeof(T));
```

### 3. 卸载之后的释放不会报错，也不会真的发生

`operator delete` 在 `g_memoryMgr == nullptr` 时走的是「打日志 + return」，所以卸载后残留的堆内存既不崩、也不回收。如果这里出现了非预期的量，说明有对象活过了 `UnityPluginUnload`，那是生命周期 bug，不是分配器 bug。
