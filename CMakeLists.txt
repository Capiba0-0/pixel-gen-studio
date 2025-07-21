cmake_minimum_required(VERSION 3.21)

project(pixel-gen-studio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3.0.0 REQUIRED COMPONENTS System Window Graphics CONFIG)
find_package(OpenGL REQUIRED)

# Manual integration of ImGui and ImGui-SFML due to issues with vcpkg port at the time of development.
# Source files are located in third_party
include_directories(include third_party/imgui/include third_party/imgui-sfml/include third_party/imnodes/include)

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

set(IMGUI_IMNODES_SOURCES
    third_party/imnodes/imnodes.cpp
)

add_executable(${PROJECT_NAME} # TODO: Split the .cpp connection into smaller CMakeLists.txt
    # Core
    src/core/application.cpp
    src/core/main.cpp

    # - Buffers
    src/core/buffers/greyscale_buffer.cpp
    src/core/buffers/pixel_buffer.cpp
    src/core/buffers/vector_field_buffer.cpp

    # - Managers
    src/core/managers/document_manager.cpp
    src/core/managers/ui_manager.cpp

    # GUI
    src/gui/canvas.cpp
    src/gui/imgui_setup.cpp

    # - Widgets
    src/gui/widgets/about_window.cpp
    src/gui/widgets/menu_bar.cpp
    src/gui/widgets/new_canvas_window.cpp

    # - Node Editor
    src/gui/node_editor/node_editor_widget.cpp
    src/gui/node_editor/node_editor_state.cpp
    src/gui/node_editor/node_editor_renderer.cpp
    src/gui/node_editor/input/commands.cpp
    src/gui/node_editor/input/node_editor_input_handler.cpp

    # Node Graph
    src/node_graph/node.cpp
    src/node_graph/evaluator.cpp

    # - Utils
    src/node_graph/utils/perlin_noise_2d.cpp

    # - Nodes
    src/node_graph/nodes/texture_output_node.cpp
    #
    src/node_graph/nodes/checker_pattern_node.cpp
    src/node_graph/nodes/noise_texture_node.cpp
    src/node_graph/nodes/voronoi_texture_node.cpp
    src/node_graph/nodes/gradient_texture_node.cpp
    #
    src/node_graph/nodes/circle_pattern_node.cpp
    src/node_graph/nodes/rectangle_pattern_node.cpp
    #
    src/node_graph/nodes/value_node.cpp
    src/node_graph/nodes/math_node.cpp
    #
    src/node_graph/nodes/rgb_node.cpp
    src/node_graph/nodes/mix_color_node.cpp
    src/node_graph/nodes/invert_color_node.cpp
    src/node_graph/nodes/hsv_node.cpp
    #
    src/node_graph/nodes/mapping_node.cpp
    #
    src/node_graph/nodes/combine_xy_node.cpp
    src/node_graph/nodes/separate_xy_node.cpp

    # Libraries
    ${IMGUI_SOURCES}
    ${IMGUI_SFML_SOURCES}
    ${IMGUI_IMNODES_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets to output directory"
)

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
