#include "ChartEditor.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include "imgui.h"
#include "../ImGuiExtra.h"

ChartEditor::ChartEditor()
{

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

	mySongTimeHandler.Update();
	myEditHandler.SetCursorInput({ float(myMouseX), float(myMouseY) });
	myEditHandler.SetVisibleItems(&(myNoteHandler.GetVisibleNotes()));

	NotificationSystem::GetInstance()->Update();

	TimeLine();
}

void ChartEditor::Draw()
{
	//null check shield
	if (mySelectedChart == nullptr)
		return void();

	float procentualChange = float(ofGetWindowHeight()) / float(mySelectedChart->background.getHeight());	
	mySelectedChart->background.draw((ofGetWindowWidth() - mySelectedChart->background.getWidth() * procentualChange) / 2, 0, int(float(mySelectedChart->background.getWidth()) * procentualChange), int(float(mySelectedChart->background.getHeight()) * procentualChange));

	TryTimelinePreview(myMouseX, myMouseY);

	myNoteHandler.DrawNoteFieldBackground();
	mySongTimeHandler.DrawWaveForm();
	myBPMLineHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
	myNoteHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
	myEditHandler.Draw();

	NotificationSystem::GetInstance()->Draw();
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
	if (EditorConfig::scale > 1.5f)
	{
		EditorConfig::scale = 1.5f;
		PUSH_NOTIFICATION(std::string("Zoom Limit Reached! (1.5x)"));
	}
	else
	{
		PUSH_NOTIFICATION(std::string("Zoomed In: ") + std::to_string(EditorConfig::scale));
	}

}

void ChartEditor::ZoomOut()
{
	EditorConfig::scale -= 0.05f;
	if (EditorConfig::scale < 0.05f)
	{
		EditorConfig::scale = 0.05f;
		PUSH_NOTIFICATION(std::string("Zoom Limit Reached! (0.05x)"));
	}
	else
	{
		PUSH_NOTIFICATION(std::string("Zoomed Out: ") + std::to_string(EditorConfig::scale));
	}
}

void ChartEditor::ScrollUp()
{
	MoveUp();
}

void ChartEditor::ScrollDown()
{
	MoveDown();
}

void ChartEditor::IncreaseSpeed()
{
	mySongTimeHandler.IncreaseSpeed();
}

void ChartEditor::DecreaseSpeed()
{
	mySongTimeHandler.DecreaseSpeed();
}

void ChartEditor::IncreaseSnapDivision()
{
	myBPMLineHandler.IncreaseBeatSnap();
}

void ChartEditor::DecreaseSnapDivision()
{
	myBPMLineHandler.DecreaseBeatSnap();
}

void ChartEditor::MoveDown()
{
	if (mySelectedChart != nullptr)
		mySongTimeHandler.SetTimeS(float(myBPMLineHandler.GetBiasedClosestBeatLineMS(mySongTimeHandler.GetCurrentTimeMS(), true)) / 1000.0);
}

void ChartEditor::MoveUp()
{
	if (mySelectedChart != nullptr)
		mySongTimeHandler.SetTimeS(float(myBPMLineHandler.GetBiasedClosestBeatLineMS(mySongTimeHandler.GetCurrentTimeMS(), false)) / 1000.0);
}

void ChartEditor::SetEditMode(int aMode)
{
	if (mySelectedChart != nullptr)
		myEditHandler.SetEditActionState((EditActionState)(aMode));
}

void ChartEditor::DoLeftClickPressedAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		TryPlaceNote(aX, aY);
		break;
	case EditActionState::EditHolds:
		TryPlaceHold(aX, aY);
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::DoRightClickPressedAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		TryDeleteNote(aX, aY);
		break;
	case EditActionState::EditHolds:
		TryDeleteNote(aX, aY);
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::DoLeftClickReleaseAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		break;
	case EditActionState::EditHolds:
		TryReleaseHold(aX, aY);
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::DoRightClickReleaseAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		break;
	case EditActionState::EditHolds:
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::DoLeftClickDragAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		break;
	case EditActionState::EditHolds:
		TryDragHold(aX, aY);
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::DoRightClickDragAction(int aX, int aY)
{
	if (mySelectedChart == nullptr)
		return void();

	switch (myEditHandler.GetEditActionState())
	{
	case EditActionState::EditNotes:
		break;
	case EditActionState::EditHolds:
		break;
	case EditActionState::Select:
		break;

	default:
		break;
	}
}

void ChartEditor::TryPlaceNote(int aX, int aY)
{
	if (IsCursorWithinBounds(aX, aY) == false)
		return void();

	myNoteHandler.PlaceNote(myEditHandler.GetColumn(aX), myBPMLineHandler.GetClosestTimePoint(myEditHandler.GetSnappedCursorPosition().y + 64));
}

void ChartEditor::TryDeleteNote(int aX, int aY)
{
	if (IsCursorWithinBounds(aX, aY) == false)
		return void();

	myNoteHandler.DeleteNote(aX, aY);
}

void ChartEditor::TryPlaceHold(int aX, int aY)
{
	if (IsCursorWithinBounds(aX, aY) == false)
		return void();

	myNoteHandler.PlaceHold(myEditHandler.GetColumn(aX), myBPMLineHandler.GetClosestTimePoint(myEditHandler.GetSnappedCursorPosition().y + 64), myDraggableHoldEnd);
	
	myHoldDrag = true;
}

void ChartEditor::TryDragHold(int aX, int aY)
{
	if (myHoldDrag == true && myDraggableHoldEnd != nullptr)
	{
		myDraggableHoldEnd->timePoint = myBPMLineHandler.GetClosestTimePoint(myEditHandler.GetSnappedCursorPosition().y + 64);
	}
}

void ChartEditor::TryReleaseHold(int aX, int aY)
{
	myHoldDrag = false;
	myDraggableHoldEnd = nullptr;

	std::sort(mySelectedChart->noteData.begin(), mySelectedChart->noteData.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->timePoint < rhs->timePoint;
	});
}

void ChartEditor::TrySelectItem(int aX, int aY)
{
	myEditHandler.TrySelectItem(aX, aY);
}

void ChartEditor::TryTimelinePreview(int aX, int aY)
{
	if (aX >= ofGetWindowWidth() - 32)
	{
		myPreviewTimeLine = float(1.0 - float(aY) / float(ofGetWindowHeight())) * (float(mySelectedChart->songLength) / 1000.f);
		myNoteHandler.DrawPreviewBox(myPreviewTimeLine, aY);
	}
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
		
			if (ImGui::MenuItem("Save"))
			{
				if(mySelectedChart != nullptr)
					myChartResourceHandler.SaveChart(myLoadedChartDirectory, mySelectedChart);
			}
			
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

		std::string selectedDifficulty = mySelectedChart == nullptr ? " --> Difficulty: No Difficulty Selected" 
																	: " --> Difficulty: " + mySelectedChart->difficultyName;	

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
    //windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;

	bool open = true;
	ImGui::Begin("TL", &open, windowFlags);

	ImGui::SetWindowSize({24.f, float(ofGetWindowHeight()) + 8 });
	ImGui::SetWindowPos({ float(ofGetWindowWidth() - 40),  - 8 });

	if (ImGui::VSliderFloat("", { 24.f,  float(ofGetWindowHeight()) /* - menuBarHeight * 2.f */ }, &myTimeLine, 0.0f, 1.0f, ""))
	{
		mySongTimeHandler.SetTimeNormalized(myTimeLine);
	}
	else
	{
		myTimeLine = mySongTimeHandler.GetCurrentTimeS() / mySongTimeHandler.GetSongLength();
	}

	ImGui::End();
}

void ChartEditor::LoadChartFromDirectory()
{
	ofFileDialogResult result = ofSystemLoadDialog("select chart folder", true);
	if (result.bSuccess)
	{
		mySelectedChartSet = myChartResourceHandler.ImportChart(result.getPath());
		myLoadedChartDirectory = result.getPath();

		if (mySelectedChartSet->charts.size() == 1)
		{
			SetSelectedChart(mySelectedChartSet->charts[0]);
		}
		else
		{
			mySelectedChart = nullptr;
			mySongTimeHandler.StopSong();
		}
	}
}

void ChartEditor::SetSelectedChart(ChartData* aChartData)
{
	mySelectedChart = aChartData;

	myEditHandler.ClearSelectedItems();

	myNoteHandler.Init(&(mySelectedChart->noteData));
	myBPMLineHandler.Init(&(mySelectedChart->BPMPoints));
	mySongTimeHandler.Init(mySelectedChart->song);
	myEditHandler.Init(&myBPMLineHandler);
	
	mySongTimeHandler.SetTimeNormalized(0.f);
	mySongTimeHandler.ResetSpeed();
}

bool ChartEditor::IsCursorWithinBounds(int aX, int aY)
{
	int leftBorder = ofGetWindowWidth() / 2 - 64 * 2;
	int rightBorder = ofGetWindowWidth() / 2 + 64 * 2;

	bool withinBounds = aX >= leftBorder && aX <= rightBorder;

	return withinBounds;
}
