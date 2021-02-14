#include "sensor_detector.hpp"

subfeature_bundle::subfeature_bundle(std::string fl, int curr_sf_num, const std::vector<double>& sensor_values) {
    this->feature_label = fl;
    this->curr_sf_number = curr_sf_num;

    // Each sensor may or may not return a current, high, and critical. Handle accordingly
    int s_count = sensor_values.size();

    if (s_count >= 1) {
        this->curr_temp = sensor_values[0];
    }

    if (s_count >= 2) {
        this->high_temp = sensor_values[1];
    }

    if (s_count >= 3) {
        this->crit_temp = sensor_values[2];
    }
}

chip_bundle::chip_bundle(const sensors_chip_name* chip_name, std::string adapter_name) {
    this->chip_name = chip_name;
    this->adapter_name = adapter_name;
}

SensorDetector::SensorDetector(bool is_loop, double hi_thresh, double crit_thresh) {
    this->is_loop = is_loop;
    this->is_init = (sensors_init(nullptr) == 0);
    this->hi_thresh = hi_thresh;
    this->crit_thresh = crit_thresh;
    
    if (is_init)
        load_chips();
}

SensorDetector::~SensorDetector() {
    sensors_cleanup();
}

bool SensorDetector::is_initialized() {
    return is_init;
}

void SensorDetector::print_info() {
    for (auto& chip : chips) {        
        // Print adapter information
        std::cout << chip.ir_chip_name << " (Adapter: " << chip.adapter_name << ")\n";
        
        if (is_loop)
            printf("\t%-20s%-15s%-15s%-15s\n", "Label", "Current", "Minimum", "Maximum");

        // Print subfeature with proper ANSI escape code coloring
        for (auto& sf : chip.subfeatures) {
            // Update current temperature
            sensors_get_value(chip.chip_name, sf.curr_sf_number, &sf.curr_temp);            
            printf("\t%-20s", sf.feature_label.c_str());

            double hi_thresh = (this->hi_thresh == -1.0) ? sf.high_temp : this->hi_thresh;
            double crit_thresh = (this->crit_thresh == -1.0) ? sf.crit_temp : this->crit_thresh;
            const char* COL_ANSI = (sf.curr_temp < hi_thresh) ?
                NOC_ANSI : 
                (sf.curr_temp >= hi_thresh && sf.curr_temp < crit_thresh) ?
                    ORG_ANSI :
                    RED_ANSI;

            if (is_loop) {
                if (sf.curr_temp < sf.min_temp)
                    sf.min_temp = sf.curr_temp;
                else if (sf.curr_temp > sf.max_temp)
                    sf.max_temp = sf.curr_temp;

                printf("%s%-15s%s%-15s%-15s\n", COL_ANSI, get_temp_str(sf.curr_temp).c_str(), NOC_ANSI,
                    get_temp_str(sf.min_temp).c_str(), get_temp_str(sf.max_temp).c_str());
            } else {
                printf("%s%-15s%s\n", COL_ANSI, get_temp_str(sf.curr_temp).c_str(), NOC_ANSI);
            }
        }
    }
}

void SensorDetector::load_chips() {
    // Must populate chips once per program execution
    const sensors_chip_name* chip_name;
    int nr = 0;
    
    while ((chip_name = sensors_get_detected_chips(nullptr, &nr))) {            
        chip_bundle curr_chip{chip_name, sensors_get_adapter_name(&chip_name->bus)};

        // Attempt to resolve adapter name            
        // Get chip's internal rep name
        curr_chip.ir_chip_name = get_chip_name_safe(chip_name);
        const sensors_feature* sf;
        int fnr = 0;
        while ((sf = sensors_get_features(chip_name, &fnr))) {
            // Build out subfeatures with featur

            std::string feature_label = get_extended_label(chip_name, sf);

            // Collect current, high, and crit temperatures per sub
            const sensors_subfeature* ssf;
            int sfnr = 0;               

            std::vector<double> sensor_values{}; 
            int curr_sf_num = -1;

            while ((ssf = sensors_get_all_subfeatures(chip_name, sf, &sfnr))) {
                double value;
                sensors_get_value(chip_name, ssf->number, &value);
                sensor_values.push_back(value);

                if (curr_sf_num == -1)
                    curr_sf_num = ssf->number;                
            } 
            
            curr_chip.subfeatures.emplace_back(feature_label, curr_sf_num, sensor_values);
        }
        chips.push_back(curr_chip);
    }
}
/**
 * Queries for length of chip name before acquiring it in a heap-allocated string
 * Prevents buffer overflow attacks
**/
std::string SensorDetector::get_chip_name_safe(const sensors_chip_name* chip) {
    std::string chip_name;
    std::size_t name_len = sensors_snprintf_chip_name(nullptr, 0, chip) + 1;

    if (name_len >= 0) {
        // Success
        char* temp_chip_name_c_str = new char[name_len];
        sensors_snprintf_chip_name(temp_chip_name_c_str, name_len, chip);
        chip_name = std::string{temp_chip_name_c_str, name_len};
        delete[] temp_chip_name_c_str;        
    } else {
        // On error, return a newly allocated copy of the chip prefix (in case they free)
        fprintf(stderr, "Unable to retrieve chip name for %s\n", chip->prefix);
        std::size_t prefix_len = strnlen(chip->path, 32);
        chip_name = std::string{chip->prefix, prefix_len};
    }

    return chip_name;
}

/**
 * Queries for label of subfeature and extends its heap allocation to include ':'
 * You have to free the returned name yourself!
**/
std::string SensorDetector::get_extended_label(const sensors_chip_name* chip_name, const sensors_feature* sf) {
    char* feature_label_c_str = sensors_get_label(chip_name, sf);
    std::string feature_label;

    if (feature_label_c_str == nullptr) {
        // Unable to find label, just use sf name
        std::size_t sf_name_len = strnlen(sf->name, 32);
        feature_label = std::string(sf->name, sf_name_len);
    }

    std::size_t feature_label_len = strnlen(feature_label_c_str, 32); // Include new colon and null terminator
    feature_label = std::string{feature_label_c_str, feature_label_len} + ':';
    
    free(feature_label_c_str);

    return feature_label;
}

std::string SensorDetector::get_temp_str(double temperature_value) {
    std::size_t req_len = snprintf(nullptr, 0, "+%0.1f°C", temperature_value) + 1;
    char* temp_c_str = new char[req_len];
    snprintf(temp_c_str, req_len, "+%0.1f°C", temperature_value);
    std::string temp_str{temp_c_str, req_len};
    delete[] temp_c_str;
    return temp_str;
}