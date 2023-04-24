/* Copyright (c) <2003-2022> <Julio Jerez, Newton Game Dynamics>
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

#ifndef _ND_MODEL_HIERARCHICAL_ARTICULATION_H__
#define _ND_MODEL_HIERARCHICAL_ARTICULATION_H__

#include "ndModelStdafx.h"
#include "ndModelBase.h"

class ndModelHierarchicalArticulation: public ndModelBase
{
	public: 
	D_CLASS_REFLECTION(ndModelHierarchicalArticulation, ndModelBase)

	class ndNode : public ndNodeHierarchy<ndNode>
	{
		public:
		ndNode(ndBodyDynamic* const body, ndNode* const parent, ndJointBilateralConstraint* const joint);
		virtual ~ndNode();
		
		ndBodyDynamic* m_body;
		ndJointBilateralConstraint* m_joint;
	};

	ndModelHierarchicalArticulation();
	virtual ~ndModelHierarchicalArticulation();

	ndNode* GetRoot() const;
	ndNode* AddRootBody(ndSharedPtr<ndBody>& rootBody);
	ndNode* AddLimb(ndNode* const parent, ndSharedPtr<ndBody>& body, ndSharedPtr<ndJointBilateralConstraint>& joint);

	ndModelHierarchicalArticulation* GetAsModelHierarchicalArticulation();

	void NormalizeMassDistribution(ndFloat32 totalMass);

	protected:
	ndNode* m_rootNode;
};

#endif 
