# better-sensors
An improvement upon the Linux utility sensors, providing min/max tracking, efficient looping, colored output, and configurability.

## Installation
  better-sensors depends upon `libsensors` and, as such, only supports Linux  
  To install better-sensors,
    1. Install `libsensors5` and `libsensors-dev` with your favorite package manager.
    2. Clone this repository.
    3. `cd better-sensors && cmake . && make`
    
## Usage
  For ease of use, it is recommended to move the better-sensors binary from `bin/better-sensors` into your PATH.
  - `better-sensors -h` for help and options.
  
## Example
  `better-sensors --loop -t 65:75` can be used in order to achieve colored output and continuous execution where:  
    - The temperature isn't colored if 65 > temp  
    - The temperature is orange if 65 <= temp < 75  
    - The temperature is red if temp >= 75  
