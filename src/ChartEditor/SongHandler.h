#pragma once

#include "../BASS/bass.h"
#include "../BASS/bass_fx.h"

#include <string>
#include <vector>

#include "ofImage.h"
#include "ofFbo.h"

struct WaveFormSlice
{
	ofFbo* surface = nullptr;
	int timeMS = -1;
};

class ofSoundPlayer;
class SongHandler
{
public:

	SongHandler();
	~SongHandler();

	void Init(std::string aPath, double aSyncThreshold = 0.02);
	void Update();
	void DrawWaveForm();

	void TogglePause();
	void SetPause(bool aPause);
	void SetTimeS(double aTimeS);
	void SetTimeNormalized(float aNormalizedTime);

	void ResetSpeed();

	void SongJumpAmount(double aSongJumpAmount);

	double GetCurrentTimeS();
	int	   GetCurrentTimeMS();
	double GetTimeNormalized();
	double GetSongLength();

	void StopSong();

	void IncreaseSpeed();
	void DecreaseSpeed();

	void ShowPlaybackRateControls();

private:


	void DrawWaveFormSliceAtIndex(int aIndex);
	void GenerateWaveForm(std::string aPath);

	void TryTimingSync();
	double GetRealCurrentTimeS();

	double myCurrentTime = 0.0;
	double mySongTimeBuffer = 0.f;

	double mySyncThreshold = 0.02;

	float mySpeed = 1.0f;

	bool myIsPausing = true;

	float* myWaveFormData = nullptr;
	std::vector<WaveFormSlice> myWaveFormStructure;
	
	int myWaveFormSliceSize = 768;

	DWORD mySongByteLength;

	//relevant BASS variables
	int myDevice = -1; // Default Sounddevice
	int myFreq = 44100; // Sample rate (Hz)
	//HSTREAM myStreamHandle; // Handle for open stream
	HSAMPLE myStreamHandle; // Handle for open stream
};