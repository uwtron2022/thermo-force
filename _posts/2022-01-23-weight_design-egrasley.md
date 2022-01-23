--- 
title: "Weight Feedback – Jan 16-22, 2022"
date: 2022-01-23 00:00:01 -0000
categories: weight-feedback
author: Ethan Grasley
---

## Current Events are Setting Plans in Motion

Work this week consisted of the creation of 3D models of the electronics, creation of a motor control simulation, and creation of the final control program. 

The 3D electronics models are being used to create a mounting plate for the electronics. This mounting plate will (ideally) be placed in an IP65 rated container. This container will then be adapted to fit to a backpack so the electronics + battery that run the feedback can move with the person, instead of being stationary. 

The motor control simulation will be used to design the control equations that will run the motor at the required current/force. Simulation is being used to ensure that the system is closed-loop stable, has minimal steady state error, and rejects disturbances (meaning the user pulling on the cable will be accounted for when trying to reach a given current/force). 

Finally, the outline of the final program that will be used to provide temperature and weight feedback has been created. This includes creation of all global variables needed, initialization of peripherals, and setting up the BLE communications module. 

![](../../assets/images/Jan16-22_WeightDesignElecModel.png)
