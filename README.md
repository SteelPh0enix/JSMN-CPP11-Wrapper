# JSMN C++11 wrapper

*jsmn (pronounced like 'jasmine') is a minimalistic JSON parser in C. It can be easily integrated into resource-limited or embedded projects*. I love the concept of this library, but one thing that bothers me is it's API.

I usually create software using modern C++ (at least C++11), therefore dealing with C-style library inside C++11 code can look pretty nasty.

That's why i created this wrapper for jsmn, which will hide C-style API from developer eyes, and give him at least *slightly* better way to deal with JSON's, while maintaining speed and effeciency of jsmn.

### What about creating JSONs? Can i do it with this library?

You know, [`snprintf`](https://en.cppreference.com/w/c/io/fprintf) is a thing. I honestly don't have any good idea on how to wrap JSON creation better and more user-friendly, than `snprintf` does, on low-level/low-resource platforms without C++ stdlib, without going into dynamic memory allocation territory. Let's just stick to it, it's not that bad.

## Dependencies

At least C++11-compatible compiler. I've used [modernized (header-only)](https://github.com/zserge/jsmn/tree/modernize) version of JSMN in development, so if you want to use standard, separated version, you have to compile and link it by yourself.

Also, it's slightly modified (added some `inline`'s to make it suitable for template usage), so i suggest to `diff` it before swapping.

In case you'd want to run tests, you will need [Catch2 library](https://github.com/catchorg/Catch2) and CMake. Go to [tests](./test) directory for more information.

**It does not need C++ standard library support, only C stdlib is necessary**, which makes it a good choice for embedded platforms (Arduino for example)

## Usage

Download JSMN and my wrapper, put them in same directory, setup your project, and then go to Wiki and **RTFM**.
