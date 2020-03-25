#include "EditHandler.h"

#include "../ChartData/ChartData.h"


#include "TimeFieldHandlers/BPMLineHandler.h"
#include "TimeFieldHandlers/NoteHandler.h"

#include "ofMain.h"

#include "NotificationSystem.h"
#include "UndoRedoHandler.h"

EditHandler::EditHandler()
{

}

EditHandler::~EditHandler()
{

}

void EditHandler::Init(BPMLineHandler* aBPMLineHandler, NoteHandler* aNoteHandler, std::vector<NoteData*>* aNoteData)
{
	myNoteData = aNoteData;
	myBPMLineHandler = aBPMLineHandler;
	myNoteHandler = aNoteHandler;
}

void EditHandler::Update(double aCurrentTime)
{
	mySavedTimeS = aCurrentTime;

	if (myDragBox == true)
		TryDragBoxSelect(aCurrentTime);

	if (myMoveAllSelectedNotes == true)
		TryMoveSelectedNotes(aCurrentTime);
}

void EditHandler::Draw()
{
	if (myCursorState == EditActionState::EditBPM)
	{
		ofSetColor(255, 255, 255, 255);
		ofDrawRectangle(ofGetWindowWidth() / 2 - 64 * 2 - 32, GetSnappedCursorPosition().y, 64 * 4 + 32 * 2, 4 );

		return void();
	}

	EditorConfig::Skin::selectImage.draw(GetSnappedCursorPosition() - ofVec2f(0, 64));

	if (myDraggableItem != nullptr)
	{
		ofSetColor(32, 255, 32, 255);

		switch (myDraggableItem->noteType)
		{
		case NoteType::HoldBegin:
		case NoteType::HoldEnd:
		{
			ofVec2f pos = { float(myDraggableItem->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(myDraggableItem->timePoint, mySavedTimeS) + 0.5)) };
			EditorConfig::Skin::selectImage.draw(pos);
		}
			break;

		case NoteType::Note:
			EditorConfig::Skin::selectImage.draw(ofVec2f(myDraggableItem->x, myDraggableItem->y));
			break;

		default:
			break;
		}

		ofSetColor(255, 255, 255, 255);
	}

	if (myDragBox == true)
	{
		ofSetColor(255, 255, 255, 96);
		ofDrawRectangle(myAnchoredDragPoint, myUpdatedDragPoint.x - myAnchoredDragPoint.x, myUpdatedDragPoint.y - myAnchoredDragPoint.y);
		ofSetColor(255, 255, 255, 255);
	}

	if (myPastePreview == true)
	{
		DrawPastePreview();
	}
}

ofVec2f EditHandler::GetSnappedCursorPosition()
{
	float inputX = myCursorPosition.x;
	float x = inputX;
	float leftBorder = ofGetWindowWidth() / 2 - EditorConfig::fieldWidth / 2;
	float rightBorder = ofGetWindowWidth() / 2 + EditorConfig::fieldWidth / 2;

	int noteWidth = EditorConfig::Skin::noteImages[0].getWidth();

	//std::cout << leftBorder << std::endl;

	switch (myCursorState)
	{
	case EditActionState::EditBPM:
	case EditActionState::EditHolds:
	case EditActionState::EditNotes:
	{
		x = ofClamp(inputX, leftBorder, rightBorder - noteWidth);

		for (size_t column = 0; column < EditorConfig::keyAmount; column++)
		{
			if (x >= leftBorder + noteWidth * column && x < leftBorder + noteWidth + noteWidth * column)
				x = leftBorder + noteWidth * column;
		}

		if (myFreePlace == true)
		{
			return ofVec2f(x, myCursorPosition.y);
		}
		else
		{
			return ofVec2f(x, myBPMLineHandler->GetClosestBeatLinePos(myCursorPosition.y));
		}
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

void EditHandler::SelectAll()
{
	for (auto item : *myNoteData)
	{
		item->selected = true;
		mySelectedItems[item] = item;
	}

	PUSH_NOTIFICATION("All Objects Selected");
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
			mySelectedItems[item] = item;
			return void();
		}
	}

	for (auto& item : mySelectedItems)
		item.first->selected = false;

	mySelectedItems.clear();
}

void EditHandler::SetFreePlace(bool aFreePlace)
{
	myFreePlace = aFreePlace;
}

void EditHandler::TryDeleteSelectedItems()
{
	for (auto note : mySelectedItems)
	{
		if (note.first->selected == false)
			continue;

		myNoteHandler->DeleteNoteByPointer(note.first);
	}

	myNoteHandler->ScheduleRewind();
	mySelectedItems.clear();
}

void EditHandler::TryFlipSelectedItemsHorizonally()
{
	for (auto note : mySelectedItems)
	{
		if (note.first->selected == false)
			continue;

		note.first->column = 3 - note.first->column;
	}
}

void EditHandler::ClickAction(int aX, int aY)
{	
	if (ImGui::GetIO().WantCaptureMouse == true)
		return void();

	if (myPastePreview == true)
		PlacePaste();

	NoteData* potentialDragNote = myNoteHandler->GetHoveredNote(aX, aY);
	if (potentialDragNote != nullptr)
	{
		myMoveAllSelectedNotes = true;
		myDraggableItem = potentialDragNote;

		if (myDraggableItem->selected == true)
			myWasItemSelected = true;
		else
		{
			myDraggableItem->selected = true;
			myWasItemSelected = false;
		}
		
		mySelectedItems[myDraggableItem] = myDraggableItem;
		return void();
	}

	for (auto note : mySelectedItems)
		note.first->selected = false;

	mySelectedItems.clear();

	myDragBox = true;
	myAnchoredDragPoint = { float(aX),float(aY) };
	myUpdatedDragPoint = { float(aX), float(aY) };

	myAnchoredTimePoint = TimeFieldHandlerBase<NoteData>::GetTimeFromScreenPoint(ofGetScreenHeight() - myAnchoredDragPoint.y, mySavedTimeS);
}

void EditHandler::DragAction(int aX, int aY)
{
	myUpdatedDragPoint = { float(aX), float(aY) };
}

void EditHandler::ReleaseAction(int aX, int aY)
{
	myDragBox = false;
	myMoveAllSelectedNotes = false;

	if (myDraggableItem != nullptr)
	{
		if (myDraggableItem->selected == true && myWasItemSelected == false)
			myDraggableItem->selected = false;

		myDraggableItem = nullptr;
	}
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
	float leftBorder = ofGetWindowWidth() / 2 - EditorConfig::fieldWidth / 2;
	float rightBorder = ofGetWindowWidth() / 2 + EditorConfig::fieldWidth / 2;

	int noteWidth = EditorConfig::Skin::noteImages[0].getWidth();

	for (size_t column = 0; column < EditorConfig::keyAmount; column++)
	{
		if (x >= leftBorder + noteWidth * column && x < leftBorder + noteWidth + noteWidth * column)
			return column;
	}

	if (aX <= leftBorder)
		return 0;

	if (aX >= leftBorder)
		return EditorConfig::keyAmount - 1;

	assert(false && "bad input coordinate");

	return -1;
}

void EditHandler::Copy()
{
	myClipBoard.clear();

	for (auto item : mySelectedItems)
	{
		if (item.first->selected == false)
			continue;

		myClipBoard.push_back(item.first);
	}
}

void EditHandler::Paste()
{
	if(myClipBoard.size() > 0)
		myPastePreview = true;
}

void EditHandler::PlacePaste()
{
	myPastePreview = false;

	int cursorTimePoint = myBPMLineHandler->GetClosestTimePoint(myCursorPosition.y);

	int minTimePoint = INT_MAX;

	for (auto item : myClipBoard)
	{
		int deltaTimePoint = cursorTimePoint - item->timePoint;

		minTimePoint = std::min(minTimePoint, item->timePoint);
	}

	for (auto item : myClipBoard)
	{
		int deltaTimePoint = item->timePoint - minTimePoint;
		
		switch (item->noteType)
		{
		case NoteType::HoldBegin:
		{
			NoteData* holdEnd;
			myNoteHandler->PlaceHold(item->column, cursorTimePoint + deltaTimePoint, holdEnd);

			holdEnd->timePoint = cursorTimePoint + (item->relevantNote->timePoint - minTimePoint);

			UndoRedoHandler::GetInstance()->PushHistory(ActionType::Place, { holdEnd->relevantNote, holdEnd });
		}
			break;

		case NoteType::Note:
			myNoteHandler->PlaceNote(item->column, cursorTimePoint + deltaTimePoint);
			break;

		default:
			break;
		}
	}

	myNoteHandler->ScheduleRewind();
}

void EditHandler::TryDragBoxSelect(double aCurrentTime)
{
	myAnchoredDragPoint.y = ofGetScreenHeight() - TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(myAnchoredTimePoint, aCurrentTime);

	ofRectangle dragBoxRect;

	dragBoxRect = { myAnchoredDragPoint, myUpdatedDragPoint.x - myAnchoredDragPoint.x, myUpdatedDragPoint.y - myAnchoredDragPoint.y };

	for (auto visibleNote : *myVisibleItems)
	{
		ofRectangle note;

		switch (visibleNote->noteType)
		{
		case NoteType::HoldBegin:
		case NoteType::HoldEnd:
		{
			note = { float(visibleNote->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(visibleNote->timePoint, aCurrentTime) + 0.5)), 64.f, 64.f };

			ofRectangle note2 = { float(visibleNote->relevantNote->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(visibleNote->relevantNote->timePoint, aCurrentTime) + 0.5)), 64.f, 64.f };

			if (dragBoxRect.intersects(note) == true || dragBoxRect.intersects(note2) == true)
			{
				if (visibleNote->selected == true)
					continue;

				mySelectedItems[visibleNote];
				mySelectedItems[visibleNote->relevantNote];
				visibleNote->selected = true;
				visibleNote->relevantNote->selected = true;
			}
			else
			{
				visibleNote->selected = false;
				visibleNote->relevantNote->selected = false;
			}
		}
		break;

		case NoteType::Note:
		{
			note = { float(visibleNote->x), float(visibleNote->y), 64.f, 64.f };

			if (dragBoxRect.intersects(note) == true)
			{
				if (visibleNote->selected == false)
				{
					if (visibleNote->selected == true)
						continue;

					mySelectedItems[visibleNote];
					visibleNote->selected = true;
				}
			}
			else
			{
				visibleNote->selected = false;
			}
		}
		break;

		default:
			break;
		}
	}
}

void EditHandler::TryMoveSelectedNotes(double aCurrentTime)
{
	if (myDraggableItem == nullptr)
		return void();

	int savedTimePoint = myDraggableItem->timePoint;
	int deltaTimePoint = myBPMLineHandler->GetClosestTimePoint(myCursorPosition.y) - savedTimePoint;

	int minColumn = 4;
	int maxColumn = -1;

	int savedColumn = myDraggableItem->column;
	int deltaColumn = GetColumn(myCursorPosition.x) - savedColumn;

	for (auto item : mySelectedItems)
	{
		if (item.first->selected == false)
			continue;

		item.first->timePoint += deltaTimePoint;

		minColumn = std::min(minColumn, item.first->column);
		maxColumn = std::max(maxColumn, item.first->column);

		myNoteHandler->AddToVisibleObjects(item.first);
		if (item.first->noteType == NoteType::HoldBegin || item.first->noteType == NoteType::HoldEnd)
		{
			(*myNoteHandler->GetVisibleHolds())[item.first] = item.first;
		}
	}

	if (minColumn + deltaColumn >= 0 && maxColumn + deltaColumn <= 3)
	{
		for (auto item : mySelectedItems)
		{
			if (item.first->selected == false)
				continue;

			item.first->column += deltaColumn;
		}
	}

	myNoteHandler->SortAllNotes();
	myNoteHandler->ScheduleRewind();
}

void EditHandler::DrawPastePreview()
{
	//could be optimized by calculating the minTimePoint once

	int cursorTimePoint = myBPMLineHandler->GetClosestTimePoint(myCursorPosition.y);

	int minTimePoint = INT_MAX;

	for (auto item : myClipBoard)
	{
		int deltaTimePoint = cursorTimePoint - item->timePoint;

		minTimePoint = std::min(minTimePoint, item->timePoint);
	}

	ofSetColor(128, 255, 128, 128);

	for (auto item : myClipBoard)
	{
		int deltaTimePoint = item->timePoint - minTimePoint;

		switch (item->noteType)
		{
			case NoteType::HoldBegin:
			{
				int deltaEndTimePoint = item->relevantNote->timePoint - minTimePoint;
				myNoteHandler->DrawHold(item->column, cursorTimePoint + deltaTimePoint, 
													  cursorTimePoint + deltaEndTimePoint);
			}
				break;

			case NoteType::Note:
				myNoteHandler->DrawNote(item->column, cursorTimePoint + deltaTimePoint);
				break;

			default:
				break;
			}
	}

	ofSetColor(255, 255, 255, 255);
}
