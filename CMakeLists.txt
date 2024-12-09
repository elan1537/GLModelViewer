cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GS_VIEWER)

### GLAD 설정 ###
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

### IMGUI 설정 ###
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

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

### 프로젝트 소스 설정 ###
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SRC_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(GS_VIEWER ${SRC_FILES} ${SRC_HEADERS})
target_include_directories(GS_VIEWER PRIVATE ${CMAKE_SOURCE_DIR}/src)

### OS별 설정 ###
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)

    find_package(OpenGL REQUIRED)

    # Homebrew를 통해 glfw가 설치되었다고 가정. (만약 find_package로 처리 가능하면 사용)
    # 예: find_library(GLFW_LIB glfw3 HINTS /opt/homebrew/lib)
    set(EXTERNAL_LINK_LIBS OpenGL::GL /opt/homebrew/lib/libglfw.3.dylib imgui glad)

    # 쉐이더 복사
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.vert DESTINATION ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.frag DESTINATION ${CMAKE_BINARY_DIR}/shaders)

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})
elseif (WIN32)
    # 윈도우의 경우 OpenGL, glfw, imgui, glad 경로를 상황에 맞게 설정해야 함
    # 예를 들어, glfw를 별도로 설치했거나 vcpkg 사용 시 find_package(glfw3 CONFIG REQUIRED) 등
    # 아래는 예시
    find_package(OpenGL REQUIRED)
    # find_package(glfw3 REQUIRED) 등
    # 예: set(EXTERNAL_LINK_LIBS OpenGL::GL glfw imgui glad)
    
    # 쉐이더 복사
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.vert DESTINATION ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.frag DESTINATION ${CMAKE_BINARY_DIR}/shaders)

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    # find_package(glfw3 REQUIRED)
    # set(EXTERNAL_LINK_LIBS OpenGL::GL glfw imgui glad)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.vert DESTINATION ${CMAKE_BINARY_DIR}/shaders)
    file(COPY ${CMAKE_SOURCE_DIR}/shaders/gaussian.frag DESTINATION ${CMAKE_BINARY_DIR}/shaders)

    target_link_libraries(GS_VIEWER PRIVATE ${EXTERNAL_LINK_LIBS})
endif()