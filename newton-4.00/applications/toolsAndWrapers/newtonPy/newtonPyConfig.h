/* Copyright (c) <2003-2021> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


#ifndef _NEWTON_PY_CONFIG_H_
#define _NEWTON_PY_CONFIG_H_

#define D_INLINE inline
#define D_CORE_API 
#define D_NEWTON_API
#define D_COLLISION_API

#define D_MSV_NEWTON_ALIGN_16
#define D_GCC_NEWTON_ALIGN_16
#define D_MSV_NEWTON_ALIGN_32
#define D_GCC_NEWTON_ALIGN_32

#define D_CLASS_REFLECTION(x)

typedef int8_t dInt8;
typedef uint8_t dUnsigned8;
typedef int16_t dInt16;
typedef uint16_t dUnsigned16;
typedef int32_t dInt32;
typedef uint32_t dUnsigned32;
typedef int64_t dInt64;
typedef uint64_t dUnsigned64;
typedef float dFloat32;
typedef double dFloat64;


#endif 

