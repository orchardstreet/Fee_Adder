cmake_minimum_required(VERSION 3.0)
project(Payment_Manager LANGUAGES C)

FIND_PACKAGE ( PkgConfig REQUIRED )
PKG_CHECK_MODULES( GTK REQUIRED gtk+-3.0 ) # or use gtk+-2.0, etc...
INCLUDE_DIRECTORIES ( ${GTK_INCLUDE_DIRS} )
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_subdirectory(src)
install(TARGETS Payment_Manager RUNTIME DESTINATION bin)

