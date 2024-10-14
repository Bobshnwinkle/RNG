# RNG

### Overview
This is a simple lightweight and high speed Pseudo-Random Number Generator setup to work on either Windows or Linux

### Installation
Copy over the fstRand.h header file into your cpp project
then just reference it at the top of any file you wish to have access with:
```
#include "path/to/fstRand.h"
```
### Testing
Build the "test.cpp" file however you so wish, and run
the program should print out 100 generated values

### Usage
To use, create an instance of the fstRand class like so:
```
auto RNG = new fstRand(); //will auto-generate a seed
```
or
```
auto RNG  = new fstRand(_float seed_); //to use a pre-defined seed
```
NOTE: the seed will not garuntee the chain of values generated, as entropy is taken from the cycle count and not the seed alone