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

#ifndef _ND_AGENT_CONTINUE_VPG_H_TRAINER_H__
#define _ND_AGENT_CONTINUE_VPG_H_TRAINER_H__

#include "ndBrainStdafx.h"
#include "ndBrain.h"
#include "ndBrainAgent.h"
#include "ndBrainTrainer.h"
#include "ndBrainReplayBuffer.h"
#include "ndBrainLossLeastSquaredError.h"

// this is an implementation of the vanilla policy Gradient as described in:
// https://spinningup.openai.com/en/latest/algorithms/vpg.html

#define ND_USE_STATE_Q_VALUE_BASE_LINE
//#define ND_USE_CONSTANT_AVERAGE_BASELINE

template<ndInt32 statesDim, ndInt32 actionDim>
class ndBrainAgentContinueVPG_Trainer : public ndBrainAgent, public ndBrainThreadPool
{
	public:
	
	class HyperParameters
	{
		public:
		HyperParameters()
		{
			m_bashBufferSize = 32;
			m_bashTrajectoryCount = 100;
			m_numberOfHiddenLayers = 3;
			m_maxTrajectorySteps = 1024 * 2;
			m_extraTrajectorySteps = 1024 * 2;
			
			m_hiddenLayersNumberOfNeurons = 64;

			//m_sigma = ndBrainFloat(0.25f);
			m_sigma = ndBrainFloat(1.0f);
			m_learnRate = ndBrainFloat(0.0005f);
			m_regularizer = ndBrainFloat(1.0e-6f);
			m_discountFactor = ndBrainFloat(0.99f);
			m_threadsCount = ndMin(ndBrainThreadPool::GetMaxThreads(), ndMin(m_bashBufferSize, 16));
			//m_threadsCount = 1;
		}

		ndBrainFloat m_sigma;
		ndBrainFloat m_learnRate;
		ndBrainFloat m_regularizer;
		ndBrainFloat m_discountFactor;

		ndInt32 m_threadsCount;
		ndInt32 m_bashBufferSize;
		ndInt32 m_maxTrajectorySteps;
		ndInt32 m_bashTrajectoryCount;
		ndInt32 m_extraTrajectorySteps;
		ndInt32 m_numberOfHiddenLayers;
		ndInt32 m_hiddenLayersNumberOfNeurons;
	};

	class ndTrajectoryStep
	{
		public:
		ndTrajectoryStep()
			:m_actions()
			,m_observation()
			,m_reward(ndBrainFloat(0.0f))
		{
		}

		ndTrajectoryStep(const ndTrajectoryStep& src)
			:m_actions(src.m_actions)
			,m_observation(src.m_observation)
			,m_reward(src.m_reward)
		{
		}

		ndTrajectoryStep& operator=(const ndTrajectoryStep& src)
		{
			new (this) ndTrajectoryStep(src);
			return*this;
		}

		ndBrainFixSizeVector<actionDim> m_actions;
		ndBrainFixSizeVector<statesDim> m_observation;
		ndBrainFloat m_reward;
	};

	ndBrainAgentContinueVPG_Trainer(const HyperParameters& hyperParameters);
	virtual ~ndBrainAgentContinueVPG_Trainer();

	ndInt32 GetFramesCount() const;
	ndInt32 GetEposideCount() const;
	ndInt32 GetEpisodeFrames() const;

	bool IsTrainer() const;

	protected:
	void Step();
	void OptimizeStep();

	void Save(ndBrainSave* const loadSave);

	void InitWeights();
	void InitWeights(ndBrainFloat weighVariance, ndBrainFloat biasVariance);

	bool IsSampling() const;
	bool IsTerminal() const;
	ndBrainFloat CalculateReward();

	private:
	void Optimize();
	void BackPropagate();
	void SaveTrajectory();
	void SelectAction(ndBrainVector& probabilities) const;

	protected:
	ndBrain m_actor;
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	ndBrain m_baseLineValue;
#endif
	ndBrainOptimizerAdam* m_optimizer;
	ndArray<ndBrainTrainer*> m_trainers;
	ndArray<ndBrainTrainer*> m_weightedTrainer;
	ndArray<ndBrainTrainer*> m_auxiliaryTrainers;
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	ndBrainOptimizerAdam* m_baseLineValueOptimizer;
	ndArray<ndBrainTrainer*> m_baseLineValueTrainers;
#endif

	ndArray<ndTrajectoryStep> m_trajectory;
	ndArray<ndTrajectoryStep> m_trajectoryAccumulator;

	ndBrainFloat m_sigma;
	ndBrainFloat m_gamma;
	ndBrainFloat m_learnRate;
	ndInt32 m_frameCount;
	ndInt32 m_framesAlive;
	ndInt32 m_eposideCount;
	ndInt32 m_bashBufferSize;
	ndInt32 m_maxTrajectorySteps;
	ndInt32 m_extraTrajectorySteps;
	ndInt32 m_bashTrajectoryIndex;
	ndInt32 m_bashTrajectoryCount;
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	ndInt32 m_baseValueWorkingBufferSize;
#endif
	ndBrainVector m_workingBuffer;
	ndMovingAverage<128> m_averageQvalue;
	ndMovingAverage<128> m_averageFramesPerEpisodes;
};

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::ndBrainAgentContinueVPG_Trainer(const HyperParameters& hyperParameters)
	:ndBrainAgent()
	,ndBrainThreadPool()
	,m_actor()
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	,m_baseLineValue()
#endif
	,m_optimizer(nullptr)
	,m_trainers()
	,m_weightedTrainer()
	,m_auxiliaryTrainers()
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	,m_baseLineValueOptimizer(nullptr)
	,m_baseLineValueTrainers()
#endif
	,m_trajectory()
	,m_trajectoryAccumulator()
	,m_sigma(hyperParameters.m_sigma)
	,m_gamma(hyperParameters.m_discountFactor)
	,m_learnRate(hyperParameters.m_learnRate)
	,m_frameCount(0)
	,m_framesAlive(0)
	,m_eposideCount(0)
	,m_bashBufferSize(hyperParameters.m_bashBufferSize)
	,m_maxTrajectorySteps(hyperParameters.m_maxTrajectorySteps)
	,m_extraTrajectorySteps(hyperParameters.m_extraTrajectorySteps)
	,m_bashTrajectoryIndex(0)
	,m_bashTrajectoryCount(hyperParameters.m_bashTrajectoryCount)
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	,m_baseValueWorkingBufferSize(0)
#endif
	,m_workingBuffer()
	,m_averageQvalue()
	,m_averageFramesPerEpisodes()
{
	// build neural net
	SetThreadCount(hyperParameters.m_threadsCount);
	ndFixSizeArray<ndBrainLayer*, 32> layers;

	layers.SetCount(0);
	layers.PushBack(new ndBrainLayerLinear(statesDim, hyperParameters.m_hiddenLayersNumberOfNeurons));
	layers.PushBack(new ndBrainLayerTanhActivation(layers[layers.GetCount() - 1]->GetOutputSize()));
	for (ndInt32 i = 1; i < hyperParameters.m_numberOfHiddenLayers; ++i)
	{
		ndAssert(layers[layers.GetCount() - 1]->GetOutputSize() == hyperParameters.m_hiddenLayersNumberOfNeurons);
		layers.PushBack(new ndBrainLayerLinear(hyperParameters.m_hiddenLayersNumberOfNeurons, hyperParameters.m_hiddenLayersNumberOfNeurons));
		layers.PushBack(new ndBrainLayerTanhActivation(hyperParameters.m_hiddenLayersNumberOfNeurons));
	}
	layers.PushBack(new ndBrainLayerLinear(hyperParameters.m_hiddenLayersNumberOfNeurons, actionDim));
	layers.PushBack(new ndBrainLayerTanhActivation(actionDim));
	for (ndInt32 i = 0; i < layers.GetCount(); ++i)
	{
		m_actor.AddLayer(layers[i]);
	}
	m_actor.InitWeightsXavierMethod();
	ndAssert(!strcmp((m_actor[m_actor.GetCount() - 1])->GetLabelId(), "ndBrainLayerTanhActivation"));

	m_trainers.SetCount(0);
	m_auxiliaryTrainers.SetCount(0);
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		ndBrainTrainer* const trainer = new ndBrainTrainer(&m_actor);
		m_trainers.PushBack(trainer);

		ndBrainTrainer* const auxiliaryTrainer = new ndBrainTrainer(&m_actor);
		m_auxiliaryTrainers.PushBack(auxiliaryTrainer);
	}

	m_weightedTrainer.PushBack(m_trainers[0]);
	m_optimizer = new ndBrainOptimizerAdam();
	m_optimizer->SetRegularizer(hyperParameters.m_regularizer);

#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	layers.SetCount(0);
	layers.PushBack(new ndBrainLayerLinear(statesDim, hyperParameters.m_hiddenLayersNumberOfNeurons));
	layers.PushBack(new ndBrainLayerTanhActivation(layers[layers.GetCount() - 1]->GetOutputSize()));
	for (ndInt32 i = 1; i < hyperParameters.m_numberOfHiddenLayers; ++i)
	{
		ndAssert(layers[layers.GetCount() - 1]->GetOutputSize() == hyperParameters.m_hiddenLayersNumberOfNeurons);
		layers.PushBack(new ndBrainLayerLinear(hyperParameters.m_hiddenLayersNumberOfNeurons, hyperParameters.m_hiddenLayersNumberOfNeurons));
		layers.PushBack(new ndBrainLayerTanhActivation(hyperParameters.m_hiddenLayersNumberOfNeurons));
	}
	layers.PushBack(new ndBrainLayerLinear(hyperParameters.m_hiddenLayersNumberOfNeurons, 1));
	for (ndInt32 i = 0; i < layers.GetCount(); ++i)
	{
		m_baseLineValue.AddLayer(layers[i]);
	}
	m_baseLineValue.InitWeightsXavierMethod();

	ndAssert(m_baseLineValue.GetOutputSize() == 1);
	ndAssert(m_baseLineValue.GetInputSize() == m_actor.GetInputSize());
	ndAssert(!strcmp((m_baseLineValue[m_baseLineValue.GetCount() - 1])->GetLabelId(), "ndBrainLayerLinear"));

	m_baseLineValueTrainers.SetCount(0);
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		ndBrainTrainer* const trainer = new ndBrainTrainer(&m_baseLineValue);
		m_baseLineValueTrainers.PushBack(trainer);
	}

	m_baseLineValueOptimizer = new ndBrainOptimizerAdam();
	//m_baseLineValueOptimizer->SetRegularizer(hyperParameters.m_regularizer);
	m_baseLineValueOptimizer->SetRegularizer(ndBrainFloat(1.0e-4f));

	m_baseValueWorkingBufferSize = m_baseLineValue.CalculateWorkingtBufferSize();
	m_workingBuffer.SetCount(m_baseValueWorkingBufferSize * hyperParameters.m_threadsCount);
#endif

	m_trajectory.SetCount(m_maxTrajectorySteps + m_extraTrajectorySteps);
	m_trajectoryAccumulator.SetCount(m_bashTrajectoryCount * m_maxTrajectorySteps + 1024);
	m_trajectory.SetCount(0);
	m_trajectoryAccumulator.SetCount(0);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::~ndBrainAgentContinueVPG_Trainer()
{
	for (ndInt32 i = 0; i < m_trainers.GetCount(); ++i)
	{
		delete m_trainers[i];
		delete m_auxiliaryTrainers[i];
	}
	delete m_optimizer;

#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	for (ndInt32 i = 0; i < m_baseLineValueTrainers.GetCount(); ++i)
	{
		delete m_baseLineValueTrainers[i];
	}
	delete m_baseLineValueOptimizer;
#endif
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::IsTrainer() const
{
	return true;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::InitWeights()
{
	m_actor.InitWeightsXavierMethod();

	#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	m_baseLineValue.InitWeightsXavierMethod();
	#endif
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::InitWeights(ndBrainFloat weighVariance, ndBrainFloat biasVariance)
{
	ndAssert(0);
	m_actor.InitWeights(weighVariance, biasVariance);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::GetFramesCount() const
{
	return m_frameCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::IsSampling() const
{
	return false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::GetEposideCount() const
{
	return m_eposideCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::GetEpisodeFrames() const
{
	return m_framesAlive;
}

#pragma optimize( "", off )
template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::BackPropagate()
{
	auto ClearGradients = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
	{
		const ndStartEnd startEnd(m_trainers.GetCount(), threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndBrainTrainer* const trainer = m_trainers[i];
			trainer->ClearGradients();
		}
	});
	ndBrainThreadPool::ParallelExecute(ClearGradients);

	const ndInt32 steps = m_trajectoryAccumulator.GetCount();
	const ndBrainFloat invSigmaSquare = ndBrainFloat(1.0f) / (m_sigma * m_sigma);

	for (ndInt32 base = 0; base < steps; base += m_bashBufferSize)
	{
		auto CalculateGradients = ndMakeObject::ndFunction([this, base, invSigmaSquare](ndInt32 threadIndex, ndInt32 threadCount)
		{
			class Loss : public ndBrainLossLeastSquaredError
			{
				public:
				Loss(ndBrainTrainer& trainer, ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>* const agent, ndInt32 index, ndBrainFloat invSigmaSquare)
					:ndBrainLossLeastSquaredError(trainer.GetBrain()->GetOutputSize())
					,m_trainer(trainer)
					,m_agent(agent)
					,m_invSigmaSquare(invSigmaSquare)
					,m_index(index)
				{
				}

				void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
				{
					const ndTrajectoryStep& trajectoryStep = m_agent->m_trajectoryAccumulator[m_index];
					ndBrainFloat logProbAdvantage = trajectoryStep.m_reward * m_invSigmaSquare;
					for (ndInt32 i = actionDim - 1; i >= 0; --i)
					{
						loss[i] = logProbAdvantage * (trajectoryStep.m_actions[i] - output[i]);
					}
				}
	
				ndBrainTrainer& m_trainer;
				ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>* m_agent;
				const ndBrainFloat m_invSigmaSquare;
				ndInt32 m_index;
			};

			ndBrainFixSizeVector<statesDim> observations;
			const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
			for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
			{
				ndBrainTrainer& trainer = *m_auxiliaryTrainers[i];
				Loss loss(trainer, this, base + i, invSigmaSquare);
				if ((base + i) < m_trajectoryAccumulator.GetCount())
				{
					trainer.BackPropagate(m_trajectoryAccumulator[base + i].m_observation, loss);
				}
				else
				{
					trainer.ClearGradients();
				}
			}
		});
		ndBrainThreadPool::ParallelExecute(CalculateGradients);

		auto AddGradients = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
		{
			const ndStartEnd startEnd(m_trainers.GetCount(), threadIndex, threadCount);
			for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
			{
				ndBrainTrainer* const trainer = m_trainers[i];
				const ndBrainTrainer* const auxiliaryTrainer = m_auxiliaryTrainers[i];
				trainer->AddGradients(auxiliaryTrainer);
			}
		});
		ndBrainThreadPool::ParallelExecute(AddGradients);
	}

	m_optimizer->AccumulateGradients(this, m_trainers);
	m_weightedTrainer[0]->ScaleWeights(ndBrainFloat(1.0f) / ndBrainFloat(m_trajectoryAccumulator.GetCount()));
	m_optimizer->Update(this, m_weightedTrainer, -m_learnRate);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::Save(ndBrainSave* const loadSave)
{
	loadSave->Save(&m_actor);
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::IsTerminal() const
{
	ndAssert(0);
	return false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainFloat ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::CalculateReward()
{
	ndAssert(0);
	return ndBrainFloat(0.0f);
}

#pragma optimize( "", off )
template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::Optimize()
{
#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	auto CalculateAdavantage = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
	{
		ndBrainFixSizeVector<1> stateValue;
		ndBrainMemVector workingBuffer(&m_workingBuffer[threadIndex * m_baseValueWorkingBufferSize], m_baseValueWorkingBufferSize);

		const ndStartEnd startEnd(m_trajectoryAccumulator.GetCount(), threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			m_baseLineValue.MakePrediction(m_trajectoryAccumulator[i].m_observation, stateValue, workingBuffer);
			m_trajectoryAccumulator[i].m_reward -= stateValue[0];
		}
	});
	ndBrainThreadPool::ParallelExecute(CalculateAdavantage);

#elif defined (ND_USE_CONSTANT_AVERAGE_BASELINE)
	// using constant base line subtractions
	ndFloat64 sum = ndFloat64(0.0f);
	for (ndInt32 i = m_trajectoryAccumulator.GetCount() - 1; i >= 0; --i)
	{
		sum += m_trajectoryAccumulator[i].m_reward;
	}

	ndBrainFloat trajectoryBaseLine = ndBrainFloat(sum / ndBrainFloat(m_trajectoryAccumulator.GetCount()));
	for (ndInt32 i = m_trajectoryAccumulator.GetCount() - 1; i >= 0; --i)
	{
		m_trajectoryAccumulator[i].m_reward -= trajectoryBaseLine;
	}

#else
	// using normalize rewards, no sure where this come from but many people since to like it, 
	// basically it is just the scaling of the rewards, to make more or less equal
	ndFloat64 sum = ndFloat64(0.0f);
	ndFloat64 sum2 = ndFloat64(0.0f);
	for (ndInt32 i = m_trajectoryAccumulator.GetCount() - 1; i >= 0; --i)
	{
		ndFloat64 x = m_trajectoryAccumulator[i].m_reward;
		sum += x;
		sum2 += x * x;
	}
	ndFloat64 den = ndFloat64(1.0f) / ndBrainFloat(m_trajectoryAccumulator.GetCount());
	ndFloat64 average = sum * den;
	ndFloat64 variance2 = ndMax((sum2 * den - average * average), ndFloat64(1.0e-12f));

	ndBrainFloat mean = ndBrainFloat(average);
	ndBrainFloat invVariance = ndBrainFloat (1.0f) / ndBrainFloat(ndSqrt(variance2));
	for (ndInt32 i = m_trajectoryAccumulator.GetCount() - 1; i >= 0; --i)
	{
		ndBrainFloat x = m_trajectoryAccumulator[i].m_reward;
		m_trajectoryAccumulator[i].m_reward = (x - mean) * invVariance;
	}
#endif

	BackPropagate();
}

#pragma optimize( "", off )
template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::SaveTrajectory()
{
	m_trajectory.SetCount(m_trajectory.GetCount() - 1);

#ifdef ND_USE_STATE_Q_VALUE_BASE_LINE
	ndInt32 stepsCount = m_trajectory.GetCount() & -m_bashBufferSize;
	if ((stepsCount + 3) > m_trajectory.GetCount())
	{
		stepsCount -= m_bashBufferSize;
	}

	for (ndInt32 base = 0; base < stepsCount; base += m_bashBufferSize)
	{
		auto BackPropagateBash = ndMakeObject::ndFunction([this, base](ndInt32 threadIndex, ndInt32 threadCount)
		{
			ndBrainFixSizeVector<1> stateValue;
			ndBrainLossHuber loss(1);
			ndBrainMemVector workingBuffer(&m_workingBuffer[threadIndex * m_baseValueWorkingBufferSize], m_baseValueWorkingBufferSize);

			const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
			ndBrainFloat gamma1 = m_gamma;
			ndBrainFloat gamma2 = gamma1 * m_gamma;
			ndBrainFloat gamma3 = gamma2 * m_gamma;
			for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
			{
				ndBrainTrainer& trainer = *m_baseLineValueTrainers[i];
				ndInt32 index = base + i;
				m_baseLineValue.MakePrediction(m_trajectory[ndInt32(index + 3)].m_observation, stateValue, workingBuffer);
				ndBrainFloat q0 = m_trajectory[ndInt32(index + 0)].m_reward;
				ndBrainFloat q1 = m_trajectory[ndInt32(index + 1)].m_reward;
				ndBrainFloat q2 = m_trajectory[ndInt32(index + 2)].m_reward;
				ndBrainFloat q3 = stateValue[0];
				stateValue[0] = q0 + q1 * gamma1 + q2 * gamma2 + q3 * gamma3;
				loss.SetTruth(stateValue);
				trainer.BackPropagate(m_trajectory[ndInt32(index)].m_observation, loss);
			}
		});

		ndBrainThreadPool::ParallelExecute(BackPropagateBash);
		m_baseLineValueOptimizer->Update(this, m_baseLineValueTrainers, m_learnRate);
	}
#endif

	// using the Bellman equation to calculate trajectory rewards. (Monte Carlo method)
	ndReal averageGain = m_trajectory[m_trajectory.GetCount() - 1].m_reward;
	for (ndInt32 i = m_trajectory.GetCount() - 2; i >= 0; --i)
	{
		m_trajectory[i].m_reward += m_gamma * m_trajectory[i + 1].m_reward;
		averageGain += m_trajectory[i].m_reward;
	}

	m_averageFramesPerEpisodes.Update(ndReal(m_trajectory.GetCount()));
	m_averageQvalue.Update(averageGain / ndReal(m_trajectory.GetCount()));

	const ndInt32 clippedTrajectorySteps = ndMin(m_maxTrajectorySteps, m_trajectory.GetCount());
	for (ndInt32 i = 0; i < clippedTrajectorySteps; ++i)
	{
		m_trajectoryAccumulator.PushBack(m_trajectory[i]);
	}
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::SelectAction(ndBrainVector& actions) const
{
	// for now use a constant deviations until the algorithm is stable 
	for (ndInt32 i = actionDim - 1; i >= 0; --i)
	{
		ndBrainFloat sample = ndGaussianRandom(actions[i], m_sigma);
		ndBrainFloat squashedAction = ndClamp(sample, ndBrainFloat(-1.0f), ndBrainFloat(1.0f));
		actions[i] = squashedAction;
	}
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::Step()
{
	ndTrajectoryStep trajectoryStep;

	GetObservation(&trajectoryStep.m_observation[0]);
	m_actor.MakePrediction(trajectoryStep.m_observation, trajectoryStep.m_actions, m_workingBuffer);

	SelectAction(trajectoryStep.m_actions);
	ApplyActions(&trajectoryStep.m_actions[0]);
	trajectoryStep.m_reward = CalculateReward();
	
	ndAssert(m_trajectory.GetCount() < m_trajectory.GetCapacity());
	m_trajectory.PushBack(trajectoryStep);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentContinueVPG_Trainer<statesDim, actionDim>::OptimizeStep()
{
	if (!m_frameCount)
	{
		ResetModel();
		m_trajectory.SetCount(0);
	}

	bool isTeminal = IsTerminal() || (m_trajectory.GetCount() >= (m_extraTrajectorySteps + m_maxTrajectorySteps));
	if (isTeminal)
	{
		SaveTrajectory();
		m_bashTrajectoryIndex++;
		if (m_bashTrajectoryIndex >= m_bashTrajectoryCount)
		{
			Optimize();
			m_trajectoryAccumulator.SetCount(0);
			m_eposideCount++;
			m_framesAlive = 0;
			m_bashTrajectoryIndex = 0;
		}
		m_trajectory.SetCount(0);
		ResetModel();
	}
	
	m_frameCount++;
	m_framesAlive++;
}

#endif 