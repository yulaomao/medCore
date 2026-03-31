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

# 3. 编译 VTK（Release）
cmake --build . --config Release --parallel 4

# 4. 安装 VTK
cmake --install . --config Release

# 返回项目根目录
cd ../..
```

### 方案 B：使用 VS IDE

1. 打开 Visual Studio
2. 文件 → 打开 → CMake，选择 D:\code\C++\medCore\thirtyPart\vtk-src\CMakeLists.txt
3. 配置后构建（选择 Release）
4. 生成后安装到 D:\code\C++\medCore\thirtyPart\vtk

### 方案 C：使用脚本（如遇权限问题）

编辑 build_vtk.ps1 并修改权限：
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\build_vtk.ps1 -Config Release -Jobs 4
```

## 验证安装

编译完成后，检查以下路径是否存在：
```
D:\code\C++\medCore\thirtyPart\vtk\lib\cmake\vtk\VTKConfig.cmake
D:\code\C++\medCore\thirtyPart\vtk\bin\vtkGUISupportQt-9.4.dll（或 .so）
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
A: 确保 VTK 安装步骤已完成，且 bin 目录中有 .dll 文件。
