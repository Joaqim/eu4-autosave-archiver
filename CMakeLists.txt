cmake_minimum_required(VERSION 3.5.1)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake_modules")

option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
option(BUILD_STATIC_PIC "Build static libraries and plugins with position-independent code" ON)
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
	find_package(Corrade REQUIRED TestSuite) 
	enable_testing() 
endif()

if(NOT BUILD_STATIC) 
	set(SHARED_OR_STATIC SHARED) 
else() 
	set(SHARED_OR_STATIC STATIC) 
endif()


add_subdirectory(src)

if(BUILD_TESTS)
	#add_subdirectory
endif()
