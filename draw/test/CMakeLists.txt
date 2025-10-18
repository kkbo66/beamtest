cmake_minimum_required (VERSION 3.1)

project ("Draw")


#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
# find_package(ROOT COMPONENTS RooFit RooFitCore Minuit REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_executable(checkamp "checkamp.cxx")
target_link_libraries(checkamp ${ROOT_LIBRARIES})

