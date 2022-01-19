--- 
title: "Temperature Feedback – Jan 1-15, 2022"
date: 2022-01-15 00:00:01 -0000
categories: temperature-feedback
author: Colin Dewar
---

## Wires and Heaters and Relays, Oh My

Work at the start of the new year consisted of planning and manufacturing of the custom circuit boards (made of thru-hole components and perf-boards) needed to simplify wiring of the temperature feedback system to the Arduino MEGA (which was selected as the controller). This circuit boards use screw terminal for connections that need to be made to external components, making assembly/disassembly/maintenance easier.

Power system design was also finalized at this time. Since we already own 14.4V batteries two buck converters will be used to convert the battery voltage to 12V and 5V. 5V will be used by the Arduino, heaters, and coolers while 12V will be used to power the motor for weight feedback.

With the circuits completed, heating/cooling controls programming on the Arduino continued. The program was tested with the final heating/cooling components wiring completed and worked successfully, although we are expecting fine-tuning will be needed when the system is fully integrated.

![](../../assets/images/Jan1-15_BuckConv.jpg)

![](../../assets/images/Jan1-15_Relays.jpg)

![](../../assets/images/Jan1-15_Thermistors.jpg)

![](../../assets/images/Jan1-15_ResHeaters.jpg)

![](../../assets/images/Jan1-15_FullTempCircuit.jpg)

![](../../assets/images/Jan1-15_CurrentElecDwg.jpg)
