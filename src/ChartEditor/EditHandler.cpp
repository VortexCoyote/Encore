#include "EditHandler.h"

#include "../ChartData/ChartData.h"


#include "TimeFieldHandlers/BPMLineHandler.h"
#include "TimeFieldHandlers/NoteHandler.h"

#include "ofMain.h"

#include "Utilities/NotificationSystem.h"
#include "Utilities/UndoRedoHandler.h"
#include "Utilities/TileableGUI.h"

EditHandler::EditHandler()
{
	TileableGUI::GetInstance()->RegisterTile([this]() {
		
		std::string modes[4] = {"Select Mode" , "Edit Note Mode", "Edit Hold Mode" , "Edit BPM Points Mode"};

		for (int n = 0; n < 4; n++)
		{
			if (ImGui::Selectable(modes[n].c_str(), static_cast<int>(myCursorState) == n))
				SetEditActionState(static_cast<EditActionState>(n));
		}

	}, "Edit Mode", true);
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

	if (myMoveAllSelectedNotes == true || myPastePreview == true)
		TryMoveSelectedNotes(aCurrentTime);
}

void EditHandler::Draw()
{
	if (myCursorState == EditActionState::EditBPM)
	{
		ofSetColor(255, 0, 255, 255);
		ofDrawRectangle(EditorConfig::leftSidePosition - EditorConfig::sideSpace, GetSnappedCursorPosition().y, EditorConfig::fieldWidth + EditorConfig::sideSpace * 2, 4 );
		ofSetColor(255, 255, 255, 255);

		
		return void(); 
	}

	if (myCursorState == EditActionState::EditNotes || myCursorState == EditActionState::EditHolds)
	{
		ofSetColor(255, 255, 255, 128);
		EditorConfig::Skin::noteImages[GetColumn(GetSnappedCursorPosition().x)].draw(GetSnappedCursorPosition() - ofVec2f(0, 64));
		ofSetColor(255, 255, 255, 255);
	}
	else
	{
		EditorConfig::Skin::selectImage.draw(GetSnappedCursorPosition() - ofVec2f(0, 64));
	}

	if (myDraggableItem != nullptr && myPastePreview == false)
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
	float leftBorder = EditorConfig::leftSidePosition;
	float rightBorder = EditorConfig::leftSidePosition + EditorConfig::fieldWidth;

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
	bool flipped = false;

	if (myPastePreview == false)
	{
		std::vector<NoteData*> notesToPushToHistory;

		for (auto item : mySelectedItems)
		{
			if (item.first->selected == false)
				continue;

			notesToPushToHistory.push_back(item.first);
		}

		UndoRedoHandler::GetInstance()->PushHistory(ActionType::Move, notesToPushToHistory);
	}

	for (auto note : mySelectedItems)
	{
		if (note.first->selected == false)
			continue;

		note.first->column = 3 - note.first->column;
		note.first->hasMoved = true;

		flipped = true;
	}

	myNoteHandler->ScheduleRewind();

	if (flipped == true)
		PUSH_NOTIFICATION("Flipped Horizontally");
}

void EditHandler::ClickAction(int aX, int aY)
{	
	if (aX <= EditorConfig::leftSidePosition - EditorConfig::sideSpace - EditorConfig::MiniMap::spacingFromLeftSide)
		return void();

	if (myPastePreview == true)
	{
		PlacePaste();
		return void();
	}

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

	ResetSelection();

	myDragBox = true;
	myAnchoredDragPoint = { float(aX),float(aY) };
	myUpdatedDragPoint = { float(aX), float(aY) };

	myAnchoredTimePoint = TimeFieldHandlerBase<NoteData>::GetTimeFromScreenPoint(ofGetWindowHeight() - myAnchoredDragPoint.y, mySavedTimeS);
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

int EditHandler::GetColumn(int aX)
{
	float inputX = aX;
	float x = inputX;
	float leftBorder = EditorConfig::leftSidePosition;
	float rightBorder = EditorConfig::leftSidePosition + EditorConfig::fieldWidth;

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
	for (auto item : myClipBoard)
		delete item;
	
	myClipBoard.clear();

	for (auto item : mySelectedItems)
	{
		if (item.first->selected == false)
			continue;

		switch (item.first->noteType)
		{
		case NoteType::HoldBegin:
		{
			NoteData* newNote = new NoteData();
			*newNote = *item.first;

			NoteData* newHoldEnd = new NoteData();
			*newHoldEnd = *item.first->relevantNote;

			newNote->relevantNote = newHoldEnd;
			newHoldEnd->relevantNote = newNote;

			myClipBoard.push_back(newHoldEnd);
			myClipBoard.push_back(newNote);
		}
			break;

		case NoteType::Note:
		{
			NoteData* newNote = new NoteData();
			*newNote = *item.first;

			myClipBoard.push_back(newNote);
		}
			break;
		default:
			break;
		}
	}

	std::sort(myClipBoard.begin(), myClipBoard.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});

}

void EditHandler::Paste()
{
	if(myClipBoard.size() > 0)
		myPastePreview = true;

	ResetSelection();

	for (auto item : myClipBoard)
	{
		item->selected = true;
		mySelectedItems[item] = item;
	}
}

void EditHandler::PlacePaste()
{
	myPastePreview = false;
	myDraggableItem = nullptr;

	ResetSelection();

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
			if (myNoteHandler->PlaceHold(item->column, cursorTimePoint + deltaTimePoint, holdEnd) == true)
			{
				holdEnd->timePoint = cursorTimePoint + (item->relevantNote->timePoint - minTimePoint);

				UndoRedoHandler::GetInstance()->PushHistory(ActionType::Place, { holdEnd->relevantNote, holdEnd });
			}
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
	myAnchoredDragPoint.y = ofGetWindowHeight() - TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(myAnchoredTimePoint, aCurrentTime);

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
			note = { float(visibleNote->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(visibleNote->timePoint, aCurrentTime) + 0.5)),  
					 EditorConfig::Skin::noteImages->getWidth(), EditorConfig::Skin::noteImages->getHeight() };

			ofRectangle note2 = { float(visibleNote->relevantNote->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(visibleNote->relevantNote->timePoint, aCurrentTime) + 0.5)),  
								  EditorConfig::Skin::noteImages->getWidth(), EditorConfig::Skin::noteImages->getHeight() };

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
			note = { float(visibleNote->x), float(ofGetWindowHeight() - int(TimeFieldHandlerBase<NoteData>::GetScreenTimePoint(visibleNote->timePoint, aCurrentTime) + 0.5)), EditorConfig::Skin::noteImages->getWidth(), EditorConfig::Skin::noteImages->getHeight() };

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

	bool hasMoved = false;

	int savedTimePoint = myDraggableItem->timePoint;
	int deltaTimePoint = myBPMLineHandler->GetClosestTimePoint(myCursorPosition.y) - savedTimePoint;

	int minColumn = EditorConfig::keyAmount;
	int maxColumn = -1;

	int savedColumn = myDraggableItem->column;
	int deltaColumn = GetColumn(myCursorPosition.x) - savedColumn;

	if ((deltaColumn != 0 || deltaTimePoint != 0) && myPastePreview == false)
	{
		std::vector<NoteData*> notesToPushToHistory;

		for (auto item : mySelectedItems)
		{
			if (item.first->selected == false)
				continue;

			item.first->hasMoved = true;
			if (mySelectedItems.size() == 1 && (item.first->noteType == NoteType::HoldBegin || item.first->noteType == NoteType::HoldEnd))
				item.first->relevantNote->hasMoved = true;

			notesToPushToHistory.push_back(item.first);
		}

		UndoRedoHandler::GetInstance()->PushHistory(ActionType::Move, notesToPushToHistory);
	}

	for (auto item : mySelectedItems)
	{
		if (item.first->selected == false)
			continue;

		item.first->timePoint += deltaTimePoint;

		minColumn = std::min(minColumn, item.first->column);
		maxColumn = std::max(maxColumn, item.first->column);

		myNoteHandler->AddToVisibleObjects(item.first);
	}


	if (minColumn + deltaColumn >= 0 && maxColumn + deltaColumn <= EditorConfig::keyAmount - 1)
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
	int minTimePoint = INT_MAX;

	for (auto item : myClipBoard)
	{
		if (item->timePoint < minTimePoint)
			myDraggableItem = item;
		
		minTimePoint = std::min(minTimePoint, item->timePoint);
	}

	ofSetColor(255, 255, 255, 128);

	for (auto item : myClipBoard)
	{
		switch (item->noteType)
		{
			case NoteType::HoldBegin:
				myNoteHandler->DrawHold(item->column, item->timePoint, item->relevantNote->timePoint);	
				break;

			case NoteType::Note:
				myNoteHandler->DrawNote(item->column, item->timePoint);
				break;

			default:
				break;
			}
	}

	ofSetColor(255, 255, 255, 255);
}

void EditHandler::ResetSelection()
{
	for (auto note : mySelectedItems)
		note.first->selected = false;

	mySelectedItems.clear();
}
