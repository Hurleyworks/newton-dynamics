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

package com.javaNewton;

import com.newton.ndVectorGlue;

public class nVector
{
    public nVector()
    {
        m_data = new float[4];
    }

    public nVector(float x, float y, float z, float w)
    {
        m_data = new float[4];
        Set(x, y, z, w);
    }

    public nVector (nVector v)
    {
        m_data = new float[4];
        Set(v);
    }

    public nVector (ndVectorGlue v)
    {
        m_data = new float[4];
        Set(v);
    }

    public void Set(float x, float y, float z, float w)
    {
        m_data[0] = x;
        m_data[1] = y;
        m_data[2] = z;
        m_data[3] = w;
    }

    public void Set(ndVectorGlue v)
    {
        m_data[0] = v.Get(0);
        m_data[1] = v.Get(1);
        m_data[2] = v.Get(2);
        m_data[3] = v.Get(3);
    }

    public void Set(nVector v)
    {
        m_data[0] = v.m_data[0];
        m_data[1] = v.m_data[1];
        m_data[2] = v.m_data[2];
        m_data[3] = v.m_data[3];
    }

    public ndVectorGlue CreateNative()
    {
        return new ndVectorGlue(m_data[0], m_data[1], m_data[2], m_data[3]);
    }

    public float[] m_data;
}
