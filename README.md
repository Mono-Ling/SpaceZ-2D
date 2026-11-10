# SpaceZ-2D

自研 2D 物理引擎：**C++20 内核 + Unity 表现层**。

内核负责宽相 BVH、窄相 GJK/EPA、接触流形、连续碰撞检测与顺序冲量求解，通过 C ABI 以 x64 原生插件（`.dll`）的形式被 Unity 调用；
Unity 侧只做组件封装与无分配数据封送。

仓库内同时存在两套内核，共用同一套 Unity 侧框架：

| 内核 | 定位 | 状态 |
| --- | --- | --- |
| `SpaceZ_2D_Alpha` | 全量**离散**碰撞检测，跑通物理求解闭环 | demo 完成，**冻结维护** |
| `SpaceZ_2D_Exp_CCD` | 全量**连续**碰撞检测（保守步进 CCD） | 当前主线，持续迭代 |

| 项目 | 说明 |
| --- | --- |
| 引擎内核 | C++20 / MSVC，`/std:c++20 /utf-8 /W4`，Debug x64（Exp-CCD 另提供 Release 配置） |
| 运行宿主 | Unity 2022.3.62f3c1（Windows x64 编辑器） |
| 求解模型 | 顺序冲量（Sequential Impulse）+ 岛划分 + 位置修正 |
| 形状 | Box（矩形）、Sphere（圆）、Capsule（胶囊） |

---

## 特性

### 碰撞检测

- 宽相：动态平衡 BVH，插入时按「最优兄弟节点 + 高度代价」选择合并对象，脏碰撞体增量同步。
- 窄相：GJK 求最近点 / 相交，EPA 求穿透深度与法线（`MAX_ITERATION = 36`）。
- 接触流形：同一对凸体可生成多个接触点（`ContactManifold`），而不是单点近似。
- 支撑函数统一走 `Support(dir, worldToLocal, localToWorld)`，旋转/缩放由变换矩阵承担，形状本身始终在局部空间。

### 连续碰撞检测（Exp-CCD）

- 扫掠宽相：每个子步按 `v · dt` 与 `ω · dt` 扩张动态碰撞体的包围盒（`SweepBound`），使 AABB 覆盖整段运动轨迹；静态体只在脏时重建。
- 保守步进（conservative advancement）：`ContinuousCollision` 用相对速度/角速度反复推进两凸体的相对位姿，直到 GJK 出现相交，返回该次碰撞的 TOI 与接触信息。
- 子步内时间推进：`CollisionStep` 收集本子步**最早**的 TOI，把子步时间截断到该时刻，只求解时间窗内（含 `Near` 容差）的碰撞对，剩余时间继续循环推进，因此一个物理帧内可以处理多次顺序碰撞。
- 推进下限：单次推进不小于「本次子步剩余时间 × `MIN_CCD_STEP`（0.01）」，持续接触时据此退出循环、把剩余时间按普通离散步进消化，避免 TOI 恒 0 造成死循环。

### 物理求解

- 岛划分：并查集把碰撞对切成互不影响的连通分量，逐岛独立迭代。
- 每个接触点：法向冲量（带目标相对速度，即恢复系数）+ 切向冲量（静摩擦钳制，超出静摩擦时切到动摩擦），迭代后再做位置修正（`RELAXATION` 松弛 + `PENETRATE_SLOP` 穿透容差 + `MAX_POS_CORRECTION` 上限）。
- 材质合并：一对碰撞体的静摩擦 / 动摩擦取两者较小值。
- 积分：半隐式欧拉，阻尼用指数衰减 `v *= exp(-drag * dt)`，按子步推进。
- 力类型：`Impulse` 立即折算成速度增量，`Sustain` 在本子步内持续作用、步进结束后清零。

### 接口与工程性

- 句柄为 `{int32 id, int32 generation}`，对象删除后代数自增，旧句柄可被检出而非野指针。
- Unity 侧批量封送：属性写入先进 `NativeList` 缓冲，每物理帧一次性 flush 到原生侧，回写也走同一块缓冲 —— 避免逐字段 P/Invoke 与 GC。
- 编辑器内走 `LoadLibrary` + 函数指针，重编 DLL 后即可热重载，无需重启 Unity；Player 走静态 `DllImport` 分支。
- 函数指针绑定有两种写法：Alpha 侧逐函数 `GetFunction(name, out delegate)`；Exp-CCD 侧用 `[NativeFunc("符号名")]` 标注委托字段，由 `AutoNativeLoader` 反射批量绑定/解绑。

---

## 目录结构

```file
SpaceZ-2D/
├─ Assets/
│  ├─ Plugins/x86_64/                      原生插件：SpaceZ_2D_Alpha.dll / SpaceZ_2D_Exp_CCD.dll / EditorPlugin.dll
│  ├─ Scenes/
│  │  ├─ AlphaSampleScene.unity            Alpha 示例场景
│  │  └─ Exp_CCD/
│  │     ├─ SampleSceneCCD.unity           Exp-CCD 单球下落验证
│  │     └─ SampleSceneStacking.unity      Exp-CCD 堆叠验证
│  └─ Scripts/
│     ├─ Test.cs                           Alpha 原生加法冒烟测试（Add.NativeAdd）
│     ├─ Framework/
│     │  ├─ Runtime/                       PlayerLoop 注入、物理帧节流、RuntimeStart/End 派发
│     │  └─ Plugin/                        DLL 动态加载（NativeLoader / AutoNativeLoader）、`[NativeFunc]`、路径解析
│     ├─ Plugins/SpaceZ_2D_Alpha/           Alpha 托管层
│     │  ├─ Add.cs                         最小导出函数连通性测试
│     │  ├─ Native/                        与 C++ 逐字段对齐的封送结构体 + Handle + ColliderType
│     │  ├─ Buffer/NativeBuffer.cs         NativeList 批量缓冲
│     │  ├─ PhySystem/                     PhySystemNative（函数指针表）/ PhySystemManaged（物理帧调度）/ PhySystemFunction（委托签名）
│     │  │  └─ Component/                  Rigidbody2DAlpha / Collider2DAlpha / Box|Sphere|CapsuleCollider2DAlpha / NativeComponent / IOnPhyUpdate
│     │  └─ Sample/InitialVelocity.cs      速度注入示例
│     └─ Plugins/SpaceZ_2D_Exp_CCD/         Exp-CCD 托管层（当前主线）
│        ├─ NativeData/                    封送结构体（含 ColliderType / ForceType）
│        ├─ System/                        PhySystemNative（`[NativeFunc]` 绑定）/ PhySystemManaged（物理帧调度）/ NativeBuffer / PhyFunctionPtr
│        │  └─ Component/                  组件基类：句柄登记、销毁、归属切换与批量下发
│        ├─ Component/                     可挂载的 Rigidbody2D_Exp_CCD / Collider2D_Exp_CCD / Box|Sphere|CapsuleCollider2D_Exp_CCD
│        └─ Sample/InitialVelocity.cs      速度注入示例
│
├─ SpaceZ_2D_Alpha/                        ★ 离散内核（demo 完成，冻结维护）
│  ├─ Core/
│  │  ├─ Math/                             Vector2/3、Matrix2x2/3x3、Transform、数学工具
│  │  ├─ Object/                           Rigidbody、Collider(+Box/Sphere/Capsule)、Handle、Object、ObjectMgr
│  │  ├─ CollisionDetection/
│  │  │  ├─ BVH/                           宽相 BVH 树与节点
│  │  │  ├─ Convex/                        Convex / Box / Sphere / Capsule
│  │  │  ├─ Geometry/                      Bound、Polygon、Simplex
│  │  │  └─ NarrowPhase/                   GJK + EPA + 接触流形
│  │  ├─ Collision/                        CollisionPair、CollisionSolveReq
│  │  ├─ Solver/                           PhySolver、SolverPair、SolverRigidbody、IslandDivider、PhyQuantities
│  │  ├─ System/                           PhySystem（对外统一门面）
│  │  └─ ToString.h                        调试输出的字符串化模板
│  ├─ UnityPlugin/
│  │  ├─ Debug/                            IUnityLog + NativeDebug（日志转发到 Console）
│  │  ├─ ExportInterface/                  C ABI 导出（EngineLifeCycle / EngineObject）+ ExportInfo 结构体
│  │  ├─ UnityInterface/                   UnityPluginLoad/Unload、IUnityInterface
│  │  └─ Test.cpp                          Add 导出，用于验证 DLL 加载链路
│  ├─ Main/main.cpp                        控制台入口（Hello World 占位）
│  ├─ Tests/PhySystemTests.cpp            不需要 Unity 的集成回归测试
│  └─ tools/                               build_plugins / build_console / build_tests / clean
│
├─ SpaceZ_2D_Exp_CCD/                      ★ 连续内核（当前主线，与 Alpha 无源码/构建依赖）
│  ├─ Core/
│  │  ├─ Math/                             Vector2/3、Matrix2x2/3x3、Transform、Math.tpp
│  │  ├─ Debug/                            Debug.h/.tpp、ToString.h
│  │  ├─ Object/                           Rigidbody、Collider(+Box/Sphere/Capsule)、Handle、ObjectMgr
│  │  │  └─ Pool/                          ObjectPool / Pool / PoolPtr（求解器临时刚体的复用池）
│  │  ├─ Collision/
│  │  │  ├─ BVH/                           宽相 BVH（含扫掠包围盒更新）
│  │  │  ├─ Convex/                        Convex / Box / Sphere / Capsule
│  │  │  ├─ Geometry/                      Bound、Polygon、Simplex
│  │  │  ├─ Info/                          CollisionInfo、CollisionPair
│  │  │  └─ NarrowPhase/                   GJK + EPA + 接触流形 + ContinuousDetectReq（CCD）
│  │  ├─ Solver/                           PhySolver、SolverPair、SolverRigidbody、SolverCollisionPoint、IslandDivider、SolveReq、PhyQuantities
│  │  └─ System/                           PhySystem.h（门面声明）、PhyEngine.cpp（步进/CCD 管线）、PhyObject.cpp（对象操作）
│  ├─ UnityPlugin/
│  │  ├─ Debug/                            IUnityLog + NativeDebug
│  │  ├─ ExportInterface/                  C ABI 导出（ExportPhyEngine / ExportPhyObject）+ ExportData 结构体
│  │  └─ UnityInterface/                   UnityPluginLoad/Unload、IUnityInterface
│  ├─ Main/Main.cpp                        控制台入口（Hello World 占位）
│  ├─ Tests/SlopeTests.cpp                斜面/基本动力学专项测试（不需要 Unity）
│  └─ tools/                               build_plugins / build_plugins_release / build_console / build_tests / clean
│
└─ EditorPlugin/                           ★ 独立编辑器插件（与两个内核都无源码/构建依赖）
   ├─ EditorPluginLoad.cpp                 UnityPluginLoad/Unload + GetUnityInterface
   ├─ IUnityInterface.h                    提供给内核 DLL 的日志接口指针
   └─ tools/                               build_editor_plugin / clean
```

> 三个 C++ 工程平级且**互不耦合**：各自的 `.vscode/`、`tools/`、`.gitignore`、`.build/` 完全独立，互不转调、互不清理。
> Unity 侧两套托管层在同一工程内共存，各自加载同名 DLL，互不感知。

---

## 物理管线

Unity 侧物理帧由注入 `PlayerLoop` 的 `PhyUpdate` / `PhyLateUpdate` 驱动，节流间隔 `PhyTime.DELAY_FRAME = 0.02s`（名义 50 Hz），`dt` 取真实帧间隔。
两套内核的托管层都注册在 `PhyLateUpdate`，每物理帧：

```step
PhySystemManaged.Step()
 ├─ ComponentUpdate()    各组件把 Transform 变化写入对应缓冲
 ├─ SynDirtyData()       缓冲 flush 到原生侧（质量/阻尼/速度/材质/变换/形状/力）
 ├─ Step(dt)             → 原生内核步进
 └─ GetDirtyNativeBodies() 读回脏刚体状态并写回 Transform
```

### Alpha：离散步进

```step
PhySystem::Step(dt)
 ├─ ClearDirtyBodies()
 ├─ ApplyForce / ApplyTorque    消费一次外部力（dt 全量）
 └─ 循环 subStep 次（默认 4）：
     SubStep(dt / subStepCount)
      ├─ AddGravity → ApplyForce → ApplyTorque   转为速度增量并清空累积量
      ├─ CollisionStep     BVH 同步 → 宽相 → GJK/EPA 窄相 → 接触流形
      ├─ SolveStep         岛划分 → 迭代冲量 → 位置修正 → 回写刚体并同步子碰撞体
      └─ TransformStep     积分位置/角度，阻尼衰减，同步子碰撞体
```

### Exp-CCD：子步内时间推进

```step
PhySystem::Step(dt)
 ├─ ClearDirtyBodies()
 └─ 循环 subStep 次（默认 6）：
     SubStep(dt / subStepCount)
      while 剩余时间 > 0：
       ├─ SynBroadPhase(剩余)     按 v·dt / ω·dt 扩张 AABB（扫掠包围盒）
       ├─ CollisionStep(剩余)     CCD 求各碰撞对 TOI → 取最早的一个 → 时间截断到该 TOI，
       │                          返回在该时刻需要求解的碰撞对
       ├─ BodyStep(截断后时间)    加力/重力 → 积分速度
       ├─ TransformStep(同)       阻尼衰减 → 积分位姿 → 同步子碰撞体
       ├─ SolveStep(碰撞对)       岛划分 → 迭代冲量 → 位置修正 → 回写刚体
       └─ 若本次推进小于 step * MIN_CCD_STEP：把剩余时间积分掉并 break
```

### 可调参数

| 参数 | Alpha | Exp-CCD | 位置 |
| --- | --- | --- | --- |
| 子步数 | 4 | 6 | `PhySystemManaged.SUB_STEP_COUNT` |
| 求解迭代次数 | 8 | 8 | `PhySystemManaged` 的 `ITERATION`（Alpha）/ `SOLVER_ITERATION`（Exp-CCD） |
| 重力加速度 | 9.8（沿 `Vector2.down`） | 9.8 | `PhyQuantities::GRAVITY_ACCELERATION` |
| 位置修正松弛系数 | 0.2 | 0.2 | `PhyQuantities::RELAXATION` |
| 单次最大位置修正 | 0.2 | 0.2 | `PhyQuantities::MAX_POS_CORRECTION` |
| 允许穿透 slop | 0.005 | 0.005 | `PhyQuantities::PENETRATE_SLOP` |
| GJK/EPA 迭代上限 | 36 | 36 | `NarrowPhase::MAX_ITERATION` |
| CCD 最小推进比例 | — | 0.01 | `PhySystem::MIN_CCD_STEP` |
| 数学容差 `Epsilon` | 1e-5 | 1e-5 | `Math::Epsilon` |

---

## 快速开始

### 环境要求

- Windows x64
- Unity **2022.3.62f3c1**（依赖 `com.unity.collections` 2.5.7 提供 `NativeList`）
- Visual Studio 2017+，含「使用 C++ 的桌面开发」工作负载（构建脚本用 `vswhere` 自动定位 MSVC）

### 构建原生库

仓库里 `Assets/Plugins/x86_64/*.dll` 已入库，只改 C# 时不需要构建；**改动 C++ 后必须重建并提交 DLL**。

在 VS Code 中打开对应 C++ 工程目录作为工作区，`Ctrl+Shift+B` 选择任务；或直接运行脚本：

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `SpaceZ_2D_Alpha/tools/build_plugins.bat` | Core + UnityPlugin → 单个 DLL | `Assets/Plugins/x86_64/SpaceZ_2D_Alpha.dll` |
| `SpaceZ_2D_Alpha/tools/build_console.bat` | Core + Main → 控制台程序 | `SpaceZ_2D_Alpha/Main/Debug/SpaceZ_2D_Alpha.exe` |
| `SpaceZ_2D_Alpha/tools/build_tests.bat` | Core + Tests → 控制台测试 | `SpaceZ_2D_Alpha/Tests/Debug/SpaceZ_SolverTests.exe` |
| `SpaceZ_2D_Alpha/tools/clean.bat` | 清理本工程产物 | 删除其 `.build/`、`Main/Debug`、`Tests/Debug`、上述 DLL 及 `.meta` |
| `SpaceZ_2D_Exp_CCD/tools/build_plugins.bat` | Core + UnityPlugin → 单个 DLL（Debug） | `Assets/Plugins/x86_64/SpaceZ_2D_Exp_CCD.dll` |
| `SpaceZ_2D_Exp_CCD/tools/build_plugins_release.bat` | 同上，`/O2 /GL` + link `/LTCG`（Release） | **同一个** `Assets/Plugins/x86_64/SpaceZ_2D_Exp_CCD.dll`（与 Debug 互相覆盖） |
| `SpaceZ_2D_Exp_CCD/tools/build_console.bat` | Core + Main → 控制台程序 | `SpaceZ_2D_Exp_CCD/Main/Debug/SpaceZ_2D_Exp_CCD.exe` |
| `SpaceZ_2D_Exp_CCD/tools/build_tests.bat` | Core + Tests → 控制台测试 | `SpaceZ_2D_Exp_CCD/Tests/Debug/SpaceZ_CcdTests.exe` |
| `SpaceZ_2D_Exp_CCD/tools/clean.bat` | 清理本工程产物 | 删除其 `.build/`、`Main/Debug`、`Tests/Debug`、上述 DLL 及 `.meta` |
| `EditorPlugin/tools/build_editor_plugin.bat` | EditorPlugin 独立编译 | `Assets/Plugins/x86_64/EditorPlugin.dll` |
| `EditorPlugin/tools/clean.bat` | 清理 EditorPlugin 产物 | 删除其 `.build/` 与 `EditorPlugin.dll` |

编译配置：

- Alpha：`Debug | x64 | /std:c++20 /utf-8 /EHsc /W4 /Zi /Od /RTC1 /MDd`
- Exp-CCD Debug：`/std:c++20 /utf-8 /W4 /Od /RTC1 /MDd`
- Exp-CCD Release：`/std:c++20 /utf-8 /W4 /O2 /GL /Gy /MD /DNDEBUG`，链接端 `/LTCG /OPT:REF /OPT:ICF`

中间文件进各自 `.build/obj_*`（Debug 与 Release 的 obj 必须分目录，否则 `/MDd` 与 `/MD` 混链会报 `LNK2038`），
调试符号 `.pdb` 也留在 `.build/`，不污染 Unity 资产目录。Exp-CCD 的 Debug/Release **共用同一个 DLL 输出路径**，
以保住 Unity 侧 `.meta` 与 GUID；代价是同一时刻目录里只能存在一种配置，因此「构建全部」不含 Release 任务。

> **DLL 被锁**：Unity 加载过原生 DLL 后会持有文件句柄，链接会失败。脚本会在链接前预删旧 DLL 并提示；
> 看到 `is locked, probably loaded by Unity Editor` 时关闭 Unity 再构建。

### 在 Unity 中运行

`RuntimeManager` 通过 `[RuntimeInitializeOnLoadMethod(BeforeSceneLoad)]` 扫描 `[RuntimeStart]` 方法并**按 Order 升序**执行：

| Order | 方法 | 作用 |
| --- | --- | --- |
| -100 | `UpdateManager.Init` | 把 `PhyLoop`（含 `PhyUpdate` / `PhyLateUpdate`）插入 `PlayerLoop` 的 `PreUpdate` 之前 |
| -100 | `PhySystem` 原生初始化（Alpha `Init` / Exp-CCD `InitNative`，各一份） | 加载同名原生 DLL，绑定全部导出函数 |
| -90 | `PhySystem.InitManaged`（Alpha / Exp-CCD 各一份） | `StartEngine(...)`、初始化缓冲、注册物理帧回调 |

退出时按 `[RuntimeEnd]` 同样升序执行：`Order 90` 停引擎并释放缓冲 → `Order 100` 卸载 DLL 并还原 `PlayerLoop`。
（`RuntimeManager` 对 `RuntimeStart` / `RuntimeEnd` 都是升序派发，靠 Order 的正负和大小决定先后。）

---

## 使用示例

在 GameObject 上挂 `Rigidbody2D_Exp_CCD`（Alpha 用 `Rigidbody2DAlpha`），在其自身或子物体上挂碰撞体组件即可，无需手动创建原生对象：

```csharp
using SpaceZ_2D.Exp.CCD;
using UnityEngine;

public class Launch : MonoBehaviour
{
    [SerializeField] private Rigidbody2D_Exp_CCD _body;

    private void Start()
    {
        // 属性写入先进缓冲，下一次物理帧统一同步到原生侧
        _body.LinearVelocity = new Vector2(3f, 0f);
        _body.AngularVelocity = 0.5f;

        _body.AddForce(new Vector2(0f, 10f));                 // 默认 Impulse：立即折算成速度增量
        _body.AddTorque(1f, ForceType.Sustain);               // Sustain：本子步内持续作用，步进后清零
    }
}
```

要点：

- **静态碰撞体**：碰撞体找不到父级刚体时以 `Handle.NULL` 创建，原生侧归入静态分支（`invMass = 0`），只参与碰撞不被推动。
- **一个刚体可挂多个碰撞体**：碰撞体的局部变换按「刚体世界变换的逆 × 自身世界变换」解算，父子变换变化会自动同步；换父级时 `OnTransformParentChanged` 会重新绑定刚体。
- **角度单位为弧度**：组件内部在 `eulerAngles.z * Deg2Rad` 与 `rotation * Rad2Deg` 之间往返。
- **矩形 `Extents` 是半长**；**胶囊 `Height` 是两端半圆圆心的间距**（总高 = `Height + 2 * Radius`）。
- **回声抑制**：刚体与碰撞体都缓存上次下发的变换，值未变化时不下发；原生回写位置后会记录 Unity 归一化后的角度，避免 `-π` 与 `π` 的表示差异触发回环。
- 原生对象在生命周期回调里创建：刚体与碰撞体都在 `OnEnable` 创建并在创建后立即注入一次初始属性；运行时改属性只发增量。

---

## 原生 ABI 契约

两侧结构体必须**逐字段、逐顺序**一致（4 字节对齐）：

- Alpha：`Assets/Scripts/Plugins/SpaceZ_2D_Alpha/Native/*.cs` ↔ `SpaceZ_2D_Alpha/UnityPlugin/ExportInterface/ExportInfo/*.h`
- Exp-CCD：`Assets/Scripts/Plugins/SpaceZ_2D_Exp_CCD/NativeData/*.cs` ↔ `SpaceZ_2D_Exp_CCD/UnityPlugin/ExportInterface/ExportData/*.h`

### Alpha

| 类型 | 大小 | 字段偏移 |
| --- | --- | --- |
| `Handle` | 8 | `id@0`, `generation@4`（`null = {-1, 0}`） |
| `CollisionPair` | 52 | `firstCollider@0`, `firstBody@8`, `secondCollider@16`, `secondBody@24`, `depth@32`, `point@36`, `normal@44` |
| `BodyState` | 32 | `handle@0`, `rotation@8`, `angularVelocity@12`, `position@16`, `linearVelocity@24` |
| `BodyMassInfo` | 16 | `handle@0`, `mass@8`, `inertia@12` |
| `ColliderMaterialInfo` | 20 | `handle@0`, `staticFriction@8`, `dynamicFriction@12`, `elasticity@16` |
| `BodyTransformInfo` | 28 | `handle@0`, `rotation@8`, `position@12`, `scale@20` |

### Exp-CCD

载荷统一是 `ExportObjInfo<{Rigidbody|Collider}, T>`（= `Handle` + `T`），C# 侧按 `Handle` 后紧跟载荷展平成一个结构体。

| 类型 | 大小 | 字段偏移 |
| --- | --- | --- |
| `Handle` | 8 | `id@0`, `generation@4`（`null = {-1, 0}`） |
| `ExportBodyInfo` | 32 | `handle@0`, `position@8`, `rotation@16`, `linearVelocity@20`, `angularVelocity@28` |
| `BodyProperty` | 20 | `handle@0`, `mass@8`, `inertia@12`, `useGravity@16`（`bool` 为 1 字节，需 `MarshalAs(U1)`） |
| `BodyDrag` | 16 | `handle@0`, `linearDrag@8`, `angularDrag@12` |
| `BodyVelocity` | 20 | `handle@0`, `linearVelocity@8`, `angularVelocity@16` |
| `BodyForce` | 20 | `handle@0`, `force@8`, `type@16`（`ForceType`） |
| `BodyTorque` | 16 | `handle@0`, `torque@8`, `type@12`（`ForceType`） |
| `ColliderProperty` | 20 | `handle@0`, `staticFriction@8`, `dynamicFriction@12`, `elasticity@16` |
| `ExportTransform` | 28 | `handle@0`, `position@8`, `rotation@16`, `scale@20` |
| `BoxExtents` | 16 | `handle@0`, `extents@8` |
| `SphereRadius` | 12 | `handle@0`, `radius@8` |
| `CapsuleSize` | 16 | `handle@0`, `radius@8`, `height@12` |

### 约定

- C# 结构体一律 `[StructLayout(LayoutKind.Sequential)]`；**输出方向**（`BodyState` / `ExportBodyInfo` / `CollisionPair`）尤其不能多字段少字段。
- 批量接口签名为 `(T* infos, int count)`，`infos` 指向托管 `NativeList` 的非托管指针，**仅在调用期间有效**，原生侧不可缓存。
- 回读接口的 `count` 是**入参容量 + 出参实数量**（`int&` → C# `ref int`）：调用方按容量分配，原生侧回写实际写入数；
  引擎不可用时 `GetDirtyBodyCount` 返回 `-1`（C# 侧夹到 0 后跳过），指针为空或 `count <= 0` 时回读函数直接返回、不改写 `count`。
- 所有角度、角速度均为**弧度**（内核全程 `Cos/Sin` 直算，无 `Deg2Rad` 转换）。
- 碰撞体类型枚举：`Box = 0`、`Sphere = 1`、`Capsule = 2`；施力类型枚举：`Impulse = 0`、`Sustain = 1`。
- 碰撞法线方向为 `first → second`，`point` 为**世界空间**接触点。

### 导出函数

均在各自 `UnityPlugin/ExportInterface/` 下，`extern "C" __declspec(dllexport)`：

- **Alpha** — `EngineLifeCycle.cpp`：`StartEngine` / `StopEngine` / `EngineStep` / `GetCollisionCount` / `GetCollisionPairs` / `GetDirtyBodyCount` / `GetDirtyBody`
- **Alpha** — `EngineObject.cpp`：`CreateBody` / `CreateCollider` / `DestroyBody` / `DestroyCollider` / `SetColliderBody` / `SetBodies` / `SetBodiesDrag` / `SetBodiesVelocity` / `SetColliders` / `SetBodiesTransform` / `SetCollidersTransform` / `SetBoxColliders` / `SetSphereColliders` / `SetCapsuleColliders` / `AddForce` / `AddTorque`
- **Exp-CCD** — `ExportPhyEngine.cpp`：`StartEngine` / `StopEngine` / `Step` / `GetDirtyBodyCount` / `GetDirtyBodies`
- **Exp-CCD** — `ExportPhyObject.cpp`：`CreateBody` / `CreateCollider` / `DestroyBody` / `DestroyCollider` / `SetColliderRigidbody` / `SetBodiesProperty` / `SetBodiesDrag` / `SetBodiesVelocity` / `SetCollidersProperty` / `SetBodiesTransform` / `SetCollidersTransform` / `SetBoxColliders` / `SetSphereColliders` / `SetCapsuleColliders` / `AddForces` / `AddTorques`

所有导出都包在 `TRY_CATCH_DEBUG` 内，异常转 `Debug::LogError`（编辑器内经 `IUnityLog` 转发到 Console），不向 Unity 抛。
`EditorPlugin.dll` 负责把 `IUnityInterface` 指针交给内核 DLL：`NativeLoader` 加载内核前先向它取指针，取不到会抛异常、插件加载整体失败，
因此编辑器下 `EditorPlugin.dll` 必须在位。

---

## 已知限制

### 通用

- **平台**：只支持 Windows x64，且构建产物为 Debug（Exp-CCD 可由 Release 脚本覆盖）。`PluginUtility` 只在 `UNITY_EDITOR_64` 下解析 DLL 路径，Player 构建走静态 `DllImport` 分支，打包链路尚未实测。
- **无触发器 / 碰撞层**：内核没有 trigger、layer、collision mask 概念，所有碰撞体两两检测。
- **无关节 / 约束**：只有接触约束。
- **退化情形**：GJK/EPA 迭代上限用尽时与「真分离」共用 `return false`，极端配置下可能漏检；穿透深度小于 `Epsilon`(1e-5) 一律判分离，恰相切不产生接触。
- `Main/main.cpp` 仍是 Hello World 占位，控制台目标目前只用于跑 Core。
- `Tests/` 是自写控制台用例，未接入 Unity Test Framework。

### Alpha

- **堆叠稳定**：受限于接触流形（多接触点）的顺序耦合，会每帧产生微小正冲量矩，碰撞盒堆叠尤为明显。Alpha 不做此项优化，改在 Exp-CCD 中处理。
- **无 CCD**：高速物体的隧穿不处理。

### Exp-CCD

- **相切语义**：碰撞判据在恰好相切时受支撑点顺序影响，镜像速度可能给出相反的「是否碰撞」结论。
- **CCD 开销不均**：在近乎持续接触的配置下推进次数明显增多，单帧耗时远高于自由飞行（实测同批用例可差 1~2 个数量级）。
- **`Simplex::SeparateDis` 函数**：该值被当作保守下界使用时，个别配置下命中会略微晚报（实测约 4% 的用例延后、最大十余毫秒）。
- **极小TOI（撞击时间）**：子步内对于极小TOI（ <= subStep * MIN_CCD_STEP）会直接推进完剩余步长以避免死循环风险，可能造成微小穿透，可通过增加子步数量缓解

---

## 开发计划

- **Alpha**：全量离散碰撞检测物理引擎；闭环物理引擎核心循环，验证物理求解与引擎核心架构。（已完成，冻结）
- **Exp-CCD**：全量连续碰撞检测物理引擎；在 Alpha 暴露的求解架构问题上优化，验证连续碰撞检测实现方案。（已完成，冻结）
- **Beta**：采用离散为主、连续为辅的物理引擎；引入碰撞层剔除、空间查询等通用 API，实现最小可用版本。（进行中）
