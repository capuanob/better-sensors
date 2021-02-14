include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(better-sensors better-sensors.cpp sensor_detector.cpp)

find_path(LM_SENSORS_INCLUDE_DIR NAMES sensors/sensors.h)
find_library(LM_SENSORS_LIBRARY NAMES libsensors sensors)
target_link_libraries(better-sensors ${LM_SENSORS_LIBRARY})