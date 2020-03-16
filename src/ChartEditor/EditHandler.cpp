#include "EditHandler.h"

#include "../ChartData/ChartData.h"

#include "TimeFieldHandlers/BPMLineHandler.h"
#include "ofMain.h"

#include "NotificationSystem.h"

EditHandler::EditHandler()
{
	myCursorImage.loadImage("images/selected.png");
}

EditHandler::~EditHandler()
{

}

void EditHandler::Init(BPMLineHandler* aBPMLineHandler)
{
	myBPMLineHandler = aBPMLineHandler;
}

void EditHandler::Update()
{
	ShowModeSelect();
}

void EditHandler::Draw()
{
	for (auto& note : mySelectedItems)	
		if (note->selected == true && note->y < ofGetWindowHeight())
			myCursorImage.draw(note->x, note->y);	

	if (myCursorState == EditActionState::EditBPM)
	{
		ofSetColor(255, 255, 255, 255);
		ofDrawRectangle(ofGetWindowWidth() / 2 - 64 * 2 - 32, GetSnappedCursorPosition().y, 64 * 4 + 32 * 2, 4 );

		return void();
	}

	myCursorImage.draw(GetSnappedCursorPosition() - ofVec2f(0, 64));
}

ofVec2f EditHandler::GetSnappedCursorPosition()
{
	float inputX = myCursorPosition.x;
	float x = inputX;
	float leftBorder = ofGetWindowWidth() / 2 - 64 * 2;
	float rightBorder = ofGetWindowWidth() / 2 + 64 * 2;

	//std::cout << leftBorder << std::endl;

	switch (myCursorState)
	{
	case EditActionState::EditBPM:
	case EditActionState::EditHolds:
	case EditActionState::EditNotes:

		x = ofClamp(inputX, leftBorder, rightBorder - 64.f);
		
		if (x >= leftBorder && x < leftBorder + 64.f)
			x = leftBorder;

		if (x >= leftBorder + 64.f && x < leftBorder + 128.f)
			x = leftBorder + 64.f;

		if (x >= leftBorder + 128.f && x < leftBorder + 192.f)
			x = leftBorder + 128.f;

		if (x >= leftBorder + 192.f && x < leftBorder + 256.f)
			x = leftBorder + 192.f;

		if (myFreePlace == true)
		{
			return ofVec2f(x, myCursorPosition.y);
		}
		else
		{
			return ofVec2f(x, myBPMLineHandler->GetClosestBeatLinePos(myCursorPosition.y));
		}

		break;


	case EditActionState::Select:
	
		break;
		
	case EditActionState::Count:
	
		break;
	
	default:
	
		break;
	}

	return { 0, 0 };
}

void EditHandler::SetCursorInput(ofVec2f aPosition)
{
	myCursorPosition = aPosition;
}

void EditHandler::SetVisibleItems(std::vector<NoteData*>* aVisibleItems)
{
	myVisibleItems = aVisibleItems;
}

void EditHandler::SetEditActionState(EditActionState aCursorState)
{
	switch (aCursorState)
	{
	case EditActionState::EditNotes:
		PUSH_NOTIFICATION("Edit Note Mode");
		break;
	case EditActionState::EditHolds:
		PUSH_NOTIFICATION("Edit Hold Mode");
		break;
	case EditActionState::Select:
		PUSH_NOTIFICATION("Select Mode");
		break;
	case EditActionState::EditBPM:
		PUSH_NOTIFICATION("Edit BPM Points Mode");
		break;

	default:
		PUSH_NOTIFICATION("Invalid Mode Input");

		return void();
		break;
	}

	myCursorState = aCursorState;
}

void EditHandler::TrySelectItem(int aX, int aY)
{
	if (myVisibleItems == nullptr)
		return void();

	for (auto& item : *myVisibleItems)
	{
		if (aX >= item->x && aX <= item->x + 64 &&
			aY >= item->y && aY <= item->y + 64)
		{
			item->selected = !item->selected;
			mySelectedItems.push_back(item);
			return void();
		}
	}

	for (auto& item : mySelectedItems)
		item->selected = false;

	mySelectedItems.clear();
}

void EditHandler::SetFreePlace(bool aFreePlace)
{
	myFreePlace = aFreePlace;
}

EditActionState EditHandler::GetEditActionState()
{
	return myCursorState;
}

void EditHandler::ClearSelectedItems()
{
	mySelectedItems.clear();
}

int EditHandler::GetColumn(int aX)
{
	float inputX = aX;
	float x = inputX;
	float leftBorder = ofGetWindowWidth() / 2 - 64 * 2;
	float rightBorder = ofGetWindowWidth() / 2 + 64 * 2;

	if (x >= 0 && x < leftBorder + 64.f)
		return 0;

	if (x >= leftBorder + 64.f && x < leftBorder + 128.f)
		return 1;

	if (x >= leftBorder + 128.f && x < leftBorder + 192.f)
		return 2;

	if (x >= leftBorder + 192.f && x < ofGetWindowWidth())
		return 3;


	assert(false && "bad input coordinate");
}

void EditHandler::ShowModeSelect()
{

}
