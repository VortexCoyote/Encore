#include "SongHandler.h"

#include "ofSoundPlayer.h"
#include "ofSoundUtils.h"
#include "ofMain.h"

#include <Windows.h>

#include "EditorConfig.h"

SongHandler::SongHandler()
{

}

SongHandler::~SongHandler()
{

}

void SongHandler::Init(std::string aPath, double aSyncThreshold)
{
	mySyncThreshold = aSyncThreshold;

	BASS_Free();
	BASS_Init(myDevice, myFreq, 0, 0, NULL);

	myStreamHandle = BASS_FX_TempoCreate(BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, BASS_STREAM_DECODE), BASS_FX_FREESOURCE);
	BASS_ChannelPlay(myStreamHandle, FALSE);
	BASS_ChannelPause(myStreamHandle);

	myIsPausing = true;
}

void SongHandler::Update()
{

	if (GetAsyncKeyState(VK_DOWN))
	{
		myCurrentTime -= 0.01;
		SetTimeS(myCurrentTime);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		myCurrentTime += 0.01;
		SetTimeS(myCurrentTime);
	}

	if (bool(GetAsyncKeyState(VK_RIGHT)) || bool(GetAsyncKeyState(VK_LEFT)))
	{
		mySpeed += 0.001f * (bool(GetAsyncKeyState(VK_RIGHT)) - bool(GetAsyncKeyState(VK_LEFT)));

		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(myStreamHandle, &info);

		BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_FREQ, float(info.freq) * mySpeed);
		BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_TEMPO, 0);

		std::cout << "Playback Speed: " << mySpeed << std::endl;
	}

	if (myCurrentTime >= GetSongLength())
		SetPause(true);
	

	BASS_Update(myStreamHandle);

	//myCurrentTime = BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetPosition(myStreamHandle, BASS_POS_BYTE));
	TryTimingSync();
}

void SongHandler::TogglePause()
{
	myIsPausing = !myIsPausing;
	
	if (myIsPausing)
		BASS_ChannelPause(myStreamHandle);
	else
		BASS_ChannelPlay(myStreamHandle, FALSE);

	myCurrentTime = GetRealCurrentTimeS();
}


void SongHandler::SetPause(bool aPause)
{
	myIsPausing = aPause;

	if (aPause)
		BASS_ChannelPause(myStreamHandle);
	else
		BASS_ChannelPlay(myStreamHandle, FALSE);

	myCurrentTime = GetRealCurrentTimeS();
}

void SongHandler::SetTimeS(double aTimeS)
{
	myCurrentTime = aTimeS;
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, myCurrentTime), BASS_POS_BYTE);
}

void SongHandler::SetTimeNormalized(float aNormalizedTime)
{
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, aNormalizedTime * GetSongLength()), BASS_POS_BYTE);
	myCurrentTime = GetRealCurrentTimeS();
}

void SongHandler::SongJumpAmount(float aSongJumpAmount)
{
	myCurrentTime += aSongJumpAmount;
	
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, myCurrentTime), BASS_POS_BYTE);

}

double SongHandler::GetCurrentTimeS()
{
	return myCurrentTime;
}

int SongHandler::GetCurrentTimeMS()
{
	return int(myCurrentTime * 1000);
}

float SongHandler::GetTimeNormalized()
{
	return myCurrentTime / GetSongLength();
}

double SongHandler::GetSongLength()
{
	return BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetLength(myStreamHandle, BASS_POS_BYTE));
}

void SongHandler::StopSong()
{
	BASS_ChannelStop(myStreamHandle);
}

void SongHandler::TryTimingSync()
{
	// **all prints here are left for debug purposes** //

	if (myIsPausing == false)
	{
		myCurrentTime += ofGetLastFrameTime() * double(mySpeed);

		double currentSongTime = GetRealCurrentTimeS();

		double syncAdjustment = (currentSongTime - myCurrentTime);

		if (abs(syncAdjustment) > mySyncThreshold)
		{
			myCurrentTime += syncAdjustment;
			std::cout << syncAdjustment << " - SYNCED!" << std::endl;
		}
		else
		{
			//std::cout << syncAdjustment << std::endl;
		}	
	}
}

double SongHandler::GetRealCurrentTimeS()
{
	return BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetPosition(myStreamHandle, BASS_POS_BYTE));
}
