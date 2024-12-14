cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GS_VIEWER)

if(WIN32)
    add_compile_options("/utf-8")
endif()

message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

### GLAD 설정 ###
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

### IMGUI 설정 ###
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

### HAPPLY 설정 ###
set(HAPPLY_DIR ${CMAKE_SOURCE_DIR}/external/happly)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/*.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

### GLFW 설정 ###
# external/glfw-3.4 에 있는 GLFW 소스를 서브모듈로 추가
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw-3.4)

### 프로젝트 소스 설정 ###
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SRC_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(GS_VIEWER ${SRC_FILES} ${SRC_HEADERS})
target_include_directories(GS_VIEWER PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(GS_VIEWER PRIVATE ${HAPPLY_DIR})

### OS별 설정 ###
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)

    find_package(OpenGL REQUIRED)

    # 이제 glfw는 add_subdirectory를 통해 빌드되었으므로 직접 링크 가능
    set(EXTERNAL_LINK_LIBS OpenGL::GL glfw imgui glad)

    # 셰이더 디렉토리 복사
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})

elseif(WIN32)
    find_package(OpenGL REQUIRED)
    # GLEW를 사용할 경우 find_package(GLEW REQUIRED) 또는 add_subdirectory 등 필요
    # 여기서는 예시로 GLEW 사용 안함

    set(EXTERNAL_LINK_LIBS OpenGL::GL glfw imgui glad)
    include_directories(${CMAKE_SOURCE_DIR}/external/glfw-3.4/include)

    # 셰이더 전체 디렉토리 복사
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})

elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    # glfw는 이미 add_subdirectory로 빌드
    set(EXTERNAL_LINK_LIBS OpenGL::GL GLEW::GLEW glfw imgui glad)

    # 셰이더 디렉토리 전체 복사
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})
endif()