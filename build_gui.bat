@echo off
chcp 65001 >nul
set TEMP=D:\Temp
set TMP=D:\Temp

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

REM 创建 .pro 文件
(
echo QT += core gui widgets
echo CONFIG += c++17 console
echo TEMPLATE = app
echo TARGET = RailwaySystemGUI
echo.
echo SOURCES += Station.cpp \
echo            Route.cpp \
echo            Train.cpp \
echo            PassengerFlow.cpp \
echo            DataAnalyzer.cpp \
echo            FileManager.cpp \
echo            AdvancedAnalyzer.cpp \
echo            TimeSeriesAnalyzer.cpp \
echo            main_gui.cpp
echo.
echo HEADERS += Station.h \
echo            Route.h \
echo            Train.h \
echo            PassengerFlow.h \
echo            DataAnalyzer.h \
echo            FileManager.h \
echo            AdvancedAnalyzer.h \
echo            TimeSeriesAnalyzer.h
echo.
echo DESTDIR = bin
echo.
echo win32 {
echo     QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=GBK
echo     LIBS += -lmingw32
echo     LIBS += $$[QT_INSTALL_LIBS]/libQt6EntryPoint.a
echo     LIBS += -lshell32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -ladvapi32
echo }
) > RailwaySystem.pro

echo Qt项目文件已创建: RailwaySystem.pro

REM 清除旧的Makefile和.qmake.cache
del /f /q Makefile >nul 2>&1
del /f /q .qmake.stash >nul 2>&1
del /f /q RailwaySystem.pro.user >nul 2>&1

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
mingw32-make -f Makefile.Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ❌ 编译失败，请检查错误日志
    pause
    exit /b 1
)

echo.
echo ========================================
echo          ✅ 编译成功！
echo ========================================
echo.
echo 可执行文件位置: bin\RailwaySystemGUI.exe
echo.

REM 运行提示
if exist "bin\RailwaySystemGUI.exe" (
    echo 是否立即运行程序? (Y/N)
    set /p choice=
    if /i "%choice%"=="Y" (
        echo 正在启动GUI版本...
        start bin\RailwaySystemGUI.exe
    )
) else (
    echo ⚠️ 警告: 未找到可执行文件，请检查编译输出
)

pause
