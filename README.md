# better-sensors
An improvement upon the Linux utility sensors, providing min/max tracking, efficient looping, colored output, and configurability.

## Installation
  better-sensors depends upon `libsensors` and, as such, only supports Linux
  - Install `libsensors5` and `libsensors-dev` using your favorite package manager
  - Clone this repository
  - Execute `cd better-sensors && cmake . && make`
    
## Usage
  For ease of use, it is recommended to move the better-sensors binary from `bin/better-sensors` into your PATH.
  - `better-sensors -h` for help and options.
  
  ### Example
  To achieve continuous execution and colored output, use `better-sensors --loop -t 65:75`  
    - Current temperature isn't colored if
    - Current temperature is orange if
    - Current temperature is red if 
