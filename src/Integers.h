//
//  Integers.h
//  Brainfucker
//
//  Created by Nicolas Winkler on 30.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#ifndef _BRAINFUCKER_INTEGERS_H
#define _BRAINFUCKER_INTEGERS_H

#include <climits>

#if (USHORT_MAX == 0xFFFF)
typedef unsigned short UInt16;
#elif (UINT_MAX == 0xFFFF)
typedef unsigned int UInt16;
#else
typedef unsigned short UInt16;
#endif


#endif // _BRAINFUCKER_INTEGERS_H
