#pragma once

#include "TimeFieldHandlerBase.h"
#include "../../ChartData/ChartData.h"

class BPMLineHandler : public TimeFieldHandlerBase<BPMData>
{

public:
	
	BPMLineHandler();
	~BPMLineHandler();

	void ShowBeatDivisionControls();
	
	void DrawRoutine(BPMData* aTimeObject, float aTimePoint) override;
	void Draw(double aTimePoint) override;

	float GetClosestBeatLinePos(float aY);
	float GetClosestBeatLineSec(float aY);

	float GetBiasedClosestBeatLineMS(int aTime, bool aDown);

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


};