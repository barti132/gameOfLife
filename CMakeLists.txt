cmake_minimum_required(VERSION 3.15)
project(gameOfLife C)

set(CMAKE_C_STANDARD 11)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

# Add an executable compiled from hello.c
ADD_EXECUTABLE(gameOfLife main.c)

# Link the target to the GTK+ libraries
TARGET_LINK_LIBRARIES(gameOfLife ${GTK3_LIBRARIES})