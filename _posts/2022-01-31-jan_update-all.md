--- 
title: "ThermoForce: State of the Project – January 2022"
date: 2022-01-31 00:00:01 -0000
categories: project-management
author: Ethan Grasley
---

# Project Progress - January 2022

January saw the finalization of design and beginning of system integration. Controls circuitry and glove design were finalized in the first two weeks of the month, along with the design and creation of the development blog website. Following this, an electronics enclosure was designed, the motor used for weight feedback characterized, and work began on glove fabrication. The BLE communications format was finalized, and the minimum viable product completed for the VR game space. At the end of the month wiring harnesses for temperature control were created and attached to the glove, a proportional-integrator control loop for weight feedback was designed, and the LEAP motion controller was tested with glove stand-ins to ensure the IR tracking still worked. 


## Temperature Feedback Development

For January, temperature feedback was focused on integrating the heaters, Peltier cooler, thermistors, and wiring into the glove.


## Weight Feedback Development

Weight feedback for January consisted of motor characterization, electrical enclosure design, thigh and wrist mount design for the motor and cable, a weight feedback control loops, and creation of the program responsible for temperature and weight control as well as BLE communication. Thanks to the facilities available to students at WatIMake, motor characterization was completed in just under 8 hours. This involved finding armature resistance and inductance, torque and back EMF constants, and motor inertia and damping values. For electronics enclosure design, it was decided that electric components would be mounted to a 3D printed plates, and these plated would then be mounted in a plastic enclosure. This enclosure in turn will be bolted to a backpack, so it can move with the user. A simple PI controller was designed for weight feedback, but the performance is unsatisfactory, so there are plans to replace it with a fuzzy logic controller in the future. Finally, in the last week of the month a program was created that implements features from all proof-of-concept programs previously completed. This means there is now a single program that is responsible for control of the entire system. 

![](../../assets/images/JanElecSystem.jpg)


## Glove Design Development

Detailed glove design and glove fabrication was completed throughout the month of January. The glove was fully fabricated, featuring a stretchy t-shirt style cotton fabric, Velcro closures for size adjustments, and snaps to combine the inner and outer layers of the glove, with the electronics hidden between the two layers. The glove was designed to be as easy to adjust and disassemble as possible to accommodate various hand sizes/shapes and to allow for easy access to the electronics when the team needs to troubleshoot them. Testing for comfort and functionality will occur throughout February.

![](../../assets/images/JanGloveDesign.jpg)


## Software Development

MVP for the game has been completed; the entire Unity scene with a range of objects with varying masses and temperatures has been implemented. These numbers feed into the stream to be sent to the controls system. A Bluetooth low energy API will be implemented to complete this communication. Additionally, the website development has started and completed and details all relevant public information including about the team, development blog posts, and about the project. The website was developed using an open-source skin on top of GitHub pages. The focus in February will be to refine the website and refine the Unity scene after successfully implementing the Bluetooth communication. 
