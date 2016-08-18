# SiCILIA
Arduino code base supplied by Ala Shaabana was modified to accomodate RPi interface. Python script was added for functionality.

arduino source 160818thu version adds fake mlx array data for testing

python script 160818 version tests for various sources of error causing the device to halt
possible reasons are following:
1. arduino did not flush
2. physical error (if fake data works fine, but sensor output did not work)
3. matching timing between RPi command and arduino output
4. input did not register (unlikely)
5. input doubles
6. output doubles
