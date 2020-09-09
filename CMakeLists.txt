cmake_minimum_required(VERSION 3.9.6)

# ---------------------------------------------------------------------------- #
# PROJECT
# name version language
# ---------------------------------------------------------------------------- #
project(vta VERSION 0.1 LANGUAGES CXX)

# ---------------------------------------------------------------------------- #
# VARIABLE
# alias and configurations
# ---------------------------------------------------------------------------- #
set(MyTarget ${PROJECT_NAME})

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# ---------------------------------------------------------------------------- #
# External dependencies
# ---------------------------------------------------------------------------- #
##
## ilang
##
find_package(ilang REQUIRED)

# ---------------------------------------------------------------------------- #
# TARGET
# executable
# ---------------------------------------------------------------------------- #
add_executable(${MyTarget} 
  app/main.cc
  src/vta_top.cc
  src/top_io.cc
  src/arch_state.cc
  src/internal_state.cc
  src/vta_instr.cc
  src/child_instr.cc
)

target_include_directories(${MyTarget} PRIVATE include)

target_link_libraries(${MyTarget} ilang::ilang)
