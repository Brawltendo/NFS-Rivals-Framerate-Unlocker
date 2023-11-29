#pragma once
#include "EAAudioCore/base.h"
#include <stdint.h>


namespace EA
{
namespace Audio
{
namespace Core
{

class GinsuPlayer
{
public:
	static void ApplyPatches();

private:

	enum Attribute
	{
		ATTRIBUTE_SETFREQUENCY,
		ATTRIBUTE_SETSHUFFLEWIDTH,
		ATTRIBUTE_SETSLEWTIMEMS,
		ATTRIBUTE_GETSAMPLERATE,
		ATTRIBUTE_GETMINFREQ,
		ATTRIBUTE_GETMAXFREQ,
		ATTRIBUTE_MAX
	};

	enum EventId
	{
		EVENT_PLAY,
		EVENT_STOP
	};

	enum EPlayParams
	{
		E_PLAYPARAMS_GINFILEPOINTER,
		E_PLAYPARAMS_MAX
	};

	class GinsuSynthData
	{
	public:
		static const unsigned int SIZE_OF_OLDDATABLOCK = 152;

		unsigned char mOldDataBlock[SIZE_OF_OLDDATABLOCK];
		int mOldDataBlockIndex;
		int mTempStoreBlockIndex;
		unsigned char* mpTempStore;
		unsigned int muTempStoreSize;
		bool mbOldDataBlockValid;
		float mLastInputSample;
		int mCycleCount;
		float mMinFrequency;
		float mMaxFrequency;
		int mSegCount;
		int mSampleCount;
		int mSampleRate;
		uintptr_t mFreqOffset;
		uintptr_t mCycleOffset;
		unsigned char* mSampleData;
		float mMinPeriod;
		int mCurrentBlock;
		bool mbPcm;
		float mSample[32];
	};
	static_assert(sizeof(GinsuSynthData) == 0x178, "Wrong size!");

public:
	void** __vftable;
	char gap8[0x10];
	void* mpSystemUseGetSystemAccessor;
	char gap_20[0x32];
	char field_52;
	char gap_53[5];
	Param mAttribute[ATTRIBUTE_MAX];
	bool mPlaying;
	int mOutputSamplesRequested;
	GinsuSynthData mSynthData;
	void* pGinFile;
	float mSampleRate;
	float mPrevSampleRate;
	float mfSlewSamplesRemaining;
	float mfPreviousFrequency;
	float mfTargetFrequency;
	float mfCurrentFrequency;
	int mPlaybackPos;
	unsigned int mRandomSeed;
	double mCurrentMixTime;
	double mNextJumpTime;

};
static_assert(sizeof(GinsuPlayer) == 0x240, "Wrong size!");

} // namespace Core
} // namespace Audio
} // namespace EA