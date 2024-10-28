CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)

SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

PROJECT(Assign_1)

# 공통 include / link directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include) 

# Add source files
FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h *.frag *.vert)

# MacOS에서 GLFW 사용
IF (APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  
  # Homebrew를 사용하여 GLFW와 GLEW 경로 설정
  INCLUDE_DIRECTORIES(/opt/homebrew/include)
  LINK_DIRECTORIES(/opt/homebrew/lib)

  # OpenGL과 GLFW 라이브러리 링크
  FIND_PACKAGE(OpenGL REQUIRED)
  SET(EXTERNAL_LINK_LIBS OpenGL::GL /opt/homebrew/lib/libglfw.3.dylib GLEW)

  # GLFW 경로 설정
  ADD_EXECUTABLE(assign_1 ${SRC} ${HDR} system.cpp)
  TARGET_LINK_LIBRARIES(assign_1 ${EXTERNAL_LINK_LIBS})

# Windows에서 freeglut 사용
ELSEIF (WIN32)
  # Windows 전용 설정
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/freeglut/include ${CMAKE_SOURCE_DIR}/glew/inc/)
  LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/freeglut/lib ${CMAKE_SOURCE_DIR}/glew/lib)

  # freeglut과 GLEW 라이브러리 링크
  SET(EXTERNAL_LINK_LIBS freeglut.lib glew32.lib)

  ADD_EXECUTABLE(assign_1 ${SRC} ${HDR})
  TARGET_LINK_LIBRARIES(assign_1 ${EXTERNAL_LINK_LIBS})

  FILE(GLOB_RECURSE DLLS ${CMAKE_SOURCE_DIR}/freeglut/*.dll ${CMAKE_SOURCE_DIR}/glew/*.dll)
  SOURCE_GROUP("DLL Files" FILES ${DLLS})
  FILE(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR})

# Linux (추가적인 설정 필요시)
ELSEIF (UNIX)
  # Linux 전용 설정을 여기에 추가할 수 있습니다.
ENDIF ()