## Third Person Shooter Project

**Authors :**
- PERCHE Alexandre
- LISE Omaya
- DEVINE Vincent
- GOUPIL Kristian

For ISART Digital
<hr /><br />

**Project description :**<br />
We made a third-person infiltration game called SAM (Security Assistance Machine). It was developped on Unreal and is playable on PS4 as well as PC. <br />
You play as SAM, a robot tasked of saving the president from his kidnappers. <br />
To do so, you have to infiltrate a secret laboratory, you may kill as many enemies as you want but if the alarm is activated the mission will fail.
<hr /><br />

![png](./Screenshots/Menu.png)
## **Commands :**
- On keyboard and mouse:
    - W/S : Move forward/backward
    - A/D : Move left/right
    - Mouse : Look around / Move aim cursor
    - Left Click : Aim
    - Right Click : Shoot
    - E : Hold to drag corpse
    - Shift : Crouch

- On Gamepad:
    - Left Joystick : Move
    - Right Joystick : Look around / Move aim cursor
    - L2 : Aim
    - R2 : Shoot
    - R1 : Hold to drag corpse
    - R3 : Crouch
<hr /><br />

**Features:**

Detection system : 
- We used Unreal Pawn system to implement the detection system (vision and hearing) which is given to the enemies raising or lowering their awareness.
- We coupled it with a personalized stealth system linked to the lights. <br />The closer the player is to the lights the quicker enemies will spot him. <br />The player can see his visibililty status on screen with a bar next to the crosshair.

Player :
- Has basic movements
- Can crouch to be more discrete and go into smaller spaces
- Can aim (when an enemy is in range a red dot will appear)
- Can shoot (only if he is aiming before)
- Can move corpses to hide them or put them into bins

Enemy:
- Enemies possess 4 behaviors :
<br />Patrol (between pre-defined points), 
<br />Suspicious (will go to last position where the player was seen),
<br />Alert (will follow and shoot the player),
<br />GoingToTheAlarm (after using all their amunitions they will go to activate the alarm)

- Enemies can communicate (share alerts and level of awareness) if they are in range of each other
- Enemies will detect corpses

![png](./Screenshots/President.png)
## **Hardest part:**
- Light system detection
- Optimizing for the PS4

## **Features to add:**
- Cover System 
- Timer system 
- Destroyable lights
- Cameras

<hr /><br />

**Platform:**
Windows
PS4
>