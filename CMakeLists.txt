CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

PROJECT(GSViewer)

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

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shader)

  file(COPY ${CMAKE_SOURCE_DIR}/shader/vshader.vert 
      DESTINATION ${CMAKE_BINARY_DIR}/shader)
  file(COPY ${CMAKE_SOURCE_DIR}/shader/fshader.frag 
      DESTINATION ${CMAKE_BINARY_DIR}/shader)

  # GLFW 경로 설정
  ADD_EXECUTABLE(gs_viewer ${SRC} ${HDR})
  TARGET_LINK_LIBRARIES(gs_viewer ${EXTERNAL_LINK_LIBS})

  add_executable(test_plyreader 
    tests/test_plyread.cpp
  )

  add_executable(test_meshreader tests/test_meshreader.cpp MeshReader.cpp textfile.cpp)
  target_link_libraries(test_meshreader ${EXTERNAL_LINK_LIBS})

# Windows에서 freeglut 사용
ELSEIF (WIN32)
  # Windows 전용 설정
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/freeglut/include ${CMAKE_SOURCE_DIR}/glew/inc/)
  LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/freeglut/lib ${CMAKE_SOURCE_DIR}/glew/lib)

  # freeglut과 GLEW 라이브러리 링크
  SET(EXTERNAL_LINK_LIBS freeglut.lib glew32.lib)

  ADD_EXECUTABLE(gs_viewer ${SRC} ${HDR})
  TARGET_LINK_LIBRARIES(gs_viewer ${EXTERNAL_LINK_LIBS})

  FILE(GLOB_RECURSE DLLS ${CMAKE_SOURCE_DIR}/freeglut/*.dll ${CMAKE_SOURCE_DIR}/glew/*.dll)
  SOURCE_GROUP("DLL Files" FILES ${DLLS})
  FILE(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${CMAKE_SOURCE_DIR}/shader/vshader.vert 
      DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${CMAKE_SOURCE_DIR}/shader/fshader.frag 
      DESTINATION ${CMAKE_BINARY_DIR})

# Linux (추가적인 설정 필요시)
ELSEIF (UNIX)
  # Linux 전용 설정을 여기에 추가할 수 있습니다.
ENDIF ()

