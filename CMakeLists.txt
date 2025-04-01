cmake_minimum_required(VERSION 3.26)
project(Airi_DesktopGril)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/mingw_64")

# 设置是否为debug模式，在构建时会自动判断是否是debug模式，因此不用显式设置
#set(D_OR_R ON)  # ON为Debug

# 是否开启控制台调试输出，这个不会自动改，release的时候记得改成OFF
set(CONSOLE ON) # ON为启用

# 设置渲染方式，选择为OpenGL，不然默认是Cocos2d，编译包报错的，CMake也会给你警告的
set(FRAMEWORK_SOURCE OpenGL)
# 在主CMakeLists.txt中添加
set(FRAMEWORK_DEFINITIOINS -DCSM_TARGET_WIN_GL)
# 查找Live2D的所有src文件
file(GLOB LappLive "LAppLive2D/Src/*.cpp")

# 添加库文件搜索路径
link_directories(
        "${CMAKE_SOURCE_DIR}/Lib/Debug/"
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        OpenGL
        OpenGLWidgets
        SerialPort
        Sql
        Svg
        WebChannel
        WebSockets
        Multimedia
        REQUIRED)

if (NOT CONSOLE)
    set(CON WIN32)
endif ()
add_executable(Airi_DesktopGril ${CON} main.cpp GLCore.cpp GLCore.h "${LappLive}"
        menu.cpp
        menu.h
        MenuSetting.cpp
        MenuSetting.h
        ConfigManager.cpp
        ConfigManager.h
        MenuModelFiles.cpp
        MenuModelFiles.h
        TableViewModel.cpp
        TableViewModel.h
        ConfirmDialog.cpp
        ConfirmDialog.h
        MessageBox.cpp
        MessageBox.h
        AudioPlayer.cpp
        AudioPlayer.h
        DanmakuManager.cpp
        DanmakuManager.h)
# message(STATUS ${CON})

# 移除预编译库链接
# 添加Framework源码作为子目录
add_subdirectory(${CMAKE_SOURCE_DIR}/Thirdparty/Framework)

target_link_libraries(Airi_DesktopGril
        Framework
        libglew32d.dll
        Live2DCubismCore_d
        glfw3
        opengl32
        glu32
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::SerialPort
        Qt6::Sql
        Qt6::Svg
        Qt6::WebChannel
        Qt6::WebSockets
        Qt6::Multimedia
        ElaWidgetTools
)

# 添加头文件搜索路径
include_directories(
        ./
        LAppLive2D/Inc
        Thirdparty/Framework/src
        Thirdparty/Framework/src/Effect
        Thirdparty/Framework/src/Id
        Thirdparty/Framework/src/Math
        Thirdparty/Framework/src/Model
        Thirdparty/Framework/src/Motion
        Thirdparty/Framework/src/Physics
        Thirdparty/Framework/src/Rendering
        Thirdparty/Framework/src/Type
        Thirdparty/Framework/src/Utils

        Thirdparty/Core/include
        Thirdparty/glew/include
        Thirdparty/glew/include/GL
        Thirdparty/glfw/include
        Thirdparty/glfw/include/GLFW
        Thirdparty/stb
        Thirdparty/ElaWidgetTools/include
)

# 添加库文件搜索路径
link_directories(
        "${CMAKE_SOURCE_DIR}/Lib/Debug/"
)
#message(STATUS CSD: "${CMAKE_SOURCE_DIR}/Lib/Debug/")


# 复制Resources目录到生成目录cmake-build-debug
file(GLOB RESOURCE_W "${CMAKE_BINARY_DIR}/Resources")
#message(STATUS RW: ${RESOURCE_W})
if (NOT RESOURCE_W) # 如果没有这个Resources目录就复制过去
    file(COPY
            "${CMAKE_SOURCE_DIR}/Resources"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

# 复制库文件中的dll文件到生成目录cmake-build-debug
file(GLOB GLEW_DLL_W "${CMAKE_BINARY_DIR}/glew32d.dll")
file(GLOB LIVE2D_DLL_W "${CMAKE_BINARY_DIR}/Live2DCubismCore.dll")
file(GLOB ELA_DLL_W "${CMAKE_BINARY_DIR}/ElaWidgetTools.dll")
if (NOT GLEW_DLL_W)
    file(COPY
            "${CMAKE_SOURCE_DIR}/Lib/Debug/glew32d.dll"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

if (NOT LIVE2D_DLL_W)
    file(COPY
            "${CMAKE_SOURCE_DIR}/Lib/Debug/Live2DCubismCore.dll"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

if (NOT ELA_DLL_W)
    file(COPY
            "${CMAKE_SOURCE_DIR}/Lib/Debug/ElaWidgetTools.dll"
            DESTINATION
            "${CMAKE_BINARY_DIR}/"
    )
endif ()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets OpenGLWidgets Network OpenGL SerialPort Sql Svg WebChannel WebSockets Multimedia)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

# 复制 MinGW 运行时库到生成目录
if(WIN32)
    # 获取 MinGW 的 bin 目录路径
    get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)

    # 复制主要的 MinGW 运行时库
    foreach(RUNTIME_LIB
            "libgcc_s_seh-1.dll"
            "libstdc++-6.dll"
            "libwinpthread-1.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_PATH}/${RUNTIME_LIB}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach()
endif()