#pragma once

#include <unordered_map>

#include "ofImage.h"
#include "ofVec2f.h"

enum class EditActionState
{
	Select,
	EditNotes,
	EditHolds,
	EditBPM,

	Count
};

struct NoteData;
class NoteHandler;
class BPMLineHandler;
class EditHandler
{
public:

	EditHandler();
	~EditHandler();

	void Init(BPMLineHandler* aBPMLineHandler, NoteHandler* aNoteHandler, std::vector<NoteData*>* aNoteData);
	void Update(double aCurrentTime);
	void Draw();

	ofVec2f GetSnappedCursorPosition();
	
	void SelectAll();

	void SetCursorInput(ofVec2f aPosition);
	void SetVisibleItems(std::vector<NoteData*>* aVisibleItems);
	void SetEditActionState(EditActionState aCursorState);
	void TrySelectItem(int aX, int aY);
	void SetFreePlace(bool aFreePlace);

	void TryDeleteSelectedItems();
	void TryFlipSelectedItemsHorizonally();
	
	//Drag Box
	void ClickAction(int aX, int aY);
	void DragAction(int aX, int aY);
	void ReleaseAction(int aX, int aY);

	EditActionState GetEditActionState();

	void ClearSelectedItems();

	int GetColumn(int aX);

private:

	void TryDragBoxSelect(double aCurrentTime);
	void TryMoveSelectedNotes(double aCurrentTime);

	ofImage myCursorImage;
	ofVec2f myCursorPosition;

	BPMLineHandler* myBPMLineHandler;
	NoteHandler* myNoteHandler;

	bool myFreePlace = false;

	bool myDragBox = false;
	bool myMoveAllSelectedNotes = false;
	bool myWasItemSelected = false;

	int myAnchoredTimePoint;

	ofVec2f myAnchoredDragPoint;
	ofVec2f myUpdatedDragPoint;

	NoteData* myDraggableItem = nullptr;
	
	EditActionState myCursorState = EditActionState::EditNotes;

	std::vector<NoteData*>* myVisibleItems = nullptr;
	std::unordered_map<NoteData*, NoteData*>  mySelectedItems;

	std::vector<NoteData*>* myNoteData = nullptr;

	double mySavedTimeS = 0.0;
};