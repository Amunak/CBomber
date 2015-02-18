# CBomber
A [Bomberman / Dyna Blaster](https://en.wikipedia.org/wiki/Bomberman_%281990_video_game%29) clone I made as my semestral work in the C++ course on [Faculty of Information Technology of the Czech Technical University](http://fit.cvut.cz/). Written in C++ using only the ncurses library. Now released under the MIT license.

## Features
* Command line interface and graphics (using ncurses)
* Enemy bombers
  * Can pick up most powerups
  * They have a very simple AI which
	* lays bombs
	* avoids exploding bombs
	* attempts to hunt the player (and other bombers) in hopefully non-random way
* *Monsters* that attack the player when he gets too close (they move randomly)
* Destructible crates!
* Power-ups (more bombs, bigger flames, speed, etc.)
* Maps can be loaded from files
  * You can make your own maps! Yay!
  * There is the 'original' map preset as well as an empty arena
  * The examples/ folder contains some example maps
* Network play with a friend
  * Tested only on localhost
  * The implementation is horrible (it is sending the hosts screen character by character, reassembling it at the client side)
  * The code itself is horrendous (even if the logic was better)

## Running

The project features a makefile. Run `make run` to compile and run the game. You can also just use `make compile` to compile the game and then run `./bomber`. `make all` (or just `make`) will compile the game *and* Doxygen's HTML documentation (which can be separately created with `make doc`).

Finally, run `make clean` to clean up the binaries and documentation.

**Note:** While the game could run on windows (at least without networking), it was tested on linux (32-bit Ubuntu installation and 64-bit [Archlinux](http://archlinux.org/)), so no guarantee that it will run there (actually no guarantee that it will run anywhere, because parts of the code are really shitty, but whatever).
