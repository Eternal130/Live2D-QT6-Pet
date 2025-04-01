cmake_minimum_required(VERSION 3.26)

set(LIB_NAME Framework)

# Force static library.
add_library(${LIB_NAME} STATIC)

add_subdirectory(src)

# Add include path.
target_include_directories(${LIB_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../glew/include  # GLEW路径
    ${CMAKE_CURRENT_SOURCE_DIR}/../glew/include/GL  # 添加这一行
    ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include  # GLFW路径
        ${RENDER_INCLUDE_PATH}
)

# Deprecated functions
# The following expressions are written for compatibility
# and will be removed in a future release.

# Add core include.
target_include_directories(${LIB_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/include
)
# Add definitions.
target_compile_definitions(${LIB_NAME}
  PRIVATE
        -DCSM_TARGET_WIN_GL
)
# 调试模式
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${LIB_NAME} PRIVATE CSM_DEBUG)
endif()
