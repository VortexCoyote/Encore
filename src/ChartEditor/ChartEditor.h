#pragma once

#include "../ChartResourceImporter/ChartResourceImporter.h"
#include "SongHandler.h"

#include "TimeFieldHandlers/NoteHandler.h"
#include "TimeFieldHandlers/BPMLineHandler.h"

class ChartEditor
{
public:
	
	ChartEditor();
	~ChartEditor();

	void Load();
	void Update();
	void Draw();

	void TogglePlaying();

	void ZoomIn();
	void ZoomOut();

	void ScrollUp();
	void ScrollDown();

	void TrySelectItem(int aX, int aY);
	void TryTimelinePreview(int aX, int aY);

	void SetMousePosition(int aX, int aY);

private:

	void MenuBar();
	void TimeLine();
	void LoadChartFromDirectory();
	void SetSelectedChart(ChartData* aChartData);
	ofVec2f GetSnappedCursorPosition();

	ChartResourceImporter myChartImporter;
	SongHandler mySongTimeHandler;
	NoteHandler myNoteHandler;
	BPMLineHandler myBPMLineHandler;

	ChartSet* mySelectedChartSet = nullptr;
	ChartData* mySelectedChart	 = nullptr;

	std::vector<NoteData*> mySelectedItems;
	std::vector<NoteData>  myStackChart;

	float myTimeLine		= 0.f;
	float myPreviewTimeLine = 0.f;
	float myZoom			= 1.f;

	int myMouseX = 0;
	int myMouseY = 0;

	ofImage myCursorImage;
};