# VTK 独立编译 - 手动步骤

## 概述
VTK 现在已从 medCore 项目中独立出来。按照以下步骤编译和安装 VTK。

## 编译 VTK（Windows Visual Studio）

### 方案 A：使用 CMake 命令行（推荐）

在项目根目录（D:\code\C++\medCore）打开 PowerShell，执行：

```powershell
# 1. 创建 VTK 构建目录
mkdir -Force build/vtk-build
cd build/vtk-build

# 2. 配置 VTK
cmake -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_INSTALL_PREFIX="D:/code/C++/medCore/thirtyPart/vtk" `
  -DBUILD_SHARED_LIBS=ON `
  -DVTK_QT_VERSION=6 `
  -DQt6_DIR="C:/Qt/6.9.1/msvc2022_64/lib/cmake/Qt6" `
  -DVTK_BUILD_TESTING=OFF `
  -DVTK_BUILD_EXAMPLES=OFF `
  -DVTK_BUILD_DOCUMENTATION=OFF `
  -DVTK_BUILD_ALL_MODULES=OFF `
  -DVTK_ENABLE_REMOTE_MODULES=OFF `
  -DVTK_ENABLE_WRAPPING=OFF `
  -DVTK_GROUP_ENABLE_Qt=YES `
  -DVTK_MODULE_ENABLE_VTK_CommonColor=YES `
  -DVTK_MODULE_ENABLE_VTK_CommonCore=YES `
  -DVTK_MODULE_ENABLE_VTK_FiltersSources=YES `
  -DVTK_MODULE_ENABLE_VTK_GUISupportQt=YES `
  -DVTK_MODULE_ENABLE_VTK_InteractionStyle=YES `
  -DVTK_MODULE_ENABLE_VTK_RenderingCore=YES `
  -DVTK_MODULE_ENABLE_VTK_RenderingOpenGL2=YES `
  ../../thirtyPart/vtk-src

# 3. 编译并安装 Debug
cmake --build . --config Debug --parallel 4
cmake --install . --config Debug

# 4. 编译并安装 Release
cmake --build . --config Release --parallel 4
cmake --install . --config Release

# 返回项目根目录
cd ../..
```

### 方案 B：使用 VS IDE

1. 打开 Visual Studio
2. 文件 → 打开 → CMake，选择 D:\code\C++\medCore\thirtyPart\vtk-src\CMakeLists.txt
3. 分别构建并安装 Debug 和 Release
4. 生成后安装到 D:\code\C++\medCore\thirtyPart\vtk

### 方案 C：使用脚本（如遇权限问题）

设置执行权限后运行：
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\build_vtk.ps1 -Jobs 4
```

## 验证安装

编译完成后，检查以下路径是否存在：
```
D:\code\C++\medCore\thirtyPart\vtk\lib\cmake\vtk-9.4\vtk-config.cmake
D:\code\C++\medCore\thirtyPart\vtk\lib\cmake\vtk-9.4\VTK-targets-debug.cmake
D:\code\C++\medCore\thirtyPart\vtk\lib\cmake\vtk-9.4\VTK-targets-release.cmake
D:\code\C++\medCore\thirtyPart\vtk\bin\vtkGUISupportQt-9.4d.dll
D:\code\C++\medCore\thirtyPart\vtk\bin\vtkGUISupportQt-9.4.dll
```

## 后续编译 medCore

VTK 安装完成后：

1. 在 VS 中重新打开 medCore 项目
2. 清空 CMake 缓存（CMake → 删除缓存）
3. CMake → 配置（会自动找到 thirtyPart/vtk）
4. 构建 medCore（VS 中不再显示 VTK 源码项目）

## 常见问题

**Q: 编译卡住？**  
A: VTK 编译可能需要 30-60 分钟。可以用指定 --parallel 数量来加速：`cmake --build . --config Release --parallel 8`

**Q: 仍然在 VS 中看到 VTK 项目？**  
A: 清空 build 目录并重新配置：
```powershell
cmake --fresh -B build -DCMAKE_BUILD_TYPE=Release
```

**Q: DLL 缺失错误？**  
A: 先确认 Debug 和 Release 都执行过 `cmake --install`，并且安装目录里同时存在 `VTK-targets-debug.cmake`、`VTK-targets-release.cmake` 以及对应的 `vtk...d.dll` / `vtk....dll`。

**Q: Debug 运行时提示找不到 Qt6OpenGLWidgets.dll 或加载了错误版本 Qt DLL？**  
A: 这通常是只安装了 Release 版 VTK。请重新执行 Debug 和 Release 两次安装，或者直接运行：
```powershell
.\build_vtk.ps1 -Jobs 4
```
