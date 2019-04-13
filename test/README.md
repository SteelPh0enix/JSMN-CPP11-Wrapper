# Running tests

## tl;dr

Download [Catch2 release *.hpp file](https://github.com/catchorg/Catch2/releases) (i've used 2.7.1), put it in directory with test files, make sure you have CMake (at least 3.0) installed, and then

```
cmake . -G"YourGeneratorHere"
```

After that, build the stuff according to selected generator, for example

### Linux

```
cmake . -G"Unix Makefiles"
make
```

### Windows, with MinGW

```
cmake . -G"MinGW Makefiles"
mingw32-make
```

## Footnotes

Tests should be executed automatically.

If you want to put test executables and stuff in different directory, just create new directory (`mkdir build`?), `cd` to it and use `..` instead of `.` in cmake command. Or just use GUI, whatever your fetishes are.

If you want to manually tell CMake where to find Catch2, just set CATCH_INCLUDE_DIR variable. (for example, `cmake . -G"Unix Makefiles" -DCATCH_INCLUDE_DIR="/home/user/catch"`)