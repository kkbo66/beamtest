cmake_minimum_required(VERSION 3.1)

project("ECALDataProcess")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GIBCXX_USE_CXX11_ABI=0")

#add_subdirectory(csv)

#SET(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(ECALTrigger "ECALTrigger.cc" ${sources} ${headers})
target_link_libraries(ECALTrigger ${ROOT_LIBRARIES})

# get template paramters [par0:decoded file]
add_executable(SeekParameters "SeekParameters.cc" ${sources} ${headers})
target_link_libraries(SeekParameters ${ROOT_LIBRARIES})

# draw and fit MIP spectra [par0:decoded file; par1: peak cut value for HG]
add_executable(FitMip "FitMip.cc" ${sources} ${headers})
target_link_libraries(FitMip ${ROOT_LIBRARIES})

add_executable(DrawValue "DrawValue.cc" ${sources} ${headers})
target_link_libraries(DrawValue ${ROOT_LIBRARIES})

# draw reconstructed energy distribution for electron [par0: reconstructed file; par1: electron beam energy]
add_executable(DrawEnergy "DrawEnergy.cc" ${sources} ${headers})
target_link_libraries(DrawEnergy ${ROOT_LIBRARIES})

# draw reconstructed position distribution for electron [par0: ecal file; par1: tracker file]/[par0: data path]
add_executable(DrawPosition "DrawPosition.cc" ${sources} ${headers})
target_link_libraries(DrawPosition ${ROOT_LIBRARIES})

# draw superimposed waveforms of all channels
add_executable(Draw5x5SumWave "Draw5x5SumWave.cc" ${sources} ${headers})
target_link_libraries(Draw5x5SumWave ${ROOT_LIBRARIES})

# draw noise, temperature and H/L gain ratio of all channels
add_executable(Draw5x5NoiseTempGratio "Draw5x5NoiseTempGratio.cc" ${sources} ${headers})
target_link_libraries(Draw5x5NoiseTempGratio ${ROOT_LIBRARIES})

# draw and fit MIP spectra of all channels
add_executable(Draw5x5MIPSpectra "Draw5x5MIPSpectra.cc" ${sources} ${headers})
target_link_libraries(Draw5x5MIPSpectra ${ROOT_LIBRARIES})

# electric calibration 
add_executable(ElectricCalibrate "ElectricCalibrate.cc" ${sources} ${headers})
target_link_libraries(ElectricCalibrate ${ROOT_LIBRARIES})

file(GLOB SHELL_SCRIPTS "${CMAKE_SOURCE_DIR}/*.sh")

file(COPY ${SHELL_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data")




