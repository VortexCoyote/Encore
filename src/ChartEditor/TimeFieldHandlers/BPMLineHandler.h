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

private:

	int	  mySnapDivision = 2;
	float mySnapQuotient = 0;	

	struct BeatLine
	{
		BeatLine(int aTimePoint, float aVisualTimePoint)
		{
			timePoint = aTimePoint;
			visualTimePoint = aVisualTimePoint;
		};

		int   timePoint;
		float visualTimePoint;
	};

	std::vector<BeatLine> myVisibleBeatLines;
};