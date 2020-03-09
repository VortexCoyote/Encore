#pragma once

#include "../BASS/bass.h"
#include "../BASS/bass_fx.h"

#include <string>

class ofSoundPlayer;
class SongHandler
{
public:

	SongHandler();
	~SongHandler();

	void Init(std::string aPath,double aSyncThreshold = 0.02);
	void Update();

	void TogglePause();
	void SetPause(bool aPause);
	void SetTimeS(double aTimeS);
	void SetTimeNormalized(float aNormalizedTime);

	void SongJumpAmount(float aSongJumpAmount);

	double GetCurrentTimeS();
	int	   GetCurrentTimeMS();
	float  GetTimeNormalized();
	double GetSongLength();

	void StopSong();

	void IncreaseSpeed();
	void DecreaseSpeed();

private:

	void TryTimingSync();
	double GetRealCurrentTimeS();

	double myCurrentTime = 0.0;
	double mySongTimeBuffer = 0.f;

	double mySyncThreshold = 0.02;

	float mySpeed = 1.0f;

	bool myIsPausing = true;

	//relevant BASS variables
	int myDevice = -1; // Default Sounddevice
	int myFreq = 44100; // Sample rate (Hz)
	HSTREAM myStreamHandle; // Handle for open stream

};