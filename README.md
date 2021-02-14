# better-sensors
An improvement upon the Linux utility sensors, providing min/max tracking, efficient looping, colored output, and configurability.

## Installation
  better-sensors depends upon `libsensors` and, as such, only supports Linux
  
  To install:
    - `sudo apt-get install libsensors5 libsensors-dev`  
    - Clone this repository
    - `cd better-sensors && cmake . && make`
    
## Usage
  For ease of use, it is recommended to move the better-sensors binary from `bin/better-sensors` into your PATH.
  - `better-sensors -h` for help and options. 
