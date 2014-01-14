libchthon
=========

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

And some utilities:

* Logging utilities.
* String formatting routine
* Engine for serializing C++ objects with unified interface for storing/restoring.
* Tiny but flexible unit-testing framework (suites, fixtures, various asserts).
* Object type registry class for simplified work with object meta-types.
* Point class, foreach macro and some bits of useful utilities.

Documentation and stuff are coming soon (I hope).
