#include "BPMLineHandler.h"
#include "imgui.h"

#include "../NotificationSystem.h"

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

void BPMLineHandler::IncreaseBeatSnap()
{
	mySnapDivision++;
	if (mySnapDivision > 192)
	{
		mySnapDivision = 192;
		PUSH_NOTIFICATION("Snap Division Limit Reached (1/192)");
	}
	else
	{
		PUSH_NOTIFICATION("Snap Division: 1/" + std::to_string(mySnapDivision));
	}

	mySnapQuotient = 1.f / float(mySnapDivision);

}

void BPMLineHandler::DecreaseBeatSnap()
{
	mySnapDivision--;
	if (mySnapDivision < 1)
	{
		mySnapDivision = 1;
		PUSH_NOTIFICATION("Snap Division Limit Reached (1/1)");
	}
	else
	{
		PUSH_NOTIFICATION("Snap Division: 1/" + std::to_string(mySnapDivision));
	}

	mySnapQuotient = 1.f / float(mySnapDivision);

}

void BPMLineHandler::Update()
{
	if (myPinnedBPMLine != nullptr)
	{
		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoTitleBar;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
		windowFlags |= ImGuiWindowFlags_NoScrollbar;

		bool open = true;

		ImGui::SetNextWindowSize(myBPMControllerDimensions);
		ImGui::SetNextWindowPos(myPinnedControllerPosition);

		ImGui::Begin(std::to_string((int)myPinnedBPMLine).c_str(), &open, windowFlags);

		if (ImGui::Button("+ 1ms"))
			myPinnedBPMLine->timePoint += 1;

		ImGui::SameLine();
		ImGui::Checkbox("Pin BPM Line     ", &myPinnedBPMLine->pinControl);

		ImGui::SameLine();

		if (ImGui::Button("X"))
			myBPMLineToDelete = myPinnedBPMLine;

		if (ImGui::Button("- 1ms"))
			myPinnedBPMLine->timePoint -= 1;

		ImGui::SameLine();

		ImGui::Text("BPM");
		ImGui::SameLine();
		ImGui::PushItemWidth(174);
		ImGui::DragFloat("", &myPinnedBPMLine->BPM, myBPMChangeValue, 0.0, 2000.f);
		ImGui::SameLine();

		ImGui::End();

		if (myPinnedBPMLine->pinControl == false)
			myPinnedBPMLine = nullptr;
	}

	if (myBPMLineToDelete != nullptr)
	{
		if (myBPMLineToDelete == myPinnedBPMLine)
		{
			myPinnedBPMLine->pinControl = false;
			myPinnedBPMLine == nullptr;
		}

		myObjectData->erase(std::remove(myObjectData->begin(), myObjectData->end(), myBPMLineToDelete), myObjectData->end());
		myVisibleObjects.erase(std::remove(myVisibleObjects.begin(), myVisibleObjects.end(), myBPMLineToDelete), myVisibleObjects.end());

		myBPMLineToDelete = nullptr;
	}
}


void BPMLineHandler::DrawRoutine(BPMData* aTimeObject, float aTimePoint)
{
	//bpm line
	int x = ofGetWindowWidth() / 2 - 64 * 2;
	int y = ofGetWindowHeight() - aTimePoint + 64;

	int width = 64 * 4;
	int height = 2;

	ofSetColor(255, 255, 255, 255);
	ofDrawRectangle({ x - 32, y - height}, width + 64, height);

	int textX = x + width + 48;
	int textY = y;

	if (y < 0)
		return void();

	if (aTimeObject->pinControl == false)
	{
		//ofSetColor(255, 255, 255, 255);
		//ofDrawCircle({ float(textX), float(textY) }, 4);

		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoTitleBar;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
		windowFlags |= ImGuiWindowFlags_NoScrollbar;

		bool open = true;

		ImGui::SetNextWindowSize(myBPMControllerDimensions);
		ImGui::SetNextWindowPos({ float(textX), float(textY) - 16 });

		ImGui::Begin(std::to_string((int)aTimeObject).c_str(), &open, windowFlags);
		
		if (ImGui::Button("+ 1ms"))
			aTimeObject->timePoint += 1;

		ImGui::SameLine();
		ImGui::Checkbox("Pin BPM Line     ", &aTimeObject->pinControl);

		ImGui::SameLine();

		if (ImGui::Button("X"))
			myBPMLineToDelete = aTimeObject;

		if (ImGui::Button("- 1ms"))
			aTimeObject->timePoint -= 1;

		ImGui::SameLine();

		ImGui::Text("BPM");
		ImGui::SameLine();
		ImGui::PushItemWidth(174);
		ImGui::DragFloat("", &aTimeObject->BPM, myBPMChangeValue, 0.0, 2000.f);
		ImGui::SameLine();

		ImGui::End();

		if (aTimeObject->pinControl == true)
		{
			if (myPinnedBPMLine != nullptr)
				myPinnedBPMLine->pinControl = false;
			
			myPinnedBPMLine = aTimeObject;
			myPinnedControllerPosition = { float(textX), float(textY) - 16 };
		}
	}
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
				ofDrawRectangle({ x - 32, lineY - height }, width + 64, height);

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
	// :(

	BeatLine beatLine = { 0, 0, 0 };

	for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
	{
		if (aTime + 4 > myVisibleBeatLines[beatIndex].timePoint)
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
			if (time + 4 > myVisibleBeatLines[beatIndex].timePoint)
			{
				time = myVisibleBeatLines[beatIndex].timePoint;
				time -= timeOffset;
				return time;
			}
		}

		return aTime - 25;
	}
	else
	{
		time += timeOffset;

		for (int beatIndex = myVisibleBeatLines.size() - 1; beatIndex >= 0; beatIndex--)
		{
			if (time + 4 > myVisibleBeatLines[beatIndex].timePoint)
			{
				time = myVisibleBeatLines[beatIndex].timePoint;
				return time;
			}
		}

		return aTime + 25;
	}
}

void BPMLineHandler::PlaceBPMLine(int aTimePoint)
{
	BPMData* bpmData = new BPMData();

	bpmData->meter = 4;
	bpmData->timePoint = aTimePoint;
	bpmData->BPM = 120.f;
	bpmData->uninherited = 1;
	bpmData->pinControl = false;

	myObjectData->push_back(bpmData);
	
	std::sort(myObjectData->begin(), myObjectData->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});


	std::string message = "Placed BPM Point: ";
	message += std::to_string(bpmData->timePoint);
	message += "ms";

	PUSH_NOTIFICATION(message);
}

void BPMLineHandler::DeleteBPMLine(int aX, int aY)
{
	int clickTimePoint = GetTimeFromScreenPoint(aY, myLastTimePoint);

	BPMData* BPMLineToDelete = nullptr;

	for (auto& BPMLine : myVisibleObjects)
	{
		int y = ofGetWindowHeight() - GetScreenTimePoint(BPMLine->timePoint, myLastTimePoint) + 64;
		
	}
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

void BPMLineHandler::SetPreciseBPMChange(bool aMode)
{
	if (aMode == true)
	{
		myBPMChangeValue = 0.0001f;
	}
	else
	{
		myBPMChangeValue = 0.01f;
	}
}

BPMLineHandler::BPMLineHandler()
{
	mySnapQuotient = 1.f / float(mySnapDivision);
	myVisibleBeatLines.reserve(100000);
}

BPMLineHandler::~BPMLineHandler()
{

}