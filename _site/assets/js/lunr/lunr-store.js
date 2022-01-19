var store = [{
        "title": "ThermoForce: State of the Project – September 2021",
        "excerpt":"Project Progress - September 2021   Project activities in September focused on project setup. This includes determining what the project focus should be, determining meeting times, setting up file storage structures, and determining primary project areas for team members. The project focus was determined by looking at the National Academy of Engineering Grand Challenges for the 21st Century and selecting the topic that interested the team the most. VR (Virtual Reality) and its slow adoption was selected as the topic of interest and feasibility research was done to determine if the scope of a VR related project was appropriate for our FYDP. After determining a VR based project was feasible, a needs assessment was performed to determine a root cause issue to focus on for the duration of this project. After selecting a final area to focus on (lack of feedback for non-visual/smell-based senses) possible solutions were discusses/generated and a final design selected. This design is the glove-based temperature and weight feedback, with infrared hand-tracking design seen in later posts.  ","categories": ["project-management"],
        "tags": [],
        "url": "/thermo-force/project-management/sept_update-all/",
        "teaser": null
      },{
        "title": "ThermoForce: State of the Project – October 2021",
        "excerpt":"Project Progress - October 2021   The start of October focused on finalizing the working design for the glove: this resulted in the design we see know, resistive heaters, a Peltier plate for cooling, motor and cable for weight feedback, a glove to attach components to, and using an IR hand tracker for game inputs. Based on the selected final design, patent research was performed to see if any patent infringement would occur if this project was to be taken public. The rest of the activities this month focused on research into the specific project areas (temperature and weight feedback, glove design, and software requirements), possible feasible designs, and basic prototyping/proof-of-concepts. Several course deliverables were also due this month.         Temperature Feedback Development   Temperature feedback activities focused on ordering flexible resistive heaters, and on simple cooling proof-of-concept using Arduino PWM, a Peltier plate, and thermistors to measure temperature.      Weight Feedback Development   Weight feedback activities for early October were focused on motor power requirement calculations based on the desired weight feedback range (0-10Kg), so a motor could be ordered. With a motor ordered, the Arduino Motor Shield Rev3 was selected as a motor controller, and preliminary coding began on motor control. Additionally, since closed loop feedback is desired, a loadcell to measure force and shunt resistor + ADC (Analog to Digital Converter) combo was ordered to be able to measure relevant system parameters.   Glove Design Development   Glove design for October focused on research into fabrics and their associated thermal properties. It was found that natural fibres were generally more thermally conductive than synthetic ones. This, and the fact that synthetic fibres tend to melt when heated, mean natural fibres like cotton will be used in the gloves design. Additional research/testing was done into metal-infused fabric to see if they achieved better thermal conductivity than pure fibres. The testing was inconclusive and due to the cost of the infused fabric we decided not to use it.   Software Development   Software development for October was limited, as final design decisions were still being made. A GitHub repository for software (Unity VR and Arduino sketches) was created, and the development environment for unity setup. Additionally, on the Arduino side a Bluetooth module was ordered and connected, and a proof of concept was completed show BLE (Bluetooth Low Energy) could be used as a means wireless communications between a computer and feedback controller (Arduino).        ","categories": ["project-management"],
        "tags": [],
        "url": "/thermo-force/project-management/oct_update-all/",
        "teaser": null
      },{
        "title": "ThermoForce: State of the Project – November 2021",
        "excerpt":"Project Progress - November 2021   November saw in-depth design and proof of concept testing begin. Logic was added to temperature feedback so a heating/cooling setpoint could be given and using temperature reading from thermistors the heaters/coolers would be turned on/off until the setpoint was reached. A testing stand was built, and weight feedback components mounted so motor characterization could be performed (this characterization is needed so simulations can be used to design responsive weight control algorithms). For glove design simulations were performed with various materials and layers makeups to determine the most thermally conductive design, while minimizing material needed.  A final design was selected that has cotton touching the skin, followed by a thermal pad, an optional thin metal strip (to add structure If needed), the heaters/coolers, then a semi-insulating removable outer layer to allow for maintenance to be performed if needed. Software development began in earnest, with hand tracking testing using the Leap Motion Sensor being performed, as well as creation of simple VR game environments.   Temperature Feedback Development   In the first week of November the temperature feedback proof of concept circuit and Arduino sketch were completed and deemed successful. Following this, circuit planning for the entire temperature feedback system began. The circuit is made up of 6 solid state relays, 6 heaters, 1 Peltier plate, and 7 thermistors (1 to measure ambient temperature). Full temperature control loop flowcharts were also created for reference.      Weight Feedback Development   For November weight feedback development consisted of building a motor test stand and completing proof of concepts for PWM motor control, current measurement with a shunt resistor and 16-bit ADC, and force measurement using a strain gauge and loadcell amplifying circuit.  After completion of the PoC’s, a simple proportional control loop was designed and tested to see if the selected sensors were suitable for a force control loop.   Glove Design Development   Glove design consisted of simulations this month. Initial simulations were done with strips of material (as opposed to a full glove shape) and results were promising. Simulations show a temperature increase of 10°C in 3 seconds (using the rated heating power of the selected resistive heaters, 6W) and a temperature drop of 25°C in 2 seconds (thanks to the Peltier plates higher power consumption of 10W). Unfortunately, it was noted that in the simulation heating/cooling was extremely local to the heater/cooler’s location, but informal real-life testing showed the heating/cooling effect was more widespread. Regardless, extra heaters were added to help spread the heating effect out. With these results a full glove model simulation was attempted, but limitations of the free program being used meant the complex geometry could not be handles. Additional testing and validation will be done as the glove is built to verify real life performance matches the simulated.        ","categories": ["project-management"],
        "tags": [],
        "url": "/thermo-force/project-management/nov_update-all/",
        "teaser": null
      },{
        "title": "ThermoForce: State of the Project – December 2021",
        "excerpt":"Project Progress - December 2021   December saw the completion of the first semester of this design project. There were several deliverables completed during the first two weeks of the month. These include logbooks detailing the past four months work, plans and schedules for the next four months of work, and a final report encapsulating the entire projects work from the beginning of September to the end of November. This includes topic selection, design generation and selection, project constrains/objectives/criteria, calculations/simulations/proof of concepts, and preliminary schedules for the next four months.  ","categories": ["project-management"],
        "tags": [],
        "url": "/thermo-force/project-management/dec_update-all/",
        "teaser": null
      },{
        "title": "Glove Design – Jan 1-15, 2022",
        "excerpt":"Sew Close to a Final Design  Glove work for the last two weeks consisted of creating and testing glove sewing patters. The current approach is a segmented design, with the full cotton-thermal pad-metal-heater-cotton inner layer being sewed (a sewing machine can stitch right through the thin metal) and the heater being silicone epoxied on and placed on the palm side of the hand, and the protective outer laying being an easily removable Velcro design that wraps around the entire finger/hand. Once the temperature control circuit is entirely built it will be integrated with glove.        ","categories": ["glove-design"],
        "tags": [],
        "url": "/thermo-force/glove-design/glove_design-estudzinski/",
        "teaser": null
      },{
        "title": "Software Design – Jan 1-15, 2022",
        "excerpt":"No Escaping Web Dev   Game development has begun on the Unity Platform with the Leap Motion developer plugin for compatibility with the infrared hand tracker. MVP (minimum viable product) goals for the game is to have an object with weight, an object that is ‘hot’ and object that is ‘cold’. Meeting with the temperature/weight feedback teams to follow to declare exactly what information is to be sent over Bluetooth to the Arduino, and in what format. Website development for basic scaffolding is complete, with ongoing iterative changes to project/product descriptions, photos, logos, blog posts, etc. Website uses Github.io pages and is live as of Jan 13, 2022.  ","categories": ["vr-hand-tracking"],
        "tags": [],
        "url": "/thermo-force/vr-hand-tracking/software_design-eneil/",
        "teaser": null
      },{
        "title": "Temperature Feedback – Jan 1-15, 2022",
        "excerpt":"Wires and Heaters and Relays, Oh My   Work at the start of the new year consisted of planning and manufacturing of the custom circuit boards (made of thru-hole components and perf-boards) needed to simplify wiring of the temperature feedback system to the Arduino MEGA (which was selected as the controller). This circuit boards use screw terminal for connections that need to be made to external components, making assembly/disassembly/maintenance easier.   Power system design was also finalized at this time. Since we already own 14.4V batteries two buck converters will be used to convert the battery voltage to 12V and 5V. 5V will be used by the Arduino, heaters, and coolers while 12V will be used to power the motor for weight feedback.   With the circuits completed, heating/cooling controls programming on the Arduino continued. The program was tested with the final heating/cooling components wiring completed and worked successfully, although we are expecting fine-tuning will be needed when the system is fully integrated.                    ","categories": ["temperature-feedback"],
        "tags": [],
        "url": "/thermo-force/temperature-feedback/temp_design-cdewar/",
        "teaser": null
      },{
        "title": "Weight Feedback – Jan 1-15, 2022",
        "excerpt":"On Soldering and PWM Troubles   Work these past two weeks consisted of soldering custom perf boards used to ease wiring of the Arduino MEGA/Motor Driver to the Bluetooth module, 16-bit ADC, shunt resistor, loadcell, and loadcell amplifier. With the circuit complete, code updating was done to bring the old proof of concept Arduino sketches up to date with the new Arduino MEGA controller.   During updating it was noticed that the shunt resistor/ADC combo consistently read 0 on the Arduino monitor, while an external multimeter showed a current draw of 240mA. Some testing needs to be done (with an oscilloscope or using the Arduino serial monitor and a low current draw motor), but the issue is that the motor control signal is pulse-width modulated, meaning the measured current is always going to be zero or maximum and is not indicative of the average motor current draw (which is needed for good, closed loop control of the motor). A resistor-capacitor low pass (averaging) filter will need to be added to the motor control circuit, so the current measuring shunt is recording the true average current the motor draws.   Time was also spent creating a full weight control electrical schematic to be added to the existing temperature control one, and summaries for the previous terms work were created for posting     ","categories": ["weight-feedback"],
        "tags": [],
        "url": "/thermo-force/weight-feedback/weight_design-egrasley/",
        "teaser": null
      },]
