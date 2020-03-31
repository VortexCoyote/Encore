#pragma once

#include <vector>
#include <stack>

#include "../ChartData/ChartData.h"

enum class ActionType
{
	Place,
	Remove,
	Move,

	COUNT
};

struct SavedNote
{
	NoteType noteType;
	int timePoint = -1;
	int column = -1;

	SavedNote* relevantNote;
	NoteData* originalNote;

	bool wasMoved = false;
};

struct Action
{
	ActionType type;
	std::vector<SavedNote> relevantNotes;
};

class NoteHandler;
class UndoRedoHandler 
{
public:

	UndoRedoHandler();
	~UndoRedoHandler();

	static UndoRedoHandler* GetInstance();

	void Init(std::vector<NoteData*>* aNoteData, NoteHandler* aNoteHandler);

	void Undo();
	void Redo();

	void PushHistory(ActionType aType, const std::vector<NoteData*>& aCollectionOfNotes);

private:

	void DoAction(Action& aAction);

	bool FindAndRemoveFromSavedNote(SavedNote aSavedNote);

	std::stack<Action> myHistory;

	std::vector<NoteData*>* myNoteData;

	static UndoRedoHandler* ourInstance;

	NoteHandler* myNoteHandler = nullptr;
};