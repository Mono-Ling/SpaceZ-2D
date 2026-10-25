# SpaceZ_2D_Exp_CCD 编译工具

> **工程定位：CCD（连续碰撞检测）实验工程。**
> 与 `SpaceZ_2D_Alpha/`（demo 阶段完成、冻结维护）、`EditorPlugin/` 平级、完全独立：
> 各自拥有 `.vscode/`、`tools/`、`.gitignore`、`.build/`，互不转调、互不清理。

Windows 下基于 MSVC（Visual Studio C++ 工具集，通过 vswhere 自动定位）的编译脚本。
编译配置见 `.vscode/tasks.json`，推荐将**本工程目录**作为 VS Code 工作区打开后用 `Ctrl+Shift+B` 调用。

## 脚本一览

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `build_plugins.bat` | 递归编译 Core + UnityPlugin，链接为**单个** DLL（与工程同名），Debug 配置 | `<Unity工程根>/Assets/Plugins/x86_64/SpaceZ_2D_Exp_CCD.dll` |
| `build_plugins_release.bat` | 同上，但用 **Release 优化配置**（`/O2 /GL` + link `/LTCG`） | **与 Debug 完全相同的** `<Unity工程根>/Assets/Plugins/x86_64/SpaceZ_2D_Exp_CCD.dll`（两者互相覆盖） |
| `build_console.bat` | 递归编译 Core + Main，链接为控制台程序 | `Main/Debug/SpaceZ_2D_Exp_CCD.exe`（+ `.pdb`） |
| `build_tests.bat` | 递归编译 Core + Tests，链接为控制台测试程序（**不进入 Unity**） | `Tests/Debug/SpaceZ_CcdTests.exe`（+ `.pdb`） |
| `clean.bat` | 清理**本工程**所有编译临时文件 | 删除本工程 `.build/`、`Main/Debug/` 与 `Tests/Debug/` 产物、`Assets/Plugins/x86_64/SpaceZ_2D_Exp_CCD.dll`（连同 Unity 生成的同名 `.meta`） |

> DLL 的调试符号 `.pdb` 输出至 `.build/`，不进入 Unity 资产目录；本机用 VS 调试时链接器记录的原始路径仍可命中。
> Release 版的 `.pdb` 与导入库 `.lib` 单独放在 `.build/release/`，避免与 Debug 的同名文件互相顶掉。
>
> **Debug 与 Release 共用同一个 DLL 输出路径**，后跑的那个覆盖前一个。这样做是为了让 Unity 侧的
> `.meta` 与 GUID 保持稳定、C# 引用不会失效；代价是同一时刻目录里只能存在一种配置。
> 因此 `构建全部` **不包含** Release 插件任务，需要 Release 版时单独执行。
>
> `clean.bat` 只删本工程的产物；同级其他 C++ 工程有各自的 `tools/clean.bat`，不在本脚本清理范围内。

## 约定与说明

- **编译配置**：Debug | x64 | `/std:c++20 /utf-8 /W4 /Od /RTC1 /MDd`；`build_plugins_release.bat` 用
  Release | x64 | `/std:c++20 /utf-8 /W4 /O2 /GL /Gy /MD /DNDEBUG`，链接端加 `/LTCG /OPT:REF /OPT:ICF`。
  `/RTC1` 与 `/ZI` 都和 release 选项互斥（`D8016`），故 Release 只保留 `/Zi`（崩溃栈仍可符号化）。
  `/MD` 要求目标机有 VC++ 2015-2022 x64 运行时（Unity Editor 自带）；若 DLL 需在任意机器上完全自包含，
  把脚本里的 `/MD` 换成 `/MT`（本工程无跨模块内存分配，两种都能用）。
  `build_plugins*.bat` 的头文件搜索路径含 `Core/` 与 `UnityPlugin/`。
- **中间文件**：`.build/obj_plugins/`、`.build/obj_plugins_release/`、`.build/obj_console/`、`.build/obj_tests/`
  分开存放（含 obj 与编译器类型库 `vc140.pdb`），互不污染——**Debug 与 Release 的 obj 绝不能混在一个目录**，
  否则 `/MDd` 与 `/MD` 的对象混链会报 `LNK2038 RuntimeLibrary` 不匹配。全部由 `clean.bat` 一并删除。
  所有中间文件路径均在脚本中显式指定，不会散落至工作目录。
- **DLL 为单一导出物**：Core 与 UnityPlugin 的符号在同一 DLL 内互相可见，UnityPlugin 中以 `extern "C" __declspec(dllexport)` 暴露给 Unity 的接口即可。
- **源文件发现**：脚本递归收集目录下所有 `.cpp`（响应文件方式传给编译器），新增子目录无需改动脚本；但**不同子目录的 `.cpp` 不要同名**（目标文件按基名平铺）。
- **UnityPlugin 现状**：已从 `SpaceZ_2D_Alpha/UnityPlugin` 拷入 Unity 桥接层——
  `Debug/`（`IUnityLog` + `NativeDebug`，向 Unity 控制台转发日志）与
  `UnityInterface/`（`UnityPluginLoad` / `UnityPluginUnload` 入口与回调注册表）。
  刻意**不含** `ExportInterface/`（导出 API，依赖 Alpha 的 Core 类型）与 `Test.cpp`（临时测试）。
  两个目录自包含，不引用 Core。因此现在跑 `build_plugins.bat` 会真的产出 `SpaceZ_2D_Exp_CCD.dll`；
  但 Unity 侧目前没有 C# 代码按这个名字 `DllImport`，拷进去不会自动被加载。
- **空目录是合法的**：`Core/`、`Tests/` 下没有 `.cpp` 时，对应脚本打印 `[SKIP]` 并以 `exit /b 0` 正常退出，不算失败。注意 `build_console.bat` 需要 `Main/` 下至少有一个 `.cpp`，否则没有 `main` 入口。
- **DLL 被锁定**：Unity Editor 加载过原生 DLL 后会持有文件句柄，此时重新链接会失败。脚本会在链接前预删除旧 DLL 并给出明确提示；遇到 `is locked, probably loaded by Unity Editor` 时，关闭 Unity 再重新构建即可。
- **编码**：脚本为纯 ASCII + CRLF。请勿在 bat 中加入中文（cmd 在 GBK 代码页下解析 UTF-8 中文批处理会错位断行）；源码含中文注释时由 `/utf-8` 选项保证按 UTF-8 解析。

## VS Code 任务

| 任务标签 | 对应脚本 |
| --- | --- |
| 构建 Unity 插件 DLL (Core+UnityPlugin) | `build_plugins.bat`（Debug） |
| 构建 Unity 插件 DLL Release (Core+UnityPlugin) | `build_plugins_release.bat`（`/O2 /GL` + `/LTCG`，覆盖同一个 DLL） |
| 构建控制台程序 (Core+Main) | `build_console.bat` |
| 构建测试程序 (Core+Tests) | `build_tests.bat` |
| 构建全部（默认 `Ctrl+Shift+B`） | 依次执行**插件 Debug**、控制台、测试三项（**不含** Release 插件） |
| 清理编译产物 | `clean.bat` |

> 表中最后一条 `C/C++: cl.exe 生成活动文件` 是 VS Code 的单文件快速编译任务（需要当前 shell 已有 MSVC 环境），**不是**本工程的主构建方式，也不是默认构建任务。
