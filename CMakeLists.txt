cmake_minimum_required(VERSION 3.16)
project(HomeWealth LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core WebSockets Charts)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        WebSockets
        Charts
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    core/core.cpp
    core/core.h
    core/protocol.cpp
    core/protocol.h
    logger/logger.cpp
    logger/logger.h
    # 主窗口
    ui/HomeWealth/HomeWealth.ui
    ui/HomeWealth/HomeWealth.cpp
    ui/HomeWealth/HomeWealth.h
    #AnimatedStackedWidget
    ui/HomeWealth/AnimatedStackedWidget/AnimatedStackedWidget.hpp
    # 首页
    ui/HomeWealth/page/page_main/page_main.ui
    ui/HomeWealth/page/page_main/page_main.cpp
    ui/HomeWealth/page/page_main/page_main.h

    # 编辑页面
    ui/HomeWealth/page/page_edit/page_edit.ui
    ui/HomeWealth/page/page_edit/page_edit.cpp
    ui/HomeWealth/page/page_edit/page_edit.h
    # 输入提示框
    ui/HomeWealth/page/page_edit/dialog_input/dialog_input.ui
    ui/HomeWealth/page/page_edit/dialog_input/dialog_input.cpp
    ui/HomeWealth/page/page_edit/dialog_input/dialog_input.h
    # combox委托
    ui/HomeWealth/page/page_edit/delegate_combox/delegate_combox.cpp
    ui/HomeWealth/page/page_edit/delegate_combox/delegate_combox.h
    #多选对话框
    ui/HomeWealth/page/dialog_multiSelect_for_familyUser/dialog_multiSelect_for_familyUser.cpp
    ui/HomeWealth/page/dialog_multiSelect_for_familyUser/dialog_multiSelect_for_familyUser.h
    ui/HomeWealth/page/dialog_multiSelect_for_category/dialog_multiSelect_for_category.cpp
    ui/HomeWealth/page/dialog_multiSelect_for_category/dialog_multiSelect_for_category.h
    #分类管理页面
    ui/HomeWealth/page/page_category/page_category.ui
    ui/HomeWealth/page/page_category/page_category.cpp
    ui/HomeWealth/page/page_category/page_category.h
    #radio委托
    ui/HomeWealth/page/page_category/delegate_radio/delegate_radio.cpp
    ui/HomeWealth/page/page_category/delegate_radio/delegate_radio.h
    #输入分类
    ui/HomeWealth/page/page_category/dialog_inputCategory/dialog_inputCategory.ui
    ui/HomeWealth/page/page_category/dialog_inputCategory/dialog_inputCategory.cpp
    ui/HomeWealth/page/page_category/dialog_inputCategory/dialog_inputCategory.h
    #登录窗口
    ui/Login/ClientLogin.ui
    ui/Login/ClientLogin.cpp
    ui/Login/ClientLogin.h
    res/HomeWealth.rc
)

# 添加资源文件
qt_add_resources(RESOURCE_FILES res/HomeWealth.qrc)

qt_add_executable(HomeWealth
    ${PROJECT_SOURCES}
    ${RESOURCE_FILES} # 添加资源到可执行文件
)

set_target_properties(HomeWealth
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_include_directories(HomeWealth
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/HomeWealth/AnimatedStackedWidget
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/HomeWealth/page
)
target_link_libraries(HomeWealth
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::WebSockets
        Qt::Charts
)
