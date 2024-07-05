/**************************************\
 pl-rt, v1.03
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt.h: Base API header
\**************************************/

#pragma once
#if defined(__cplusplus) && !defined(PLRTCPP)
	#error Using the C headers in a C++ program is not supported. Please use plrt.hpp
#endif

#include <plrt-types.h>
#include <plrt-memory.h>
#include <plrt-string.h>
#include <plrt-file.h>
#include <plrt-token.h>
#include <plrt-plml.h>
#include <plrt-posix.h>
