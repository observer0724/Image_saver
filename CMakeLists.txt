cmake_minimum_required(VERSION 2.8.3)
project(moving_test)

find_package(roscpp REQUIRED)
find_package(roslib REQUIRED)
find_package(catkin REQUIRED COMPONENTS cwru_davinci_traj_streamer roslib)


find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
add_definitions(${EIGEN_DEFINITIONS})

#find_package(catkin_simple REQUIRED)

#catkin_simple()

include_directories(${catkin_INCLUDE_DIRS} include)
catkin_package(
	INCLUDE_DIRS ${catkin_INCLUDE_DIRS}
	DEPENDS roscpp roslib
	LIBRARIES playfile_record
	CATKIN_DEPENDS cwru_davinci_traj_streamer roscpp roslib
)


# Executables


add_executable(moving_test src/moving_test.cpp)

target_link_libraries(moving_test ${catkin_LIBRARIES})
