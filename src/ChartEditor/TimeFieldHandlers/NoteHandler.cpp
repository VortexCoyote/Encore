#include "NoteHandler.h"

#include "../NotificationSystem.h"
#include "../UndoRedoHandler.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include <string>

NoteHandler::NoteHandler()
{
	myNoteImage[0].loadImage("images/arrows/left.png");
	myNoteImage[1].loadImage("images/arrows/down.png");
	myNoteImage[2].loadImage("images/arrows/up.png");
	myNoteImage[3].loadImage("images/arrows/right.png");
	
	myHoldCapImage.loadImage("images/holdcap.png");
	myHoldBodyImage.loadImage("images/holdbody.png");
	
	mySelectedImage.loadImage("images/selected.png");
	
	myHitLineImage.loadImage("images/hitline.png");

	myVisibleObjects.reserve(100000);

	myPreviewBuffer.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);
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
			float x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
			float y = ofGetWindowHeight() - note->visibleTimePoint;

			if (aX > x && aX < x + 64 &&
				aY > y && aY < y + 64)
			{
				return note;
			}
		}
			break;

		case NoteType::HoldEnd:
		case NoteType::Note:

			if (aX > note->x && aX < note->x + 64 &&
				aY > note->y && aY < note->y + 64)
			{
				return note;
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
	NoteData* note = GetHoveredNote(aX, aY);

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

			PUSH_NOTIFICATION(message);

			RemoveVisibleHold(note);
			RemoveVisibleHold(note->relevantNote);

			UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });
			
			myObjectData->erase(noteToDelete);
			myObjectData->erase(std::find(myObjectData->begin(), myObjectData->end(), note->relevantNote));

			delete note->relevantNote;
			delete note;
		}
			break;

		case NoteType::Note:
		{
			std::string message = "Deleted Note [";
			message += std::to_string(note->column + 1);
			message += "] ";
			message += std::to_string(note->timePoint);
			message += "ms";

			PUSH_NOTIFICATION(message);

			UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });

			myObjectData->erase(noteToDelete);
			delete note;
		}
			break;

		default:

			PUSH_NOTIFICATION_DEBUG("Invalid NoteType");

			break;
		}
	}
}

void NoteHandler::DeleteNoteByPointer(NoteData* aNotePointer)
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

			PUSH_NOTIFICATION(message);

			RemoveVisibleHold(note);
			RemoveVisibleHold(note->relevantNote);

			UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });

			myObjectData->erase(noteToDelete);
			myObjectData->erase(std::find(myObjectData->begin(), myObjectData->end(), note->relevantNote));

			delete note->relevantNote;
			delete note;
		}
		break;

		case NoteType::Note:
		{
			std::string message = "Deleted Note [";
			message += std::to_string(note->column + 1);
			message += "] ";
			message += std::to_string(note->timePoint);
			message += "ms";

			PUSH_NOTIFICATION(message);

			UndoRedoHandler::GetInstance()->PushHistory(ActionType::Remove, { note });

			myObjectData->erase(noteToDelete);
			delete note;
		}
		break;

		default:

			PUSH_NOTIFICATION_DEBUG("Invalid NoteType");

			break;
		}
	}
}

void NoteHandler::PlaceHold(int acolumn, int aTimePoint, NoteData*& aHoldEndOut)
{
	NoteData* note = new NoteData();
	NoteData* holdNote = new NoteData();

	note->timePoint = aTimePoint;
	note->column = acolumn;
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


void NoteHandler::DrawRoutine(NoteData* aTimeObject, float aTimePoint)
{
	int column = aTimeObject->column;

	aTimeObject->x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
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

		myNoteImage[column].draw(aTimeObject->x, aTimeObject->y);

		break;

	default:

		std::cout << "invalid notetype" << std::endl;
		
		break;
	}

	if (aTimeObject->selected)
		mySelectedImage.draw(aTimeObject->x, aTimeObject->y);
}

void NoteHandler::VisibleHoldDrawRoutine(double aTimePoint)
{
	for (auto& hold : myVisibleHolds)
	{
		float y = ofGetWindowHeight() - GetScreenTimePoint(hold.first->timePoint, aTimePoint);
		float yParent = ofGetWindowHeight() - GetScreenTimePoint(hold.first->relevantNote->timePoint, aTimePoint);

		myHoldCapImage.draw(hold.first->x, yParent - myHoldCapImage.getHeight() / 2.f);
		
		myHoldBodyImage.draw(hold.second->x, y + myNoteImage[hold.first->column].getHeight() / 2.f, myHoldBodyImage.getWidth(), (GetScreenTimePoint(hold.second->timePoint, 0) - GetScreenTimePoint(hold.second->relevantNote->timePoint, 0)));
		
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

void NoteHandler::Draw(double aTimePoint)
{
	if (myObjectData == nullptr)
		return void();

	VisibleHoldDrawRoutine(aTimePoint);

	TimeFieldHandlerBase<NoteData>::Draw(aTimePoint);

	myHitLineImage.draw(ofGetWindowWidth() / 2 - myNoteImage[0].getWidth() * 2, ofGetWindowHeight() - EditorConfig::hitLinePosition /*+ myNoteImage[0].getHeight()*/ );
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
	ofDrawRectangle(ofGetWindowWidth() / 2 - myNoteImage[0].getWidth() * 2 - 32, 0.f, 1.f, myNoteImage[0].getWidth() * 4 + 64, ofGetWindowHeight());
	ofSetColor(255, 255, 255, 255);
}
