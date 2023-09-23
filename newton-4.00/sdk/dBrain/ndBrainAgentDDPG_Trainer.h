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

#ifndef _ND_BRAIN_AGENT_DDPG_TRAINER_H__
#define _ND_BRAIN_AGENT_DDPG_TRAINER_H__

#include "ndBrainStdafx.h"
#include "ndBrain.h"
#include "ndBrainAgent.h"
#include "ndBrainTrainer.h"
#include "ndBrainReplayBuffer.h"
#include "ndBrainLossLeastSquaredError.h"

// this is an implementation of the vanilla deep deterministic 
// policy gradient for continues control re enforcement learning.  
// ddpg algorithm as described in: https://arxiv.org/pdf/1509.02971.pdf

template<ndInt32 statesDim, ndInt32 actionDim>
class ndBrainAgentDDPG_Trainer: public ndBrainAgent, public ndBrainThreadPool
{
	public:
	class HyperParameters
	{
		public:
		HyperParameters()
		{
			m_bashBufferSize = 64;
			m_discountFactor = ndBrainFloat(0.99f);
			m_regularizer = ndBrainFloat(1.0e-6f);
			m_actorLearnRate = ndBrainFloat(0.0005f);
			m_criticLearnRate = ndBrainFloat(0.001f);
			m_replayBufferSize = 1024 * 512;
			m_replayBufferPrefill = 1024 * 4;
			m_softTargetFactor = ndBrainFloat(1.0e-3f);
			m_actionNoiseVariance = ndBrainFloat(0.05f);
			m_threadsCount = ndMin(ndBrainThreadPool::GetMaxThreads(), m_bashBufferSize / 4);
		}

		ndBrainFloat m_regularizer;
		ndBrainFloat m_discountFactor;
		ndBrainFloat m_actorLearnRate;
		ndBrainFloat m_criticLearnRate;
		ndBrainFloat m_softTargetFactor;
		ndBrainFloat m_actionNoiseVariance;

		ndInt32 m_threadsCount;
		ndInt32 m_bashBufferSize;
		ndInt32 m_replayBufferSize;
		ndInt32 m_replayBufferPrefill;
	};

	ndBrainAgentDDPG_Trainer(const HyperParameters& hyperParameters, const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic);
	~ndBrainAgentDDPG_Trainer();

	ndInt32 GetFramesCount() const;
	ndInt32 GetEposideCount() const;
	ndInt32 GetEpisodeFrames() const;
	ndBrainFloat GetCurrentValue() const;

	ndBrainFloat GetLearnRate() const;
	void SetLearnRate(ndBrainFloat learnRate);

	ndBrainFloat GetActionNoise() const;
	void SetActionNoise(ndBrainFloat noiseVaraince);

	protected:
	void Step();
	void Optimize();
	void OptimizeStep();
	bool IsTrainer() const;
	void Save(ndBrainSave* const loadSave) const;
	bool IsSampling() const;
	bool IsTerminal() const;
	ndBrainFloat GetReward() const;
	void SetBufferSize(ndInt32 size);
	ndBrainFloat PerturbeAction(ndBrainFloat action) const;
	void BackPropagateActor(const ndUnsigned32* const bashIndex);
	void BackPropagateCritic(const ndUnsigned32* const bashIndex);

	void InitWeights();
	void InitWeights(ndBrainFloat weighVariance, ndBrainFloat biasVariance);

	void BackPropagate();
	void CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions);

	ndBrain m_actor;
	ndBrain m_critic;
	ndBrain m_targetActor;
	ndBrain m_targetCritic;
	ndBrainOptimizerAdam* m_actorOptimizer;
	ndBrainOptimizerAdam* m_criticOptimizer;
	ndArray<ndBrainTrainer*> m_actorTrainers;
	ndArray<ndBrainTrainer*> m_criticTrainers;

	ndArray<ndInt32> m_bashSamples;
	ndBrainReplayBuffer<ndBrainFloat, statesDim, actionDim> m_replayBuffer;
	ndBrainReplayTransitionMemory<ndBrainFloat, statesDim, actionDim> m_currentTransition;

	ndBrainFloat m_currentQvalue;
	ndBrainFloat m_discountFactor;
	ndBrainFloat m_actorLearnRate;
	ndBrainFloat m_criticLearnRate;
	ndBrainFloat m_softTargetFactor;
	ndBrainFloat m_actionNoiseVariance;
	ndInt32 m_frameCount;
	ndInt32 m_framesAlive;
	ndInt32 m_eposideCount;
	ndInt32 m_bashBufferSize;
	ndInt32 m_replayBufferPrefill;
	bool m_collectingSamples;
};

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentDDPG_Trainer<statesDim, actionDim>::ndBrainAgentDDPG_Trainer(const HyperParameters& hyperParameters, const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic)
	:ndBrainAgent()
	,m_actor(*(*actor))
	,m_critic(*(*critic))
	,m_targetActor(m_actor)
	,m_targetCritic(m_critic)
	,m_actorOptimizer(nullptr)
	,m_criticOptimizer(nullptr)
	,m_bashSamples()
	,m_replayBuffer()
	,m_currentTransition()
	,m_currentQvalue(ndBrainFloat(0.0f))
	,m_discountFactor(hyperParameters.m_discountFactor)
	,m_actorLearnRate(hyperParameters.m_actorLearnRate)
	,m_criticLearnRate(hyperParameters.m_criticLearnRate)
	,m_softTargetFactor(hyperParameters.m_softTargetFactor)
	,m_actionNoiseVariance(hyperParameters.m_actionNoiseVariance)
	,m_frameCount(0)
	,m_framesAlive(0)
	,m_eposideCount(0)
	,m_bashBufferSize(hyperParameters.m_bashBufferSize)
	,m_replayBufferPrefill(hyperParameters.m_replayBufferPrefill)
	,m_collectingSamples(true)
{
	ndAssert(m_critic.GetOutputSize() == 1);
	ndAssert(m_critic.GetInputSize() == (m_actor.GetInputSize() + m_actor.GetOutputSize()));
	ndAssert(!strcmp((m_actor[m_actor.GetCount() - 1])->GetLabelId(), "ndBrainLayerTanhActivation"));
	//ndAssert(!strcmp((m_critic[m_critic.GetCount() - 1])->GetLabelId(), "ndBrainLayerReluActivation"));
	
	m_actorTrainers.SetCount(0);
	m_criticTrainers.SetCount(0);
	//SetThreadCount(hyperParameters.m_threadsCount);
SetThreadCount(16);
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		m_actorTrainers.PushBack(new ndBrainTrainer(&m_actor));
		m_criticTrainers.PushBack(new ndBrainTrainer(&m_critic));
	}
	
	SetBufferSize(hyperParameters.m_replayBufferSize);
	InitWeights();

	m_actorOptimizer = new ndBrainOptimizerAdam();
	m_criticOptimizer = new ndBrainOptimizerAdam();
	m_actorOptimizer->SetRegularizer(hyperParameters.m_regularizer);
	m_criticOptimizer->SetRegularizer(hyperParameters.m_regularizer);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentDDPG_Trainer<statesDim, actionDim>::~ndBrainAgentDDPG_Trainer()
{
	for (ndInt32 i = 0; i < m_actorTrainers.GetCount(); ++i)
	{
		delete m_actorTrainers[i];
		delete m_criticTrainers[i];
	}

	delete m_actorOptimizer;
	delete m_criticOptimizer;
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentDDPG_Trainer<statesDim, actionDim>::IsTrainer() const
{
	return true;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::InitWeights()
{
	m_actor.InitWeightsXavierMethod();
	m_critic.InitWeightsXavierMethod();

	m_targetActor.CopyFrom(m_actor);
	m_targetCritic.CopyFrom(m_critic);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::InitWeights(ndBrainFloat weighVariance, ndBrainFloat biasVariance)
{
	m_actor.InitWeights(weighVariance, biasVariance);
	m_critic.InitWeights(weighVariance, biasVariance);

	m_targetActor.CopyFrom(m_actor);
	m_targetCritic.CopyFrom(m_critic);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetLearnRate() const
{
	return m_criticLearnRate;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::SetLearnRate(ndBrainFloat learnRate)
{
	m_criticLearnRate = learnRate;
	m_actorLearnRate = learnRate * ndBrainFloat (0.125f);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetActionNoise() const
{
	return m_actionNoiseVariance;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::SetActionNoise(ndBrainFloat noiseVariance)
{
	m_actionNoiseVariance = noiseVariance;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCurrentValue() const
{
	return m_currentQvalue;
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentDDPG_Trainer<statesDim, actionDim>::IsSampling() const
{
	return m_collectingSamples;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetFramesCount() const
{
	return m_frameCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetEposideCount() const
{
	return m_eposideCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetEpisodeFrames() const
{
	return m_framesAlive;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::SetBufferSize(ndInt32 size)
{
	m_replayBuffer.SetSize(size);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::BackPropagateCritic(const ndUnsigned32* const shuffleBuffer)
{
	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer](ndInt32 threadIndex, ndInt32 threadCount)
	{
		class Loss: public ndBrainLossLeastSquaredError
		{
			public:
			Loss(ndBrainTrainer& trainer, ndBrainAgentDDPG_Trainer<statesDim, actionDim>* const agent, ndBrainFloat discountFactor)
				:ndBrainLossLeastSquaredError(trainer.GetBrain()->GetOutputSize())
				,m_criticTrainer(trainer)
				,m_agent(agent)
				,m_reward(0.0f)
				,m_discountFactor(discountFactor)
				,m_isTerminal(false)
			{
			}

			void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
			{
				ndAssert(loss.GetCount() == 1);
				ndAssert(output.GetCount() == 1);
				ndAssert(m_truth.GetCount() == m_criticTrainer.GetBrain()->GetOutputSize());

				ndBrainFloat criticOutputBuffer[2];
				ndBrainMemVector criticOutput(criticOutputBuffer, 1);

				ndBrainFloat targetValue = m_reward;
				if (!m_isTerminal)
				{
					ndBrainMemVector criticInput(m_targetInputBuffer, statesDim + actionDim);
					m_agent->m_targetCritic.MakePrediction(criticInput, criticOutput);
					targetValue = m_reward + m_discountFactor * criticOutput[0];
				}
				criticOutput[0] = targetValue;
				
				SetTruth(criticOutput);
				ndBrainLossLeastSquaredError::GetLoss(output, loss);
			}

			ndBrainTrainer& m_criticTrainer;
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>* m_agent;
			ndBrainFloat m_reward;
			ndBrainFloat m_discountFactor;
			bool m_isTerminal;
			ndBrainFloat m_targetInputBuffer[(statesDim + actionDim) * 2];
		};

		ndBrainTrainer& trainer = *m_criticTrainers[threadIndex];
		Loss loss(trainer, this, m_discountFactor);

		ndBrainFloat actorInputBuffer[statesDim * 2];
		ndBrainFloat actorOutputBuffer[actionDim * 2];
		ndBrainFloat inputBuffer[(statesDim + actionDim) * 2];
		ndBrainMemVector actorInput(actorInputBuffer, statesDim);
		ndBrainMemVector actorOutput(actorOutputBuffer, actionDim);
		ndBrainMemVector input(inputBuffer, statesDim + actionDim);

		const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndInt32 index = ndInt32(shuffleBuffer[i]);
			const ndBrainReplayTransitionMemory<ndBrainFloat, statesDim, actionDim>& transition = m_replayBuffer[index];

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				actorInput[j] = transition.m_nextState[j];
				loss.m_targetInputBuffer[j] = transition.m_nextState[j];
			}
			m_targetActor.MakePrediction(actorInput, actorOutput);
			for (ndInt32 j = 0; j < actionDim; ++j)
			{
				loss.m_targetInputBuffer[j + statesDim] = actorOutput[j];
			}

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				input[j] = transition.m_state[j];
			}
			for (ndInt32 j = 0; j < actionDim; ++j)
			{
				input[j + statesDim] = transition.m_action[j];
			}

			loss.m_reward = transition.m_reward;
			loss.m_isTerminal = transition.m_terminalState;
			trainer.BackPropagate(input, loss);
		}
	});

	ndBrainThreadPool::ParallelExecute(PropagateBash);
	m_criticOptimizer->Update(this, m_criticTrainers, m_criticLearnRate);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::BackPropagateActor(const ndUnsigned32* const shuffleBuffer)
{
	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer](ndInt32 threadIndex, ndInt32 threadCount)
	{
		class ActorLoss: public ndBrainLoss
		{
			public:
			ActorLoss(ndBrainTrainer& actorTrainer, ndBrainAgentDDPG_Trainer<statesDim, actionDim>* const agent)
				:ndBrainLoss()
				,m_actorTrainer(actorTrainer)
				,m_agent(agent)
				,m_index(0)
			{
			}

			void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
			{
				ndAssert(loss.GetCount() == actionDim);
				ndAssert(output.GetCount() == actionDim);
				const ndBrainReplayTransitionMemory<ndBrainFloat, statesDim, actionDim>& transition = m_agent->m_replayBuffer[m_index];
				
				ndBrainFloat criticInputBuffer[(statesDim + actionDim) * 2];
				ndBrainMemVector input(criticInputBuffer, statesDim + actionDim);
				for (ndInt32 i = 0; i < statesDim; ++i)
				{
					input[i] = transition.m_state[i];
				}
				for (ndInt32 i = 0; i < actionDim; ++i)
				{
					input[i + statesDim] = output[i];
				}
				m_agent->m_critic.CalculateInputGradient(input, input);
				
				for (ndInt32 i = 0; i < actionDim; ++i)
				{
					loss[i] = input[statesDim + i];
				}
			}

			ndBrainTrainer& m_actorTrainer;
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>* m_agent;
			ndInt32 m_index;
		};

		ndBrainTrainer& actorTrainer = *m_actorTrainers[threadIndex];
		ActorLoss loss(actorTrainer, this);

		ndBrainFloat inputBuffer[statesDim * 2];
		ndBrainMemVector input(inputBuffer, statesDim);

		const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndInt32 index = ndInt32(shuffleBuffer[i]);
			const ndBrainReplayTransitionMemory<ndBrainFloat, statesDim, actionDim>& transition = m_replayBuffer[index];

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				input[j] = transition.m_state[j];
			}
			loss.m_index = index;
			actorTrainer.BackPropagate(input, loss);
		}
	});

	ParallelExecute(PropagateBash);
	m_actorOptimizer->Update(this, m_actorTrainers, -m_actorLearnRate);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::BackPropagate()
{
	ndFixSizeArray<ndUnsigned32, 1024> shuffleBuffer;
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		shuffleBuffer.PushBack (ndRandInt() % m_replayBuffer.GetCount());
	}

	BackPropagateCritic(&shuffleBuffer[0]);
	BackPropagateActor(&shuffleBuffer[0]);
	m_targetActor.SoftCopy(m_actor, m_softTargetFactor);
	m_targetCritic.SoftCopy(m_critic, m_softTargetFactor);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::Save(ndBrainSave* const loadSave) const
{
	loadSave->Save(&m_actor);
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentDDPG_Trainer<statesDim, actionDim>::IsTerminal() const
{
	ndAssert(0);
	return false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetReward() const
{
	ndAssert(0);
	return ndBrainFloat(0.0f);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentDDPG_Trainer<statesDim, actionDim>::PerturbeAction(ndBrainFloat action) const
{
	ndBrainFloat actionNoise = ndBrainFloat(ndGaussianRandom(ndFloat32 (action), ndFloat32 (m_actionNoiseVariance)));
	return ndClamp(actionNoise, ndBrainFloat(-1.0f), ndBrainFloat(1.0f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::Optimize()
{
	BackPropagate();
	if (IsSampling())
	{
		ndExpandTraceMessage("%d start training: episode %d\n", m_frameCount, m_eposideCount);
	}
	m_collectingSamples = false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions)
{
	ndBrainFloat buffer[(statesDim + actionDim) * 2];
	ndBrainMemVector criticInput(buffer, statesDim + actionDim);
	for (ndInt32 i = 0; i < statesDim; ++i)
	{
		criticInput[i] = state[i];
	}
	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		criticInput[i + statesDim] = actions[i];
	}
	ndBrainMemVector criticOutput(&m_currentQvalue, 1);
	m_critic.MakePrediction(criticInput, criticOutput);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::Step()
{
	ndBrainFloat stateBuffer[statesDim * 2];
	ndBrainFloat actionBuffer[actionDim * 2];
	ndBrainMemVector state(stateBuffer, statesDim);
	ndBrainMemVector actions(actionBuffer, actionDim);

	GetObservation(&m_currentTransition.m_state[0]);
	for (ndInt32 i = 0; i < statesDim; ++i)
	{
		state[i] = m_currentTransition.m_state[i];
	}
	m_actor.MakePrediction(state, actions);

	// explore environment
	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		actions[i] = PerturbeAction(actions[i]);
	}
	ApplyActions(&actions[0]);
	m_currentTransition.m_reward = GetReward();

	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		m_currentTransition.m_action[i] = actions[i];
	}

	if (!IsSampling())
	{
		// Get Q vale from Critic
		CalculateQvalue(state, actions);
	}
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentDDPG_Trainer<statesDim, actionDim>::OptimizeStep()
{
	if (!m_frameCount)
	{
		ResetModel();
	}

	GetObservation(&m_currentTransition.m_nextState[0]);
	m_currentTransition.m_terminalState = IsTerminal();
	m_replayBuffer.AddTransition(m_currentTransition);

	if (m_frameCount > m_replayBufferPrefill)
	{
		Optimize();
	}

	if (m_currentTransition.m_terminalState)
	{
		ResetModel();
		m_currentTransition.Clear();
		if (IsSampling() && (m_eposideCount % 100 == 0))
		{
			ndExpandTraceMessage("collecting samples: frame %d out of %d, episode %d \n", m_frameCount, m_replayBuffer.GetCapacity(), m_eposideCount);
		}
		m_eposideCount++;
		m_framesAlive = 0;
	}

	m_frameCount++;
	m_framesAlive++;
}

#endif 
