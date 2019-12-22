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

private:

	int	  mySnapDivision = 2;
	float mySnapQuotient = 0;
};