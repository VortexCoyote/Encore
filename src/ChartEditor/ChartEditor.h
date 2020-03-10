#pragma once

#include "../ChartResourceImporter/ChartResourceImporter.h"
#include "SongHandler.h"
#include "NoteSelectionHandler.h"

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

	void IncreaseSpeed();
	void DecreaseSpeed();

	void TryPlaceNote(int aX, int aY);
	void TryDeleteNote(int aX, int aY);
	
	void TrySelectItem(int aX, int aY);
	void TryTimelinePreview(int aX, int aY);

	void SetMousePosition(int aX, int aY);

private:

	void MenuBar();
	void TimeLine();
	void LoadChartFromDirectory();
	void SetSelectedChart(ChartData* aChartData);

	ChartResourceImporter myChartImporter;
	SongHandler mySongTimeHandler;
	NoteHandler myNoteHandler;
	BPMLineHandler myBPMLineHandler;
	NoteSelectionHandler myNoteSelectionHandler;

	ChartSet* mySelectedChartSet = nullptr;
	ChartData* mySelectedChart	 = nullptr;

	float myTimeLine		= 0.f;
	float myPreviewTimeLine = 0.f;
	float myZoom			= 1.f;

	int myMouseX = 0;
	int myMouseY = 0;
};