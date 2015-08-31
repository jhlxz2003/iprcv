#ifndef _USTRDUP_H
#define _USTRDUP_H

// Copyright (c) 1996-2008 Live Networks, Inc.  All rights reserved.
// A C++ equivalent to the standard C routine "strdup()".
// This generates a char* that can be deleted using "delete[]"
// Header

char* u_strdup(char const* str);
// Note: strDup(NULL) returns NULL

char* u_strdupsz(char const* str);
// Like "strDup()", except that it *doesn't* copy the original.
// (Instead, it just allocates a string of the same size as the original.)

#endif
