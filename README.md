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
  
  ### Example
  `better-sensors --loop -t 65:75` 
   Will execute better-sensors in a continuous loop and print:
    - standard-color text if temp < 65
    - <span style="color:orange">orange/span> temperatures if 65 <= temp < 75
    - <span style="color:red"> text</span> temperatures if 75 <= temp
    
