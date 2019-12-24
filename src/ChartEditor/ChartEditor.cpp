#include "ChartEditor.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include "imgui.h"
#include "../ImGuiExtra.h"

ChartEditor::ChartEditor()
{
	mySelectableItems.reserve(100000);
}

ChartEditor::~ChartEditor()
{

}

void ChartEditor::Load()
{

}

void ChartEditor::Update()
{
	MenuBar();

	//null check shield
	if (mySelectedChart == nullptr)
		return void();

	myBPMLineHandler.ShowBeatDivisionControls();
	mySongTimeHandler.Update();
	TimeLine();
}

void ChartEditor::Draw()
{
	//null check shield
	if (mySelectedChart == nullptr)
		return void();

	mySelectedChart->background.draw((ofGetWindowWidth() - mySelectedChart->background.getWidth()) / 2, 0);

	TryTimelinePreview(myMouseX, myMouseY);

	myNoteHandler.DrawNoteFieldBackground();
	myBPMLineHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
	myNoteHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
}

void ChartEditor::TogglePlaying()
{
	if (mySelectedChart)
	{
		mySongTimeHandler.TogglePause();
	}
}

void ChartEditor::ZoomIn()
{
	EditorConfig::scale += 0.05f;
}

void ChartEditor::ZoomOut()
{
	EditorConfig::scale -= EditorConfig::scale >= 0.f ? 0.05f : 0.f;
}

void ChartEditor::ScrollUp()
{
	//TODO: Scroll should move with snapamount
	if (mySelectedChart != nullptr)
		mySongTimeHandler.SongJumpAmount(0.1f);
}

void ChartEditor::ScrollDown()
{
	//TODO: Scroll should move with snapamount
	if (mySelectedChart != nullptr)
		mySongTimeHandler.SongJumpAmount(-0.1f);
}

void ChartEditor::TrySelectItem(int aX, int aY)
{
	for (auto& item : myNoteHandler.GetVisibleNotes())
	{
		if (aX >= item->x && aX <= item->x + 64 &&
			aY >= item->y && aY <= item->y + 64)
		{
			item->selected = !item->selected;
			mySelectedItems.push_back(item);
			return void();
		}
	}

	for (auto& item : mySelectedItems)
		item->selected = false;	

	mySelectedItems.clear();
}

void ChartEditor::TryTimelinePreview(int aX, int aY)
{
	if (aX >= ofGetWindowWidth() - 32)
	{
		myPreviewTimeLine = float(1.0 - float(aY) / float(ofGetWindowHeight())) * (float(mySelectedChart->songLength) / 1000.f);
		myNoteHandler.DrawPreviewBox(myPreviewTimeLine, aY);
	}
}

void ChartEditor::RescaleBackground()
{
	if (mySelectedChart == nullptr)
		return void();

	float procentualChange = float(ofGetWindowHeight()) / float(mySelectedChart->background.getHeight());

	mySelectedChart->background.resize(int(float(mySelectedChart->background.getWidth()) * procentualChange), int(float(mySelectedChart->background.getHeight()) * procentualChange));
}

void ChartEditor::SetMousePosition(int aX, int aY)
{
	myMouseX = aX;
	myMouseY = aY;
}

void ChartEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Chart"))
		{
			if (ImGui::MenuItem("New"));

			if (ImGui::MenuItem("Open"))
				LoadChartFromDirectory();
		
			if (ImGui::MenuItem("Save"));
			
			if (ImGui::MenuItem("Export"));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}

			ImGui::EndMenu();
		}

		std::string selectedDifficulty = mySelectedChart == nullptr ? " | Difficulty: No Difficulty Selected" 
																	: " | Difficulty: " + mySelectedChart->difficultyName;	

		if (ImGui::BeginMenu(selectedDifficulty.c_str()))
		{
			if (mySelectedChartSet != nullptr)
				for (auto& difficulty : mySelectedChartSet->charts)           
					if (ImGui::MenuItem(difficulty->difficultyName.c_str()))                    
						SetSelectedChart(difficulty);
					
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ChartEditor::TimeLine()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;

	bool open = true;
	ImGui::Begin("TL", &open, windowFlags);

	ImGui::SetWindowSize({24.f, float(ofGetWindowHeight()) + 8 });
	ImGui::SetWindowPos({ float(ofGetWindowWidth()) - 40,  - 8 });

	if (ImGui::VSliderFloat("", { 24.f,  float(ofGetWindowHeight()) /* - menuBarHeight * 2.f */ }, &myTimeLine, 0.0f, 1.0f, ""))
	{
		mySongTimeHandler.SetPause(true);
		mySongTimeHandler.SetTimeS(myTimeLine * (float(mySelectedChart->songLength) / 1000.0));
	}
	else
	{
		mySongTimeHandler.ResetToPreviousPauseState();
		myTimeLine = mySongTimeHandler.GetCurrentTimeS() / (float(mySelectedChart->songLength) / 1000.0);
	}

	ImGui::End();
}

void ChartEditor::LoadChartFromDirectory()
{
	ofFileDialogResult result = ofSystemLoadDialog("select chart folder", true);
	if (result.bSuccess)
	{
		mySelectedChartSet = myChartImporter.ImportChart(result.getPath());
		
		if (mySelectedChartSet->charts.size() == 1)
		{
			SetSelectedChart(mySelectedChartSet->charts[0]);
		}
		else
		{
			mySelectedChart = nullptr;
		}
	}
}

void ChartEditor::SetSelectedChart(ChartData* aChartData)
{
	if(mySelectedChart)
		mySelectedChart->song.stop();

	mySelectedChart = aChartData;

	mySelectableItems.clear();
	mySelectedItems.clear();

	for (auto& note : mySelectedChart->noteData)
	{
		SelectableItem selectableNote;
		selectableNote.noteData = note;
		selectableNote.selected = false;
		selectableNote.timePoint = note->timePoint;

		mySelectableItems.push_back(selectableNote);
	}

	myNoteHandler.Init(&mySelectableItems);
	myBPMLineHandler.Init(&(mySelectedChart->BPMPoints));
	mySongTimeHandler.Init(&(mySelectedChart->song), 0 /*-mySelectedChart->BPMPoints[0].timePoint*/ );
	
	mySongTimeHandler.SetTimeNormalized(0.f);
}

