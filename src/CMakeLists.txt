project(save-archiver_PROJECT)
set(EXECUTABLE_NAME save-archiver)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

list(APPEND SOURCES
  main.cpp
  KNearestOcr.cpp
  Config.cpp
  )

add_executable(${EXECUTABLE_NAME} ${SOURCES})

# simplefilewatcher
set(sfw_DIR ${CMAKE_SOURCE_DIR}/third_party/simplefilewatcher)
include_directories(${sfw_DIR}/includes)

file(GLOB sfw_SRC ${sfw_DIR}/source/*.cpp)

add_library(sfw ${sfw_SRC})
set_target_properties(sfw PROPERTIES COMPILE_FLAGS "-w -Wunused-parameter -static") # Suppress Warnings
list(APPEND LIBRARIES sfw)
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
list(APPEND LIBRARIES ${X11_LIBRARIES})

# OpenCV
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIR})
list(APPEND LIBRARIES ${OpenCV_LIBRARIES})

# Tesseract
find_package( PkgConfig REQUIRED)
pkg_search_module( TESSERACT REQUIRED tesseract )
pkg_search_module( LEPTONICA REQUIRED lept )

include_directories( ${TESSERACT_INCLUDE_DIRS} )
include_directories( ${LEPTONICA_INCLUDE_DIRS} )

link_directories( ${TESSERACT_LIBRARY_DIRS} )
link_directories( ${LEPTONICA_LIBRARY_DIRS} )

list( APPEND LIBRARIES ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES} )

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARIES})

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/build)

