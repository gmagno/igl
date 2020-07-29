# project
cmake_minimum_required(VERSION 3.10)
project(igl)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

# generic vars
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# igl
set(SOURCES
    "${SRC_DIR}/igl.cpp"
    "${SRC_DIR}/loader.cpp"
    "${SRC_DIR}/widgets.cpp"
    "${SRC_DIR}/cli.cpp"
    "${SRC_DIR}/stb_image.cpp"
    "${LIB_DIR}/imgui/examples/imgui_impl_glfw.cpp"
    "${LIB_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
)
file(GLOB_RECURSE HEADERS "${INC_DIR}/*")
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE "${INC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# glfw
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# imgui
set(IMGUI_DIR "${LIB_DIR}/imgui")
add_library("imgui"
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
)
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}" "${IMGUI_DIR}/examples")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}" "${IMGUI_DIR}/examples")
target_link_libraries(${PROJECT_NAME} "imgui")

# spdlog
set(SPDLOG_DIR "${LIB_DIR}/spdlog")
add_subdirectory("${SPDLOG_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${SPDLOG_DIR}/include")
target_link_libraries(${PROJECT_NAME} "spdlog")

# fmt
set(FMT_DIR "${LIB_DIR}/fmt")
add_subdirectory("${FMT_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${FMT_DIR}/include")
target_link_libraries(${PROJECT_NAME} "fmt")

# stb
set(STB_DIR "${LIB_DIR}/stb")
add_library("stb" "${SRC_DIR}/stb_image.cpp")
target_include_directories("stb" PRIVATE "${STB_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}")
target_link_libraries(${PROJECT_NAME} "stb")

# CLI11
set(CLI11_DIR "${LIB_DIR}/CLI11")
add_subdirectory("${CLI11_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CLI11_DIR}/include")
target_link_libraries(${PROJECT_NAME} "CLI11")


# packaging
install(TARGETS igl)
set(CPACK_PACKAGE_NAME "igl")
set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_PACKAGE_VENDOR "Vendor name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Some summary")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/data" "data")

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /install
    /.*build.*
    /\\\\.DS_Store
)
include(CPack)
