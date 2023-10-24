cmake_minimum_required(VERSION 3.10)

project(GAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories("/usr/include/fmod/")
set(FMOD_LIBRARIES "/usr/lib/libfmod.so")

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  SDL2_image
  ${FMOD_LIBRARIES}
  box2d
)
