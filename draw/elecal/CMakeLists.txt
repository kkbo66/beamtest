cmake_minimum_required (VERSION 3.1)

project ("Electron Calibration")


#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
# find_package(ROOT COMPONENTS RooFit RooFitCore Minuit REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_executable(getPeak "getPeak.cxx")
target_link_libraries(getPeak ${ROOT_LIBRARIES})

add_executable(drawCal "drawCal.cxx")
target_link_libraries(drawCal ${ROOT_LIBRARIES})

add_executable(drawPeak "drawPeak.cxx")
target_link_libraries(drawPeak ${ROOT_LIBRARIES})

add_executable(drawledPeak "drawledPeak.cxx")
target_link_libraries(drawledPeak ${ROOT_LIBRARIES})

