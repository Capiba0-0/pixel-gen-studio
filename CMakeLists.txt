cmake_minimum_required(VERSION 3.21)

project(pixel-gen-studio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 3.0.0 REQUIRED COMPONENTS System Window Graphics CONFIG)
find_package(OpenGL REQUIRED)

# Manual integration of ImGui and ImGui-SFML due to issues with vcpkg port at the time of development. Source files are located in third_party
include_directories(include third_party/imgui/include third_party/imgui-sfml/include)

set(IMGUI_SOURCES
    third_party/imgui/imgui.cpp 
    third_party/imgui/imgui_demo.cpp 
    third_party/imgui/imgui_draw.cpp 
    third_party/imgui/imgui_tables.cpp 
    third_party/imgui/imgui_widgets.cpp 
)

set(IMGUI_SFML_SOURCES
    third_party/imgui-sfml/imgui-SFML.cpp
)

add_executable(${PROJECT_NAME}
    src/core/application.cpp
    src/core/main.cpp 
    src/core/pixel_buffer.cpp
        src/core/managers/document_manager.cpp
    src/core/managers/ui_manager.cpp
    src/gui/widgets/about_window.cpp
    src/gui/widgets/menu_bar.cpp
    src/gui/widgets/new_canvas_window.cpp
    src/gui/canvas.cpp
    src/gui/imgui_setup.cpp
    ${IMGUI_SOURCES} 
    ${IMGUI_SFML_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)

# Робимо так, щоб копіювання виконувалось перед запуском MyApp
add_dependencies(${PROJECT_NAME} copy_assets)

# Coping DLLs to the directory of .exe file
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND_EXPAND_LISTS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying DLLs to build directory"
    )
endif()