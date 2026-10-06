# SpaceZ_2D_Alpha 编译工具

Windows 下基于 MSVC（Visual Studio C++ 工具集，通过 vswhere 自动定位）的编译脚本。
编译配置见 `.vscode/tasks.json`，推荐在 VS Code 中用 `Ctrl+Shift+B` 调用。

## 脚本一览

| 脚本 | 作用 | 产物 |
| --- | --- | --- |
| `build_plugins.bat` | 递归编译 Core + UnityPlugin，链接为**单个** DLL（与工程同名） | `<Unity工程根>/Assets/Plugins/x86_64/SpaceZ_2D_Alpha.dll` |
| `build_console.bat` | 递归编译 Core + Main，链接为控制台程序 | `Main/Debug/SpaceZ_2D_Alpha.exe`（+ `.pdb`） |
| `build_editor_plugin.bat` | 递归编译 EditorPlugin，**独立**链接为 DLL（不链接、不包含 Core） | `<Unity工程根>/Assets/Plugins/x86_64/EditorPlugin.dll` |
| `clean.bat` | 清理所有编译临时文件 | 删除 `.build/`（含 DLL 的 `.pdb`/`.lib`/`.exp`）、`Main/Debug/` 产物、`Assets/Plugins/x86_64/` 下的两个 DLL（连同 Unity 生成的同名 `.meta`） |

> DLL 的调试符号 `.pdb` 输出至 `.build/`，不进入 Unity 资产目录；本机用 VS 调试时链接器记录的原始路径仍可命中。

## 约定与说明

- **编译配置**：Debug | x64 | `/std:c++20 /utf-8 /W4 /MDd`。`build_plugins.bat` / `build_console.bat` 的头文件搜索路径含 `Core/` 与 `UnityPlugin/`；`build_editor_plugin.bat` 为独立编译，无 Core 头文件路径。
- **中间文件**：`.build/obj_plugins/`、`.build/obj_console/`、`.build/obj_editor_plugin/` 分开存放（含 obj 与编译器类型库 `vc140.pdb`），互不污染；由 `clean.bat` 一并删除。所有中间文件路径均在脚本中显式指定，不会散落至工作目录。
- **DLL 为单一导出物**：Core 与 UnityPlugin 的符号在同一 DLL 内互相可见，UnityPlugin 中以 `extern "C" __declspec(dllexport)` 暴露给 Unity 的接口即可（Unity 侧由 `NativeLoader.cs` 按名加载）。
- **源文件发现**：两个构建脚本递归收集目录下所有 `.cpp`（响应文件方式传给编译器），新增子目录无需改动脚本；但**不同子目录的 `.cpp` 不要同名**（目标文件按基名平铺）。
- **DLL 被锁定**：Unity Editor 加载过原生 DLL 后会持有文件句柄，此时重新链接会失败。构建脚本会在链接前预删除旧 DLL 并给出明确提示；遇到 `is locked, probably loaded by Unity Editor` 时，关闭 Unity 再重新构建即可。
- **编码**：脚本为纯 ASCII + CRLF。请勿在 bat 中加入中文（cmd 在 GBK 代码页下解析 UTF-8 中文批处理会错位断行）；源码含中文注释时由 `/utf-8` 选项保证按 UTF-8 解析。

## VS Code 任务

| 任务标签 | 对应脚本 |
| --- | --- |
| 构建 Unity 插件 DLL (Core+UnityPlugin) | `build_plugins.bat` |
| 构建控制台程序 (Core+Main) | `build_console.bat` |
| 构建编辑器插件 DLL (EditorPlugin) | `build_editor_plugin.bat` |
| 构建全部 | 依次执行上三项 |
| 清理编译产物 | `clean.bat` |
