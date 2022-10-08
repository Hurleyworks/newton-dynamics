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

package com.example.androidapp;

public class SceneMeshSegment
{
    public SceneMeshSegment(int indexOffset, int indexCount)
    {
        m_indexCount = indexCount;
        m_indexOffset = indexOffset;
    }


    private int m_indexCount;
    private int m_indexOffset;
}