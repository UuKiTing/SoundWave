cmake_minimum_required(VERSION 3.16)

project(SoundWave VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 自动处理 MOC, UIC, RCC（替代手动设置）
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 1. 查找 Qt6 依赖
find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia Sql Widgets Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)


# 开启 IDE 物理文件夹映射
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 2. 搜集源码与资源 (加入 CONFIGURE_DEPENDS 确保新建文件能被自动识别)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
    "src/*ui"
)


file(GLOB_RECURSE RES_FILES CONFIGURE_DEPENDS
    "resources/*.qrc"
    "*.qrc"
)

# 3. 依据磁盘路径自动生成 Qt Creator 项目树的文件夹节点 (使用现代 TREE 模式，替代手动循环)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES} ${RES_FILES})

# 4. 构建目标
# 注意：如果你的 main.cpp 在 src 文件夹内部，请在这里删掉 `main.cpp`，
# 因为 ${SRC_FILES} 已经包含它了。如果 main.cpp 在项目根目录，则保持原样。
qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    main.cpp
    ${SRC_FILES}
    ${UI_FILES}
    ${RES_FILES}
)

target_link_libraries(SoundWave PRIVATE Qt6::Core)
target_link_libraries(SoundWave PRIVATE Qt6::Core)

# 5. 配置 AUTOUIC 搜索路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOUIC_SEARCH_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/forms"
)



# 6. 头文件与第三方库依赖
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # 确保能找到 Qt 自动生成的 ui_*.h 和 moc_*.cpp 文件
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/views
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/delegates
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/imageloader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mediator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/player
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models/proxies
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http
    ${CMAKE_CURRENT_SOURCE_DIR}/taglib/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/taglib/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Multimedia
    Qt6::Sql
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    tag
)

# 7. 平台属性设置（对齐官方模板）
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 8. 安装规则
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 9. Qt6 最终化（与 MANUAL_FINALIZATION 配合）
qt_finalize_executable(${PROJECT_NAME})
