# Project - Self-driving RC car - Ela21, YRGO
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/car.jpg" width="600" >
Fig.1 - The final version of our car.

## Introduction
The aim of this school project was to transform a radio-controlled car into a self-driving car to get around a race track. But the desire to learn led to completely different goals. As I was responsible for software development, I took the opportunity to fill these weeks with as many new experiences as I could come up with.

#### Development environment
All code was written in Visual Studio Code using the [PlatformIO extension](https://platformio.org/) and the [wizio-pico](https://github.com/Wiz-IO/wizio-pico)

#### CARISMA GT24TR 1:24 4WD MICRO TRUGGY RTR
The car that was chosen met the competition rules size requirements. Also appeared to have sufficient ground clearance,
strong brushless motor and support for 2s lipo battery.

#### Arduino Nano RP2040 Connect
Tired of the arduino uno we used in school, I looked for a more powerful MCU. Preferably with floating point support which could come in use for the pid regulation.

#### VL53L0X TOF - distance sensor
Two sensors were found among the leftovers from last year's students. The official drivers were really heavy. But found [this lightweight driver](https://github.com/artfulbytes/vl6180x_vl53l0x_msp430) made by artfulbytes.
This lightweight bare metal drivers were easy to follow so i made a port but slightly different..

#### MPU-6050 IMU
This Six-Axis gyro was also found among the leftovers. All info about registers was clearly written in the data sheet but it was harder to understand how to use this device properly. A few days were clearliy not enough to master this device.

#### QUICRUN 10BL60 ESC 1/10
Cheap and relatively small ESC was bought to come
further in the project. Had to remove the brake function to enable a proper reverse. The ESC was controlled
with a 50Hz pwm signal that was based on a duty cycle of 1.5ms.

#### Savöx SH-0257MG
The 3-pole servo has built-in control and therefore requires
only power and a pwm signal to change its position.
The servo was controlled with a 240Hz pwm signal that was based on a
duty cycle of 1.5 ms.

## Drivetrain issues 
Only a fraction of the power from the brushless motor was used. The reason was the
high ratio to the transmission(5:1). When the power supplied to the engine was too high, the car ran the risk of
either fly over the obstacle or do a backflip, see Movie.6. As the power was too low, the car could not pull itself up
the obstacle. The difference between too much and too little power was very low and was hard to adjust. 
More time, a voltage regulator and a 10T gear should have helped alot. (Watch the movies at the end for more details.)

<br><br><br><br>
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/schematics.png" width="800" >
<br>Fig.2 - schematics.

<br><br><br><br>
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/pin.jpg" width="600">
<br>Fig.3 - pinplaning.

<br><br><br><br>
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/oscilloscope.jpg" width="600" >
<br>Fig.4 - This project actually inspired me to buy my own oscilloscope. A necessity if you want to debug or verify the communication regardless of whether it is i2c or pwm signals. Can highly recommend the 1104X-E.

## flowcharts

<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/main.png" width="600" >
Fig.5 - Overview.
<br><br><br><br>
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/pid.png" width="600" >
Fig.6 - The pid regulation that turned out to work very well.

<br><br><br><br>
<img src="https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/speedctrl.png" width="600" >
Fig.7 - The speed control function..  

<br><br><br><br>
## Videos
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov1.jpg)](https://youtu.be/T6Ry1QDTvqE)
Movie.1 - First testrun with drivetrain. However, the pid regulation was already tested by pushing the car forward by hand.

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov2.jpg)](https://youtu.be/GdP7vjcRZxI)
Movie.2 - Obstacle avoidance system. Fun to develop. But caused issues when discovered the uphill obstacles. 

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov3.jpg)](https://youtu.be/kByNOn7Lbe0)
Movie.3 - Gyro in action. To solve the issue that was caused by the "obstacle avoidance system" a very experimental feature implemented 
like 2 days before the competiotion. Not ideal but fun tho.

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov4.jpg)](https://youtu.be/v_iZuXGWwUY)
Movie.4 - Uphill start with gyro. worked like a charm.

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov5.jpg)](https://youtu.be/X4wRlS_64eo)
Movie.5 - Just before the competition. Top notch pid regulation. Without competitors though. Turned out that the 
"obstacle avoidance system" was a huge disadvantage in the competition. XD

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov6.jpg)](https://youtu.be/dhRyVaw5hpI)
Movie.6 - High voltage issue. Too high voltage caused by a new charger. A voltage regulator would solve many problems and make adjustments more easy.

<br><br><br><br>
[![Watch the video](https://github.com/peter-strom/elektronikprojekt/blob/cff85e32945de04a21a150acce7a834be64f445f/doc/mov7.jpg)](https://youtu.be/kcBSzKzJYdA)
Movie.7 - ESC issue. Too fast restart sets the Electronic speed controller in a "slow-mode". We knew the problem but thought we could handle it by wait 10 seconds every restart. 
Probalby caused by the capacitors and a better solution should have been to discharge them on shutdown.
