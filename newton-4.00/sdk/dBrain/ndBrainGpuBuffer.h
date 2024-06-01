/* Copyright (c) <2003-2022> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/
#ifndef __ND_BRAIN_GPU_BUFFER_H__
#define __ND_BRAIN_GPU_BUFFER_H__

#include "ndBrainStdafx.h"
//#include "ndBrainGpuContext.h"

class ndBrainVector;
class ndBrainGpuContext;
class ndBrainGpuBufferBase;

class ndScopeMapBuffer
{
	public:
	ndScopeMapBuffer(ndBrainGpuBufferBase& buffer);
	~ndScopeMapBuffer();

	void* GetPointer() const;

	private:
	void* m_mappedMemory;
	ndBrainGpuBufferBase* m_buffer;
};

#if !defined (D_USE_VULKAN_SDK)

class ndBrainGpuBufferBase : public ndClassAlloc
{
	protected:
	ndBrainGpuBufferBase(ndBrainGpuContext* const context, ndInt32 sizeInByte);
	virtual ~ndBrainGpuBufferBase();

	public:
	void* GetBuffer() const;
	ndInt32 SizeInBytes() const;

	protected:
	ndInt32 m_sizeInBytes;
	friend class ndScopeMapBuffer;
};

#else

class ndBrainGpuBufferBase : public ndClassAlloc
{
	protected:
	ndBrainGpuBufferBase(ndBrainGpuContext* const context, ndInt32 sizeInByte);
	virtual ~ndBrainGpuBufferBase();

	uint32_t FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

	public:
	VkBuffer GetBuffer() const;
	ndInt32 SizeInBytes() const;

	protected:
	VkBuffer m_buffer;
	VkDeviceMemory m_bufferMemory;
	ndBrainGpuContext* m_context;
	ndInt32 m_sizeInBytes;
	friend class ndScopeMapBuffer;
};

#endif

// **************************************************************************
// 
// 
// **************************************************************************
class ndBrainGpuFloatBuffer : public ndBrainGpuBufferBase
{
	public:
	ndBrainGpuFloatBuffer(ndBrainGpuContext* const context, ndInt32 size);

	void UnloadData(ndBrainVector& output);
	void LoadData(const ndBrainVector& input);
};

#endif