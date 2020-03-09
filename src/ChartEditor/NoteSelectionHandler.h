#pragma once

#include <unordered_map>

#include "ofImage.h"
#include "ofVec2f.h"

enum class NoteCursorState
{
	Place,
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

	int GetColumn();

private:

	ofImage myCursorImage;
	ofVec2f myCursorPosition;

	BPMLineHandler* myBPMLineHandler;

	NoteCursorState myCursorState = NoteCursorState::Place;

	std::vector<NoteData*>* myVisibleItems = nullptr;
	std::vector<NoteData*> mySelectedItems;

};