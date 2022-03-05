--- 
title: "Weight Feedback – Feb 20-March 05, 2022"
date: 2022-03-5 00:00:01 -0000
categories: weight-feedback
author: Ethan Grasley
---

## Full FAT

The week of Feb 20th saw the test fitting and full assembly of the weight feedback components. This includes the loadcell arms, motor pully and bracket, and thigh mount. The test fit was successful, and the assembly is now complete. Work is progressing on the weight feedback controller. The PI controller mentioned earlier was implemented and works very well, but there are issues. Namely the controller breaks down if the user tries to move their arm up – the motor is not back drivable so there needs to be logic (using information from the loadcell) that will reverse the motor direction and use a different controller to allow the user to move their arm away. 

This past week saw the implementation of a fuzzy logic controller/algorithm to handle changing the direction of the motor. This controller fuses changes in the current and loadcell readings to determine the likelihood that the person is lifting, lowering, or holding their arm stationary. This algorithm has not been tuned to our system, but already works very well at changing the motor direction to match the user's movement. The PI controller mentioned earlier is still implemented to hold the users arm stationary or pull it down, but an updated controller needs implementing and tuning to handle letting the user raise their arm. 
