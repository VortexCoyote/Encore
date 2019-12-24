#pragma once

#include "ofImage.h"
#include "ofFbo.h"

#include "../../ChartData/ChartData.h"

#include "TimeFieldHandlerBase.h"

#include <unordered_map>

class NoteHandler : public TimeFieldHandlerBase<SelectableItem>
{
public:

	NoteHandler();
	~NoteHandler();

	void Init(std::vector<SelectableItem>* aObjectData) override;

	void Draw(double aTimePoint) override;

	void DrawPreviewBox(double aTimePoint, float aMouseY);
	void DrawNoteFieldBackground();

	std::vector<SelectableItem*>& GetVisibleNotes();

private:

	void DrawRoutine(SelectableItem* aTimeObject, float aTimePoint) override;
	void VisibleHoldDrawRoutine(double aTimePoint);

	ofFbo myPreviewBuffer;

	ofImage myNoteImage[4];
	ofImage mySelectedImage;

	ofImage myHoldBodyImage;
	ofImage myHoldCapImage;

	ofImage myHitLineImage;

	std::unordered_map<NoteData*, SelectableItem*> myVisibleHolds;
	std::vector<std::unordered_map<NoteData*, SelectableItem*>::iterator> myVisibleHoldsToRemove;
};