# Tretis : My little Tetris clone

I am making this Tetris clone with C++ and the SFML graphic library.
This is still in crazy development.
There is not a lot of things working.

## How to build

### Windows
No.

### Linux (except NixOS but it's coming... I hope)
1. Install the SFML library version 2.6 __(I am trying to do it automatically with xmake but It's not going very well...)__
2. Install the **xmake** build system
3. `git clone` the repo where you want, `cd` into it
4. `xmake f -m debug` for debug mode and `xmake f -m release` for release mode
5. `xmake run` if you just want to run it and `xmake install --admin` if you want to install it.

## Functionalities (for now...)
- There is every Tetromino (called Tretominos here)
- Functionning hard drop
- Complete implementation of the **Super Rotation System** !
- Proper windows scaling
- Random Block Selection
- Block Holding
- Shadow Block
- Working official score system (except Back-2-Back)


## Controls
*Only QWERTY Keyboards are supported right now*
- **A, Left** : Go left
- **D, Right** : Go right
- **W, X** : Rotate block clockwise
- **Z** : Rotate block counterclockwise
- **S** : Soft Drop
- **Space** : Hard drop
- **C** : Hold
- **P** : On/Off phantom block


## TODO
- [x] xmake.lua refactoring
- [x] SCORE
- [x] Removing lines
- [x] Random block selection
- [x] Block queue
- [x] Hold block
- [x] Logging System
- [ ] Levels (going faster)
- [ ] Combo Bonus
- [ ] Score registration in a file
- [ ] Saves & Load
- [ ] Make the font work when you're not inside project dir !!!
- [ ] Game Over
- [ ] Start menu
- [ ] Pause menu
- [ ] Mouse control
- [ ] SFML installed from xmake.lua


(I won't make sound, I don't like sound).


## Licence
dowhateveryouwantwithitidc
