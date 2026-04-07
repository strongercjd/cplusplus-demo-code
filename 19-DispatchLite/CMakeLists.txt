cmake_minimum_required(VERSION 3.10)
project(job_schedule_design_demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(job_schedule_demo
  main.cpp
)

if(UNIX)
  target_link_libraries(job_schedule_demo PRIVATE pthread)
endif()
