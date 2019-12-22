#pragma once

#include "ofImage.h"
#include "ofFbo.h"

#include "../../ChartData/ChartData.h"

#include "TimeFieldHandlerBase.h"

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
	
	void ClearLNs();
	void AddLN(SelectableItem aLN);

private:

	void DrawRoutine(SelectableItem* aTimeObject, float aTimePoint) override;

	ofFbo myPreviewBuffer;

	ofImage myNoteImage[4];
	ofImage mySelectedImage;

	ofImage myHoldBodyImage;
	ofImage myHoldCapImage;

	ofImage myHitLineImage;

	unsigned int myLastLNIndex = 0;
	double		 myLastLNTimePoint = 0.0;

	std::vector<SelectableItem> myLNs;
};