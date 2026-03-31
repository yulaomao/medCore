# VTK 独立编译工作流说明

## 修改内容

1. **CMakeLists.txt 修改**（已完成）
   - 删除了 `add_subdirectory(thirtyPart/vtk-src...)` 行
   - 改为 `find_package(VTK)` 从预编译目录查找
   - VTK 安装目标指定为 `thirtyPart/vtk`

2. **VS 项目变化**
   - VS 解决方案里不再有 VTK 源码编译项目
   - medCore 直接链接预编译的 VTK
   - VTK 源码改变不会触发 medCore 的 CMake 重配置

3. **VTK 部署路径**
   - 从 `build/third_party/vtk/bin` 改为 `thirtyPart/vtk/bin`
   - medCore 构建后自动复制 VTK DLL

## 使用步骤

### 第一次（编译 VTK）
```powershell
# 在项目根目录运行
.\build_vtk.ps1 -Config Debug -Jobs 4
# 或用 Release
.\build_vtk.ps1 -Config Release -Jobs 4
```

编译完成后 VTK 将安装到 `thirtyPart/vtk`

### 后续（编译 medCore）
照常在 VS 或 CMake 中编译 medCore，无需担心 VTK 源码变化

## 文件
- `build_vtk.ps1` - VTK 独立编译脚本
- `CMakeLists.txt` - 已修改，现在用 find_package(VTK)
