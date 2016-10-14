//--------------------------------------------------------------------------------------
// File: DXUTMisc.cpp
//
// Shortcut macros and functions for using DX objects
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#include "dxut.h"
#include "dxutmisc.h"
#include <mmsystem.h>
#pragma comment( lib, "winmm.lib" )

#undef min // use __min instead
#undef max // use __max instead

//--------------------------------------------------------------------------------------
CDXUTTimer* WINAPI DXUTGetGlobalTimer()
{
    // Using an accessor function gives control of the construction order
    static CDXUTTimer timer;
    return &timer;
}


//--------------------------------------------------------------------------------------
CDXUTTimer::CDXUTTimer()
{
    m_bTimerStopped = true;
    m_llQPFTicksPerSec = 0;

    m_llStopTime = 0;
    m_llLastElapsedTime = 0;
    m_llBaseTime = 0;

    // Use QueryPerformanceFrequency to get the frequency of the counter
    LARGE_INTEGER qwTicksPerSec = { 0 };
    QueryPerformanceFrequency( &qwTicksPerSec );
    m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Reset()
{
    LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

    m_llBaseTime = qwTime.QuadPart;
    m_llLastElapsedTime = qwTime.QuadPart;
    m_llStopTime = 0;
    m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Start()
{
    // Get the current time
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );

    if( m_bTimerStopped )
        m_llBaseTime += qwTime.QuadPart - m_llStopTime;
    m_llStopTime = 0;
    m_llLastElapsedTime = qwTime.QuadPart;
    m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Stop()
{
    if( !m_bTimerStopped )
    {
        LARGE_INTEGER qwTime = { 0 };
        QueryPerformanceCounter( &qwTime );
        m_llStopTime = qwTime.QuadPart;
        m_llLastElapsedTime = qwTime.QuadPart;
        m_bTimerStopped = TRUE;
    }
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::Advance()
{
    m_llStopTime += m_llQPFTicksPerSec / 10;
}


//--------------------------------------------------------------------------------------
double CDXUTTimer::GetAbsoluteTime()
{
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );

    double fTime = qwTime.QuadPart / ( double )m_llQPFTicksPerSec;

    return fTime;
}


//--------------------------------------------------------------------------------------
double CDXUTTimer::GetTime()
{
    LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

    double fAppTime = ( double )( qwTime.QuadPart - m_llBaseTime ) / ( double )m_llQPFTicksPerSec;

    return fAppTime;
}


//--------------------------------------------------------------------------------------
void CDXUTTimer::GetTimeValues( double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime )
{
    assert( pfTime && pfAbsoluteTime && pfElapsedTime );

    LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

    float fElapsedTime = ( float )( ( double )( qwTime.QuadPart - m_llLastElapsedTime ) / ( double )
                                    m_llQPFTicksPerSec );
    m_llLastElapsedTime = qwTime.QuadPart;

    // Clamp the timer to non-negative values to ensure the timer is accurate.
    // fElapsedTime can be outside this range if processor goes into a 
    // power save mode or we somehow get shuffled to another processor.  
    // However, the main thread should call SetThreadAffinityMask to ensure that 
    // we don't get shuffled to another processor.  Other worker threads should NOT call 
    // SetThreadAffinityMask, but use a shared copy of the timer data gathered from 
    // the main thread.
    if( fElapsedTime < 0.0f )
        fElapsedTime = 0.0f;

    *pfAbsoluteTime = qwTime.QuadPart / ( double )m_llQPFTicksPerSec;
    *pfTime = ( qwTime.QuadPart - m_llBaseTime ) / ( double )m_llQPFTicksPerSec;
    *pfElapsedTime = fElapsedTime;
}


//--------------------------------------------------------------------------------------
float CDXUTTimer::GetElapsedTime()
{
    LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

    double fElapsedTime = ( float )( ( double )( qwTime.QuadPart - m_llLastElapsedTime ) / ( double )
                                     m_llQPFTicksPerSec );
    m_llLastElapsedTime = qwTime.QuadPart;

    // See the explanation about clamping in CDXUTTimer::GetTimeValues()
    if( fElapsedTime < 0.0f )
        fElapsedTime = 0.0f;

    return ( float )fElapsedTime;
}


//--------------------------------------------------------------------------------------
// If stopped, returns time when stopped otherwise returns current time
//--------------------------------------------------------------------------------------
LARGE_INTEGER CDXUTTimer::GetAdjustedCurrentTime()
{
    LARGE_INTEGER qwTime;
    if( m_llStopTime != 0 )
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter( &qwTime );
    return qwTime;
}

//--------------------------------------------------------------------------------------
bool CDXUTTimer::IsStopped()
{
    return m_bTimerStopped;
}