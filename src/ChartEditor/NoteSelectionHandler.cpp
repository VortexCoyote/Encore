#include "NoteSelectionHandler.h"

#include "../ChartData/ChartData.h"

#include "TimeFieldHandlers/BPMLineHandler.h"
#include "ofMain.h"

NoteSelectionHandler::NoteSelectionHandler()
{
	myCursorImage.loadImage("images/selected.png");
}

NoteSelectionHandler::~NoteSelectionHandler()
{

}

void NoteSelectionHandler::Init(BPMLineHandler* aBPMLineHandler)
{
	myBPMLineHandler = aBPMLineHandler;
}

void NoteSelectionHandler::Draw()
{
	for (auto& note : mySelectedItems)	
		if (note->selected == true && note->y < ofGetWindowHeight())
			myCursorImage.draw(note->x, note->y);	

	myCursorImage.draw(GetSnappedCursorPosition());
}

ofVec2f NoteSelectionHandler::GetSnappedCursorPosition()
{

	float inputX = myCursorPosition.x;
	float x = inputX;
	float leftBorder = ofGetWindowWidth() / 2 - 64 * 2.f;
	float rightBorder = ofGetWindowWidth() / 2 + 64 * 2.f;

	switch (myCursorState)
	{
	case NoteCursorState::Place:

		x = floor(ofClamp(inputX, leftBorder, rightBorder - 64.f) / 64.f) * 64.f;
		return ofVec2f(x, myBPMLineHandler->GetClosestBeatLinePos(myCursorPosition.y) - myCursorImage.getHeight());

		break;

	case NoteCursorState::Select:
	
		break;
		
	case NoteCursorState::Count:
	
		break;
	
	default:
	
		break;
	}

	return { 0, 0 };
}

void NoteSelectionHandler::SetCursorInput(ofVec2f aPosition)
{
	myCursorPosition = aPosition;
}

void NoteSelectionHandler::SetVisibleItems(std::vector<NoteData*>* aVisibleItems)
{
	myVisibleItems = aVisibleItems;
}

void NoteSelectionHandler::TrySelectItem(int aX, int aY)
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

void NoteSelectionHandler::ClearSelectedItems()
{
	mySelectedItems.clear();
}
