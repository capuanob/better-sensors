#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <sensors/sensors.h>
#include <string>
#include <thread>
#include <vector>
/**
 * Structure to hold identifying and temperature information of a libsensor subfeature
**/


struct subfeature_bundle {
public:
    std::string feature_label{}; // Label of associated feature
    int curr_sf_number{}; // Identifying number of the current temperature subfeature
    double curr_temp{}; // Current temperature
    double high_temp = std::numeric_limits<double>::max(); // Threshold for orange display
    double crit_temp = std::numeric_limits<double>::max(); // Threshold for red display
    double min_temp = std::numeric_limits<double>::max();  // Minimum temperature encountered on this subfeature
    double max_temp = std::numeric_limits<double>::min();  // Maximum temperature encountered on this subfeature

    subfeature_bundle(std::string fl, int curr_sf_num, const std::vector<double>& sensor_values);
    
    void add_temp(double temp, int ssf_number) {
        long i = temp_idx++;

        switch(i) {
        case 0:
            curr_temp = temp;
            curr_sf_number = ssf_number;
            break;
        case 1:
            high_temp = temp;
            break;
        case 2:
            crit_temp = temp;
            break;
        default:
            break;
        }
    }

private:
    int64_t temp_idx; // Track where to insert next temperature
};


/**
 * Holds all information needed and requested for each chip
**/
struct chip_bundle {
public:
    const sensors_chip_name* chip_name;
    std::string adapter_name;
    std::string ir_chip_name; // Pointer to internal representation of chip name
    std::vector<subfeature_bundle> subfeatures;

    chip_bundle(const sensors_chip_name* chip_name, std::string adapter_name);
};

/**
 * Handles construction and destruction of libsensor configuration file and
 * abstracts any interaction with libsensor
**/
class SensorDetector {
public:
    SensorDetector(bool is_loop, double hi_thresh, double crit_thresh);
    ~SensorDetector();

    bool is_initialized();
    void print_info();
private:
    bool is_init{};
    bool is_loop{};
    double hi_thresh{};
    double crit_thresh{};
    const char* NOC_ANSI = "\033[0m";    // Print normal
    const char* ORG_ANSI = "\033[0;33m"; // Print orange
    const char* RED_ANSI = "\033[0;31m"; // Print red    
    std::vector<chip_bundle> chips{};

    void load_chips();
    std::string get_chip_name_safe(const sensors_chip_name* chip_name);
    std::string get_extended_label(const sensors_chip_name* chip_name, const sensors_feature* sf);
    std::string get_temp_str(double temperature_value);
};