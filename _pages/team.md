---
permalink: /meet-the-team/
defaults:
# _pages
- scope:
  path: ""
  type: pages
  values:
  layout: single
---

<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html {
  box-sizing: border-box;
}

*, *:before, *:after {
box-sizing: inherit;
}

.column {
float: left;
width: 50%;
margin-bottom: 16px;
padding: 0 8px;
}

@media screen and (max-width: 650px) {
.column {
width: 100%;
display: block;
}
}

.card {
box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);
}

.container {
padding: 0 16px;
}

.container::after, .row::after {
content: "";
clear: both;
display: table;
}

.title {
color: grey;
}

.button {
border: none;
outline: 0;
display: inline-block;
padding: 8px;
color: white;
background-color: #000;
text-align: center;
cursor: pointer;
width: 100%;
}

.button:hover {
background-color: #555;
}
</style>
</head>
<body>

<h2>Meet Our Team :)</h2>
<br>

<div class="row">
  <div class="column">
    <div class="card">
      <img src="../assets/images/cdewar.jpg" alt="Colin" style="width:100%">
      <div class="container">
        <h2>Colin Dewar</h2>
        <p class="title">Temperature Feedback Lead</p>
        <p>Hi! I’m Colin and I’m the temperature feedback lead for ThermoForce. I’m a 4B Mechatronics Engineering student at the University of Waterloo.</p>
        <p>I'm responsible for temperature feedback controller selection, temperature sensor selection, heating element selection, cooling element selection, temperature feedback proof of concept, temperature circuit design, and temperature controls.</p>
        <p><button class="button"><a href="mailto:cgdewar@uwaterloo.ca" style="text-decoration:none !important; color:#FFFFFF;text-decoration:none;">Contact</a></button></p>
      </div>
    </div>
  </div>
  <div class="column">
    <div class="card">
      <img src="../assets/images/estudzinski.jpeg" alt="EmilyS" style="width:100%">
      <div class="container">
        <h2>Emily Studzinski</h2>
        <p class="title">Glove Design Lead</p>
        <p>Hi! I’m also Emily and I’m the glove design lead for ThermoForce. I’m a 4B Mechatronics Engineering student at the University of Waterloo.</p>
        <p>I’m responsible for material thermal transfer testing, material selection, material thermal energy transfer simulations, glove design aesthetics, glove design drawings, glove assembly and user testing for glove sub-system.</p>
        <p><button class="button"><a href="mailto:estudzinski@uwaterloo.ca" style="text-decoration:none !important; color:#FFFFFF;text-decoration:none;">Contact</a></button></p>
      </div>
    </div>
  </div>
</div>
<div class="row">
  <div class="column">
    <div class="card">
      <img src="../assets/images/egrasely.jpg" alt="Ethan" style="width:100%">
      <div class="container">
        <h2>Ethan Grasely</h2>
        <p class="title">PM & Weight Feedback Lead</p>
         <p>Hi! I’m Ethan and I’m the project manager and weight feedback lead for ThermoForce. I’m a 4B Mechatronics Engineering student at the University of Waterloo.</p>
        <p>I’m responsible for the project timeline, scheduling and leading meetings, logbook submissions, timekeeping, presentation planning, deliverable planning and outlining, and budgeting. As weight feedback team lead, I’m responsible for overall architecture of weight feedback system, meaning parts selection and verification, circuit building, controls loop design and implementation, and design/fabrication of the electronics mounting mechanism.</p>
        <p><button class="button"><a href="mailto:emgrasley@uwaterloo.ca" style="text-decoration:none !important; color:#FFFFFF;text-decoration:none;">Contact</a></button></p>
      </div>
    </div>
  </div>
  <div class="column">
    <div class="card">
      <img src="../assets/images/eneil.jpeg" alt="EmilyN" style="width:100%">
      <div class="container">
        <h2>Emily Neil</h2>
        <p class="title">Software Lead</p>
         <p>Hi! I’m Emily and I’m the software lead for ThermoForce. I’m a 4B Mechatronics Engineering student at the University of Waterloo.</p>
        <p>I’m responsible for website architecture and maintenance, VR development platform design, hand tracking to application to controller architecture design, anti-gravity check design and proof of concept for hand tracking.</p>
        <p><button class="button"><a href="mailto:eneil@uwaterloo.ca" style="text-decoration:none !important; color:#FFFFFF;text-decoration:none;">Contact</a></button></p>
      </div>
    </div>
  </div>
</div>

</body>
</html>
