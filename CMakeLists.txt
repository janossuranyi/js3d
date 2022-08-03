cmake_minimum_required(VERSION 3.7)

project(JS3d VERSION 1.0) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/") 

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
# find_package(assimp REQUIRED HINTS ${ASSIMP_DIR}) 

add_definitions(
  -DGLEW_STATIC
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

# JS3D
file(GLOB JS3D_SOURCE  js3d/*.cpp)
file(GLOB JS3D_INCLUDE js3d/*.h)
file(GLOB JS3D_RENDERER_INC js3d/renderer/opengl/*.h)
file(GLOB JS3D_RENDERER_SRC js3d/renderer/opengl/*.cpp)

file(GLOB SOIL2_INCLUDE external/soil2/incs/*.h)
file(GLOB SOIL2_SOURCE external/soil2/srcs/*.c)

add_library(JS3D STATIC 
  ${JS3D_SOURCE}
  ${JS3D_INCLUDE}
  ${JS3D_RENDERER_INC}
  ${JS3D_RENDERER_SRC}
)

add_library(SOIL2 STATIC
  ${SOIL2_INCLUDE}
  ${SOIL2_SOURCE}
)

add_library(glew STATIC
  external/glew-2.1.0/src/glew.c
  external/glew-2.1.0/include/GL/glew.h
)

add_library(stb_image STATIC
  external/stb/stb_image.h
  external/stb/stb_image.c
)

add_library(tinygltf STATIC
  external/tinygltf/tiny_gltf.h
  external/tinygltf/tiny_gltf.cpp
)

include_directories(
  ./
  js3d/
  js3d/renderer/opengl/
  external/glew-2.1.0/include/
  external/glm/
  external/stb/
  external/soil2/incs/
  external/tinygltf/
  ${SDL2_INCLUDE_DIR}
)

link_directories(
#  ${ASSIMP_LIBRARY_DIRS}
)

file(GLOB SHADERS assets/shaders/*.glsl)
file(GLOB SHADERS_INC assets/shaders/*.inc)

add_executable(demo	
  demo/demo.cpp
  demo/demo.h
  demo/render_worker.h
  demo/render_worker.cpp
  ${SHADERS}
)

target_link_libraries(demo
  JS3D
  glew
  stb_image
  tinygltf
  ${OPENGL_LIBRARY}
  ${SDL2_LIBRARIES}
)

if(WIN32)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
  else()
    set(ARCH "x86")
  endif()

#  add_custom_command(TARGET demo POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#     "${assimp_DIR}/../../../bin/assimp${ASSIMP_LIBRARY_SUFFIX}.dll"
#     $<TARGET_FILE_DIR:demo>)

  add_custom_command(TARGET demo POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy
     "${SDL2_INCLUDE_DIR}/../lib/${ARCH}/SDL2.dll"
     $<TARGET_FILE_DIR:demo>)
endif()

set(RENDERER
  ${JS3D_RENDERER_INC} ${JS3D_RENDERER_SRC}
)

source_group("Shaders" FILES ${SHADERS} )
source_group("renderer\\OpenGL" FILES ${RENDERER} )

