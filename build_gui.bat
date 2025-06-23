@echo off
echo ========================================
echo    川渝轨道交通系统 - Qt GUI编译脚本
echo ========================================
echo.

REM 检查是否安装了Qt
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 未找到Qt开发环境
    echo 请确保已安装Qt6并将bin目录添加到PATH环境变量
    echo.
    echo 或者您可以运行控制台版本:
    echo    g++ -std=c++17 Station.cpp Route.cpp Train.cpp PassengerFlow.cpp DataAnalyzer.cpp FileManager.cpp main.cpp -o railway_system.exe
    echo.
    pause
    exit /b 1
)

echo 检测到Qt环境...
qmake -version

echo.
echo 正在创建Qt项目文件...

REM 创建.pro文件
(
echo QT += core widgets
echo CONFIG += c++17
echo TARGET = RailwaySystemGUI
echo TEMPLATE = app
echo.
echo SOURCES += \
echo     Station.cpp \
echo     Route.cpp \
echo     Train.cpp \
echo     PassengerFlow.cpp \
echo     DataAnalyzer.cpp \
echo     FileManager.cpp \
echo     main_gui.cpp
echo.
echo HEADERS += \
echo     Station.h \
echo     Route.h \
echo     Train.h \
echo     PassengerFlow.h \
echo     DataAnalyzer.h \
echo     FileManager.h
echo.
echo # 设置输出目录
echo DESTDIR = bin
echo.
echo # Windows特定设置
echo win32 {
echo     CONFIG += console
echo     QMAKE_CXXFLAGS += /utf-8
echo }
echo.
echo # 资源文件
echo RC_ICONS = icon.ico
) > RailwaySystem.pro

echo Qt项目文件已创建: RailwaySystem.pro

echo.
echo 正在生成Makefile...
qmake RailwaySystem.pro

if %ERRORLEVEL% NEQ 0 (
    echo 错误: qmake生成失败
    pause
    exit /b 1
)

echo.
echo 正在编译Qt GUI版本...
mingw32-make

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo 编译失败，尝试使用nmake...
    nmake
    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo 错误: 编译失败
        echo 请检查Qt安装和编译器设置
        pause
        exit /b 1
    )
)

echo.
echo ========================================
echo          编译成功！
echo ========================================
echo.
echo 可执行文件位置: bin\RailwaySystemGUI.exe
echo.
echo 运行方法:
echo   cd bin
echo   RailwaySystemGUI.exe
echo.

if exist "bin\RailwaySystemGUI.exe" (
    echo 是否立即运行程序? (Y/N)
    set /p choice=
    if /i "%choice%"=="Y" (
        echo 正在启动GUI版本...
        start bin\RailwaySystemGUI.exe
    )
) else (
    echo 警告: 未找到可执行文件
)

pause 