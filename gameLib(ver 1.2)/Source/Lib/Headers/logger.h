#pragma once
#include <Windows.h>
#include <stdio.h>

//Log output
void Print(const char* format, ...);


#if 	defined(_DEBUG)
#define	LOGGER(...)		{ Print(__VA_ARGS__); }
#else
#define	LOGGER(...)		{}
#endif
