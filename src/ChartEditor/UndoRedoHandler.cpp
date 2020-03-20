#include "UndoRedoHandler.h"

#include "../ChartData/ChartData.h"

#include "NotificationSystem.h"

#include "TimeFieldHandlers/NoteHandler.h"

UndoRedoHandler* UndoRedoHandler::ourInstance = nullptr;

UndoRedoHandler::UndoRedoHandler()
{
	
}

UndoRedoHandler::~UndoRedoHandler()
{
}

UndoRedoHandler* UndoRedoHandler::GetInstance()
{
	if (ourInstance == nullptr)
		ourInstance = new UndoRedoHandler();

	return ourInstance;
}

void UndoRedoHandler::Init(std::vector<NoteData*>* aNoteData, NoteHandler* aNoteHandler)
{
	while (myHistory.empty() == false)
		myHistory.pop();
	
	myNoteData = aNoteData;
	myNoteHandler = aNoteHandler;
}

void UndoRedoHandler::Undo()
{
	if (myHistory.empty() == true)
		return void();

	Action action = myHistory.top();

	DoAction(action);

	myHistory.pop();
}

void UndoRedoHandler::Redo()
{
	PUSH_NOTIFICATION_COLORED("Not yet implemented!", ofColor(255, 25, 25, 255));
}

void UndoRedoHandler::PushHistory(ActionType aType, const std::vector<NoteData*>& aCollectionOfNotes)
{
	Action action;
	action.type = aType;
	
	for (auto note : aCollectionOfNotes)
	{
		SavedNote savedNote;
		savedNote.noteType = note->noteType;
		savedNote.timePoint = note->timePoint;
		savedNote.column = note->column;

		if (action.type == ActionType::Remove && (note->noteType == NoteType::HoldBegin || note->noteType == NoteType::HoldEnd))
		{
			savedNote.relevantNote = new SavedNote();
			savedNote.relevantNote->noteType = note->relevantNote->noteType;
			savedNote.relevantNote->column = note->relevantNote->column;
			savedNote.relevantNote->timePoint = note->relevantNote->timePoint;
		}

		action.relevantNotes.push_back(savedNote);
	}

	myHistory.push(action);
}

void UndoRedoHandler::DoAction(Action& aAction)
{
	switch (aAction.type)
	{
	case ActionType::Place:
	{
		for (auto note : aAction.relevantNotes)
		{
			switch (note.noteType)
			{
			case NoteType::Note:

				if (FindAndRemoveFromSavedNote(note) == true)
				{
					PUSH_NOTIFICATION("Undid Note Placement");
				}
				else
				{
					PUSH_NOTIFICATION_COLORED("Could Not Find Note!", ofColor(255, 25, 25, 255));
				}

				break;

			case NoteType::HoldBegin:
			case NoteType::HoldEnd:

				if (FindAndRemoveFromSavedNote(note) == true)
				{
					if (note.noteType == NoteType::HoldBegin)
						PUSH_NOTIFICATION("Undid HoldBegin Placement");

					if (note.noteType == NoteType::HoldEnd)
						PUSH_NOTIFICATION("Undid HoldEnd Placement");
				}
				else
				{
					PUSH_NOTIFICATION_COLORED("Could Not Find Hold!", ofColor(255, 25, 25, 255));
				}

				break;

			default:
				break;
			}
		}
	}

	break;

	case ActionType::Remove:
	{
		for (auto note : aAction.relevantNotes)
		{
			switch (note.noteType)
			{
			case NoteType::Note:
			{
				NoteData* newNote = new NoteData();

				newNote->noteType = note.noteType;
				newNote->column = note.column;
				newNote->timePoint = note.timePoint;

				myNoteData->push_back(newNote);
			}
			break;

			case NoteType::HoldBegin:
			case NoteType::HoldEnd:
			{
				NoteData* hold1 = new NoteData();
				NoteData* hold2 = new NoteData();

				hold1->noteType = note.noteType;
				hold1->column = note.column;
				hold1->timePoint = note.timePoint;

				hold2->noteType = note.relevantNote->noteType;
				hold2->column = note.relevantNote->column;
				hold2->timePoint = note.relevantNote->timePoint;

				hold1->relevantNote = hold2;
				hold2->relevantNote = hold1;

				myNoteData->push_back(hold2);
				myNoteData->push_back(hold1);

				delete note.relevantNote;
			}
			break;

			default:
				break;
			}
		}
	}

	break;

	case ActionType::Move:

		break;

	default:
		break;
	}

	std::sort(myNoteData->begin(), myNoteData->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});
}

bool UndoRedoHandler::FindAndRemoveFromSavedNote(SavedNote aSavedNote)
{
	auto it = std::find_if(myNoteData->begin(), myNoteData->end(), [aSavedNote](const NoteData* aNote) -> bool 
	{
		return aNote->noteType == aSavedNote.noteType && 
			   aNote->timePoint == aSavedNote.timePoint && 
			   aNote->column == aSavedNote.column;
	});

	if (it != myNoteData->end())
	{
		if (aSavedNote.noteType == NoteType::HoldBegin || aSavedNote.noteType == NoteType::HoldEnd)
		{
			myNoteHandler->RemoveVisibleHold(*it);
		}

		delete* it;
		myNoteData->erase(it);

		return true;
	}

	return false;
}
