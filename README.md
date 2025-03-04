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


### Controls
- **A, D** : go left and right
- **W** : Rotate block
- **S** : Make block fall slowly
- **space** : Hard drop
- **P** : On/Off phantom block


## TODO
- [x] xmake.lua refactoring
- [ ] SCORE
- [ ] Removing lines
- [ ] Animations
- [ ] Random block selection
- [ ] Block queue
- [ ] Levels (going faster)
- [ ] Stash block
- [ ] Mouse control
- [ ] SFML installed from xmake.lua
- [ ] build on NixOS
- [ ] A LOT of esthetical and functional refactoring
- [ ] Switching Keyboards
- [ ] Pause menu


(I won't make sound, I don't like sound).


## Licence
dowhateveryouwantwithitidc
