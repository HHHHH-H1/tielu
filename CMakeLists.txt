cmake_minimum_required(VERSION 3.10)
project(RailwaySystem VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt库（如果需要GUI）
find_package(Qt6 COMPONENTS Core Widgets Charts QUIET)

if(Qt6_FOUND)
    message(STATUS "Qt6 found, building GUI version")
    set(BUILD_GUI ON)
else()
    message(STATUS "Qt6 not found, building console version")
    set(BUILD_GUI OFF)
endif()

# 源文件
set(SOURCES
    Station.cpp
    Route.cpp
    Train.cpp
    PassengerFlow.cpp
    DataAnalyzer.cpp
    FileManager.cpp
    TimeSeriesAnalyzer.cpp
    main.cpp
)

# 头文件
set(HEADERS
    Station.h
    Route.h
    Train.h
    PassengerFlow.h
    DataAnalyzer.h
    FileManager.h
    TimeSeriesAnalyzer.h
)

if(BUILD_GUI)
    # GUI版本
    set(GUI_SOURCES
        ${SOURCES}
        MainWindow.cpp
        ChartWidget.cpp
    )
    set(GUI_HEADERS
        ${HEADERS}
        MainWindow.h
        ChartWidget.h
    )
    
    qt_add_executable(RailwaySystemGUI ${GUI_SOURCES} ${GUI_HEADERS})
    qt_add_resources(RailwaySystemGUI "resources" PREFIX "/" FILES data/stations.csv data/routes.csv)
    
    target_link_libraries(RailwaySystemGUI Qt6::Core Qt6::Widgets Qt6::Charts)
else()
    # 控制台版本
    add_executable(RailwaySystem ${SOURCES} ${HEADERS})
endif()

# 设置输出目录
set_target_properties(RailwaySystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 