#pragma once

#include<Windows.h>
//フレーム間の時間計算クラス
class HighResolutionTimer
{
public:
	//コンストラクタ
	HighResolutionTimer() : mDeltaTime(-1.0), mPausedTime(0), mStopped(false)
	{
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		mSecondsPerCount = 1.0 / static_cast<double>(counts_per_sec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mThisTime));
		mBaseTime = mThisTime;
		mLastTime = mThisTime;
	}

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is mStopped.
	float time_stamp() const  // in seconds
	{
		// If we are mStopped, do not count the time that has passed since we mStopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--mPausedTime-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        start_time     mStopTime    mThisTime

		if (mStopped)
		{
			return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
		}

		// The distance mThisTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mThisTime:  
		//
		//  (mThisTime - mPausedTime) - mBaseTime 
		//
		//                     |<--mPausedTime-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        start_time     mThisTime
		else
		{
			return static_cast<float>(((mThisTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
		}
	}

	float time_interval() const  // in seconds
	{
		return static_cast<float>(mDeltaTime);
	}

	void reset() // Call before message loop.
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mThisTime));
		mBaseTime = mThisTime;
		mLastTime = mThisTime;

		mStopTime = 0;
		mStopped = false;
	}

	void start() // Call when unpaused.
	{
		LONGLONG start_time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  mBaseTime       mStopTime        start_time     
		if (mStopped)
		{
			mPausedTime += (start_time - mStopTime);
			mLastTime = start_time;
			mStopTime = 0;
			mStopped = false;
		}
	}

	void stop() // Call when paused.
	{
		if (!mStopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mStopTime));
			mStopped = true;
		}
	}
	//時間を計算する
	void tick() // Call every frame.
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mThisTime));
		// Time difference between this frame and the previous.
		mDeltaTime = (mThisTime - mLastTime)*mSecondsPerCount;

		// Prepare for next frame.
		mLastTime = mThisTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

private:
	double mSecondsPerCount;
	double mDeltaTime;

	LONGLONG mBaseTime;
	LONGLONG mPausedTime;
	LONGLONG mStopTime;
	LONGLONG mLastTime;
	LONGLONG mThisTime;

	bool mStopped;
};
