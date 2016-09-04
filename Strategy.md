# Slim strategy description

This file goal is to define the behavior of the robot during a combat.

### Inputs:
1. Motor measured speed left and right (`RPM`)
1. Distance sensor left and right (`active` / `not active`)
1. Euler angles defined orientation (`deg`)
1. Orientation in quaternions (`quaternions`)
1. Acceleration of the robot in 3D (`m / s^2`)
1. Angular speed of the robot (`deg / s^2`)
1. Tire traction available, left and right (`available` / `not available`)
1. **TODO**

### Outputs:
1. Motor set point speed, left and right (`RPM`)
1. **TODO**

### States:
1. Initial state - the moment after the start signal was applied.
  - General approach is to start going trough the terrain in a pattern, pattern is **TODO**
1. Contact waiting state
  - We need to detect contact event (**TODO**: how to distinguish contact).
  - We need to control position changing using the pattern decided in the previous state (**TODO**: maybe PID control of path travelled by every tire).
1. Contact detected state
  - Once contact is detected we need to decide action (**TODO**).
  - Different contacts must have different actions (**TODO**).
1. Action tracking state
  - Once the opponent position is known and the counter action is chosen we need to make sure the action is done properly.
  - Here we need to watch out for disposition problems like lifting us up while trying to push the opponent on the front.
1. _OPTIONAL_: Action failure state
  - The action has failed in execution (**TODO**: definition of action failure).
  - New action is to be chosen and passed to execution.
