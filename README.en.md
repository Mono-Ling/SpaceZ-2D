# SpaceZ-2D

[简体中文](README.md) | English

A self-built 2D physics engine: **C++20 core + Unity presentation layer**.

The core handles broad-phase BVH, narrow-phase GJK/EPA, contact manifolds, optional continuous collision detection and sequential-impulse solving. It is consumed by Unity as an x64 native plugin (`.dll`) over a C ABI; the Unity side only does component wrapping and allocation-free data marshalling.

The current mainline is **`SpaceZ_2D_Beta`**: it keeps the discrete-solver skeleton, narrows continuous detection down to a per-collider opt-in, and adds collision layers, spatial queries, a custom memory manager and Profiler integration. This README focuses on Beta; the other two cores are summarized as historical versions.

## Projects

| Project | Role | Status |
| --- | --- | --- |
| `SpaceZ_2D_Beta` | **Current mainline**: discrete-first plus per-collider continuous detection, collision layers, spatial queries, custom memory management, Profiler | In progress |
| `SpaceZ_2D_Exp_CCD` | Full continuous detection (conservative-advancement CCD), used to validate the CCD approach and stacking stability | Done, frozen |
| `SpaceZ_2D_Alpha` | First discrete core, closed the physics-solving loop end to end | Demo done, frozen |
| `EditorPlugin` | Small standalone plugin that hands an `IUnityInterface` pointer to the core DLLs | Tooling |

The three C++ projects sit next to `Assets/` and are **fully decoupled**: each has its own `.vscode/`, `Tools/`, `.gitignore` and `.build/`, they never call or clean each other. On the Unity side each core has its own managed layer, loading a DLL with the same name, unaware of the others.

| Item | Details |
| --- | --- |
| Core toolchain | C++20 / MSVC, `/std:c++20 /utf-8 /W4`, x64, Debug and Release configurations |
| Host runtime | Unity 2022.3.62f3c1 (Windows x64 editor) |
| Solver model | Sequential impulse + island division + position correction |
| Shapes | Line, Box, Sphere, Capsule (Line is new in Beta; the other two cores only have the latter three) |

### Version history

- **Alpha**: proved the path "broad phase → narrow phase → manifold → island division → sequential impulse" works; the bottleneck was that ordering coupling between contact points makes box stacks drift continuously.
- **Exp-CCD**: switched to full conservative-advancement CCD, which fixed both tunnelling and stacking, at the cost of a per-frame cost far above free flight while in sustained contact.
- **Beta**: splits the difference — ordinary frames run discrete, and only colliders flagged `Continuous` get sweeping plus TOI advancement. Meanwhile it adds the general-purpose capabilities (layers, spatial queries, memory, Profiler) that the first two versions never had.

---

## Mainline: SpaceZ_2D_Beta

### Features

#### Collision detection

- Broad phase: a dynamically balanced BVH. Insertion walks down the tree picking the sibling by "smallest area increase + height cost (`HEIGHT_COST_WEIGHT = 0.005`)", and supports incremental update/removal by handle. Nodes store `ColliderHandle / RigidbodyHandle / LayerMask / Bound` directly.
- Layer filtering happens in the broad phase: node comparison tests the layer masks first, so non-matching pairs never reach the narrow phase and never produce solve requests.
- Narrow phase: GJK for closest points / intersection, EPA for penetration depth and normal (`MAX_ITERATION = 36`), then `ContactManifold` expands a single EPA result into a multi-point manifold.
- Support functions all go through `Support(dir, local2world, world2local)`: the world direction is mapped into local space with `world2local` to find the support point, then that point is mapped back to world space with `local2world`. Rotation and scale are carried by the transform matrices, so shapes always stay in local space.
- Pairs of adjacent support points go through `Edge(normal, local2world)`: the normal is mapped into local space with the transpose of `local2world`, and both endpoints are mapped back with `local2world`.

#### Mixed collision modes (discrete + optional continuous)

- Every collider carries a `CollisionMode::Discrete | Continuous` flag — configured per collider, not a global switch.
- `SynContinuousBroadPhase` expands the AABB of continuous colliders by `v · dt` and `ω · dt` (`SweepBound`) so the bound covers the whole motion of the step; discrete bodies are only rebuilt when dirty.
- `ContinueCollision` repeatedly advances the relative pose of two convex bodies using relative linear/angular velocity until GJK reports intersection, then returns the TOI and contact data of that hit.
- `ContinuousCollisionStep` collects every TOI within the step, takes the **earliest** one, truncates the remaining time to it and solves only the pairs inside that window; the leftover time keeps iterating. So a single substep can resolve several sequential impacts.
- Advancement floor `time * MIN_CCD_STEP`: a TOI below it does not open a new window and is simply folded into the current advance, which avoids a TOI that is always 0 causing an endless loop; `MAX_CCD_ITERATION` bounds the whole loop.
- `Continuous` only takes effect when the collider is attached to a rigid body — a static collider silently degrades to `Discrete`.

#### Physics solving

- Island division: a union-find splits collision pairs into independent connected components, each solved separately.
- Per contact point: normal impulse (target relative velocity derived from restitution) plus tangent impulse (static-friction clamp, switching to dynamic friction once exceeded); after the iterations, position correction (`RELAXATION` + `PENETRATE_SLOP` tolerance + `MAX_POS_CORRECTION` cap).
- `Precompute()` is called once outside the iteration loop, so restitution does not decay with iteration count.
- Material merging: static friction, dynamic friction and restitution of a pair take the smaller of the two, and restitution is clamped to `[0, 1]`.
- Integration: semi-implicit Euler for position, exponential damping `v *= exp(-drag * dt)`; forces come in `Impulse` (immediately converted to a velocity delta) and `Sustain` (accumulated, scaled by `dt` during the step, cleared afterwards).

#### Collision events

- `CollisionPairBuffer` diffs against the previous frame's visible set to produce **Enter / Stay / Exit** tables: Enter and Stay carry contact info lists, Exit carries only the object pair.
- Each frame starts with `ResetVisible()` and ends with `DiffExitPairs()`, so Exit strictly means "present last frame, absent this frame".
- The native side exports all three (`EnterCollisionPairs` / `StayCollisionPairs` / `ExitPairs`); the managed layer does not consume them yet.

#### Spatial queries

- Overlap queries: `OverlapBox` / `OverlapSphere` / `OverlapCapsule`. They first compute the query shape's own bound and run BVH `BoundOverlap`, then confirm with GJK and return the hit collider handles.
- Ray queries: `Raycast` returns the nearest hit, `RaycastAll` returns every hit sorted by distance. The broad phase pre-filters with `Ray` vs. AABB, and the narrow phase uses a "segment vs. convex" GJK variant for the exact distance (`RAY_CAST_MAX_ITERATION = 72`).
- Both take a `LayerMask` and `maxDis`; the direction is normalized internally, and a zero vector counts as a miss.

#### Interface and engineering

- Handles are `{int32 id, uint32 generation}`; deletion bumps the generation, so stale handles are detected instead of becoming dangling pointers.
- Memory: the core replaces the global `operator new/delete` with a paged allocator — 1 MiB pages plus an AVL tree of free pages (address lookup, adjacent pages merged automatically), `align_val_t` over-aligned allocation support, and a live-address set in Debug builds that validates frees. Pages themselves come from a named allocator on Unity's `IUnityMemoryManager` (`SpaceZ-2D-Beta Native`), so every native allocation shows up in Unity's memory report.
- Profiler: six markers — `Step`, `OverlapBox/Sphere/Capsule`, `Raycast/RaycastAll` — all under the `Physics2D` category. During static initialization they only register a callback and are created for real once `UnityPluginLoad` hands over `IUnityProfiler`.
- Managed-side batching: property writes go into `NativeList` buffers (allocated on the native heap) and are flushed once per physics frame through batch entry points; results come back as native `NativeArray`s that the managed layer consumes in place and returns immediately.
- In the editor it uses `LoadLibrary` plus reflection binding (`[NativeFunc]` + `AutoNativeLoader`), so rebuilding the DLL hot-reloads it; the non-editor branch uses static `DllImport`.

### Physics pipeline

The Unity-side physics frame is driven by `PhyUpdate` / `PhyLateUpdate` injected into `PlayerLoop`, throttled at `PhyTime.DELAY_FRAME = 0.02s` (nominally 50 Hz), with `dt` taken from the real frame interval. The managed layer registers on `PhyLateUpdate`, so every physics frame:

```step
PhySystemManaged.Step()
 ├─ ComponentUpdate()        components write Transform / property changes into their buffers
 ├─ SyncDirtyData()          flush all 12 NativeLists to the native batch entry points
 ├─ Step(dt)                 → native core step
 └─ ApplyDirtyBodies()       read back dirty bodies (position/rotation/velocity), apply, free buffer
```

Native side:

```step
PhySystem::Step(dt)
 ├─ clear the dirty-body set, reset the collision-pair visible set
 ├─ return early when dt <= 0
 └─ substep loop (dt / SUB_STEP_COUNT):
     SubStep(step)
      while remaining time > 0 and iterations < MAX_CCD_ITERATION:
       ├─ ContinuousCollisionStep(remaining)  sync dirty colliders → discrete/continuous broad phase
       │                                      → TOI for pairs with a Continuous side
       │                                      → truncate remaining time to the earliest TOI,
       │                                        return the pairs to solve at that instant
       ├─ TransformStep(toi)                 damping + pose integration, sync owned colliders
       ├─ BodyStep(toi)                      gravity / external forces → velocity delta, clear accumulators
       ├─ SolveStep(pairs)                   island division → impulse iterations → position correction
       │                                     → write back bodies and child colliders
       └─ remaining time -= toi
      leftover time (only when the iteration cap bailed out): TransformStep + BodyStep integrate it
      discrete tail:
       ├─ DiscreteCollisionStep()            sync broad phase → GJK / EPA → contact manifold
       └─ SolveStep(discrete pairs)
```

### Tunables

| Parameter | Value | Location |
| --- | --- | --- |
| Substep count | 6 | `PhySystemManaged.SUB_STEP_COUNT` |
| Solver iteration count | 8 | `PhySystemManaged.SOLVER_ITERATION` |
| CCD minimum advancement ratio | 0.001 | `PhySystem::MIN_CCD_STEP` |
| CCD iteration cap per substep | 36 | `PhySystem::MAX_CCD_ITERATION` |
| Gravity | 9.8 (along `Vector2.down`) | `PhyQuantities::GRAVITY_ACCELERATION` |
| Position-correction relaxation | 0.2 | `PhyQuantities::RELAXATION` |
| Max position correction per pass | 0.2 | `PhyQuantities::MAX_POS_CORRECTION` |
| Penetration slop | 0.005 | `PhyQuantities::PENETRATE_SLOP` |
| GJK / EPA iteration cap | 36 | `NarrowPhase::MAX_ITERATION` |
| Ray narrow-phase iteration cap | 72 | `NarrowPhase::RAY_CAST_MAX_ITERATION` |
| Math tolerance `Epsilon` | 1e-5 | `Math::Epsilon` |

---

## Repository layout

```file
SpaceZ-2D/
├─ Assets/
│  ├─ Plugins/x86_64/                      native plugins: SpaceZ_2D_Beta.dll (mainline) / Alpha / Exp_CCD / EditorPlugin.dll
│  ├─ Scenes/Beta/SampleSceneStacking.unity Beta stacking validation scene
│  └─ Scripts/
│     ├─ Framework/Runtime/                PlayerLoop injection, physics-frame throttle, RuntimeStart/End dispatch
│     ├─ Framework/Plugin/                 dynamic DLL loading (NativeLoader / AutoNativeLoader), [NativeFunc], path resolution
│     └─ Plugins/SpaceZ_2D_Beta/           Beta managed layer
│        ├─ System/PhySystemNative.cs      exported symbol ↔ C# signature mapping (reflection in editor / DllImport elsewhere)
│        ├─ System/PhySystemManaged.cs     frame loop, component registry, dirty-data batching and write-back
│        ├─ System/PhySystemQuery.cs       managed exit for spatial queries (NativeArray → List / out params)
│        ├─ System/Buffer/                 NativeArray / NativeList (contiguous buffers on the native heap)
│        ├─ System/NativeData/             marshalling structs aligned field-by-field with the native side
│        ├─ System/Component/              component base classes: handle registry, ownership switching, batched dispatch
│        ├─ Component/                     attachable components: Rigidbody2D_Z / Box|Sphere|Capsule|LineCollider2D_Z
│        └─ Sample/InitailVelocity.cs      velocity injection sample
│
├─ SpaceZ_2D_Beta/                         ★ current mainline core
│  ├─ Core/
│  │  ├─ Math/                             Vector2/3, Matrix2x2/3x3, Math.tpp
│  │  ├─ Debug/                            Debug, DebugFormat, ToString
│  │  ├─ Memory/                           paged memory management (replaces global operator new)
│  │  ├─ Object/                           Rigidbody, Collider(+Line/Box/Sphere/Capsule), Transform, Handle
│  │  │  └─ Container/                     ObjectMgr, ObjectPool, UnionFindSet, ObjectBuffer
│  │  ├─ Collision/
│  │  │  ├─ BroadPhase/                    BVH tree, nodes, queries
│  │  │  ├─ Convex/                        Convex / Line / Box / Sphere / Capsule
│  │  │  ├─ Geometry/                      Bound, Polygon, Simplex, Ray
│  │  │  ├─ Info/                          ConvexDetectReq, ContinueDetectReq, CollisionInfo, RayHit
│  │  │  ├─ Layer/                         LayerMask (64-bit mask)
│  │  │  └─ NarrowPhase/                   GJK / EPA / contact manifold / continuous detection / ray GJK
│  │  ├─ Solver/                           PhySolver, SolverPair, SolverRigidbody, SolverCollisionPoint, IslandDivider
│  │  └─ System/                           PhySystem (facade) + PhyEngine / PhyObject / SpaceQuery + CollisionPairBuffer
│  ├─ UnityPlugin/
│  │  ├─ ExportData/                       cross-boundary structs (Handle / ObjInfo / ObjProperty / BodyState / CollisionPair / RaycastHit / NativeArray)
│  │  ├─ ExportInterface/                  C ABI: PhyEngine / PhyObject / PhyQuery / NativeBuffer
│  │  ├─ Memory/                           hooks IUnityMemoryManager so page allocations are tracked by Unity
│  │  ├─ Profiler/                         hooks IUnityProfiler, Physics2D-category markers
│  │  ├─ Debug/                            IUnityLog forwarding
│  │  └─ UnityPlugin/                      UnityPluginLoad / Unload + multicast load callbacks
│  ├─ Main/Main.cpp                        console entry (placeholder)
│  ├─ Tools/                               build scripts (build / build_console / build_plugins / build_plugins_release / build_tests / clean)
│  ├─ Tools/README.md                      build script details, obj directory layout
│  └─ UnityPlugin/README.md                three hard rules for heap allocation during static init (must read)
│
├─ SpaceZ_2D_Exp_CCD/                      historical: full-CCD core (frozen)
├─ SpaceZ_2D_Alpha/                        historical: discrete core (frozen)
└─ EditorPlugin/                           provides IUnityInterface to the core DLLs (required in the editor)
```

---

## Getting started

### Requirements

- Windows x64
- Unity **2022.3.62f3c1** (editor only)
- Visual Studio 2017+ with the "Desktop development with C++" workload (the build scripts locate MSVC via `vswhere`)

### Building the native library

The `Assets/Plugins/x86_64/*.dll` binaries are committed, so C#-only changes need no build; **after touching C++ you must rebuild and commit the DLL**.

Open `SpaceZ_2D_Beta/` as the VS Code workspace and pick a task with `Ctrl+Shift+B`, or run a script directly:

| Script | Purpose | Output |
| --- | --- | --- |
| `SpaceZ_2D_Beta/Tools/build_plugins.bat` | Core + UnityPlugin → single DLL (Debug) | `Assets/Plugins/x86_64/SpaceZ_2D_Beta.dll` |
| `SpaceZ_2D_Beta/Tools/build_plugins_release.bat` | Same, `/O2 /GL` + link `/LTCG` (Release) | **the same** DLL (overwrites / is overwritten by Debug) |
| `SpaceZ_2D_Beta/Tools/build_console.bat` | Core + Main → console program | `SpaceZ_2D_Beta/Main/Debug/SpaceZ_2D_Beta.exe` |
| `SpaceZ_2D_Beta/Tools/build_tests.bat` | Core + Tests → console tests (`Tests/` is currently empty, prints `[SKIP]` and exits normally) | `SpaceZ_2D_Beta/Tests/Debug/` |
| `SpaceZ_2D_Beta/Tools/clean.bat` | Clean this project's artifacts | removes `.build/`, `Main/Debug`, `Tests/Debug`, the DLL above and its `.meta` |
| `EditorPlugin/tools/build_editor_plugin.bat` | Build EditorPlugin on its own | `Assets/Plugins/x86_64/EditorPlugin.dll` |

Compile flags:

- Debug: `/std:c++20 /utf-8 /W4 /Od /RTC1 /MDd`
- Release: `/std:c++20 /utf-8 /W4 /O2 /GL /Gy /MD /DNDEBUG`, with `/LTCG /OPT:REF /OPT:ICF` at link time

Intermediates go to `.build/obj_<target>/` **mirroring the source tree level by level** (`cl.exe` names objects by base name only, so flattening would write `Core/Debug/X.cpp` and `UnityPlugin/Debug/X.cpp` to the same `X.obj`). Debug and Release objects must live in separate directories, otherwise mixing `/MDd` and `/MD` fails with `LNK2038`. `.pdb` files stay in `.build/` and never pollute the Unity asset folder. Debug and Release share a single DLL output path to keep the Unity-side `.meta` and GUID stable; the cost is that only one configuration can exist at a time, which is why the "build all" task excludes Release.

> **DLL locked**: once Unity has loaded the native DLL it holds a file handle and linking fails. If you see `is locked, probably loaded by Unity Editor`, close Unity and rebuild.

### Running in Unity

`RuntimeManager` uses `[RuntimeInitializeOnLoadMethod(BeforeSceneLoad)]` to scan `[RuntimeStart]` methods and runs them **in ascending Order**:

| Order | Method | Purpose |
| --- | --- | --- |
| -100 | `UpdateManager.Init` | inserts `PhyLoop` (with `PhyUpdate` / `PhyLateUpdate`) before `PreUpdate` in `PlayerLoop` |
| -100 | `PhySystem.InitNative` | loads `SpaceZ_2D_Beta.dll` and binds every `[NativeFunc]` field by reflection (including the buffer API on `NativeArray`) |
| -90 | `PhySystem.InitManaged` | `StartEngine(6, 8)`, initializes buffers, registers the physics-frame callback |

On exit the same ascending rule applies to `[RuntimeEnd]`: `Order 90` stops the engine and frees buffers, then `Order 100` unloads the DLL and restores `PlayerLoop`.
In the editor `EditorPlugin.dll` must be present: `NativeLoader` asks it for the `IUnityInterface` pointer before loading a core, and a failure there throws and aborts plugin loading entirely.

---

## Usage example

Add `Rigidbody2D_Z` to a GameObject and shape components on it or its children — no native objects need to be created by hand:

```csharp
using SpaceZ_2D.Beta;
using UnityEngine;

public class Launch : MonoBehaviour
{
    [SerializeField] private Rigidbody2D_Z _body;

    private void Start()
    {
        // Writes go into buffers first and are synced to the native side on the next physics frame
        _body.LinearVelocity = new Vector2(3f, 0f);
        _body.AngularVelocity = 0.5f;

        _body.AddForce(new Vector2(0f, 10f));      // ForceType.Impulse: immediately converted to a velocity delta
        _body.AddTorque(1f, ForceType.Sustain);    // Sustain: applied during this substep, cleared afterwards
    }

    private void Update()
    {
        // Spatial queries go through the static facade; the managed layer reclaims the result memory
        if(PhySystem.Raycast(Vector2.zero, Vector2.down, out RaycastHitInfo hit))
            Debug.Log($"hit {hit.collider} at {hit.distance}");
    }
}
```

Notes:

- **Lifetime**: rigid bodies are created in `Awake` and destroyed in `OnDestroy`; colliders are created in `OnEnable` and destroyed in `OnDisable` and `OnDestroy` (disabling frees the native object, re-enabling rebuilds it).
- **Static colliders**: when no parent rigid body is found, the collider is created with a null handle and takes the native static branch (`invMass = 0`) — it collides but is never pushed.
- **Layers and modes**: `Layer` is a 64-bit mask (layer 0 by default, i.e. `1UL`) and the broad phase filters by mask intersection; `Mode` is `Discrete` or `Continuous`, and `Continuous` requires being attached to a rigid body, otherwise it degrades to `Discrete`.
- **Echo suppression**: both bodies and colliders cache the last dispatched value and skip unchanged frames. A collider's local transform is derived as "inverse of the body's world transform × its own world transform", and changing parents re-binds the body through `OnTransformParentChanged`.
- **Units**: angles and angular velocities are radians; box `Extents` is the half-extent; capsule `Height` is the cylindrical segment (total height = `Height + 2 * Radius`); line `Length` is the full length along local X, centered at the origin.
- **Query exits**: `OverlapBox / OverlapSphere / OverlapCapsule` fill a `List<Handle>` and return the hit count; `Raycast` returns whether it hit and fills a `RaycastHitInfo` (`default` on miss); `RaycastAll` fills a `List<RaycastHitInfo>` already sorted by distance.

---

## Native ABI contract

Where the structs live:

- C#: `Assets/Scripts/Plugins/SpaceZ_2D_Beta/System/NativeData/*.cs`
- Native: `SpaceZ_2D_Beta/UnityPlugin/ExportData/*.h`

Both sides must match **field by field and in order** (4-byte alignment). Conventions:

- Batch input entry points are all `(T* infos, int count)`, where `T` is an "8-byte handle + payload" flattened struct; `infos` points at the native memory of a managed `NativeList` and is **valid only for the duration of the call** — the native side must not cache it.
- Output is always `NativeArray{void* ptr, int size}`, allocated by the native side with `new[]`; the caller must release it with `DestroyBuffer`. Element types must be trivially destructible (`Handle`, `RaycastHit` and friends).
- `bool` is 1 byte across the boundary: `UnmanagedType.U1` in payloads and `[return: MarshalAs(UnmanagedType.I1)]` for return values. The default 4-byte `BOOL` shifts the following fields or reads garbage in the high bytes.
- Angles and angular velocities are radians throughout; contact normals point `first → second`, and `point` is a world-space contact point.
- `layer` is always passed as a 64-bit mask rather than a struct; 0 matches no layer at all.

Exported functions (all under `SpaceZ_2D_Beta/UnityPlugin/ExportInterface/`, declared `extern "C" __declspec(dllexport)`):

- `PhyEngine.cpp`: `StartEngine` / `StopEngine` / `Step` / `DirtyBodies` / `EnterCollisionPairs` / `StayCollisionPairs` / `ExitPairs`
- `PhyObject.cpp`: `CreateBody` / `CreateCollider` / `DestroyBody` / `DestroyCollider` / `SetColliderBody` / `SetBodiesProperty` / `SetBodiesVelocity` / `AddForces` / `AddTorques` / `SetCollidersProperty` / `SetCollidersCollision` / `SetBodiesTransform` / `SetCollidersTransform` / `SetLineColliders` / `SetBoxColliders` / `SetSphereColliders` / `SetCapsuleColliders`
- `PhyQuery.cpp`: `OverlapBox` / `OverlapSphere` / `OverlapCapsule` / `Raycast` / `RaycastAll`
- `NativeBuffer.cpp`: `CreateBuffer` / `DestroyBuffer`

Every export is wrapped in `TRY_CATCH_DEBUG`: exceptions become `Debug::LogError` (forwarded to the Console via `IUnityLog` in the editor) and never propagate into Unity; when the memory manager or the physics system is not ready, the call returns an empty result immediately.

---

## Known limitations

- **Platform**: Windows x64 editor only. `PluginUtility` resolves DLL paths under `UNITY_EDITOR_64` only, and the Player build path is untested.
- **Collision events**: the native side already produces and exports the Enter / Stay / Exit tables; the managed layer does not consume them yet (not reading them costs nothing and leaks nothing).
- **No triggers, no joints**: colliders have no "detect but don't respond" mode and there is no constraint type at all; layers only filter by mask intersection, there is no layer collision matrix.
- **Continuous detection scope**: only pairs with at least one `Continuous` side are covered; advances shorter than `MIN_CCD_STEP` (0.001) are merged, so extreme speeds can still tunnel. Reduce `dt` or raise the substep count to mitigate.
- **Degenerate cases**: when GJK/EPA exhaust their iteration cap they share `return false` with a genuinely separated pair, so pathological setups may miss a hit; penetration depth below `Epsilon` (1e-5) always counts as separated, and exact tangency produces no contact.
- **No heap allocation during static initialization**: the core replaces the global `operator new`, but the allocator is only ready after `UnityPluginLoad`, so containers or strings constructed during static storage make `LoadLibrary` fail outright. Use `UnityPlugin::NativeAllocator` for static containers or defer construction to a function-local `static`; see `SpaceZ_2D_Beta/UnityPlugin/README.md`.
- `Main/Main.cpp` is still a Hello World placeholder; the `Tests/` directory is empty for now, and the build script prints `[SKIP]` and exits normally.

---

## Roadmap

- **Beta (in progress)**: a general-purpose engine that is discrete-first with continuous on demand. Next up is dispatching collision events to the managed layer (enter / stay / exit), then extending spatial queries and adding a layer collision matrix as needed.
- **Alpha / Exp-CCD (done, frozen)**: kept as historical validation of the solver architecture and the CCD approach — sources and build scripts are preserved but no longer iterated on.
