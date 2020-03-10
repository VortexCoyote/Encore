#include "BPMLineHandler.h"
#include "imgui.h"



void BPMLineHandler::ShowBeatDivisionControls()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;

	bool open = true;

	ImGui::Begin("Beat Division", &open, windowFlags);


	ImGui::Text("Beat Division");

	if (ImGui::Button("+"))
		mySnapDivision++;

	ImGui::SameLine();
	if (ImGui::Button("-"))
		mySnapDivision--;

	ImGui::SameLine();
	ImGui::Text(" 1 / ");
	
	ImGui::SameLine();
	ImGui::SliderInt("", &mySnapDivision, 1, 32);

	mySnapQuotient = 1.f / float(mySnapDivision);
	ImGui::SetWindowPos({ 8, ofGetWindowHeight() - ImGui::GetWindowHeight() - 8 });

	ImGui::End();
}

void BPMLineHandler::DrawRoutine(BPMData* aTimeObject, float aTimePoint)
{
	//bpm line
	int x = ofGetWindowWidth() / 2 - 64 * 2;
	int y = ofGetWindowHeight() - aTimePoint + 64;

	int width = 64 * 4;
	int height = 2;

	ofSetColor(255, 255, 255, 255);
	ofDrawRectangle({ x, y - height}, width, height);

	int textX = x + width + 4;
	int textY = y;

	if (y < 0)
		return void();

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;

	bool open = true;

	ImGui::SetNextWindowSize({256, 32});
	ImGui::SetNextWindowPos({ float(textX), float(textY) });

	ImGui::Begin(std::to_string((int)aTimeObject).c_str(), &open, windowFlags);

	ImGui::Text("BPM");
	ImGui::SameLine();
	ImGui::DragFloat("", &aTimeObject->BPM, 0.1, 0.0, 999999999.f);

	ImGui::End();
}

void BPMLineHandler::Draw(double aTimePoint)
{
	//TODO: abstractify these into seperate functions (beat division lookup and all of that)
	myVisibleBeatLines.clear();

	if (myVisibleObjects.size() > 0)
	{
		BPMData ghostBPMData;
		ghostBPMData.BPM	   = myVisibleObjects[myVisibleObjects.size() - 1]->BPM;
		ghostBPMData.timePoint = GetTimeFromScreenPoint(64 + ofGetWindowHeight(), aTimePoint);

		myVisibleObjects.push_back(&ghostBPMData);

		for (unsigned int bpmLineIndex = 0; bpmLineIndex < myVisibleObjects.size() - 1; bpmLineIndex++ )
		{
			int x = ofGetWindowWidth() / 2 - 64 * 2;
			int y = ofGetWindowHeight() - GetScreenTimePoint(myVisibleObjects[bpmLineIndex]->timePoint, aTimePoint) + 64;

			int width = 64 * 4;
			int height = 2;

			int lineY = y;
			int time  = myVisibleObjects[bpmLineIndex]->timePoint;

			int timeOffset = 0;

			//oh god lol
			if (ofGetWindowHeight() - GetScreenTimePoint(time, aTimePoint) > ofGetWindowHeight())
			{
				int timeJumps = int(float(GetTimeFromScreenPoint(0, aTimePoint) - time) / (60000.f / myVisibleObjects[bpmLineIndex]->BPM));
				timeOffset = (60000.f / myVisibleObjects[bpmLineIndex]->BPM) * float(timeJumps);
				time += timeOffset;
			}
			
			int lineForwardIndex = 1;
			while (time <= myVisibleObjects[bpmLineIndex + 1]->timePoint)
			{
				myVisibleBeatLines.emplace_back(time, lineY, myVisibleObjects[bpmLineIndex]->BPM);
				
				ofSetColor(255, 64, 64, 255);
				ofDrawRectangle({ x, lineY - height }, width, height);

				time = timeOffset + (myVisibleObjects[bpmLineIndex]->timePoint + (60000.f / myVisibleObjects[bpmLineIndex]->BPM * mySnapQuotient) * float(lineForwardIndex));

				lineForwardIndex++;

				lineY = ofGetWindowHeight() - GetScreenTimePoint(time, aTimePoint) + 64;

				if (lineY <= 0)
					break;
			}
		}

		myVisibleObjects.pop_back();
	}

	ofSetColor(255, 255, 255, 255);
	TimeFieldHandlerBase<BPMData>::Draw(aTimePoint);
}

float BPMLineHandler::GetClosestBeatLinePos(float aY)
{
	for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
	{
		if (aY <= myVisibleBeatLines[beatIndex].visualTimePoint)
		{
			return myVisibleBeatLines[beatIndex].visualTimePoint;
		}
	}

	return 0.0f;
}

float BPMLineHandler::GetClosestBeatLineSec(float aY)
{
	for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
	{
		if (aY <= myVisibleBeatLines[beatIndex].visualTimePoint)
		{
			return float(myVisibleBeatLines[beatIndex].timePoint) / 1000.f;
		}
	}

	return 0.0f;
}

float BPMLineHandler::GetBiasedClosestBeatLineMS(int aTime, bool aDown)
{
	//:(

	BeatLine beatLine = { 0, 0, 0 };

	for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
	{
		if (aTime <= myVisibleBeatLines[beatIndex].timePoint)
		{
			beatLine = myVisibleBeatLines[beatIndex];
			break;
		}
	}

	int time = aTime;

	double timeOffset = ((60000 / beatLine.BPM * mySnapQuotient));

	if (aDown == true)
	{
		for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
		{
			if (time > myVisibleBeatLines[beatIndex].timePoint)
			{
				time = myVisibleBeatLines[beatIndex].timePoint + 3;
				break;
			}
		}

		time -= (timeOffset);

	}
	else
	{
		time += timeOffset;

		for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
		{
			if (time > myVisibleBeatLines[beatIndex].timePoint)
			{
				time = myVisibleBeatLines[beatIndex].timePoint + 3;
				break;
			}
		}
	}

	return time;
}

int BPMLineHandler::GetClosestTimePoint(float aY)
{
	for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
	{
		if (aY <= myVisibleBeatLines[beatIndex].visualTimePoint)
		{
			return myVisibleBeatLines[beatIndex].timePoint;
		}
	}

	return -1;
}

BPMLineHandler::BPMLineHandler()
{
	mySnapQuotient = 1.f / float(mySnapDivision);
	myVisibleBeatLines.reserve(100000);
}

BPMLineHandler::~BPMLineHandler()
{

}