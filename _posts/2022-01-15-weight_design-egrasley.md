--- 
title: "Weight Feedback – Jan 1-15, 2022"
date: 2022-01-15 00:00:01 -0000
categories: weight-feedback
author: Ethan Grasley
---

## On Soldering and PWM Troubles

Work these past two weeks consisted of soldering custom perf boards used to ease wiring of the Arduino MEGA/Motor Driver to the Bluetooth module, 16-bit ADC, shunt resistor, loadcell, and loadcell amplifier. With the circuit complete, code updating was done to bring the old proof of concept Arduino sketches up to date with the new Arduino MEGA controller. 

During updating it was noticed that the shunt resistor/ADC combo consistently read 0 on the Arduino monitor, while an external multimeter showed a current draw of 240mA. Some testing needs to be done (with an oscilloscope or using the Arduino serial monitor and a low current draw motor), but the issue is that the motor control signal is pulse-width modulated, meaning the measured current is always going to be zero or maximum and is not indicative of the average motor current draw (which is needed for good, closed loop control of the motor). A resistor-capacitor low pass (averaging) filter will need to be added to the motor control circuit, so the current measuring shunt is recording the true average current the motor draws. 

Time was also spent creating a full weight control electrical schematic to be added to the existing temperature control one, and summaries for the previous terms work were created for posting

![](assets/images/Jan1-15_StackWeightCtrlCirc.jpg)
