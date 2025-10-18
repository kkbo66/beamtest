cmake_minimum_required (VERSION 3.1)

project ("Draw")


#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
# find_package(ROOT COMPONENTS RooFit RooFitCore Minuit REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_executable(drawwave "drawwave.cxx")
target_link_libraries(drawwave ${ROOT_LIBRARIES})

add_executable(draw5x5 "draw5x5.cxx")
target_link_libraries(draw5x5 ${ROOT_LIBRARIES})

add_executable(drawunwave "drawunwave.cxx")
target_link_libraries(drawunwave ${ROOT_LIBRARIES})

