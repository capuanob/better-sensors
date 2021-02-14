include_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lsensors")

add_executable(better-sensors better-sensors.cpp sensor_detector.cpp)