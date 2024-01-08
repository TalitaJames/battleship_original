cmake_minimum_required(VERSION 3.21)

project(battleship)

file(GLOB srcs src/*.cpp src/*.h)

# ---- JSON BITS ----
include(FetchContent)
FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

#  ---- Thread bits ----
set(CMAKE_THREAD_PREFER_PTHREAD ON)                                                                                                                                                                                                           
set(THREADS_PREFER_PTHREAD_FLAG ON)                                                                                                                                                                                                           
find_package(Threads REQUIRED)                                                                                                                                                                                                                
add_executable(${PROJECT_NAME} ${srcs})


target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads glaze::glaze)