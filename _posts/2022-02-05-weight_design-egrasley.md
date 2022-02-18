--- 
title: "Weight Feedback – Jan 30-Feb 5, 2022"
date: 2022-02-05 00:00:01 -0000
categories: weight-feedback
author: Ethan Grasley
---

## 3 Ways to Feedback

Weight feedback featured a triple feature this week. Motor characterization and control loop design, electronics enclosure and mounting design, and motor thigh mount + wrist anchor point design.  

Motor characterization went more smoothly this week, with all physical motor characteristics found in just under 2 hours. From there a motor control simulation was built using Simulink. A simple proportional integrator controller can be used to control the motor when pulling down on the user's arm. A second controller needs to be designed for when the user is moving their arm up, since the tracking requirements are different there. 

The baseplate and enclosure that the circuitry and batteries will mount to were finalized. A slight redesign of the baseplate was required to accommodate the fact that the enclosure is going to be slightly shorter than assumed, to better fit into a backpack. 

Finally, a mounting system to mount the motor and pulley to the user's thigh, and a way to mount the loadcell to the motor cable were designed this week. The thigh mount has adjustable straps and high-density foam to accommodate a range of thigh sizes, and a solid metal plate to mount the motor securely. The loadcell mount uses swivelling eye bolts so there is no range of motion loss in the user. 

With this electronics enclosure and weight feedback design are complete!

![](../../assets/images/Jan30-Feb5_WeightFeedback1.png)

![](../../assets/images/Jan30-Feb5_WeightFeedback2.png)

![](../../assets/images/Jan30-Feb5_WeightFeedback3.png)

![](../../assets/images/Jan30-Feb5_WeightFeedback4.png)
