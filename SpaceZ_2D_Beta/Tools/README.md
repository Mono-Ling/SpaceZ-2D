# SpaceZ_2D_Beta 编译工具

> **工程定位：Beta 主线工程。**
> 与 `SpaceZ_2D_Alpha/`（demo 阶段完成、冻结维护）、`SpaceZ_2D_Exp_CCD/`（CCD 实验）、`EditorPlugin/` 平级、完全独立：
> 各自拥有 `.vscode/`、`Tools/`、`.gitignore`、`.build/`，互不转调、互不清理。

Windows 下基于 MSVC（Visual Studio C++ 工具集，通过 `vswhere` 自动定位）的编译脚本。
编译配置见 `.vscode/tasks.json`，推荐将**本工程目录**作为 VS Code 工作区打开后用 `Ctrl+Shift+B` 调用。

## 脚本一览

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `build_console.bat` | 递归编译 Core + Main，链接为控制台程序 | `Main/Debug/SpaceZ_2D_Beta.exe`（+ `.pdb`） |
| `build_tests.bat` | 递归编译 Core + Tests，链接为控制台测试程序（**不进入 Unity**） | `Tests/Debug/SpaceZ_2D_BetaTests.exe`（+ `.pdb`） |
| `build_plugins.bat` | 递归编译 Core + UnityPlugin，链接为**单个** DLL（与工程同名），Debug 配置 | `<Unity工程根>/Assets/Plugins/x86_64/SpaceZ_2D_Beta.dll` |
| `build_plugins_release.bat` | 同上，但用 **Release 优化配置**（`/O2 /GL` + link `/LTCG`） | **与 Debug 完全相同的** DLL 路径（两者互相覆盖） |
| `clean.bat` | 清理**本工程**所有编译临时文件 | 删除 `.build/`、`Main/Debug/` 与 `Tests/Debug/` 产物、`Assets/Plugins/x86_64/SpaceZ_2D_Beta.dll`（连同 Unity 生成的同名 `.meta`） |
| `build.bat` | 以上五个脚本的共同实现（`build.bat console\|tests\|plugins\|plugins-release\|clean`），**不要直接调用**，改由上面五个薄封装转调 | — |

> DLL 的调试符号 `.pdb` 输出至 `.build/`（Release 至 `.build/release/`），**不进入 Unity 资产目录**；
> `.vscode/launch.json` 的「附加到 Unity」配置已把这两条路径写进 `symbolSearchPath`，attach 后可直接下断点。
>
> **Debug 与 Release 共用同一个 DLL 输出路径**，后跑的那个覆盖前一个。这样做是为了让 Unity 侧的
> `.meta` 与 GUID 保持稳定、C# 引用不会失效；代价是同一时刻目录里只能存在一种配置。
> 因此 `构建全部` **不包含** Release 插件任务，需要 Release 版时单独执行。

## 中间文件布局：为什么 obj 目录要镜像源码树

```
.build/
  obj_console/       Core\... + Main\...
  obj_tests/         Core\... + Tests\...
  obj_plugins/       Core\... + UnityPlugin\...
  obj_plugins_release/ 同上，Release 专用
  link_<target>.rsp  传给 link 的 obj 清单
  SpaceZ_2D_Beta.pdb Debug 插件的符号
  release/           Release 插件的 pdb 与导入库 .lib
```

obj **按源码路径逐级镜像**，例如：

| 源文件 | 目标文件 |
| --- | --- |
| `Core/Debug/Foo.cpp` | `.build/obj_plugins/Core/Debug/Foo.obj` |
| `UnityPlugin/Debug/Foo.cpp` | `.build/obj_plugins/UnityPlugin/Debug/Foo.obj` |
| `Core/Math/Bar.cpp` | `.build/obj_plugins/Core/Math/Bar.obj` |

**这条不是风格问题，是链接错误的主要来源**：`cl.exe` 只按**源文件基名**命名 `.obj`。
如果所有 obj 平铺在一个 `/Fo` 目录里，上面第一行和第二行就会写到**同一个** `Foo.obj`——
后编译的静默覆盖前一个，链接时表现为 `LNK2005 已经定义` / `LNK2019 无法解析的外部符号`
（报的符号"明明存在"），或者更糟：链上了另一份实现，编译链接都过、运行结果不对。

所以本项目**不做 obj 平铺**，即使跨模块（`Core` 与 `UnityPlugin`）出现同名文件夹、
或同一模块内不同子目录出现同名 `.cpp`，也不会互相覆盖。代价是每个**源码目录**一次 `cl` 调用（不是每个文件一次）。

配套的两个约定：

- **每次编译前先删掉该目标的旧 obj**（`del /s /q "%OBJ_DIR%\*.obj"`）。本项目每次都是全量重编，
  删旧 obj 零成本，却能保证「删掉/改名的 .cpp 不会留下野 obj 被链接进去」。
- **Debug 与 Release 的 obj 绝不能共用一个目录**：`/MDd` 与 `/MD` 的对象混链会报
  `LNK2038 RuntimeLibrary 不匹配`。

## 约定与说明

- **编译配置**：Debug | x64 | `/std:c++20 /utf-8 /W4 /Od /RTC1 /MDd`；Release 插件用
  Release | x64 | `/std:c++20 /utf-8 /W4 /O2 /GL /Gy /MD /DNDEBUG`，链接端加 `/LTCG /OPT:REF /OPT:ICF`。
  `/RTC1` 与 `/ZI` 都和 release 选项互斥（`D8016`），故 Release 只保留 `/Zi`（崩溃栈仍可符号化）。
  `/MD` 要求目标机有 VC++ 2015-2022 x64 运行时（Unity Editor 自带）；若要 DLL 完全自包含，把脚本里的 `/MD` 换成 `/MT`。
- **头文件搜索路径**：`%BETA_ROOT%`（工程根）+ 当前目标参与的模块根（`Core`，插件目标再加 `UnityPlugin`）。
  两种包含写法都可用：
  - `#include <Core/Math/Vector2.h>` —— **推荐**，带模块名，跨模块同名目录不会有歧义；
  - `#include "Math/Vector2.h"` —— 相对模块根，靠 `/I` 解析。
  注意 `cl` 会**优先搜索源文件所在目录**，所以 `Core/Debug/X.cpp` 里写 `#include "Debug/Debug.h"`
  永远解析到自己模块；跨模块引用请用带模块名的写法。
- **源文件发现**：脚本递归遍历模块下每个子目录并各自编到镜像目录，新增子目录无需改动脚本。
- **空目录是合法的**：`Core/`、`UnityPlugin/`、`Tests/` 下没有 `.cpp` 时脚本打印 `[SKIP]` 并 `exit /b 0`
  正常退出，不算失败。但 `build_console.bat` 需要 `Main/` 下至少一个带 `main` 的 `.cpp`。
- **DLL 被锁定**：Unity Editor 加载过原生 DLL 后会持有文件句柄。脚本会在链接前预删除旧 DLL，
  遇到 `is locked, probably loaded by Unity Editor` 时关闭 Unity 再构建即可。
- **编码**：脚本为**纯 ASCII + CRLF**（`.gitattributes` 已把 `*.bat` 钉成 `eol=crlf`）。
  请勿在 bat 中加入中文——cmd 在 GBK 代码页下解析 UTF-8 中文批处理会错位断行，
  把注释片段当命令执行（表现为「一串乱码汉字 + 不是内部或外部命令」）。
  源码里的中文注释由 `/utf-8` 保证按 UTF-8 解析。

## VS Code 任务

| 任务标签 | 对应脚本 |
| --- | --- |
| 构建控制台程序 (Core+Main) | `build_console.bat` |
| 构建 Unity 插件 DLL (Core+UnityPlugin) | `build_plugins.bat`（Debug） |
| 构建 Unity 插件 DLL Release (Core+UnityPlugin) | `build_plugins_release.bat`（覆盖同一个 DLL） |
| 构建测试程序 (Core+Tests) | `build_tests.bat` |
| 构建全部（默认 `Ctrl+Shift+B`） | 依次执行控制台、插件 Debug、测试三项（**不含** Release 插件） |
| 清理编译产物 | `clean.bat` |

启动/调试配置（`.vscode/launch.json`）：

| 配置 | 用途 |
| --- | --- |
| 运行控制台程序 (Core+Main) | `F5` 前自动跑 `build_console.bat`，调试 `Main/Debug/SpaceZ_2D_Beta.exe` |
| 附加到 Unity (调试原生 DLL) | attach 到 Unity Editor 进程调试插件；pdb 在 `.build/`，已写进 `symbolSearchPath` |

> 提醒：**改完 C++ 要手动重编**。Unity 编译 C#、进 Play、打包都不会触发这些脚本
> （插件工程在 `Assets/` 外，Unity 根本不知道它存在）；反方向是自动的——DLL 一变 Unity 就重新导入。
