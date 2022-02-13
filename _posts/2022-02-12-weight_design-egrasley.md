--- 
title: "Weight Feedback – Feb 6-12, 2022"
date: 2022-02-12 00:00:01 -0000
categories: weight-feedback
author: Ethan Grasley
---

## A Weighty Proposition

For this week, pure weight feedback activities were put on hold, and focus was shifted to creation of a single controller program to handle Bluetooth communications and temperature and weight feedback control. This was completed on Thursday night, and the entire software suite tested live with the glove, with minimal rework needed. 

Some research was done on alternative weight feedback controllers. A proportional integrator is currently implemented, but performance is not good. The plan is to look into/develop a fuzzy logic controller to handle the uncertainty a human introduces to the system, which should result in better overall performance. 
