# 川渝轨道交通客流分析系统 - 构建指导

## 项目文件结构

本项目已完成核心架构设计和实现框架，包含以下文件：

### 核心类文件
- `Station.h/cpp` - 站点类，管理站点基本信息
- `Route.h/cpp` - 线路类，管理线路和站点序列
- `Train.h/cpp` - 列车类，管理列车时刻表和载客信息
- `PassengerFlow.h/cpp` - 客流管理类，处理客流数据和分析
- `DataAnalyzer.h/cpp` - 数据分析类，提供多维度分析功能
- `FileManager.h/cpp` - 文件管理类，负责数据存储和读取

### 主程序文件
- `main.cpp` - 主程序，包含控制台界面和示例数据

### 配置文件
- `CMakeLists.txt` - CMake构建配置
- `Makefile` - Make构建配置

### 文档文件
- `README.md` - 项目说明文档
- `DESIGN.md` - 系统设计文档
- `BUILD.md` - 构建指导文档（本文件）

## 编译说明

### 环境要求
- C++17兼容的编译器（如g++、clang++、MSVC）
- Windows/Linux/macOS操作系统

### 编译方法

#### 方法1：直接编译（推荐）
由于当前环境的路径编码问题，建议在英文路径下重新创建项目：

1. 将项目文件复制到英文路径（如 `C:\railway_system\`）
2. 打开命令行，切换到项目目录
3. 执行编译命令：

```bash
# Windows (MinGW)
g++ -std=c++17 Station.cpp Route.cpp Train.cpp PassengerFlow.cpp main.cpp -o railway_system.exe

# Linux/macOS
g++ -std=c++17 Station.cpp Route.cpp Train.cpp PassengerFlow.cpp main.cpp -o railway_system
```

#### 方法2：分步编译
```bash
# 编译对象文件
g++ -std=c++17 -c Station.cpp -o Station.o
g++ -std=c++17 -c Route.cpp -o Route.o
g++ -std=c++17 -c Train.cpp -o Train.o
g++ -std=c++17 -c PassengerFlow.cpp -o PassengerFlow.o
g++ -std=c++17 -c main.cpp -o main.o

# 链接生成可执行文件
g++ Station.o Route.o Train.o PassengerFlow.o main.o -o railway_system
```

## 已知问题和解决方案

### 1. 路径编码问题
**问题**: 当前路径包含中文字符，可能导致编译失败
**解决**: 将项目复制到纯英文路径下

### 2. 头文件依赖问题
**问题**: 某些头文件可能在不同环境下不可用
**解决**: 
- 确保使用C++17标准
- 如有必要，移除或替换不兼容的头文件

### 3. 编译器兼容性
**问题**: 不同编译器对C++17特性支持不同
**解决**: 
- 使用较新版本的g++（7.0+）或clang++（5.0+）
- MSVC需要Visual Studio 2017或更新版本

## 运行说明

### 基本运行
编译成功后，直接运行可执行文件：
```bash
# Windows
railway_system.exe

# Linux/macOS
./railway_system
```

### 功能演示
程序启动后会显示菜单界面，包含以下功能：
1. 显示所有站点信息
2. 显示所有线路信息
3. 显示所有列车信息
4. 显示客流统计信息
5. 分析站点客流排行
6. 分析川渝双向流量
7. 分析列车载客率
8. 预测客流变化
9. 生成日报告
0. 退出系统

### 示例数据
程序内置川渝地区主要站点的示例数据：
- 重庆北站、重庆西站
- 成都东站、成都南站
- 成渝高铁线路
- G8501、G8502次列车

## 扩展开发

### Qt图形界面版本
如需开发GUI版本，需要：
1. 安装Qt6开发环境
2. 修改CMakeLists.txt启用Qt支持
3. 创建MainWindow.cpp和相关UI文件

### 数据存储功能
如需完整的文件存储功能：
1. 完善FileManager类的实现
2. 创建data目录和CSV文件
3. 实现完整的数据导入导出功能

### 高级分析功能
可扩展的分析功能：
1. 更复杂的预测算法（回归分析、时间序列等）
2. 机器学习模型集成
3. 地理信息系统(GIS)支持
4. 实时数据接口

## 故障排除

### 常见编译错误
1. **"fatal error: can't create"** - 路径问题，使用英文路径
2. **"undefined reference"** - 链接问题，确保所有.cpp文件都被编译
3. **"error: 'std' has not been declared"** - 缺少using namespace std或std::前缀

### 运行时错误
1. **程序闪退** - 检查是否有未捕获的异常
2. **中文显示乱码** - 设置正确的控制台编码
3. **文件读写失败** - 检查文件路径和权限

## 技术支持

如遇到问题，可以：
1. 检查编译器版本和C++17支持
2. 查看错误日志和输出信息
3. 尝试在不同环境下编译运行
4. 参考项目文档和设计说明

## 项目状态

当前项目已完成：
- ✅ 核心类设计和实现
- ✅ 面向对象架构设计
- ✅ 基础数据分析功能
- ✅ 控制台用户界面
- ✅ 示例数据和演示功能

待完善功能：
- 🔲 完整的文件存储系统
- 🔲 Qt图形用户界面
- 🔲 高级数据分析算法
- 🔲 可视化图表生成
- 🔲 配置文件管理

本项目为教学项目，展示了完整的软件工程开发过程，从需求分析到系统实现，体现了面向对象的设计思想和C++编程技术。 