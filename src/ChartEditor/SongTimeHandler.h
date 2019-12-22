#pragma once

class ofSoundPlayer;
class SongTimeHandler
{
public:

	SongTimeHandler();
	~SongTimeHandler();

	void Init(ofSoundPlayer* aSoundPlayer, int aOffset = 0 ,double aSyncThreshold = 0.02);
	void Update();

	void TogglePause();
	void ResetToPreviousPauseState();
	void SetPause(bool aPause);
	void SetTimeS(double aTimeS);
	void SetTimeNormalized(float aNormalizedTime);

	void SongJumpAmount(float aSongJumpAmount);

	double GetCurrentTimeS();
	int	   GetCurrentTimeMS();
	float  GetTimeNormalized();

private:

	void TryTimingSync();

	double myCurrentTime = 0.0;
	double mySongTimeBuffer = 0.f;

	double mySyncThreshold = 0.02;

	float mySpeed = 1.0f;

	ofSoundPlayer* mySoundPlayer = nullptr;

	bool myIsPausing = true;

	int myOffset;
};