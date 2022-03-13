--- 
title: "Software – Feb 20-March 05, 2022"
date: 2022-03-05 00:00:01 -0000
categories: vr-hand-tracking
author: Emily Neil
---

## I Eat Serial For Breakfast !

This week, the bidirectional serial connection from the Arduino to Untiy was established. Object data (temperature and weight) are sent from the Unity game to the Arudino controller via serial communication. Since the Arduino cannot facilitate two serial ports of communication, all logs from the Arduino (mainly for debuggin purposes), are sent back to the Unity platform and are printed on the Unity console. We now have a fully functional prototype, and will begin working on replacing serial with Bluetooth!
