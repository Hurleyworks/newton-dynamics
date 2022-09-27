/* Copyright (c) <2003-2021> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

// File : example.i 
%module (directors="1") newton

#pragma SWIG nowarn=312		//Nested union not supported
#pragma SWIG nowarn=325		//Nested struct not currently supported 
#pragma SWIG nowarn=389		//operator[] ignored
#pragma SWIG nowarn=401		//Nothing known about base class
#pragma SWIG nowarn=473		//Returning a pointer or reference in a director method is not recommended.
#pragma SWIG nowarn=516		//Overloaded method ignored

%begin 
%{
	#pragma warning(disable:4127 4316 4456 4701 4706)
%}

%{
	#include <ndNewton.h>
	#include "nConfig.h"
	#include "ndWorldGlue.h"
	#include "ndShapeGlue.h"
	#include "ndVectorGlue.h"
	#include "ndMatrixGlue.h"
	#include "ndShapeBoxGlue.h"
	#include "ndRigidBodyGlue.h"
	#include "ndBodyNotifyGlue.h"
	#include "ndShapeInstanceGlue.h"
%}

%feature("director") ndBodyNotifyGlue;

%include "nConfig.h"

//%rename(GetElement) ndVector::operator[](ndInt32 i);
//%rename(GetElement) ndVector::operator[](ndInt32 i) const;
//%rename(Add) ndVector::operator+(const ndVector& src) const;
//%rename(Sub) ndVector::operator-(const ndVector& src) const;
//%rename(Mul) ndVector::operator*(const ndVector& src) const;
//%rename(AddEqual) ndVector::operator+=(const ndVector& src);
//%rename(SubEqual) ndVector::operator-=(const ndVector& src);
//%rename(MulEqual) ndVector::operator*=(const ndVector& src);
//%rename(Or) ndVector::operator|(const ndVector& src) const;
//%rename(And) ndVector::operator&(const ndVector& src) const;
//%rename(Xor) ndVector::operator^(const ndVector& src) const;
//%rename(Less) ndVector::operator<(const ndVector& src) const;
//%rename(Greather) ndVector::operator>(const ndVector& src) const;
//%rename(Identical) ndVector::operator==(const ndVector& src) const;
//%rename(LessEqual) ndVector::operator<=(const ndVector& src) const;
//%rename(GreatherEqual) ndVector::operator>=(const ndVector& src) const;
//%rename(GetElement) ndBigVector::operator[](ndInt32 i);
//%rename(GetElement) ndBigVector::operator[](ndInt32 i) const;
//%rename(Add) ndBigVector::operator+(const ndBigVector& src) const;
//%rename(Sub) ndBigVector::operator-(const ndBigVector& src) const;
//%rename(Mul) ndBigVector::operator*(const ndBigVector& src) const;
//%rename(AddEqual) ndBigVector::operator+=(const ndBigVector& src);
//%rename(SubEqual) ndBigVector::operator-=(const ndBigVector& src);
//%rename(MulEqual) ndBigVector::operator*=(const ndBigVector& src);
//%rename(Or) ndBigVector::operator|(const ndBigVector& src) const;
//%rename(And) ndBigVector::operator&(const ndBigVector& src) const;
//%rename(Xor) ndBigVector::operator^(const ndBigVector& src) const;
//%rename(Less) ndBigVector::operator<(const ndBigVector& src) const;
//%rename(Greather) ndBigVector::operator>(const ndBigVector& src) const;
//%rename(Identical) ndBigVector::operator==(const ndBigVector& src) const;
//%rename(LessEqual) ndBigVector::operator<=(const ndBigVector& src) const;
//%rename(GreatherEqual) ndBigVector::operator>=(const ndBigVector& src) const;


%rename(Assigment) ndShapeInstance::operator=;  
%rename(GetElement) ndMatrix::operator[](ndInt32 i);
%rename(GetElement) ndMatrix::operator[](ndInt32 i) const;
%rename(GetElement) ndMatrix::operator*(const ndMatrix& src) const;
%rename(Add) ndQuaternion::operator+(const ndQuaternion& src) const;
%rename(Sub) ndQuaternion::operator-(const ndQuaternion& src) const;
%rename(Mul) ndQuaternion::operator*(const ndQuaternion& src) const;

%rename(Create) ndShapeStatic_bvh::operator new;  
%rename(Destroy) ndShapeStatic_bvh::operator delete;  

%include "../../../sdk/dCore/ndVector.h"
%include "../../../sdk/dCore/ndMatrix.h"
%include "../../../sdk/dCore/ndQuaternion.h"

%include "../../../sdk/dCollision/ndShape.h"
%include "../../../sdk/dCollision/ndShapeBox.h"
%include "../../../sdk/dCollision/ndShapeCone.h"
%include "../../../sdk/dCollision/ndShapeConvex.h"
%include "../../../sdk/dCollision/ndShapeSphere.h"
%include "../../../sdk/dCollision/ndShapeInstance.h"
%include "../../../sdk/dCollision/ndShapeCapsule.h"
%include "../../../sdk/dCollision/ndShapeCompound.h"
%include "../../../sdk/dCollision/ndShapeCylinder.h"
%include "../../../sdk/dCollision/ndShapeStatic_bvh.h"
%include "../../../sdk/dCollision/ndShapeStaticMesh.h"
%include "../../../sdk/dCollision/ndShapeConvexHull.h"
%include "../../../sdk/dCollision/ndShapeHeightfield.h"
%include "../../../sdk/dCollision/ndShapeChamferCylinder.h"

%include "../../../sdk/dCollision/ndBody.h"
%include "../../../sdk/dCollision/ndBodyKinematic.h"
%include "../../../sdk/dCollision/ndBodyTriggerVolume.h"
%include "../../../sdk/dCollision/ndBodyPlayerCapsule.h"
%include "../../../sdk/dCollision/ndBodyKinematicBase.h"

%include "../../../sdk/dCollision/ndBodyNotify.h"
%include "../../../sdk/dCollision/ndContactNotify.h"
%include "../../../sdk/dCollision/ndRayCastNotify.h"
%include "../../../sdk/dCollision/ndBodiesInAabbNotify.h"

%include "../../../sdk/dNewton/ndWorld.h"

%include "ndMatrixGlue.h"
%include "ndVectorGlue.h"
%include "ndWorldGlue.h"
%include "ndShapeGlue.h"
%include "ndShapeBoxGlue.h"
%include "ndRigidBodyGlue.h"
%include "ndBodyNotifyGlue.h"
%include "ndShapeInstanceGlue.h"

