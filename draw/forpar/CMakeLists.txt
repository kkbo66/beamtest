cmake_minimum_required (VERSION 3.1)

project ("Fit")


#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
# find_package(ROOT COMPONENTS RooFit RooFitCore Minuit REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_executable(fitmip "fitmip.cxx")
target_link_libraries(fitmip ${ROOT_LIBRARIES})

add_executable(getmip "getmip.cxx")
target_link_libraries(getmip ${ROOT_LIBRARIES})

add_executable(getnosped "getnosped.cxx")
target_link_libraries(getnosped ${ROOT_LIBRARIES})

add_executable(gethmax "gethmax.cxx")
target_link_libraries(gethmax ${ROOT_LIBRARIES})

add_executable(drawhlratio "drawhlratio.cxx")
target_link_libraries(drawhlratio ${ROOT_LIBRARIES})
