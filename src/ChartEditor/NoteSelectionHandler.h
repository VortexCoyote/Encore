#pragma once

#include <unordered_map>

#include "ofImage.h"
#include "ofVec2f.h"

enum class NoteCursorState
{
	Edit,
	Select,

	Count
};

struct NoteData;
class BPMLineHandler;
class NoteSelectionHandler
{
public:

	NoteSelectionHandler();
	~NoteSelectionHandler();

	void Init(BPMLineHandler* aBPMLineHandler);
	void Draw();

	ofVec2f GetSnappedCursorPosition();
	
	void SetCursorInput(ofVec2f aPosition);
	void SetVisibleItems(std::vector<NoteData*>* aVisibleItems);
	void TrySelectItem(int aX, int aY);

	void ClearSelectedItems();

	int GetColumn(int aX);

private:

	ofImage myCursorImage;
	ofVec2f myCursorPosition;

	BPMLineHandler* myBPMLineHandler;

	NoteCursorState myCursorState = NoteCursorState::Edit;

	std::vector<NoteData*>* myVisibleItems = nullptr;
	std::vector<NoteData*> mySelectedItems;

};