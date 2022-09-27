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

#ifndef _ND_SHAPE_BOX_GLUE_H_
#define _ND_SHAPE_BOX_GLUE_H_

#include "ndShapeGlue.h"
#include "ndShapeBox.h"

class ndShapeBoxGlue: public ndShapeGlue
{
	public:
	ndShapeBoxGlue(float size_x, float size_y, float size_z)
		:ndShapeGlue(new ndShapeBox(size_x, size_y, size_z))
	{
	}
};

#endif 

