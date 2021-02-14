#include <iostream>
#include "sensor_detector.hpp"

bool parse_flags(int argc, char** argv);

constexpr const int VERSION_MAJOR = 1;
constexpr const int VERSION_MINOR = 0;
constexpr const int VERSION_PATCH = 0;

bool should_loop = false;
double hi_thresh = -1.0;
double crit_thresh = -1.0;

int main(int argc, char** argv) {
    bool should_loop = parse_flags(argc, argv);
    SensorDetector sensor_detector{should_loop, hi_thresh, crit_thresh};

    if (!sensor_detector.is_initialized()) {
        std::cerr << "Unable to properly initialize configuration file. No recovery possible!\n";
        return 1;
    }

    if (should_loop) {
        std::system("clear");
        while(true) {
            sensor_detector.print_info();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::system("clear");
        }
    } else {
        sensor_detector.print_info();
    }
}

/**
 * TODO: Add some form of configuration for CRIT and HIGH temperatures
 * TODO: Add looping mode, tracking subfeature min and max
**/

bool parse_flags(int argc, char** argv) {
    // No arguments passed
    if (argc <= 1) {       
        return should_loop;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print help information
            printf("%s\n", "Usage: better-sensors [OPTION]...");
            printf("%-20s %s\n", "-h, --help", "Display this help text");
            printf("%-20s %s\n", "-v, --version", "Display the program's version");
            printf("%-20s %s\n", "-l, --loop", "Operate indefinitely, tracking minimum and maximum temperatures");
            printf("%-20s %s\n", "-t [hi]:[crit]", "Optional high and critical thresholds can be specified for colored output.");

            exit(0);
        } else if (strncmp(argv[i], "-v", sizeof("-v")) == 0 || strncmp(argv[i], "--version", sizeof("--version")) == 0) {
            printf("better-sensor, v%d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
            exit(0);
        } else if (strncmp(argv[i], "-l", sizeof("-v")) == 0 || strncmp(argv[i], "--loop", sizeof("--loop")) == 0) {
            should_loop = true;
        } else if (strncmp(argv[i], "-t", sizeof("-t")) == 0) {
            if (i + 1 < argc) {
                // Ensure proper format and numeric 0.0 <= x <= 100.0      
                const char* threshold = argv[i + 1];
                if (sscanf(threshold, "%lf:%lf", &hi_thresh, &crit_thresh) != 2 
                    || (hi_thresh >= crit_thresh)
                    || (hi_thresh < 0.0 || crit_thresh > 100.0)
                ) {
                    fprintf(stderr, "Improper syntax, must be numeric 0.0<=n<=100.0\n");
                    exit(1);
                }
                i += 1;
            } else {
                fprintf(stderr, "Must specify high and critical thresholds in form [hi]:[crit]\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Unknown argument, try better-sensors -h\n");
            exit(1);
        }
    }
    return should_loop;
}