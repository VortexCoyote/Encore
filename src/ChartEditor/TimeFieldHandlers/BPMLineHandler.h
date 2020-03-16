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

	void UpdatePinnedController();

	void DrawRoutine(BPMData* aTimeObject, float aTimePoint) override;
	void Draw(double aTimePoint) override;

	float GetClosestBeatLinePos(float aY);
	float GetClosestBeatLineSec(float aY);

	float GetBiasedClosestBeatLineMS(int aTime, bool aDown);

	void PlaceBPMLine(int aTimePoint);

	int GetClosestTimePoint(float aY);

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

	BPMData* myPinnedController = nullptr;
	ImVec2 myPinnedControllerPosition;
	ImVec2 myPinnedControllerDimensions;
};