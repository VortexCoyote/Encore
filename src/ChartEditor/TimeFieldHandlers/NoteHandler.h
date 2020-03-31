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

	NoteData* GetHoveredNote(int aX, int aY);

	void RemoveVisibleHold(NoteData* aNote);

	void PlaceNote(int aColumn, int aTimePoint);
	void DeleteNote(int aX, int aY);
	void DeleteNoteByPointer(NoteData* aNotePointer);

	bool PlaceHold(int aColumn, int aTimePoint, NoteData*& aHoldEndOut);

	std::unordered_map<NoteData*, NoteData*>* GetVisibleHolds();

	void SortAllNotes();

	void DrawNote(int aColumn, int aTimePoint);
	void DrawHold(int aColumn, int aTimePointBegin, int aTimePointEnd);

	bool showColumnLines = false;

private:

	void DrawRoutine(NoteData* aTimeObject, float aTimePoint) override;
	void VisibleHoldDrawRoutine(double aTimePoint);

	int GetItemPosXbyColumn(int aColumn);

	std::unordered_map<NoteData*, NoteData*> myVisibleHolds;
	std::vector<std::unordered_map<NoteData*, NoteData*>::iterator> myVisibleHoldsToRemove;

	ofFbo myPreviewBuffer;

	ofImage myHitLineImage;
};