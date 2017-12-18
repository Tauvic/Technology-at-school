# Intro

<img align="right" height="200" src="./images/MeLineFollowerSensor_2.png">

* Goal: Provide an easy to use robot platform to schools for teaching technology, programming and robotics.
* Aproach: Use the Makeblock mBot robot, add a user friendly interface and make it move reliable and semi-autonomous.

# Getting started

* Get a robot
  * Buy a [Makeblock](http://www.makeblock.com/) mBot robot
  * If you don't have one use a simulated robot ([V-REP](../V-REP))
* Programming
  * Read [get started](./get_started)
  * https://www.codingkids.nl/mBot-lessen.html
* Read
  * My blog on [robotics](https://tauvicr.wordpress.com/tag/robotics/)
  * [mBlock (Scratch) & mBot "Rocks with the Robots"](https://cdn.shopify.com/s/files/1/1003/6824/files/mBlock_Rocks_with_Robots_Makeblockshop_Compressed_Small.pdf?12769062248011349873)"
  * [Scratch "The Adventures of Mike"](https://cdn.shopify.com/s/files/1/1003/6824/files/Scratch_2.0_The_Adventures_of_Mike_Makeblockshop_Compressed.pdf?12769062248011349873)
  * [Openlab](http://openlab.makeblock.com/) for interesting projects
  * [Physcomp](https://itp.nyu.edu/physcomp/) for in depth about robot hardware and programming

# Development

My idea is to develop a robot that can perfom tasks such as a clerk in a warehouse driving around, picking up goods and delivering them. to achieve that i have to improve the standard mBot robot. For progress check out [projects](https://github.com/Tauvic/Technology-at-school/projects)

* Easy robot programming
  * RoboCode Basic (a robot that can do basic line following with a 6 sensor line sensor array)
  * RoboCode Advanced (semi-autonomous line following, detect road crossings, drives with a PID controller)
  * RoboGrip (manus roboticus, includes a gripper hand)
  * RoboVision (visis roboticus)
* Improve editor
  * Snap4Arduino https://snap.berkeley.edu/SnapManual.pdf
  * http://www.snap-apps.org/
* PID controller
  * Arduino https://playground.arduino.cc/Code/PIDLibrary
  * Background info (http://brettbeauregard.com/blog/category/pid/)
* Sensors
  * Line following array
  * NXP FXAS21002C Gyroscope
  * Energy management battery voltage / current http://www.vwlowen.co.uk/arduino/current/current.htm
  * Color sensor http://forum.makeblock.com/t/extension-for-color-sensor-tcs-34725/7331/23
* Actuators
  * Gripper http://forum.makeblock.com/t/mcore-robotic-arm-gripper/5821/4
  * Better control over motor
    * https://www.precisionmicrodrives.com/application-notes/ab-026-sensorless-speed-stabiliser-for-a-dc-motor
    * https://www.precisionmicrodrives.com/application-notes/ab-021-measuring-rpm-from-back-emf
    * https://robidouille.wordpress.com/2009/09/20/back-emf/
    * https://arduino-info.wikispaces.com/SmallSteppers


![Scratch Programming](https://pbs.twimg.com/media/DOt5ZuGVQAAhaVC.jpg)
