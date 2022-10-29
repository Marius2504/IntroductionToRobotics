# Introduction to Robotics (2022 - 2023)

Description about the repository: "Introduction to Robotics laboratory homeworks", taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. Each homework includes requirements, implementation details, code and image files.


## Homework 1

Technical Task: Use a separat potentiometer in controlling each of the
color of the RGB led (Red, Green and Blue). The control must be done
with digital electronics (aka you must read the value of the potentiometer with Arduino, and write a mapped value to each of the pins connected
to the led.

  Picture link: https://postimg.cc/vgZQZjRB
  Video Link: https://youtu.be/_CK0SyTvXvU

## Homework 2
#### Components: 
5 LEDs, 1 button, 1 buzzer, resistors and wires (per logic)
#### General description: 
Building the traffic lights for a crosswalk. You will use 2 LEDs to represent the traffic lights for people (red and green) and 3 LEDs to represent the traffic lights for cars (red, yellow and green).See the states it needs to go through. If anything is not clear, ask. Also, see the uploaded video (the intervals are different, but the states flow is
the same). It’s a traffic lights system for people and cars - don’t overthinkit.

The system has the following states:
1. State 1 (default, reinstated after state 4 ends): green light for cars,
red light for people, no sounds. Duration: indefinite, changed by
pressing the button.
2. State 2 (initiated by counting down 8 seconds after a button press):
the light should be yellow for cars, red for people and no sounds.
Duration: 3 seconds.
3. State 3 (initiated after state 2 ends): red for cars, green for people
and a beeping sound from the buzzer at a constant interval. Duration:
8 seconds.
4. State 4 (initiated after state 3 ends): red for cars, blinking green
for people and a beeping sound from the buzzer, at a constant interval, faster than the beeping in state 3. This state should last 4
seconds.
