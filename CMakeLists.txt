
cmake_minimum_required(VERSION 2.8)

project(CompareCenterlines)
include_directories(include, headers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
find_package(VTK REQUIRED)
find_package(OpenCV REQUIRED)

include(${VTK_USE_FILE})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB SOURCES "src/*.cxx")
set(${PROJECT_NAME}_plist_source "src/ConvertPlist.cpp")
set(${PROJECT_NAME}_plist_header "src/ConvertPlist.h")

set(${PROJECT_NAME}_vtp_source "src/VtpHelpper.cpp")
set(${PROJECT_NAME}_vtp_header "src/VtpHelpper.h")

add_library(vtkOBJWriter STATIC ${SOURCES})
add_library(ConvertPlist SHARED ${${PROJECT_NAME}_plist_source}
                                ${${PROJECT_NAME}_plist_header})
add_library(VtpHelpper SHARED ${${PROJECT_NAME}_vtp_source}
                                ${${PROJECT_NAME}_vtp_header})

add_executable(CompareCenterlines MACOSX_BUNDLE CompareCenterlines vtkOBJWriter.cxx)

if(VTK_LIBRARIES)
  target_link_libraries(CompareCenterlines ${VTK_LIBRARIES} ${OpenCV_LIBS} vtkOBJWriter ConvertPlist VtpHelpper)
else()
  target_link_libraries(CompareCenterlines vtkHybrid vtkWidgets vtkOBJWriter ${OpenCV_LIBS} ConvertPlist VtpHelpper)
endif()
