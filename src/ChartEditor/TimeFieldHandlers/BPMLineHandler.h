#pragma once

#include "TimeFieldHandlerBase.h"
#include "../../ChartData/ChartData.h"

#include "imgui.h"

class BPMLineHandler : public TimeFieldHandlerBase<BPMData>
{

public:
	
	BPMLineHandler();
	~BPMLineHandler();

	void ShowBeatDivisionControls();
	
	void IncreaseBeatSnap();
	void DecreaseBeatSnap();

	void Update();

	void DrawRoutine(BPMData* aTimeObject, float aTimePoint) override;
	void Draw(double aTimePoint) override;

	float GetClosestBeatLinePos(float aY);
	float GetClosestBeatLineSec(float aY);

	float GetBiasedClosestBeatLineMS(int aTime, bool aDown);

	void PlaceBPMLine(int aTimePoint);
	void DeleteBPMLine(int aX, int aY);

	int GetClosestTimePoint(float aY);

	void SetPreciseBPMChange(bool aMode);

	void ClearAllCurrentBeatLines();

private:

	int	  mySnapDivision = 2;
	float mySnapQuotient = 0;	

	struct BeatLine
	{
		BeatLine(int aTimePoint, float aVisualTimePoint, float aBPM)
		{
			timePoint = aTimePoint;
			visualTimePoint = aVisualTimePoint;
			BPM = aBPM;
		};

		int   timePoint;
		float visualTimePoint;
		float BPM;
	};

	std::vector<BeatLine> myVisibleBeatLines;

	ImVec2 myBPMControllerDimensions = { 220.f, 52.f};
	BPMData* myPinnedBPMLine = nullptr;
	ImVec2 myPinnedControllerPosition;

	BPMData* myBPMLineToDelete = nullptr;


	float myBPMChangeValue = 0.01f;
};