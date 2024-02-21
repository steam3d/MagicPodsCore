#pragma once

#include <cstdio>

#define LOG_RELEASE(format, ...) printf(format "\n" __VA_OPT__(,) __VA_ARGS__)

#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf(format "\n" __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif