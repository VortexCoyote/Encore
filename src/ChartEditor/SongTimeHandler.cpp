#include "SongTimeHandler.h"

#include "ofSoundPlayer.h"
#include "ofSoundUtils.h"
#include "ofMain.h"

#include <Windows.h>


SongTimeHandler::SongTimeHandler()
{

}

SongTimeHandler::~SongTimeHandler()
{

}

void SongTimeHandler::Init(ofSoundPlayer* aSoundPlayer, int aOffset, double aSyncThreshold)
{
	mySoundPlayer   = aSoundPlayer;
	mySyncThreshold = aSyncThreshold;

	myIsPausing = true;

	mySoundPlayer->stop();
	mySoundPlayer->play();
	mySoundPlayer->setPaused(myIsPausing);

	myOffset = aOffset;
}

void SongTimeHandler::Update()
{
	if (mySoundPlayer == nullptr)
		return void();

	if (mySoundPlayer->getPosition() >= 0.999)
	{
		mySoundPlayer->play();
		
		myIsPausing = true;
		mySoundPlayer->setPaused(myIsPausing);
		mySoundPlayer->setPosition(0.999);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		myCurrentTime -= 0.01;
		mySoundPlayer->setPositionMS(myCurrentTime * 1000);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		myCurrentTime += 0.01;
		mySoundPlayer->setPositionMS(myCurrentTime * 1000);
	}

	mySpeed += 0.001f * (bool(GetAsyncKeyState(VK_RIGHT)) - bool(GetAsyncKeyState(VK_LEFT)));

	mySoundPlayer->setSpeed(mySpeed);

	ofSoundUpdate();

	TryTimingSync();
}

void SongTimeHandler::TogglePause()
{
	myIsPausing = !myIsPausing;
	mySoundPlayer->setPaused(myIsPausing);
}

void SongTimeHandler::ResetToPreviousPauseState()
{
	mySoundPlayer->setPaused(myIsPausing);
}

void SongTimeHandler::SetPause(bool aPause)
{
	mySoundPlayer->setPaused(aPause);
}

void SongTimeHandler::SetTimeS(double aTimeS)
{
	myCurrentTime = aTimeS + double(myOffset) / 1000.0;
	mySoundPlayer->setPositionMS(myCurrentTime * 1000);
}

void SongTimeHandler::SetTimeNormalized(float aNormalizedTime)
{
	mySoundPlayer->setPosition(aNormalizedTime);
	myCurrentTime = double(mySoundPlayer->getPositionMS()) / 1000.0;
}

void SongTimeHandler::SongJumpAmount(float aSongJumpAmount)
{
	myCurrentTime += aSongJumpAmount;
	mySoundPlayer->setPositionMS(myCurrentTime * 1000);
}

double SongTimeHandler::GetCurrentTimeS()
{
	return myCurrentTime + double(myOffset) / 1000.0;
}

int SongTimeHandler::GetCurrentTimeMS()
{
	return int(myCurrentTime * 1000) + myOffset;
}

float SongTimeHandler::GetTimeNormalized()
{
	return mySoundPlayer->getPosition();
}

void SongTimeHandler::TryTimingSync()
{
	// **all prints here are left for debug purposes** //

	if (myIsPausing == false)
	{
		myCurrentTime += ofGetLastFrameTime() * double(mySpeed);

		double currentSongTime = double(mySoundPlayer->getPositionMS()) / 1000.f;

		//std::cout << myCurrentTime << " : " << currentSongTime << std::endl;

		if (mySongTimeBuffer != currentSongTime)
		{
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

			mySongTimeBuffer = currentSongTime;
		}
	}
}
