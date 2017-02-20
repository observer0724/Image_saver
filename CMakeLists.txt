cmake_minimum_required(VERSION 2.8.3)
project(test_scale)



find_package(OpenCV REQUIRED)

#find_package(catkin_simple REQUIRED)
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

find_package(catkin REQUIRED COMPONENTS message_generation std_msgs sensor_msgs 
cwru_opencv_common 
#tool_model
roscpp
)

#catkin_simple()
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS} include)
catkin_package(CATKIN_DEPENDS message_runtime std_msgs sensor_msgs roscpp)
# example boost usage
# find_package(Boost REQUIRED COMPONENTS system thread)

# C++0x support - not quite the same as final C++11!
# use carefully;  can interfere with point-cloud library
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pg -Q" )
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -Q")


# Libraries: uncomment the following and edit arguments to create a new library
# add_library(my_lib src/my_lib.cpp)   


add_executable(test_scale src/test_scale.cpp)
#the following is required, if desire to link a node in this package with a library created in this same package
# edit the arguments to reference the named node and named library within this package
# target_link_library(example my_lib)
target_link_libraries(test_scale ${OpenCV_LIBRARIES} ${catkin_LIBRARIES})

#cs_install()
#cs_export()

