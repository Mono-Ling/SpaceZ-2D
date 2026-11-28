# SpaceZ-2D

[English](README.en.md) | 简体中文

自研 2D 物理引擎：**C++20 内核 + Unity 表现层**。

内核负责宽相 BVH、窄相 GJK/EPA、接触流形、可选连续碰撞检测与顺序冲量求解，通过 C ABI 以 x64 原生插件（`.dll`）的形式被 Unity 调用；Unity 侧只做组件封装与无分配数据封送。

当前主线是 **`SpaceZ_2D_Beta`**：在离散求解的骨架上，把连续检测收敛成「按碰撞体可选」，并补上碰撞层、空间查询、自定义内存管理与 Profiler 接入。本 README 以 Beta 为主，另外两个内核只作历史版本摘要。

## 工程一览

| 工程 | 定位 | 状态 |
| --- | --- | --- |
| `SpaceZ_2D_Beta` | **当前主线**：离散为主 + 按碰撞体可选连续检测、碰撞层、空间查询、自定义内存管理、Profiler | 进行中 |
| `SpaceZ_2D_Exp_CCD` | 全量连续检测（保守步进 CCD），用于验证 CCD 方案与堆叠稳定性 | 完成，冻结 |
| `SpaceZ_2D_Alpha` | 第一版离散内核，跑通物理求解闭环 | demo 完成，冻结 |
| `EditorPlugin` | 独立小插件，向内核 DLL 提供 `IUnityInterface` 指针 | 工具工程 |

三个 C++ 工程与 `Assets/` 平级、**互不耦合**：各自的 `.vscode/`、`Tools/`、`.gitignore`、`.build/` 完全独立，互不转调、互不清理；Unity 侧按目录各有一套托管层，各自加载同名 DLL，互不感知。

| 项目 | 说明 |
| --- | --- |
| 引擎内核 | C++20 / MSVC，`/std:c++20 /utf-8 /W4`，x64，提供 Debug 与 Release 配置 |
| 运行宿主 | Unity 2022.3.62f3c1（Windows x64 编辑器） |
| 求解模型 | 顺序冲量（Sequential Impulse）+ 岛划分 + 位置修正 |
| 形状 | Line、Box、Sphere、Capsule（Line 为 Beta 新增，其余两个内核只有后三种） |

### 版本沿革

- **Alpha**：验证「宽相 → 窄相 → 流形 → 岛划分 → 顺序冲量」这条路能跑通；瓶颈是接触点的顺序耦合会让盒堆叠持续漂移。
- **Exp-CCD**：改成全量保守步进 CCD，隧穿与堆叠都解决，代价是持续接触时单帧开销远高于自由飞行。
- **Beta**：两者折中——常规帧走离散，只有标记为 `Continuous` 的碰撞体才做扫掠与 TOI 推进；同时补层、空间查询、内存与 Profiler 等的通用能力。

---

## 当前主线：SpaceZ_2D_Beta

### 特性

#### 碰撞检测

- 宽相：动态平衡 BVH，插入时按「最小面积增量 + 高度代价（`HEIGHT_COST_WEIGHT = 0.005`）」逐层选择兄弟节点，支持按句柄增量更新与删除；节点内直接存 `ColliderHandle / RigidbodyHandle / LayerMask / Bound`。
- 层过滤在宽相完成：节点比较时先看层掩码是否相交，不匹配的对不会进入窄相，也不产生求解请求。
- 窄相：GJK 求最近点 / 相交，EPA 求穿透深度与法线（`MAX_ITERATION = 36`），再由 `ContactManifold` 从单个 EPA 结果扩展出多接触点流形。
- 支撑函数统一走 `Support(dir, local2world, world2local)`：先用 `world2local` 把世界方向变换到局部空间求支撑点，再用 `local2world` 把该点送回世界空间。旋转与缩放由变换矩阵承担，形状本身始终留在局部空间。
- 相邻支撑点对走 `Edge(normal, local2world)`：法线用 `local2world` 的转置变换到局部空间，两端点同样经 `local2world` 送回世界空间。

#### 混合碰撞模式（离散 + 可选连续）

- 每个碰撞体带 `CollisionMode::Discrete | Continuous`，逐碰撞体配置，不是全局开关。
- `SynContinuousBroadPhase` 按 `v · dt` 与 `ω · dt` 扩张连续碰撞体的 AABB（`SweepBound`），让包围盒覆盖本步运动轨迹；离散体只在脏时重建。
- `ContinueCollision` 用相对速度 / 角速度反复推进两凸体的相对位姿，直到 GJK 出现相交，返回该次碰撞的 TOI 与接触信息。
- `ContinuousCollisionStep` 收集本步内所有 TOI，取**最早**的一个把剩余时间截断到该时刻，只求解时间窗内的碰撞对；剩余时间继续循环推进。因此一个子步内可以处理多次顺序碰撞。
- 推进下限 `time * MIN_CCD_STEP`：小于此值的 TOI 不单独开窗，直接并进当前推进，避免 TOI 恒 0 造成死循环；整体迭代由 `MAX_CCD_ITERATION` 兜底。
- `Continuous` 需要碰撞体挂在刚体上才生效，静态碰撞体会自动退化为 `Discrete`。

#### 物理求解

- 岛划分：并查集把碰撞对切成互不影响的连通分量，逐岛独立迭代。
- 每个接触点：法向冲量（目标相对速度带恢复系数）+ 切向冲量（静摩擦钳制，超出静摩擦切到动摩擦），迭代完再做位置修正（`RELAXATION` 松弛 + `PENETRATE_SLOP` 穿透容差 + `MAX_POS_CORRECTION` 上限）。
- `Precompute()` 在迭代循环之外只调一次，恢复系数不会随迭代次数衰减。
- 材质合并：一对碰撞体的静摩擦 / 动摩擦 / 恢复系数取两者较小值，恢复系数再截断到 `[0, 1]`。
- 积分：位移用半隐式欧拉，阻尼按指数衰减 `v *= exp(-drag * dt)`；力分 `Impulse`（立刻折算速度增量）与 `Sustain`（累积后在步进时按 `dt` 折算、步进结束清零）。

#### 碰撞事件

- `CollisionPairBuffer` 用「上一帧可见集」做差分，产出 **Enter / Stay / Exit** 三张表：Enter 与 Stay 带接触信息列表，Exit 只带碰撞对象对。
- 每帧开头 `ResetVisible()`、结尾 `DiffExitPairs()`，因此 Exit 严格对应「上一帧在、本帧不在」。
- 原生侧已完整导出（`EnterCollisionPairs` / `StayCollisionPairs` / `ExitPairs`），托管层暂未消费。

#### 空间查询

- 重叠查询：`OverlapBox` / `OverlapSphere` / `OverlapCapsule`，先用查询体自算包围盒走 BVH `BoundOverlap`，再用 GJK 精确判定，返回命中的碰撞体句柄。
- 射线查询：`Raycast` 取最近命中、`RaycastAll` 返回按距离升序的全部命中；宽相用 `Ray` 与包围盒求交预筛，窄相用「线段 ↔ 凸体」的 GJK 变体求精确距离（`RAY_CAST_MAX_ITERATION = 72`）。
- 两者都带 `LayerMask` 与 `maxDis`；方向向量内部归一化，零向量视为未命中。

#### 接口与工程性

- 句柄为 `{int32 id, uint32 generation}`，对象删除后代数自增，旧句柄可被检出而非野指针。
- 内存：内核把全局 `operator new/delete` 换成了页式分配器——1 MiB 页 + AVL 空闲页树（按地址查找、相邻页自动合并）、支持 `align_val_t` 过对齐分配、Debug 下维护在用地址集合做非法释放校验。页本身从 Unity `IUnityMemoryManager` 的具名 Allocator（`SpaceZ-2D-Beta Native`）申请，所有原生分配都记在 Unity 的内存报表里。
- Profiler：`Step`、`OverlapBox/Sphere/Capsule`、`Raycast/RaycastAll` 六个标记，分类统一为 `Physics2D`；标记在静态存储期只登记回调，等 `UnityPluginLoad` 拿到 `IUnityProfiler` 后再真正创建。
- Unity 侧批量封送：属性写入先进 `NativeList` 缓冲（内存分配在原生堆），每物理帧一次性 flush 给批量接口；回读结果也是原生 `NativeArray`，在托管层就地消费后立刻归还。
- 编辑器内走 `LoadLibrary` + 反射绑定（`[NativeFunc]` + `AutoNativeLoader`），重编 DLL 后即可热重载；非编辑器分支走静态 `DllImport`。

### 物理管线

Unity 侧物理帧由注入 `PlayerLoop` 的 `PhyUpdate` / `PhyLateUpdate` 驱动，节流间隔 `PhyTime.DELAY_FRAME = 0.02s`（名义 50 Hz），`dt` 取真实帧间隔。托管层注册在 `PhyLateUpdate`，每物理帧：

```step
PhySystemManaged.Step()
 ├─ ComponentUpdate()        各组件把 Transform / 属性变化写进对应缓冲
 ├─ SyncDirtyData()          12 条 NativeList 一次性 flush 到原生批量接口
 ├─ Step(dt)                 → 原生内核步进
 └─ ApplyDirtyBodies()       读回脏刚体（位置/旋转/速度）写回组件并释放缓冲
```

原生侧：

```step
PhySystem::Step(dt)
 ├─ 清空脏刚体表、重置碰撞对可见集
 ├─ dt <= 0 直接返回
 └─ 子步循环（dt / SUB_STEP_COUNT）：
     SubStep(step)
      while 剩余时间 > 0 且迭代次数 < MAX_CCD_ITERATION：
       ├─ ContinuousCollisionStep(剩余)  脏碰撞体同步 → 离散/连续宽相 → 含 Continuous 的对求 TOI
       │                                 → 取最早 TOI 截断剩余时间，返回该时刻要解的碰撞对
       ├─ TransformStep(toi)             阻尼衰减 + 位姿积分，同步名下碰撞体
       ├─ BodyStep(toi)                  重力 / 外力 → 速度增量，清空累积量
       ├─ SolveStep(碰撞对)              岛划分 → 迭代冲量 → 位置修正 → 回写刚体与子碰撞体
       └─ 剩余时间 -= toi
      剩余时间（迭代上限提前退出时）：TransformStep + BodyStep 直接积分掉
      离散收尾：
       ├─ DiscreteCollisionStep()        宽相同步 → GJK / EPA → 接触流形
       └─ SolveStep(离散碰撞对)
```

### 可调参数

| 参数 | 值 | 位置 |
| --- | --- | --- |
| 子步数 | 6 | `PhySystemManaged.SUB_STEP_COUNT` |
| 求解迭代次数 | 8 | `PhySystemManaged.SOLVER_ITERATION` |
| CCD 最小推进比例 | 0.001 | `PhySystem::MIN_CCD_STEP` |
| 子步内 CCD 迭代上限 | 36 | `PhySystem::MAX_CCD_ITERATION` |
| 重力加速度 | 9.8（沿 `Vector2.down`） | `PhyQuantities::GRAVITY_ACCELERATION` |
| 位置修正松弛系数 | 0.2 | `PhyQuantities::RELAXATION` |
| 单次最大位置修正 | 0.2 | `PhyQuantities::MAX_POS_CORRECTION` |
| 允许穿透 slop | 0.005 | `PhyQuantities::PENETRATE_SLOP` |
| GJK / EPA 迭代上限 | 36 | `NarrowPhase::MAX_ITERATION` |
| 射线窄相迭代上限 | 72 | `NarrowPhase::RAY_CAST_MAX_ITERATION` |
| 数学容差 `Epsilon` | 1e-5 | `Math::Epsilon` |

---

## 目录结构

```file
SpaceZ-2D/
├─ Assets/
│  ├─ Plugins/x86_64/                      原生插件：SpaceZ_2D_Beta.dll（主线）/ Alpha / Exp_CCD / EditorPlugin.dll
│  ├─ Scenes/Beta/SampleSceneStacking.unity Beta 堆叠验证场景
│  └─ Scripts/
│     ├─ Framework/Runtime/                PlayerLoop 注入、物理帧节流、RuntimeStart/End 派发
│     ├─ Framework/Plugin/                 DLL 动态加载（NativeLoader / AutoNativeLoader）、[NativeFunc]、路径解析
│     └─ Plugins/SpaceZ_2D_Beta/           Beta 托管层
│        ├─ System/PhySystemNative.cs      导出符号 ↔ C# 签名映射（编辑器反射绑定 / 非编辑器 DllImport）
│        ├─ System/PhySystemManaged.cs     帧循环、组件登记、脏数据打包下发与回写
│        ├─ System/PhySystemQuery.cs       空间查询的托管出口（NativeArray → List / out 参数）
│        ├─ System/Buffer/                 NativeArray / NativeList（分配在原生堆上的连续缓冲）
│        ├─ System/NativeData/             与原生逐字段对齐的封送结构体
│        ├─ System/Component/              组件基类：句柄登记、归属切换、批量下发
│        ├─ Component/                     可挂载组件：Rigidbody2D_Z / Box|Sphere|Capsule|LineCollider2D_Z
│        └─ Sample/InitailVelocity.cs      速度注入示例
│
├─ SpaceZ_2D_Beta/                         ★ 当前主线内核
│  ├─ Core/
│  │  ├─ Math/                             Vector2/3、Matrix2x2/3x3、Math.tpp
│  │  ├─ Debug/                            Debug、DebugFormat、ToString
│  │  ├─ Memory/                           页式内存管理（替换全局 operator new）
│  │  ├─ Object/                           Rigidbody、Collider(+Line/Box/Sphere/Capsule)、Transform、Handle
│  │  │  └─ Container/                     ObjectMgr、ObjectPool、UnionFindSet、ObjectBuffer
│  │  ├─ Collision/
│  │  │  ├─ BroadPhase/                    BVH 树、节点、查询
│  │  │  ├─ Convex/                        Convex / Line / Box / Sphere / Capsule
│  │  │  ├─ Geometry/                      Bound、Polygon、Simplex、Ray
│  │  │  ├─ Info/                          ConvexDetectReq、ContinueDetectReq、CollisionInfo、RayHit
│  │  │  ├─ Layer/                         LayerMask（64 位掩码）
│  │  │  └─ NarrowPhase/                   GJK / EPA / 接触流形 / 连续检测 / 射线 GJK
│  │  ├─ Solver/                           PhySolver、SolverPair、SolverRigidbody、SolverCollisionPoint、IslandDivider
│  │  └─ System/                           PhySystem（门面）+ PhyEngine / PhyObject / SpaceQuery + CollisionPairBuffer
│  ├─ UnityPlugin/
│  │  ├─ ExportData/                       跨边界结构体（Handle / ObjInfo / ObjProperty / BodyState / CollisionPair / RaycastHit / NativeArray）
│  │  ├─ ExportInterface/                  C ABI：PhyEngine / PhyObject / PhyQuery / NativeBuffer
│  │  ├─ Memory/                           接 IUnityMemoryManager，把页分配挂到 Unity 记账
│  │  ├─ Profiler/                         接 IUnityProfiler，Physics2D 分类标记
│  │  ├─ Debug/                            IUnityLog 日志转发
│  │  └─ UnityPlugin/                      UnityPluginLoad / Unload + 多播加载回调
│  ├─ Main/Main.cpp                        控制台入口（占位）
│  ├─ Tools/                               构建脚本（build / build_console / build_plugins / build_plugins_release / build_tests / clean）
│  ├─ Tools/README.md                      构建脚本细节、obj 目录布局
│  └─ UnityPlugin/README.md                静态初始化期堆分配的三条硬规则（必读）
│
├─ SpaceZ_2D_Exp_CCD/                      历史：全量 CCD 内核（冻结）
├─ SpaceZ_2D_Alpha/                        历史：离散内核（冻结）
└─ EditorPlugin/                           向内核 DLL 提供 IUnityInterface（编辑器下必需）
```

---

## 快速开始

### 环境要求

- Windows x64
- Unity **2022.3.62f3c1**（编辑器内运行）
- Visual Studio 2017+，含「使用 C++ 的桌面开发」工作负载（构建脚本用 `vswhere` 自动定位 MSVC）

### 构建原生库

仓库里 `Assets/Plugins/x86_64/*.dll` 已入库，只改 C# 时不需要构建；**改动 C++ 后必须重建并提交 DLL**。

在 VS Code 中打开 `SpaceZ_2D_Beta/` 作为工作区，`Ctrl+Shift+B` 选择任务；或直接运行脚本：

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `SpaceZ_2D_Beta/Tools/build_plugins.bat` | Core + UnityPlugin → 单个 DLL（Debug） | `Assets/Plugins/x86_64/SpaceZ_2D_Beta.dll` |
| `SpaceZ_2D_Beta/Tools/build_plugins_release.bat` | 同上，`/O2 /GL` + link `/LTCG`（Release） | **同一个** DLL（与 Debug 互相覆盖） |
| `SpaceZ_2D_Beta/Tools/build_console.bat` | Core + Main → 控制台程序 | `SpaceZ_2D_Beta/Main/Debug/SpaceZ_2D_Beta.exe` |
| `SpaceZ_2D_Beta/Tools/build_tests.bat` | Core + Tests → 控制台测试（`Tests/` 当前为空，打印 `[SKIP]` 后正常退出） | `SpaceZ_2D_Beta/Tests/Debug/` |
| `SpaceZ_2D_Beta/Tools/clean.bat` | 清理本工程产物 | 删除 `.build/`、`Main/Debug`、`Tests/Debug`、上述 DLL 及 `.meta` |
| `EditorPlugin/tools/build_editor_plugin.bat` | EditorPlugin 独立编译 | `Assets/Plugins/x86_64/EditorPlugin.dll` |

编译配置：

- Debug：`/std:c++20 /utf-8 /W4 /Od /RTC1 /MDd`
- Release：`/std:c++20 /utf-8 /W4 /O2 /GL /Gy /MD /DNDEBUG`，链接端 `/LTCG /OPT:REF /OPT:ICF`

中间文件进 `.build/obj_<target>/`，并按源码路径**逐级镜像**（`cl.exe` 只按基名命名 obj，平铺会把 `Core/Debug/X.cpp` 与 `UnityPlugin/Debug/X.cpp` 写到同一个 `X.obj`）；Debug 与 Release 的 obj 必须分目录，`/MDd` 与 `/MD` 混链会报 `LNK2038`。`.pdb` 留在 `.build/`，不污染 Unity 资产目录。Debug/Release 共用同一个 DLL 输出路径，为的是保住 Unity 侧 `.meta` 与 GUID，代价是同一时刻目录里只能存在一种配置，因此「构建全部」不含 Release 任务。

> **DLL 被锁**：Unity 加载过原生 DLL 后会持有文件句柄，链接会失败。看到 `is locked, probably loaded by Unity Editor` 时关闭 Unity 再构建。

### 在 Unity 中运行

`RuntimeManager` 通过 `[RuntimeInitializeOnLoadMethod(BeforeSceneLoad)]` 扫描 `[RuntimeStart]` 方法并**按 Order 升序**执行：

| Order | 方法 | 作用 |
| --- | --- | --- |
| -100 | `UpdateManager.Init` | 把 `PhyLoop`（含 `PhyUpdate` / `PhyLateUpdate`）插入 `PlayerLoop` 的 `PreUpdate` 之前 |
| -100 | `PhySystem.InitNative` | 加载 `SpaceZ_2D_Beta.dll`，反射绑定全部 `[NativeFunc]` 字段（含 `NativeArray` 上的缓冲 API） |
| -90 | `PhySystem.InitManaged` | `StartEngine(6, 8)`、初始化缓冲、注册物理帧回调 |

退出时按 `[RuntimeEnd]` 升序执行：`Order 90` 停引擎并释放缓冲 → `Order 100` 卸载 DLL 并还原 `PlayerLoop`。
编辑器下 `EditorPlugin.dll` 必须在位：`NativeLoader` 加载内核前先向它取 `IUnityInterface` 指针，取不到会抛异常、插件加载整体失败。

---

## 使用示例

在 GameObject 上挂 `Rigidbody2D_Z`，在其自身或子物体上挂形状组件即可，无需手动创建原生对象：

```csharp
using SpaceZ_2D.Beta;
using UnityEngine;

public class Launch : MonoBehaviour
{
    [SerializeField] private Rigidbody2D_Z _body;

    private void Start()
    {
        // 属性写入先进缓冲，下一次物理帧统一同步到原生侧
        _body.LinearVelocity = new Vector2(3f, 0f);
        _body.AngularVelocity = 0.5f;

        _body.AddForce(new Vector2(0f, 10f));      // ForceType.Impulse：立即折算成速度增量
        _body.AddTorque(1f, ForceType.Sustain);    // Sustain：本子步内持续作用，步进后清零
    }

    private void Update()
    {
        // 空间查询可直接调静态门面，结果内存由托管层回收
        if(PhySystem.Raycast(Vector2.zero, Vector2.down, out RaycastHitInfo hit))
            Debug.Log($"命中 {hit.collider}，距离 {hit.distance}");
    }
}
```

要点：

- **生命周期**：刚体在 `Awake` 创建、`OnDestroy` 销毁；碰撞体在 `OnEnable` 创建、`OnDisable` 与 `OnDestroy` 销毁（禁用即释放原生对象，重新启用会重建）。
- **静态碰撞体**：找不到父级刚体时以空句柄创建，原生侧归入静态分支（`invMass = 0`），只参与碰撞不被推动。
- **层与模式**：`Layer` 是 64 位掩码（默认层 0，即 `1UL`），宽相按掩码相交过滤；`Mode` 取 `Discrete` / `Continuous`，`Continuous` 需挂在刚体上，否则退化为 `Discrete`。
- **回声抑制**：刚体与碰撞体都缓存上次下发的值，未变化时不下发；碰撞体的局部变换按「刚体世界变换的逆 × 自身世界变换」解算，换父级时 `OnTransformParentChanged` 会重新绑定刚体。
- **单位**：角度与角速度均为弧度；矩形 `Extents` 是半长；胶囊 `Height` 是圆柱段高度（总高 = `Height + 2 * Radius`）；线段 `Length` 是全长、沿局部 X 轴、中心在原点。
- **查询出口**：`OverlapBox / OverlapSphere / OverlapCapsule` 填 `List<Handle>` 并返回命中数；`Raycast` 返回是否命中并回填 `RaycastHitInfo`（未命中时为 `default`），`RaycastAll` 填 `List<RaycastHitInfo>`，已按距离升序。

---

## 原生 ABI 契约

结构体定义位置：

- C#：`Assets/Scripts/Plugins/SpaceZ_2D_Beta/System/NativeData/*.cs`
- 原生：`SpaceZ_2D_Beta/UnityPlugin/ExportData/*.h`

两侧必须**逐字段、逐顺序**一致（4 字节对齐）。约定：

- 入参批量接口统一为 `(T* infos, int count)`，`T` 是「8 字节句柄 + 载荷」展平后的结构；`infos` 指向托管 `NativeList` 的原生内存，**仅在调用期间有效**，原生侧不可缓存。
- 出参统一 `NativeArray{void* ptr, int size}`，由原生 `new[]` 分配、调用方必须调 `DestroyBuffer` 回收；元素类型须平凡析构（`Handle`、`RaycastHit` 一类）。
- `bool` 跨边界只占 1 字节：载荷里用 `UnmanagedType.U1`，返回值用 `[return: MarshalAs(UnmanagedType.I1)]`；默认 4 字节 `BOOL` 会把后续字段顶偏或读到高位垃圾。
- 角度、角速度全程弧度；碰撞法线方向为 `first → second`，`point` 为世界空间接触点。
- `layer` 一律传 64 位掩码而不是结构体，传 0 表示不匹配任何层。

导出函数（均在 `SpaceZ_2D_Beta/UnityPlugin/ExportInterface/`，`extern "C" __declspec(dllexport)`）：

- `PhyEngine.cpp`：`StartEngine` / `StopEngine` / `Step` / `DirtyBodies` / `EnterCollisionPairs` / `StayCollisionPairs` / `ExitPairs`
- `PhyObject.cpp`：`CreateBody` / `CreateCollider` / `DestroyBody` / `DestroyCollider` / `SetColliderBody` / `SetBodiesProperty` / `SetBodiesVelocity` / `AddForces` / `AddTorques` / `SetCollidersProperty` / `SetCollidersCollision` / `SetBodiesTransform` / `SetCollidersTransform` / `SetLineColliders` / `SetBoxColliders` / `SetSphereColliders` / `SetCapsuleColliders`
- `PhyQuery.cpp`：`OverlapBox` / `OverlapSphere` / `OverlapCapsule` / `Raycast` / `RaycastAll`
- `NativeBuffer.cpp`：`CreateBuffer` / `DestroyBuffer`

所有导出都包在 `TRY_CATCH_DEBUG` 内：异常转 `Debug::LogError`（编辑器内经 `IUnityLog` 转发到 Console），不向 Unity 抛；内存管理器或物理系统未就绪时直接返回空结果。

---

## 已知限制

- **平台**：只支持 Windows x64 编辑器。`PluginUtility` 只在 `UNITY_EDITOR_64` 下解析 DLL 路径，Player 打包链路未实测。
- **碰撞事件**：原生侧已产出并导出 Enter / Stay / Exit 三张表，托管层尚未消费（不取即不产生额外开销）。
- **无触发器 / 无关节**：碰撞体没有「只检测不响应」的模式，也没有任何约束类型；层只做「掩码相交」过滤，没有分层碰撞矩阵。
- **连续检测范围**：只覆盖至少一端为 `Continuous` 的碰撞对；小于 `MIN_CCD_STEP`（0.001）的时间推进会被合并，极端高速仍可能穿透，靠减小 `dt` 或增加子步缓解。
- **退化情形**：GJK/EPA 迭代上限用尽时与「真分离」共用 `return false`，极端配置下可能漏检；穿透深度小于 `Epsilon`(1e-5) 一律判分离，恰相切不产生接触。
- **静态初始化期禁止堆分配**：内核替换了全局 `operator new`，而分配器要等 `UnityPluginLoad` 才就绪，静态存储期的容器 / 字符串构造会让 `LoadLibrary` 直接失败。需要静态容器时改用 `UnityPlugin::NativeAllocator` 或推迟到函数内 `static`，详见 `SpaceZ_2D_Beta/UnityPlugin/README.md`。
- `Main/Main.cpp` 仍是 Hello World 占位；`Tests/` 目录暂空，构建脚本打印 `[SKIP]` 正常退出。

---

## 开发计划

- **Beta（进行中）**：以离散为主、连续为辅的通用物理引擎。下一步是把碰撞事件派发接到托管层（进入 / 停留 / 离开），并按需要扩展空间查询与分层碰撞矩阵。
- **Alpha / Exp-CCD（已完成，冻结）**：作为求解架构与 CCD 方案的历史验证，保留源码与构建脚本，不再迭代。
