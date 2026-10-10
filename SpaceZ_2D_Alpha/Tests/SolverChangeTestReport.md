# 位置修正（ApplyPositionCorrection）测试报告

- **日期**：2026-10-10
- **被测对象**：工作区未提交改动（本轮新增「位置修正」）
- **产物**：`Tests/Debug/SpaceZ_SolverTests.exe` + `.pdb`
- **构建**：复刻 `tools/build_tests.bat` 的命令行（cl.exe 直调，Debug x64，`/W4 /MDd /RTC1`）
- **测试源**：`Tests/SolverTests.cpp`（工作区中曾被删除，本次按新 API 重建并扩展 E 节）

---

## 一、结论

| 项 | 结果 |
|---|---|
| 编译（Core 9 个 TU + Tests） | **0 error / 0 warning**（`/W4`） |
| 运行 | **通过 49 / 失败 0 / 已知未修复 2**，exit 0 |
| 回归 | **无**。A（冲量/摩擦）、B（集成/对象池）、C（分岛）三个既有节全绿 |
| 本轮新功能 | **位置修正全部断言通过**（E1 单元级 13 条 + E2 集成 2 条） |
| 测试区分度 | 已用变异验证（见第四节）：移除调用后 E2 立即变红 |

---

## 二、本轮改动

| 文件 | 改动 |
|---|---|
| `Core/Collision/CollisionPair.h` | 新增 `firstAnchorPoint` / `secondAnchorPoint`（刚体本地坐标系锚点）与 `SetCollisionPoint()` |
| `Core/Solver/SolverPair.h/.cpp` | 构造函数改为 3 参数（带锚点对）；新增成员初始化默认值；新增 `ApplyPositionCorrection()` |
| `Core/Solver/SolverRigidbody.h/.cpp` | 新增 `ApplayPositionImpulse()` / `ApplayAngleImpulse()` / `PointLocalToWorld()` |
| `Core/Solver/PhySolver.cpp` | `CreateSolverPairs` 传入锚点；`ImpulseIteration` 在冲量迭代后追加 `_iteration` 轮位置修正 |
| `Core/Solver/PhyQuantities.h`（新增） | `RELAXATION=0.2`、`MAX_POS_CORRECTION=0.2`、`PENETRATE_SLOP=0.005` |

位置修正算式（`SolverPair::ApplyPositionCorrection`）：

```
worldA = first.PointLocalToWorld(firstAnchorPoint)
worldB = second.PointLocalToWorld(secondAnchorPoint)
depth  = Dot(worldA - worldB, normal)                    // >0 表示重叠
d      = Clamp(depth - PENETRATE_SLOP, 0, MAX_POS_CORRECTION)
p      = RELAXATION * d / invNormalEffMass * normal
first.position += invMass_a * (-p) ;  second.position += invMass_b * (+p)
angle += invInertia * Cross(r, ∓p)
```

---

## 三、E 节覆盖（本轮新功能的回归位）

单元级统一场景：动体 `m=1, I=1` 置于原点、静态体、`normal=(1,0)`、接触点取原点
⇒ `invNormalEffMass = 1` ⇒ `p = 0.2 × d`，每条断言都先算解析解再比对。

| 断言 | 解析解 | 实测 |
|---|---|---|
| `depth=0.105` 推开量 | `0.2×(0.105-0.005) = 0.02` | PASS |
| 切向不动 / 锚点在轴上无力矩 | `0` | PASS |
| 静态体不动（`invMass=0`） | `0` | PASS |
| `depth=0.003 < slop` | 完全不动 | PASS |
| `depth=1.005` 被上限截断 | `0.2×0.2 = 0.04` | PASS |
| 已分离（`depth=-1`） | 不被误修正 | PASS |
| 锚点偏离连线 `rA=(0.605,0.5)` | `angle += 0.5×0.02 = 0.01` | PASS |
| 60 轮弛豫单调收敛 | `depth → 0.005` | PASS |
| 极大穿透 200 轮 | 保持有限值 | PASS |

集成级：`SolveStep` 下 `_iteration=4` 轮弛豫，`e = 0.1×0.8⁴ = 0.04096`
⇒ 求解体 `x = -0.05904`，与实测逐位吻合。

---

## 四、测试有效性验证（变异）

在**临时副本**（不动项目源码）中把 `PhySolver` 里的位置修正调用替换为 `(void)p;`，
其余不变，重编重跑：

| 版本 | 通过 | 失败 | exit |
|---|---:|---:|---:|
| 原样 | 49 | 0 | 0 |
| 移除位置修正调用 | 48 | **1** | **1** |

7 条失败精确落在 `E2 SolveStep 4 轮位置修正` 这一条上，其余节全绿 —— 证明该断言
由位置修正单独决定，而非空转。（E1 为单元级直调，不经 `PhySolver`，故不受影响，符合预期。）

---

## 五、遗留与观察（均非本次改动引入）

1. **[KNOWN] 结果未回写 `Rigidbody`**：`SolveStep` 仍只写 `SolverRigidbody` 内部状态，
   速度与位置都不回写原对象。位置修正做完了但游戏世界看不到 —— 属里程碑，非缺陷。
2. **锚点未填充时会退化**：`CollisionPair` 的锚点默认 `(0,0)`，若碰撞检测侧尚未调用
   `SetCollisionPoint`，位置修正会以「质心之差」当作穿透深度，可能产生无效推开。
   当前测试是手工填入锚点才触发修正路径。
3. `invNormalEffMass` 由 `SetRigidbody` 按 `point` 预算，而修正量按锚点世界坐标计算；
   两者一致时无偏差，前提同上（锚点须真实填充）。
4. 拼写：`ApplayPositionImpulse` / `ApplayAngleImpulse` 疑为 `Apply*` 笔误。
5. `SolverRigidbody::PointLocalToWorld` 不修改对象，建议加 `const`。
