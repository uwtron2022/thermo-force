--- 
title: "ThermoForce: State of the Project – November 2021"
date: 2021-11-30 00:00:01 -0000
categories: project-management
author: Ethan Grasely
---

# Project Progress - November 2021

November saw in-depth design and proof of concept testing begin. Logic was added to temperature feedback so a heating/cooling setpoint could be given and using temperature reading from thermistors the heaters/coolers would be turned on/off until the setpoint was reached. A testing stand was built, and weight feedback components mounted so motor characterization could be performed (this characterization is needed so simulations can be used to design responsive weight control algorithms). For glove design simulations were performed with various materials and layers makeups to determine the most thermally conductive design, while minimizing material needed.  A final design was selected that has cotton touching the skin, followed by a thermal pad, an optional thin metal strip (to add structure If needed), the heaters/coolers, then a semi-insulating removable outer layer to allow for maintenance to be performed if needed. Software development began in earnest, with hand tracking testing using the Leap Motion Sensor being performed, as well as creation of simple VR game environments.


## Temperature Feedback Development

In the first week of November the temperature feedback proof of concept circuit and Arduino sketch were completed and deemed successful. Following this, circuit planning for the entire temperature feedback system began. The circuit is made up of 6 solid state relays, 6 heaters, 1 Peltier plate, and 7 thermistors (1 to measure ambient temperature). Full temperature control loop flowcharts were also created for reference.

![](../../assets/images/NovTempFeedbackSchematic.png)


## Weight Feedback Development

For November weight feedback development consisted of building a motor test stand and completing proof of concepts for PWM motor control, current measurement with a shunt resistor and 16-bit ADC, and force measurement using a strain gauge and loadcell amplifying circuit.  After completion of the PoC’s, a simple proportional control loop was designed and tested to see if the selected sensors were suitable for a force control loop.


## Glove Design Development

Glove design consisted of simulations this month. Initial simulations were done with strips of material (as opposed to a full glove shape) and results were promising. Simulations show a temperature increase of 10°C in 3 seconds (using the rated heating power of the selected resistive heaters, 6W) and a temperature drop of 25°C in 2 seconds (thanks to the Peltier plates higher power consumption of 10W). Unfortunately, it was noted that in the simulation heating/cooling was extremely local to the heater/cooler's location, but informal real-life testing showed the heating/cooling effect was more widespread. Regardless, extra heaters were added to help spread the heating effect out. With these results a full glove model simulation was attempted, but limitations of the free program being used meant the complex geometry could not be handles. Additional testing and validation will be done as the glove is built to verify real life performance matches the simulated.

![](../../assets/images/NovGloveStripSim.png)

![](../../assets/images/NovGloveTempPlacement.png)
