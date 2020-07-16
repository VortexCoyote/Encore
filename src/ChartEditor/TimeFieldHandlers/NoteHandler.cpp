#include "NoteHandler.h"

#include "../Utilities/NotificationSystem.h"
#include "../Utilities/UndoRedoHandler.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include <string>

NoteHandler::NoteHandler()
{
	myHitLineImage.loadImage("skin/hitline.png");

	myVisibleObjects.reserve(100000);

	myPreviewBuffer.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);

	myAlwaysClearVisibles = true;
}

NoteHandler::~NoteHandler()
{

}

void NoteHandler::Init(std::vector<NoteData*>* aObjectData)
{
	TimeFieldHandlerBase<NoteData>::Init(aObjectData);

	myVisibleHolds.clear();
	myVisibleHoldsToRemove.clear();
}


std::vector<NoteData*>& NoteHandler::GetVisibleNotes()
{
	return myVisibleObjects;
}

NoteData* NoteHandler::GetHoveredNote(int aX, int aY)
{
	auto& visibleNotes = GetVisibleNotes();

	for (auto note : visibleNotes)
	{
		switch (note->noteType)
		{
		case NoteType::HoldBegin:
		{
			int column = note->column;
			float x = EditorConfig::leftSidePosition + EditorConfig::Skin::noteImages[column].getWidth() * column;
			float y = ofGetWindowHeight() - note->visibleTimePoint;

			if (aX > x && aX < x + EditorConfig::Skin::noteImages[column].getWidth() &&
				aY > y && aY < y + EditorConfig::Skin::noteImages[column].getWidth())
			{
				return note;
			}
		}
			break;

		case NoteType::HoldEnd:
		case NoteType::Note:
		{
			int column = note->column;
			if (aX > note->x&& aX < note->x + EditorConfig::Skin::noteImages[column].getWidth() &&
				aY > note->y&& aY < note->y + EditorConfig::Skin::noteImages[column].getWidth())
			{
				return note;
			}
		}

			break;

		default:

			std::cout << "invalid notetype" << std::endl;

			break;
		}
	}

	return nullptr;
}

void NoteHandler::RemoveVisibleHold(NoteData* aNote)
{
	auto note = myVisibleHolds.find(aNote);

	if (note != myVisibleHolds.end())
		myVisibleHolds.erase(note);
}

void NoteHandler::PlaceNote(int aColumn, int aTimePoint)
{
	if (CheckForDuplicates(aColumn, aTimePoint) == true)
	{
		PUSH_NOTIFICATION_COLORED("Can't Place Duplicates!", ofColor(255, 25, 25, 255));

		return void();
	}

	NoteData* note = new NoteData();

	note->column = aColumn;

	note->noteType = NoteType::Note;
	note->timePoint = aTimePoint;

	myObjectData->push_back(note);

	std::sort(myObjectData->begin(), myObjectData->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});

	std::string message = "Placed Note [";
	message += std::to_string(note->column + 1);
	message += "] ";
	message += std::to_string(note->timePoint);
	message += "ms";

	UndoRedoHandler::GetInstance()->PushHistory(ActionType::Place, { note });

	PUSH_NOTIFICATION(message);
}

void NoteHandler::DeleteNote(int aX, int aY)
{
	DeleteNoteByPointer(GetHoveredNote(aX, aY));
}

void NoteHandler::DeleteNoteByPointer(NoteData* aNotePointer, bool aRegisterHistory)
{
	NoteData* note = aNotePointer;

	if (note == nullptr)
		return void();

	auto noteToDelete = std::find(myObjectData->begin(), myObjectData->end(), note);

	if (noteToDelete != myObjectData->end())
	{
		switch (note->noteType)
		{
		case NoteType::HoldBegin:
		case NoteType::HoldEnd:
		{
			std::string message = "Deleted Hold [";
			message += std::to_string(note->column + 1);
			message += "] ";
			message += std::to_string(note->relevantNote->timePoint);
			message += "ms";

			RemoveVisibleHold(note);
			RemoveVisibleHold(note->relevantNote);

			if (aRegisterHistory == true)
			{
				PUSH_NOTIFICATION(message);
				UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });
			}

			myObjectData->erase(noteToDelete);
			myObjectData->erase(std::find(myObjectData->begin(), myObjectData->end(), note->relevantNote));

			if (note->hasMoved == false)
			{
				delete note->relevantNote;
				delete note;
			}
		}
		break;

		case NoteType::Note:
		{
			std::string message = "Deleted Note [";
			message += std::to_string(note->column + 1);
			message += "] ";
			message += std::to_string(note->timePoint);
			message += "ms";

			if (aRegisterHistory == true)
			{
				PUSH_NOTIFICATION(message);
				UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });
			}

			myObjectData->erase(noteToDelete);

			if(note->hasMoved == false)
				delete note;
		}
		break;

		default:

			PUSH_NOTIFICATION_DEBUG("Invalid NoteType");

			break;
		}
	}
}

bool NoteHandler::PlaceHold(int aColumn, int aTimePoint, NoteData*& aHoldEndOut)
{
	if (CheckForDuplicates(aColumn, aTimePoint) == true)
	{
		aHoldEndOut = nullptr;
		PUSH_NOTIFICATION_COLORED("Can't Place Duplicates!", ofColor(255, 25, 25, 255));

		return false;
	}

	NoteData* note = new NoteData();
	NoteData* holdNote = new NoteData();

	note->timePoint = aTimePoint;
	note->column = aColumn;
	note->noteType = NoteType::HoldBegin;

	holdNote->timePoint = note->timePoint;
	holdNote->column = note->column;
	holdNote->noteType = NoteType::HoldEnd;

	note->relevantNote = holdNote;
	holdNote->relevantNote = note;

	aHoldEndOut = holdNote;

	myObjectData->push_back(note);
	myObjectData->push_back(holdNote);

	std::sort(myObjectData->begin(), myObjectData->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});

	std::string message = "Placed Hold [";
	message += std::to_string(note->column + 1);
	message += "] ";
	message += std::to_string(note->timePoint);
	message += "ms";

	PUSH_NOTIFICATION(message);

	return true;
}

std::unordered_map<NoteData*, NoteData*>* NoteHandler::GetVisibleHolds()
{
	return &myVisibleHolds;
}

void NoteHandler::SortAllNotes()
{
	std::sort(myObjectData->begin(), myObjectData->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});
}

void NoteHandler::DrawNote(int aColumn, int aTimePoint)
{
	double screenTimePoint = GetScreenTimePoint(aTimePoint, myLastTimePoint);

	int x = GetItemPosXbyColumn(aColumn);
	int y = ofGetWindowHeight() - int(screenTimePoint + 0.5f);

	EditorConfig::Skin::noteImages[aColumn].draw(x, y);
}

void NoteHandler::DrawHold(int aColumn, int aTimePointBegin, int aTimePointEnd)
{
	double screenTimePointBegin = GetScreenTimePoint(aTimePointBegin, myLastTimePoint);
	double screenTimePointEnd = GetScreenTimePoint(aTimePointEnd, myLastTimePoint);

	int x = GetItemPosXbyColumn(aColumn);

	int yBegin = ofGetWindowHeight() - int(screenTimePointBegin + 0.5f);
	int yEnd = ofGetWindowHeight() - int(screenTimePointEnd + 0.5f);
	
	if (aTimePointBegin != aTimePointEnd)
	{
		EditorConfig::Skin::holdCapImage.draw(x, yEnd);

		EditorConfig::Skin::holdBodyImage.draw(x, yBegin + EditorConfig::Skin::noteImages[aColumn].getHeight() / 2,
		EditorConfig::Skin::holdBodyImage.getWidth(), (GetScreenTimePoint(aTimePointBegin, 0) - GetScreenTimePoint(aTimePointEnd, 0)) + EditorConfig::Skin::noteImages[aColumn].getHeight() / 2);
	}

	EditorConfig::Skin::noteImages[aColumn].draw(x, yBegin);
}


void NoteHandler::DrawRoutine(NoteData* aTimeObject, float aTimePoint)
{
	int column = aTimeObject->column;

	aTimeObject->x = GetItemPosXbyColumn(column);
	aTimeObject->y = ofGetWindowHeight() - int(aTimePoint + 0.5f);

	switch (aTimeObject->noteType)
	{
	case NoteType::HoldEnd:

		aTimeObject->relevantNote->x = aTimeObject->x;	
		aTimeObject->relevantNote->y = aTimeObject->y;	

		myVisibleHolds[aTimeObject->relevantNote] = aTimeObject->relevantNote;

		break;

	case NoteType::HoldBegin:

		myVisibleHolds[aTimeObject] = aTimeObject;

	case NoteType::Note:

		EditorConfig::Skin::noteImages[column].draw(aTimeObject->x, aTimeObject->y);

		break;

	default:

		std::cout << "invalid notetype" << std::endl;
		
		break;
	}

	if (aTimeObject->selected)
		EditorConfig::Skin::selectImage.draw(aTimeObject->x, aTimeObject->y);
}

void NoteHandler::VisibleHoldDrawRoutine(double aTimePoint)
{
	for (auto& hold : myVisibleHolds)
	{
		float y = ofGetWindowHeight() - GetScreenTimePoint(hold.first->timePoint, aTimePoint);
		float yParent = ofGetWindowHeight() - GetScreenTimePoint(hold.first->relevantNote->timePoint, aTimePoint);

		if (hold.first->timePoint != hold.first->relevantNote->timePoint)
		{
			EditorConfig::Skin::holdCapImage.draw(hold.first->x, yParent);

			EditorConfig::Skin::holdBodyImage.draw(hold.second->x, y + EditorConfig::Skin::noteImages[hold.first->column].getHeight() / 2,
			EditorConfig::Skin::holdBodyImage.getWidth(), (GetScreenTimePoint(hold.second->timePoint, 0) - GetScreenTimePoint(hold.second->relevantNote->timePoint, 0)) + EditorConfig::Skin::noteImages[hold.first->column].getHeight() / 2);
		}

		if (yParent > ofGetWindowHeight() || y < 0)
		{
			myVisibleHoldsToRemove.push_back(myVisibleHolds.find(hold.first));
		}
	}

	for (auto& holdToRemove : myVisibleHoldsToRemove)
	{
		myVisibleHolds.erase(holdToRemove);
	}

	myVisibleHoldsToRemove.clear();
}

bool NoteHandler::CheckForDuplicates(int aColumn, float aTimePoint)
{
	for (auto visibleNote : myVisibleObjects)
		if (visibleNote->column == aColumn
			&& visibleNote->timePoint <= aTimePoint + 1
			&& visibleNote->timePoint >= aTimePoint - 1)
		{
			return true;
		}

	return false;
}

int NoteHandler::GetItemPosXbyColumn(int aColumn)
{
	return EditorConfig::leftSidePosition + EditorConfig::Skin::noteImages[aColumn].getWidth() * aColumn;
}

void NoteHandler::Draw(double aTimePoint)
{
	if (myObjectData == nullptr)
		return void();

	VisibleHoldDrawRoutine(aTimePoint);

	TimeFieldHandlerBase<NoteData>::Draw(aTimePoint);

	if (showColumnLines == true)
	{
		ofSetColor(255, 255, 255, 64);
		for (int column = 0; column < EditorConfig::keyAmount; column++)
		{
			ofDrawLine(GetItemPosXbyColumn(column), 0, GetItemPosXbyColumn(column), ofGetWindowHeight());
		}

		ofDrawLine(GetItemPosXbyColumn(EditorConfig::keyAmount - 1) + EditorConfig::Skin::noteImages[0].getWidth() , 0, GetItemPosXbyColumn(EditorConfig::keyAmount - 1) + EditorConfig::Skin::noteImages[0].getWidth(), ofGetWindowHeight());


		ofSetColor(255, 255, 255, 255);
	}



	myHitLineImage.draw(EditorConfig::leftSidePosition, ofGetWindowHeight() - EditorConfig::hitLinePosition, EditorConfig::fieldWidth, myHitLineImage.getHeight());
	
	std::stringstream time;
	time << (int(aTimePoint * 1000.0)) << "ms";

	auto stringDimensions = StringHelpfunctions::getBitmapStringBoundingBox(time.str().c_str());

	ofVec2f timePointPos = { EditorConfig::leftSidePosition + EditorConfig::fieldWidth / 2.f - stringDimensions.width / 2.f,  ofGetWindowHeight() - EditorConfig::hitLinePosition + stringDimensions.height + 4 };

	ofSetColor(0, 0, 0, 196);
	ofDrawRectangle(timePointPos, stringDimensions.width, stringDimensions.height);
	ofSetColor(255, 255, 255, 255);

	ofDrawBitmapString(time.str().c_str(), { timePointPos.x, timePointPos.y + stringDimensions.height - 1 });
}

void NoteHandler::DrawPreviewBox(double aTimePoint, float aMouseY)
{
/*	myPreviewBuffer.begin();
	
	ofClear(0, 0, 0, 0);

	int   oldNoteIndex = myLastObjectIndex;
	float oldTimePoint = myLastTimePoint;
	std::vector<SelectableItem*> oldVisibleObjects = myVisibleObjects;

	TimeFieldHandlerBase<SelectableItem>::Draw(aTimePoint);

	myLastObjectIndex = oldNoteIndex;
	myLastTimePoint = oldTimePoint;
	myVisibleObjects = oldVisibleObjects;

	myPreviewBuffer.end();

	int width  = ofGetWindowWidth()  / 2.8;
	int height = ofGetWindowHeight() / 2.8;
	
	myPreviewBuffer.draw({ ofGetWindowWidth() - width + 64, aMouseY - height / 2}, width, height);*/
}

void NoteHandler::DrawNoteFieldBackground()
{
	ofSetColor(0, 0, 0, 216);
	ofDrawRectangle(EditorConfig::leftSidePosition - EditorConfig::sideSpace, 0.f, 1.f, EditorConfig::fieldWidth + EditorConfig::sideSpace * 2.f, ofGetWindowHeight());
	ofSetColor(255, 255, 255, 255);
}
