#pragma once

#include "ofImage.h"
#include "ofFbo.h"

#include "../../ChartData/ChartData.h"

#include "TimeFieldHandlerBase.h"

#include <unordered_map>

class NoteHandler : public TimeFieldHandlerBase<NoteData>
{
public:

	NoteHandler();
	~NoteHandler();

	void Init(std::vector<NoteData*>* aObjectData) override;

	void Draw(double aTimePoint) override;

	void DrawPreviewBox(double aTimePoint, float aMouseY);
	void DrawNoteFieldBackground();

	std::vector<NoteData*>& GetVisibleNotes();

private:

	void DrawRoutine(NoteData* aTimeObject, float aTimePoint) override;
	void VisibleHoldDrawRoutine(double aTimePoint);

	std::unordered_map<NoteData*, NoteData*> myVisibleHolds;
	std::vector<std::unordered_map<NoteData*, NoteData*>::iterator> myVisibleHoldsToRemove;

	ofFbo myPreviewBuffer;

	ofImage myNoteImage[4];
	ofImage mySelectedImage;

	ofImage myHoldBodyImage;
	ofImage myHoldCapImage;

	ofImage myHitLineImage;
};