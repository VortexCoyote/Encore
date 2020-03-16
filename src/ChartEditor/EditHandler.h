#pragma once

#include <unordered_map>

#include "ofImage.h"
#include "ofVec2f.h"

enum class EditActionState
{
	EditNotes,
	EditHolds,
	Select,
	EditBPM,

	Count
};

struct NoteData;
class BPMLineHandler;
class EditHandler
{
public:

	EditHandler();
	~EditHandler();

	void Init(BPMLineHandler* aBPMLineHandler);
	void Update();
	void Draw();

	ofVec2f GetSnappedCursorPosition();
	
	void SetCursorInput(ofVec2f aPosition);
	void SetVisibleItems(std::vector<NoteData*>* aVisibleItems);
	void SetEditActionState(EditActionState aCursorState);
	void TrySelectItem(int aX, int aY);
	void SetFreePlace(bool aFreePlace);

	EditActionState GetEditActionState();

	void ClearSelectedItems();

	int GetColumn(int aX);

private:

	void ShowModeSelect();

	ofImage myCursorImage;
	ofVec2f myCursorPosition;

	BPMLineHandler* myBPMLineHandler;

	bool myFreePlace = false;

	EditActionState myCursorState = EditActionState::EditNotes;

	std::vector<NoteData*>* myVisibleItems = nullptr;
	std::vector<NoteData*> mySelectedItems;
};