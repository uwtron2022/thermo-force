---
permalink: /design/weight-feedback/
---

![](../../assets/images/DesignWeightFeedback.jpg)

ThermoForce will use a motor and cable system to provide weight feedback, with a goal feedback weight of 5Kg minimum. Since this project is intended as a proof-of-concept, design decisions were made to help reduce costs. This means an Arduino MEGA and Arduino Motor Shield will be used as the main controller and motor driver. An external shunt resistor and ADC (Analog to Digital Converter) will be used to measure motor current, and a loadcell/loadcell amplifier will be used to measure the force exerted on the motor cable (indirectly measuring motor torque), so closed loop force control can be used. Ideally, a motor driver would be used that offers built in current sensing and torque control, to simplify the overall design. 

Using known relationships for motor current and torque, control schemes can be derived to control the force exerted by the motor by adjusting the current. Since the control board selected does not offer direct current control, the current will be controlled indirectly by adjusting the voltage supplied to the motor. Currently, testing is being done to ensure that the force and current measured by the sensors is accurate. Following this, tests will be done to determine motor characteristics so control loops can be designed and implemented to reach the required performance. 
