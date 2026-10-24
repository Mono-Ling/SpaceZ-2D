# SpaceZ-2D

自研 2D 物理引擎：**C++20 内核 + Unity 表现层**。

内核负责宽相 BVH、窄相 GJK/EPA、接触流形与顺序冲量求解，通过 C ABI 以 x64 原生插件（`.dll`）的形式被 Unity 调用；
Unity 侧只做组件封装与无分配数据封送。

| 项目 | 说明 |
| --- | --- |
| 引擎内核 | C++20 / MSVC，`/std:c++20 /utf-8 /W4`，Debug x64 |
| 运行宿主 | Unity 2022.3.62f3c1（Windows x64 编辑器） |
| 求解模型 | 顺序冲量（Sequential Impulse）+ 岛划分 + 位置修正 |
| 形状 | Box（矩形）、Sphere（圆）、Capsule（胶囊） |

---

## 特性

## 碰撞检测

- 宽相：动态平衡 BVH，插入时按「最优兄弟节点 + 高度代价」选择合并对象，脏碰撞体增量同步（`SynBreadthPhase`）。
- 窄相：GJK 求最近点 / 相交，EPA 求穿透深度与法线（`MaxIteration = 36`）。
- 接触流形：同一对凸体可生成多个接触点（`ContactManifold`），而不是单点近似。
- 支撑函数统一走 `Support(dir, worldToLocal, localToWorld)`，旋转/缩放由变换矩阵承担，形状本身始终在局部空间。

## 物理求解

- 岛划分：并查集把碰撞对切成互不影响的连通分量，逐岛独立迭代。
- 每个接触点：法向冲量（含恢复系数速度偏置）+ 切向冲量（静摩擦钳制 / 动摩擦），迭代后再做位置修正（松弛系数 + 穿透容差）。
- 积分：半隐式欧拉，阻尼用指数衰减 `v *= exp(-drag * dt)`，按子步推进（默认 4 子步 × 8 次迭代）。

## 接口与工程性

- 句柄为 `{int32 id, int32 generation}`，对象删除后代数自增，旧句柄可被检出而非野指针。
- Unity 侧批量封送：属性写入先进 `NativeList` 缓冲，每物理帧一次性 flush 到原生侧（`SyncAndClear`），回写也走同一块缓冲 —— 避免逐字段 P/Invoke 与 GC。
- 编辑器内走 `LoadLibrary` + 函数指针（`NativeLoader`），重编 DLL 后即可热重载，无需重启 Unity；Player 走静态 `DllImport` 分支。

---

## 目录结构

```file
SpaceZ-2D/
├─ Assets/
│  ├─ Plugins/x86_64/                 SpaceZ_2D_Alpha.dll（物理内核）、EditorPlugin.dll（编辑器桥）
│  ├─ Scenes/SampleScene.unity
│  └─ Scripts/
│     ├─ Test.cs                      原生加法冒烟测试（Add.NativeAdd）
│     ├─ Framework/
│     │  ├─ Runtime/                  PlayerLoop 注入、物理帧计时、RuntimeStart/End 初始化派发
│     │  └─ Plugin/                   NativeLibrary 动态加载与卸载
│     └─ Plugins/SpaceZ_2D_Alpha/
│        ├─ Native/                   与 C++ 逐字段对齐的封送结构体 + Handle + ColliderType
│        ├─ Buffer/                   NativeList 缓冲（只读批量输入 / 批量输出）
│        ├─ PhySystem/                托管物理系统
│        │  ├─ PhySystemNative.cs     P/Invoke 与函数指针绑定
│        │  ├─ PhySystemManaged.cs    物理帧调度（组件更新 → 数据同步 → 步进 → 回写）
│        │  ├─ PhySystemFunction.cs   UNITY_EDITOR 下的委托签名
│        │  └─ Component/             Rigidbody2DAlpha / Box|Sphere|CapsuleCollider2DAlpha
│        └─ Sample/InitialVelocity.cs 初始速度示例
│
├─ SpaceZ_2D_Alpha/                   ★ C++ 物理内核 + Unity 导出层（demo 阶段完成，冻结维护）
│  ├─ Core/
│  │  ├─ Math/                        Vector2/3、Matrix2x2/3x3、Transform、数学工具
│  │  ├─ Object/                      Rigidbody、Collider(+Box/Sphere/Capsule)、Handle、ObjectMgr
│  │  ├─ CollisionDetection/
│  │  │  ├─ BVH/                      宽相 BVH 树与节点
│  │  │  ├─ Convex/                   Convex / Box / Sphere / Capsule
│  │  │  ├─ Geometry/                 Bound、Polygon、Simplex
│  │  │  └─ NarrowPhase/              GJK + EPA + 接触流形
│  │  ├─ Solver/                      PhySolver、SolverPair、SolverRigidbody、IslandDivider
│  │  ├─ Collision/                   CollisionPair、CollisionSolveReq
│  │  └─ System/                      PhySystem（对外统一门面）
│  ├─ UnityPlugin/                    C ABI 导出（EngineLifeCycle / EngineObject）、NativeDebug、IUnityInterface
│  ├─ Main/                           控制台入口（当前为 Hello World 占位）
│  ├─ Tests/                          PhySystemTests.cpp：不需要 Unity 的集成回归测试
│  └─ tools/                          build_plugins / build_console / build_tests / clean
│
└─ EditorPlugin/                      ★ 独立编辑器插件（与 Alpha 无源码/构建依赖）
   └─ EditorPluginLoad.cpp            UnityPluginLoad/Unload + GetUnityInterface
```

> 三个 C++ 工程平级且**互不耦合**：各自的 `.vscode/`、`tools/`、`.build/`、`.gitignore` 完全独立，互不转调、互不清理。

---

## 物理管线

Unity 侧每物理帧（`PhyLateUpdate`，节流 `PhyTime.DELAY_FRAME = 0.02s`，名义 50 Hz）：

```step
PhySystemManaged.Step()
 ├─ ComponentUpdate()    各组件把 Transform 变化写入对应缓冲
 ├─ SyncDirtyData()      缓冲 flush 到原生侧（质量/阻尼/速度/材质/变换/形状/力）
 ├─ EngineStep(dt)       → PhySystem::Step
 └─ GetDirtyBodies()     读回刚体状态并写回 Transform
```

原生侧 `PhySystem::Step(dt)`：

```step
清空脏刚体集合
消费一次外部力 / 力矩（dt 全量）
循环 subStep 次（默认 4）：
  SubStep(dt / subStepCount)
   ├─ AddGravity           f += m · g
   ├─ ApplyForce / Torque  转为速度增量并清空累积量，标记脏刚体
 ├─ CollisionStep        BVH 同步 → 宽相 → GJK/EPA 窄相 → 接触流形
 ├─ SolveStep            岛划分 → 迭代冲量 → 位置修正 → 回写刚体并同步子碰撞体
 └─ TransformStep        积分位置/角度，阻尼衰减，同步子碰撞体
```

可调参数：

| 参数 | 默认值 | 位置 |
| --- | --- | --- |
| 子步数 `subStep` | 4 | `PhySystemManaged.SUB_STEP_COUNT` |
| 迭代次数 `iteration` | 8 | `PhySystemManaged.ITERATION` |
| 重力加速度 | 9.8（沿 `Vector2.down`） | `PhyQuantities::GRAVITY_ACCELERATION` |
| 位置修正松弛系数 | 0.2 | `PhyQuantities::RELAXATION` |
| 单次最大位置修正 | 0.2 | `PhyQuantities::MAX_POS_CORRECTION` |
| 允许穿透 slop | 0.005 | `PhyQuantities::PENETRATE_SLOP` |
| GJK/EPA 迭代上限 | 36 | `NarrowPhase::MAX_ITERATION` |
| 数学容差 `Epsilon` | 1e-5 | `Math::Epsilon` |

---

## 快速开始

### 环境要求

- Windows x64
- Unity **2022.3.62f3c1**（依赖 `com.unity.collections` 2.5.7 提供 `NativeList`）
- Visual Studio 2017+，含「使用 C++ 的桌面开发」工作负载（构建脚本用 `vswhere` 自动定位 MSVC）

### 构建原生库

仓库里 `Assets/Plugins/x86_64/*.dll` 已入库，只改 C# 时不需要构建；**改动 C++ 后必须重建并提交 DLL**。

在 VS Code 中打开 `SpaceZ_2D_Alpha/` 作为工作区，`Ctrl+Shift+B` 选择任务；或直接运行脚本：

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `SpaceZ_2D_Alpha/tools/build_plugins.bat` | Core + UnityPlugin → 单个 DLL | `Assets/Plugins/x86_64/SpaceZ_2D_Alpha.dll` |
| `SpaceZ_2D_Alpha/tools/build_console.bat` | Core + Main → 控制台程序 | `Main/Debug/SpaceZ_2D_Alpha.exe` |
| `SpaceZ_2D_Alpha/tools/build_tests.bat` | Core + Tests → 控制台测试 | `Tests/Debug/SpaceZ_SolverTests.exe` |
| `SpaceZ_2D_Alpha/tools/clean.bat` | 清理本工程产物 | 删除 `.build/`、`Main/Debug`、`Tests/Debug`、上述 DLL 及 `.meta` |
| `EditorPlugin/tools/build_editor_plugin.bat` | EditorPlugin 独立编译 | `Assets/Plugins/x86_64/EditorPlugin.dll` |
| `EditorPlugin/tools/clean.bat` | 清理 EditorPlugin 产物 | 删除其 `.build/` 与 `EditorPlugin.dll` |

编译配置：`Debug | x64 | /std:c++20 /utf-8 /EHsc /W4 /Zi /Od /RTC1 /MDd`。
中间文件进各自 `.build/obj_*`，调试符号 `.pdb` 也留在 `.build/`，不污染 Unity 资产目录。

> **DLL 被锁**：Unity 加载过原生 DLL 后会持有文件句柄，链接会失败。脚本会在链接前预删旧 DLL 并提示；
> 看到 `is locked, probably loaded by Unity Editor` 时关闭 Unity 再构建。

### 在 Unity 中运行

`RuntimeManager` 通过 `[RuntimeInitializeOnLoadMethod(BeforeSceneLoad)]` 扫描 `[RuntimeStart]` 方法并**按 Order 顺序**执行：

| Order | 方法 | 作用 |
| --- | --- | --- |
| -100 | `UpdateManager.Init` | 把 `PhyLoop`（含 `PhyUpdate` / `PhyLateUpdate`）插入 `PlayerLoop` 的 `PreUpdate` 之前 |
| -100 | `PhySystem.Init` | 加载原生 DLL，绑定全部导出函数 |
| -90 | `PhySystem.InitManaged` | `StartEngine(4, 8)`、初始化缓冲、注册物理帧回调 |

退出时按 `[RuntimeEnd]` **升序**执行：`Order 90` 停引擎并释放缓冲 → `Order 100` 卸载 DLL。
（`RuntimeManager` 对 `RuntimeStart` / `RuntimeEnd` 都是升序派发，靠 Order 的正负和大小决定先后。）

---

## 使用示例

在 GameObject 上挂 `Rigidbody2DAlpha`，在其自身或子物体上挂碰撞体组件即可，无需手动创建原生对象：

```csharp
using SpaceZ_2D.Alpha;
using UnityEngine;

public class Launch : MonoBehaviour
{
    [SerializeField] private Rigidbody2DAlpha _body;

    private void Start()
    {
        // 属性写入先进缓冲，下一次物理帧统一同步到原生侧
        _body.LinearVelocity = new Vector2(3f, 0f);
        _body.AngularVelocity = 0.5f;

        _body.AddForce(new Vector2(0f, 10f));   // 单帧冲量，物理帧消费后清零
        _body.AddTorque(1f);
    }
}
```

要点：

- **静态碰撞体**：碰撞体找不到父级 `Rigidbody2DAlpha` 时按静态处理（`Body = null`，`invMass = 0`），只参与碰撞不被推动。
- **一个刚体可挂多个碰撞体**，父子变换变化会自动同步；换父级时 `OnTransformParentChanged` 会重新绑定刚体。
- **角度单位为弧度**：组件内部在 `transform.eulerAngles.z * Deg2Rad` 与 `state.rotation * Rad2Deg` 之间往返。
- **矩形 `Extents` 是半长**；**胶囊 `Height` 是两端半圆圆心的间距**（总高 = `Height + 2 * Radius`）。
- 原生对象在生命周期回调里创建：刚体在 `Awake`、碰撞体在 `OnEnable`，创建后立即注入一次初始属性；运行时改属性只发增量。

---

## 原生 ABI 契约

C# 侧 `Assets/Scripts/Plugins/SpaceZ_2D_Alpha/Native/*.cs` 与 C++ 侧
`UnityPlugin/ExportInterface/ExportInfo/*.h` 必须**逐字段、逐顺序**一致（4 字节对齐）。

| 类型 | 大小 | 字段偏移 |
| --- | --- | --- |
| `Handle` | 8 | `id@0`, `generation@4`（`null = {-1, 0}`） |
| `CollisionPair` | 52 | `firstCollider@0`, `firstBody@8`, `secondCollider@16`, `secondBody@24`, `depth@32`, `point@36`, `normal@44` |
| `BodyState` | 32 | `handle@0`, `rotation@8`, `angularVelocity@12`, `position@16`, `linearVelocity@24` |
| `BodyMassInfo` | 16 | `handle@0`, `mass@8`, `inertia@12` |
| `ColliderMaterialInfo` | 20 | `handle@0`, `staticFriction@8`, `dynamicFriction@12`, `elasticity@16` |
| `BodyTransformInfo` | 28 | `handle@0`, `rotation@8`, `position@12`, `scale@20` |

约定：

- C# 结构体一律 `[StructLayout(LayoutKind.Sequential)]`；**输出方向**（`BodyState`、`CollisionPair`）尤其不能多字段少字段。
- 批量接口签名为 `(T* infos, int count)`，`infos` 指向托管 `NativeList` 的非托管指针，**仅在调用期间有效**，原生侧不得缓存。
- `GetCollisionPairs` / `GetDirtyBody` 的 `count` 是**入参容量 + 出参实数量**：调用方按容量分配，原生侧回写实际写入数；
  引擎不可用、指针为空或发生异常时回写 `count = -1`（C# 侧 `NativeDataBuffer` 会把它夹到 0 并跳过，`GetDirtyBodies` 另做 `count < 0` 报错）。
- 所有角度、角速度均为**弧度**（内核全程 `Cos/Sin` 直算，无 `Deg2Rad` 转换）。
- 碰撞体类型枚举：`Box = 0`、`Sphere = 1`、`Capsule = 2`。
- 碰撞法线方向为 `first → second`，`point` 为**世界空间**接触点。

导出函数分两组，均在 `UnityPlugin/ExportInterface/`：

- `EngineLifeCycle.cpp`：`StartEngine` / `StopEngine` / `EngineStep` / `GetCollisionCount` / `GetCollisionPairs` / `GetDirtyBodyCount` / `GetDirtyBody`
- `EngineObject.cpp`：`CreateBody` / `CreateCollider` / `DestroyBody` / `DestroyCollider` / `SetColliderBody` / `SetBodies` / `SetBodiesDrag` / `SetBodiesVelocity` / `SetColliders` / `SetBodiesTransform` / `SetCollidersTransform` / `SetBoxColliders` / `SetSphereColliders` / `SetCapsuleColliders` / `AddForce` / `AddTorque`

所有导出都包在 `TRY_CATCH_DEBUG` 内，异常转 `Debug::LogError`（编辑器内经 `IUnityLog` 转发到 Console），不向 Unity 抛。

---

## Alpha已知限制

- **堆叠稳定**：Alpha版本受限于接触流形（多接触点）顺序耦合，会每帧产生微小正冲量矩；碰撞盒堆叠尤为明显
- **平台**：只支持 Windows x64，且构建产物为 Debug。`PluginUtility` 仅在 `UNITY_EDITOR_64` 下返回 DLL 路径，Player 构建走静态 `DllImport` 分支，尚未实测打包链路。
- **无触发器 / 碰撞层**：内核没有 trigger、layer、collision mask 概念，所有碰撞体两两检测。
- **无 CCD**：高速物体的隧穿不处理。
- **无关节 / 约束**：只有接触约束。
- **退化情形**：GJK/EPA 迭代上限用尽时与「真分离」共用 `return false`，极端配置下可能漏检；穿透深度小于 `Epsilon`(1e-5) 一律判分离，恰相切不产生接触。
- `Main/main.cpp` 仍是 Hello World 占位，控制台目标目前只用于跑 Core。
- `Tests/` 是自写控制台用例，未接入 Unity Test Framework。

---

## 开发计划

- **Alpha**：全量离散碰撞检测物理引擎；闭环物理引擎核心循环，验证物理求解与引擎核心架构。（已完成）
- **Exp-CCD**：全量连续碰撞检测物理引擎；在Alpha暴露物理求解架构问题上优化，验证连续碰撞检测实现方案。
- **Beta**：采用离散为主连续为辅的物理引擎；引入碰撞层剔除、空间查询等通用API，实现最小可用版本
