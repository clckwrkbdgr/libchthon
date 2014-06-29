# libchthon

A compact C++ library (primarily for roguelike development).

This library was created during development of roguelike Temple of Trials and was separated at some point,
when it became big and self-aware. Older history for all objects can be found in Temple of Trials repo:
[https://github.com/umi0451/temple-of-trials](https://github.com/umi0451/temple-of-trials)

Features roguelike game helper classes:

* Roguelike game class (extendable for game customization) with powerful game flow based on actions.
* 2D generic map class.
* Flexible actions use and a lot of predefined actions.
* Basic AI (very basic but extendable).
* Cells, objects, items, monsters with various properties and basic Info class.
* Field-of-vision classes.
* Pathfinding algorithms (currently only A\*).

And some utilities:

* Logging utilities.
* String formatting routine
* Engine for serializing C++ objects with unified interface for storing/restoring.
* Tiny but flexible unit-testing framework (suites, fixtures, various asserts).
* Point class, foreach macro and some bits of useful utilities.
* Pixmap class with attention on XPM files.
* XML reader class for simple XML file iteration.

## Installation

Run `make lib` to produce `libchthon2.so` or `libchthon2.dll` (depends on platform). Currently it builds under Linux and Windows (using MinGW).
Run `make docs` to create documentation. It will be placed in `docs/html` directory. It uses Doxygen.
Makefile target `make all` runs both `lib` and `docs`.
Makefile target `make install` installs lib and docs (they needed to be created beforehand) system-wide or locally. Makefile variable `INSTALL_PREFIX` controls destination location. It defaults to `/usr/local`.
