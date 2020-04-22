#include "ChartEditor.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "../ImGuiExtra.h"
#include "../imgui_stdlib.h"

#include "Utilities/UndoRedoHandler.h"
#include "Utilities/TileableGUI.h"

#include <regex>

ChartEditor::ChartEditor()
{

}

ChartEditor::~ChartEditor()
{

}

bool ChartEditor::ShouldBlockInput()
{
	return myNewChartSetPopup || myNewDifficultyPopup;
}

void ChartEditor::Load()
{
	TileableGUI::GetInstance()->Init();
}

void ChartEditor::Update()
{
	AdjustFieldPosition();

	MenuBar();

	DoNewChartSetWindow();
	DoNewDifficultyWindow();
	DoShortcutsWindow();

	NotificationSystem::GetInstance()->Update();

	//null check shield
	if (mySelectedChart == nullptr)
		return void();

	/*if (mySelectedChart->BPMPoints.empty() == true)
		myFreePlace = true;*/

	mySongTimeHandler.Update();

	myEditHandler.SetCursorInput({ float(myMouseX), float(myMouseY) });
	myEditHandler.SetVisibleItems(&(myNoteHandler.GetVisibleNotes()));
	myEditHandler.Update(mySongTimeHandler.GetCurrentTimeS());
	myBPMLineHandler.Update();

	TileableGUI::GetInstance()->Update();

	myMiniMap.Update();
}

void ChartEditor::Draw()
{
	//null check shield
	if (mySelectedChart == nullptr)
	{
		return void();
	}

	if (mySelectedChart->backgroundFileName != "")
	{
		ofSetColor(255, 255, 255, 200);
		float procentualChange = float(ofGetWindowHeight()) / float(mySelectedChart->background.getHeight());
		mySelectedChart->background.draw((ofGetWindowWidth() - mySelectedChart->background.getWidth() * procentualChange) / 2, 0, int(float(mySelectedChart->background.getWidth()) * procentualChange), int(float(mySelectedChart->background.getHeight()) * procentualChange));
		ofSetColor(255, 255, 255, 255);
	}

	myNoteHandler.DrawNoteFieldBackground();
	mySongTimeHandler.DrawWaveForm();
	myBPMLineHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
	myNoteHandler.Draw(mySongTimeHandler.GetCurrentTimeS());
	myEditHandler.Draw();

	myMiniMap.Draw();
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
		myNoteHandler.ScheduleRewind();
		myBPMLineHandler.ScheduleRewind();
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

void ChartEditor::Copy()
{
	if (mySelectedChart != nullptr && myEditHandler.GetEditActionState() == EditActionState::Select)
		myEditHandler.Copy();
}

void ChartEditor::Paste()
{
	if (mySelectedChart != nullptr && myEditHandler.GetEditActionState() == EditActionState::Select)
		myEditHandler.Paste();
}

void ChartEditor::SetEditMode(EditActionState aMode)
{
	if (mySelectedChart != nullptr)
		myEditHandler.SetEditActionState(aMode);
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
	case EditActionState::EditBPM:
		TryPlaceBPMLine(aX, aY);
		break;

	case EditActionState::Select:
		myEditHandler.ClickAction(aX, aY);
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
		myEditHandler.ReleaseAction(aX, aY);
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
		myEditHandler.DragAction(aX, aY);
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

	myNoteHandler.PlaceNote(myEditHandler.GetColumn(aX), GetScreenTimePoint(aY));
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

	if(myNoteHandler.PlaceHold(myEditHandler.GetColumn(aX), GetScreenTimePoint(aY), myDraggableHoldEnd) == true)
		myHoldDrag = true;
}

void ChartEditor::TryDragHold(int aX, int aY)
{
	if (myHoldDrag == true && myDraggableHoldEnd != nullptr)
	{
		myDraggableHoldEnd->timePoint = GetScreenTimePoint(aY);
	}
}

void ChartEditor::TryReleaseHold(int aX, int aY)
{
	if (myHoldDrag == false)
		return void();

	if (myDraggableHoldEnd->timePoint == myDraggableHoldEnd->relevantNote->timePoint)
	{
		UndoRedoHandler::GetInstance()->PushHistory(ActionType::Place, { myDraggableHoldEnd->relevantNote,  myDraggableHoldEnd });

		int column = myDraggableHoldEnd->relevantNote->column;
		int timePoint = myDraggableHoldEnd->relevantNote->timePoint;

		myNoteHandler.DeleteNoteByPointer(myDraggableHoldEnd);
		myNoteHandler.PlaceNote(column, timePoint);

		myHoldDrag = false;
		myDraggableHoldEnd = nullptr;

		return void();
	}

	UndoRedoHandler::GetInstance()->PushHistory(ActionType::Place, { myDraggableHoldEnd->relevantNote,  myDraggableHoldEnd });

	myHoldDrag = false;
	myDraggableHoldEnd = nullptr;

	myNoteHandler.SortAllNotes();
}

void ChartEditor::TryPlaceBPMLine(int aX, int aY)
{
	if (IsCursorWithinBounds(aX, aY) == false)
		return void();

	myBPMLineHandler.PlaceBPMLine(GetScreenTimePoint(aY));		
}

void ChartEditor::TrySelectItem(int aX, int aY)
{
	myEditHandler.TrySelectItem(aX, aY);
}


void ChartEditor::TrySaveCurrentChart()
{
	if (mySelectedChart != nullptr)
		myChartResourceHandler.SaveChart(mySelectedChartSet->saveDirectory, myLoadedChartDirectory, mySelectedChart);
}

void ChartEditor::TryDeleteSelected()
{
	if (mySelectedChart == nullptr)
		return void();

	myEditHandler.TryDeleteSelectedItems();
}

void ChartEditor::TryFlipSelected()
{
	if (mySelectedChart == nullptr)
		return void();

	myEditHandler.TryFlipSelectedItemsHorizonally();
}

void ChartEditor::TrySelectAll()
{
	if (mySelectedChart == nullptr)
		return void();

	myEditHandler.SetEditActionState(EditActionState::Select);
	myEditHandler.SelectAll();
}

void ChartEditor::SetMousePosition(int aX, int aY)
{
	myMouseX = aX;
	myMouseY = aY;
}

void ChartEditor::DoUndo()
{
	UndoRedoHandler::GetInstance()->Undo();
}

void ChartEditor::TryDropFilesAction(std::string aPath)
{
	if (mySelectedChart == nullptr)
		return void();

	std::string typeEnding;

	int i = aPath.size() - 1;
	while (aPath[i] != '.')
	{
		typeEnding += aPath[i];
		i--;
	}

	std::reverse(typeEnding.begin(), typeEnding.end());

	if (typeEnding == "png" || typeEnding == "jpg")
	{
		std::string imagePath = mySelectedChartSet->saveDirectory + "/" + mySelectedChart->difficultyName + "." + typeEnding;

		boost::filesystem::copy_file(aPath, imagePath, boost::filesystem::copy_option::overwrite_if_exists);

		mySelectedChart->backgroundFileName = mySelectedChart->difficultyName + "." + typeEnding;
		mySelectedChart->background.load(imagePath);

		PUSH_NOTIFICATION("Background changed to " + aPath);
	}
	else
	{
		PUSH_NOTIFICATION_COLORED("Invalid image file!", ofColor(255, 25, 25, 255));
	}
}

void ChartEditor::DoShiftAction(bool aShiftDown)
{
	myFreePlace = aShiftDown;
	myEditHandler.SetFreePlace(aShiftDown);
	myBPMLineHandler.SetPreciseBPMChange(aShiftDown);
}

void ChartEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Chart Set"))
			{
				myNewChartSetPopup = true; 

				if (myNewChartSet == nullptr)
					myNewChartSet = new ChartSet();	
			}

			if (ImGui::MenuItem("New Difficulty"))
			{
				if (mySelectedChartSet != nullptr)
				{
					myNewDifficultyPopup = true;
					
					if (myNewChart == nullptr)
						myNewChart = new ChartData();
				}
				else
				{
					PUSH_NOTIFICATION_COLORED("You need to create a new chartset first!", ofColor(255, 25, 25, 255));
				}
			}
			
			ImGui::Separator();

			if (ImGui::MenuItem("Open", "CTRL+O"))
				LoadChartFromDirectory();
		
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				TrySaveCurrentChart();
			}
			
			if (ImGui::MenuItem("Export", "CTRL+E"))
				ExportChart();

			if (ImGui::MenuItem("Open in osu!", "F5"))
				OpenChartWithOsu();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z"))
				DoUndo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "CTRL+C"))
				Copy();

			if (ImGui::MenuItem("Paste", "CTRL+V"))
				Paste();

			if (ImGui::MenuItem("Delete", "DELETE"))
				myEditHandler.TryDeleteSelectedItems();

			ImGui::Separator();

			if (ImGui::MenuItem("Mirror", "CTRL+H"))
				myEditHandler.TryFlipSelectedItemsHorizonally();
				
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::Checkbox("Use Pitched Rate", &mySongTimeHandler.usePitch))
				mySongTimeHandler.UpdateRateOption();

			if (ImGui::Checkbox("Show Column Lines", &myNoteHandler.showColumnLines));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Shortcuts", "F1"))
				myShowShortcuts = true;

			ImGui::EndMenu();
		}

		std::string selectedDifficulty = mySelectedChart == nullptr ? "* Difficulty: No Difficulty Selected" 
																	: "* Difficulty: " + mySelectedChart->difficultyName;	

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

	mySkinHandler.LoadSkin(mySelectedChart->keyAmount);
	EditorConfig::keyAmount = mySelectedChart->keyAmount;

	myEditHandler.ResetSelection();
	myBPMLineHandler.ClearAllCurrentBeatLines();

	myNoteHandler.Init(&(mySelectedChart->noteData));
	UndoRedoHandler::GetInstance()->Init(&(mySelectedChart->noteData), &myNoteHandler);
	myBPMLineHandler.Init(&(mySelectedChart->BPMPoints));
	mySongTimeHandler.Init(mySelectedChart->songPath);
	myEditHandler.Init(&myBPMLineHandler, &myNoteHandler, &(mySelectedChart->noteData));
	
	mySongTimeHandler.SetTimeNormalized(0.f);
	mySongTimeHandler.ResetSpeed();

	myMiniMap.Init(&myNoteHandler, &mySongTimeHandler);

	//if (mySelectedChart->BPMPoints.empty() == true)
		//myEditHandler.SetEditActionState(EditActionState::EditBPM);
}

void ChartEditor::DoNewChartSetWindow()
{
	if (myNewChartSetPopup == true)
	{
		ImGui::OpenPopup("New Chart Set");
	}
	else
	{
		return void();
	}

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	//windowFlags |= ImGuiWindowFlags_NoCollapse;
	//windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	bool open = true;
	if (ImGui::BeginPopupModal("New Chart Set", &open, windowFlags))
	{
		ImGui::Text("Meta Data");
		ImGui::InputText("Artist", &(myNewChartSet->artist));
		ImGui::InputText("Song Title", &(myNewChartSet->songTitle));
		ImGui::InputText("Charter", &(myNewChartSet->charter));

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();


		std::string audioButtonLabel = "Select Audio File";
		
		if (myNewChartSet->audioFilePath != "")
		{
			audioButtonLabel += " ( " + myNewChartSet->audioFileName + " )";
		}
		else
		{
			audioButtonLabel += " ( ... )";
		}

		if (ImGui::Button(audioButtonLabel.c_str()))
		{
			ofFileDialogResult result = ofSystemLoadDialog("Load File (.mp3, .wav, .ogg)");
			if (result.bSuccess) 
			{
				std::string path = result.getPath();
				myNewChartSet->audioFileName = result.getName();
				myNewChartSet->audioFilePath = result.getPath();

				myNewChartSet->audioFilePath.erase(path.size() - myNewChartSet->audioFileName.size(), path.size() - 1);
			}
		}

		std::string directoryLabel = "Choose Chart Folder";

		if (myNewChartSet->audioFilePath != "")
		{
			directoryLabel += " ( " + myNewChartSet->saveDirectory + " )";
		}
		else
		{
			directoryLabel += " ( ... )";
		}

		if (ImGui::Button(directoryLabel.c_str()))
		{
			ofFileDialogResult result = ofSystemLoadDialog("Load File (.mp3, .wav, .ogg)", true);

			if (result.bSuccess)
			{
				std::string path = result.getPath();

				myNewChartSet->saveDirectory = path;
			}
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Create"))
		{
			if (myNewChartSet->artist == "")
				PUSH_NOTIFICATION_COLORED("You need to specify the artist!", ofColor( 255, 25, 25, 255));
			else if (myNewChartSet->songTitle == "")
				PUSH_NOTIFICATION_COLORED("You need to specify the song title!", ofColor(255, 25, 25, 255));
			else if (myNewChartSet->charter == "")
				PUSH_NOTIFICATION_COLORED("You need to specify the charter!", ofColor(255, 25, 25, 255));
			else if (myNewChartSet->audioFileName == "")
				PUSH_NOTIFICATION_COLORED("You need to select an audio file!", ofColor(255, 25, 25, 255));
			else
			{
				myNewChartSetPopup = false;
				mySelectedChartSet = myNewChartSet;

				PUSH_NOTIFICATION("Chart Set Created!");

				myChartResourceHandler.RegisterChartSet(myNewChartSet);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			delete myNewChartSet;
			myNewChartSet = nullptr;

			myNewChartSetPopup = false;
		}

		ImGui::SetWindowPos({ ofGetWindowWidth() / 2.f - ImGui::GetWindowSize().x / 2.f, ofGetWindowHeight() / 2.f - ImGui::GetWindowSize().y / 2.f });
		ImGui::EndPopup();

	}
}

void ChartEditor::DoNewDifficultyWindow()
{
	if (myNewDifficultyPopup == true)
	{
		ImGui::OpenPopup("New Difficulty");
	}
	else
	{
		return void();
	}

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	//windowFlags |= ImGuiWindowFlags_NoCollapse;
	//windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;



	bool open = true;
	if (ImGui::BeginPopupModal("New Difficulty", &open, windowFlags))
	{
		ImGui::Text("Meta Data");
		ImGui::InputText("Difficulty Name", &(myNewChart->difficultyName));
		ImGui::SliderInt("Keymode", &myNewChart->keyAmount, 1, 16);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Difficulty Modifiers");

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Create"))
		{
			if (myNewChart->difficultyName == "")
			{
				PUSH_NOTIFICATION_COLORED("Please choose a difficulty name!", ofColor(255, 25, 25, 255));
			}
			else
			{
				myChartResourceHandler.GenerateChartDifficulty(mySelectedChartSet, myNewChart);

				myNewDifficultyPopup = false;

				mySelectedChartSet->charts.push_back(myNewChart);

				if (mySelectedChart != nullptr)
				{
					for (auto bpmPoint : mySelectedChart->BPMPoints)
					{
						BPMData* bpmData = new BPMData(*bpmPoint);
						myNewChart->BPMPoints.push_back(bpmData);
					}

					myNewChart->backgroundFileName = mySelectedChart->backgroundFileName;
					myNewChart->background.load(mySelectedChartSet->saveDirectory + "\\" + mySelectedChart->backgroundFileName);
				}

				SetSelectedChart(myNewChart);

				myNewChart = nullptr;
				
				PUSH_NOTIFICATION("Chart Set Created!");
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			delete myNewChart;
			myNewChart = nullptr;

			myNewDifficultyPopup = false;
		}

		ImGui::SetWindowPos({ ofGetWindowWidth() / 2.f - ImGui::GetWindowSize().x / 2.f, ofGetWindowHeight() / 2.f - ImGui::GetWindowSize().y / 2.f });
		ImGui::EndPopup();
	}
}

void ChartEditor::DoShortcutsWindow()
{
	if (myShowShortcuts == true)
	{
		ImGui::OpenPopup("Shortcuts");
	}
	else
	{
		return void();
	}

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	//windowFlags |= ImGuiWindowFlags_NoCollapse;
	//windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::SetNextWindowSize({ 512, 192 });

	bool open = true;
	if (ImGui::BeginPopupModal("Shortcuts", &open, windowFlags))
	{
		ImGui::BeginColumns("Shortcut", 2);

		ImGui::Text("Edit Mode");
		ImGui::Text("Note Mode");
		ImGui::Text("Hold Mode");
		ImGui::Text("Timepoint Mode");

		ImGui::Text("Scroll Through Time");
		ImGui::Text("Playback Speed");
		ImGui::Text("Zoom");
		ImGui::Text("Change Snap");
		ImGui::Text("Freeplace");
		
		ImGui::NextColumn();

		ImGui::Text("1");
		ImGui::Text("2");
		ImGui::Text("3");
		ImGui::Text("4");

		ImGui::Text("SCROLL");
		ImGui::Text("SHIFT+SCROLL");
		ImGui::Text("CTRL+SCROLL");
		ImGui::Text("ALT+SCROLL");
		ImGui::Text("SHIFT");

		ImGui::EndColumns();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Close"))
			myShowShortcuts = false;

		ImGui::SetWindowPos({ ofGetWindowWidth() / 2.f - ImGui::GetWindowSize().x / 2.f, ofGetWindowHeight() / 2.f - ImGui::GetWindowSize().y / 2.f });
		ImGui::EndPopup();
	}
}

void ChartEditor::AdjustFieldPosition()
{
	EditorConfig::leftSidePosition = ofGetWindowWidth() / 2.f + EditorConfig::leftSidePositionFromCenter;
}

void ChartEditor::ExportChart()
{
	if (mySelectedChart == nullptr)
		return void();

	std::string command = "7za.exe a -tzip \"";
	command += mySelectedChartSet->saveDirectory + "\\" + mySelectedChartSet->artist + " - " + mySelectedChartSet->songTitle + ".osz\" ";
	command += "-spf \"" + mySelectedChartSet->saveDirectory + "\\" + '\"';

	ofSystem(command);

	std::string message = "Exported Chart To ";
	message += mySelectedChartSet->saveDirectory + "\\ exported.osz";

	PUSH_NOTIFICATION(message);
}

void ChartEditor::OpenChartWithOsu()
{
	if (mySelectedChart == nullptr)
		return void();

	std::string command = "7za.exe a -tzip ";
	command += "temp.osz ";
	command += "-spf \"" + mySelectedChartSet->saveDirectory + "\\" + '\"';

	ofSystem(command);
	ofSystem("start temp.osz");

	PUSH_NOTIFICATION("Opening osu! ...");
}

bool ChartEditor::IsCursorWithinBounds(int aX, int aY)
{
	int leftBorder = EditorConfig::leftSidePosition - EditorConfig::sideSpace;
	int rightBorder = EditorConfig::leftSidePosition + EditorConfig::fieldWidth + EditorConfig::sideSpace;

	bool withinBounds = aX >= leftBorder && aX <= rightBorder;

	return withinBounds;
}

int ChartEditor::GetScreenTimePoint(float aY)
{
	if (myFreePlace == true)
	{
		int y = ofGetWindowHeight() - aY;

		y -= (EditorConfig::hitLinePosition);
		y /= EditorConfig::scale;

		int timePointMS = y + (mySongTimeHandler.GetCurrentTimeS() * 1000.0);

		return timePointMS;
	}
	else
	{
		return myBPMLineHandler.GetClosestTimePoint(myEditHandler.GetSnappedCursorPosition().y);
	}
}
