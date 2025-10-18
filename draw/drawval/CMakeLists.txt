cmake_minimum_required (VERSION 3.1)

project ("Draw")


#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
# find_package(ROOT COMPONENTS RooFit RooFitCore Minuit REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_executable(drawval "drawval.cxx")
target_link_libraries(drawval ${ROOT_LIBRARIES})

add_executable(drawsumwave "drawsumwave.cxx")
target_link_libraries(drawsumwave ${ROOT_LIBRARIES})

add_executable(draw5x5sumwave "draw5x5sumwave.cxx")
target_link_libraries(draw5x5sumwave ${ROOT_LIBRARIES})

add_executable(drawhlratio "drawhlratio.cxx")
target_link_libraries(drawhlratio ${ROOT_LIBRARIES})

