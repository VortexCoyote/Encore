#pragma once

#include "../ChartResourceImporter/ChartResourceImporter.h"
#include "SongTimeHandler.h"

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
	void RescaleBackground();

	void SetMousePosition(int aX, int aY);

private:

	void MenuBar();
	void TimeLine();
	void LoadChartFromDirectory();
	void SetSelectedChart(ChartData* aChartData);

	ChartResourceImporter myChartImporter;
	SongTimeHandler mySongTimeHandler;
	NoteHandler myNoteHandler;
	BPMLineHandler myBPMLineHandler;

	ChartSet* mySelectedChartSet = nullptr;
	ChartData* mySelectedChart	 = nullptr;

	std::vector<SelectableItem> mySelectableItems;
	std::vector<SelectableItem*> mySelectedItems;

	float myTimeLine		= 0.f;
	float myPreviewTimeLine = 0.f;
	float myZoom			= 1.f;

	int myMouseX = 0;
	int myMouseY = 0;
};