---
permalink: /design/vr-hand-tracking/
---

![](../../assets/images/DesignHandTracking.png)

The hand tracking subsystem must be able to track the 3D coordinates of at least each finger and the palm for both user’s hands within a range that is suitable for interacting with a virtual reality program. The hand tracking subsystem must also be able to track the user’s hand movements when covered by a glove. The Leap Motion infrared (IR) hand tracking system was chosen because it is the only hand tracking system available on the market that can meet the constraint of tracking multiple points on the hand and has predefined software modules to track hands with 1.2mm of accuracy. The Leap Motion controller tracks hand coordinate data and sends this data to the application layer (video game) which runs on a Unity platform. Logic on the application layer builds these coordinates into hand objects, which interact with the video game environment. Interaction between objects in the game (hand-to-object contact in the video game) trigger events. These events are then sent to the controller to command the weight and temperature feedback control loops.

All forms of feedback and events will use Unity’s scripting API (as opposed to the manual API) to automate weight and temperature assignments for objects, event triggering, and retrieval of weight and temperature attributes from objects.

The mass of objects will be defined with numerical constants using Unity’s Rigidbody.mass class. The temperature of objects will be defined with numerical constants using Unity’s Emission class.

The selected location for the Leap Motion controller is stationary mounting. This allows the connection from the hand tracker to the application to be via USB, removing software development overhead. It also removes the cost of buying a headset. It is important to note that the setting on Unity to switch between stationary and headset configuration is one button, so this can be easily re-designed if necessary.

Another consideration during the design of the hand tracking system was the need for a mechanism by which the weight feedback system is turned off once the user’s hands reach below the motor and cable position. The design for the anti-gravity check is to track an additional tool using Leap Motion’s Unity class ToolModel. This class allows objects (other than hands) to be tracked. As such, a simple pre-defined tool may be attached to the motor location; if the user’s hands go below the tool, the weight system will be turned off.
