# Tretis : My little Tetris clone

I am making this Tetris clone with C++ and the SFML graphic library.
This is still in crazy development.
There is almost nothing working correctly.

## How to build

1. Install the SFML library __(I am trying to do it automatically with xmake but I think I'm stupid...)__
2. Install the **xmake** build system
3. `git clone` the repo where you want, `cd` into it
4.
```sh
xmake run
```

## Functionalities (for now...)
- There is every Tetromino (called Tretominos here)
- Functionning hard drop
- Complete implementation of the **Super Rotation System** !
- Proper windows scaling
- Random Block Selection
- Block Holding
- Shadow Block


### Controls
*Only QWERTY Keyboards are supported right now*
- **A, Left** : Go left
- **D, Right** : Go right
- **W, X** : Rotate block clockwise
- **Z** : Rotate block counterclockwise
- **S** : Make block fall slowly
- **Space** : Hard drop
- **C** : Hold
- **P** : On/Off phantom block


## TODO
- [x] xmake.lua refactoring
- [ ] SCORE
- [ ] Score registration in a file
- [ ] Saves & Load
- [x] Removing lines
- [ ] Animations
- [x] Random block selection
- [x] Block queue
- [ ] Levels (going faster)
- [x] Hold block
- [ ] Mouse control
- [ ] SFML installed from xmake.lua
- [ ] build on NixOS
- [ ] A LOT of esthetical and functional refactoring
- [ ] Switching Keyboards
- [ ] Pause menu
- [ ] Start menu
- [x] Logging System
- [ ] Game Over
- [ ] Make the font work when you're not inside project dir !!!


(I won't make sound, I don't like sound).


## Licence
dowhateveryouwantwithitidc
