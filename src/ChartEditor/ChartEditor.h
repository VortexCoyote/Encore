#pragma once

#include "ResourceManagement/ChartResourceHandler.h"
#include "ResourceManagement/SkinHandler.h"

#include "SongHandler.h"
#include "EditHandler.h"
#include "MiniMap.h"

#include "TimeFieldHandlers/NoteHandler.h"
#include "TimeFieldHandlers/BPMLineHandler.h"

#include "NotificationSystem.h"

class ChartEditor
{
public:
	
	ChartEditor();
	~ChartEditor();

	bool ShouldBlockInput();

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

	void IncreaseSnapDivision();
	void DecreaseSnapDivision();

	void MoveDown();
	void MoveUp();

	void Copy();
	void Paste();

	void SetEditMode(EditActionState aMode);
	void SetMousePosition(int aX, int aY);
	void DoUndo();

	void TryDropFilesAction(std::string aPath);

	//Click Edit Actions
	void DoLeftClickPressedAction(int aX, int aY);
	void DoRightClickPressedAction(int aX, int aY);

	void DoLeftClickReleaseAction(int aX, int aY);
	void DoRightClickReleaseAction(int aX, int aY);

	void DoLeftClickDragAction(int aX, int aY);
	void DoRightClickDragAction(int aX, int aY);

	void DoShiftAction(bool aFreePlace);

	//Edit Actions
	void TryPlaceNote(int aX, int aY);
	void TryDeleteNote(int aX, int aY);
	
	void TryPlaceHold(int aX, int aY);
	void TryDragHold(int aX, int aY);
	void TryReleaseHold(int aX, int aY);
	
	void TryPlaceBPMLine(int aX, int aY);

	void TrySelectItem(int aX, int aY);
	void TryTimelinePreview(int aX, int aY);

	void TrySaveCurrentChart();

	void TryDeleteSelected();
	void TryFlipSelected();
	void TrySelectAll();

private:

	void MenuBar();
	void LoadChartFromDirectory();
	void SetSelectedChart(ChartData* aChartData);

	void DoNewChartSetWindow();
	void DoNewDifficultyWindow();
	void DoShortcutsWindow();

	bool IsCursorWithinBounds(int aX, int aY);

	int GetScreenTimePoint(float aY);

	ChartResourceHandler myChartResourceHandler;
	SkinHandler mySkinHandler;

	SongHandler mySongTimeHandler;
	NoteHandler myNoteHandler;
	BPMLineHandler myBPMLineHandler;
	EditHandler myEditHandler;

	MiniMap myMiniMap;

	ChartSet* mySelectedChartSet = nullptr;
	ChartData* mySelectedChart	 = nullptr;

	ChartSet* myNewChartSet		 = nullptr;
	ChartData* myNewChart		 = nullptr;

	std::string myLoadedChartDirectory = "";

	float myPreviewTimeLine = 0.f;
	float myZoom			= 1.f;

	int myMouseX = 0;
	int myMouseY = 0;

	bool myHoldDrag = false;
	bool myFreePlace = false;

	NoteData* myDraggableHoldEnd = nullptr;

	bool myNewChartSetPopup = false;
	bool myNewDifficultyPopup = false; 
	bool myShowShortcuts = false;
};