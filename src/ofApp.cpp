#include "ofApp.h"

#include "BASS/bass.h"
#include "GLFW/glfw3.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(500);
	ofSetVerticalSync(false);
	ofSetBackgroundColor({0, 0, 0, 255});

	myGui.setup();
	myChartEditor.Load();

	ImGui::GetIO().IniFilename = nullptr;

	ofSetEscapeQuitsApp(false);
}

//--------------------------------------------------------------
void ofApp::update() {

	myGui.begin();
	
	myChartEditor.Update();

	if (myChartEditor.ShouldBlockInput() == false)
		myChartEditor.SetMousePosition(mouseX, mouseY);
}

//--------------------------------------------------------------
void ofApp::draw() {

	myChartEditor.Draw();

	myGui.end();

	NotificationSystem::GetInstance()->Draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (myChartEditor.ShouldBlockInput() == true)
		return void();

	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.DoLeftClickDragAction(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.DoRightClickDragAction(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	if (ImGui::GetIO().WantCaptureMouse == true)
		return void();

	if (myChartEditor.ShouldBlockInput() == true)
		return void();

	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.DoLeftClickPressedAction(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.DoRightClickPressedAction(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

	if (myChartEditor.ShouldBlockInput() == true)
		return void();

	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.DoLeftClickReleaseAction(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.DoRightClickReleaseAction(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	
	if (myChartEditor.ShouldBlockInput() == true)
		return void();

	if (myControlKey == true)
	{
		if (scrollY < 0)
			myChartEditor.ZoomOut();

		if (scrollY > 0)
			myChartEditor.ZoomIn();

		return void();
	}

	if (myAltKey == true)
	{
		if (scrollY < 0)
			myChartEditor.DecreaseSnapDivision();

		if (scrollY > 0)
			myChartEditor.IncreaseSnapDivision();

		return void();
	}

	if (myShiftKey == true)
	{
		if (scrollY < 0)
			myChartEditor.DecreaseSpeed();

		if (scrollY > 0)
			myChartEditor.IncreaseSpeed();

		return void();
	}


	if (scrollY < 0)
		myChartEditor.ScrollUp();

	if (scrollY > 0)
		myChartEditor.ScrollDown();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {


}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

void ofApp::keyPressed(ofKeyEventArgs& keyEvent)
{
	if (myChartEditor.ShouldBlockInput() == true)
		return void();

	int key = keyEvent.keycode;

#define KEY_PLUS 45
#define KEY_MINUS 47

	if (key == GLFW_KEY_SPACE)
		myChartEditor.TogglePlaying();

	if (keyEvent.hasModifier(OF_KEY_ALT))
		myAltKey = true;

	if (keyEvent.hasModifier(OF_KEY_SHIFT))
	{
		myChartEditor.DoShiftAction(true);
		myShiftKey = true;
	}

	if (keyEvent.hasModifier(OF_KEY_CONTROL))
	{
		myControlKey = true;

		if (key == KEY_PLUS)
			myChartEditor.IncreaseSnapDivision();

		if (key == KEY_MINUS)
			myChartEditor.DecreaseSnapDivision();

		if (key == GLFW_KEY_S)
			myChartEditor.TrySaveCurrentChart();

		if (key == GLFW_KEY_Z)
			myChartEditor.DoUndo();

		if (key == GLFW_KEY_H)
			myChartEditor.TryFlipSelected();

		if (key == GLFW_KEY_A)
			myChartEditor.TrySelectAll();

		if (key == GLFW_KEY_C)
			myChartEditor.Copy();

		if (key == GLFW_KEY_V)
			myChartEditor.Paste();
	}
	else
	{
		if (key == KEY_PLUS)
			myChartEditor.ZoomIn();

		if (key == KEY_MINUS)
			myChartEditor.ZoomOut();
	}

	if (key == GLFW_KEY_DELETE)
		myChartEditor.TryDeleteSelected();


	if (key == GLFW_KEY_RIGHT)
		myChartEditor.IncreaseSpeed();

	if (key == GLFW_KEY_LEFT)
		myChartEditor.DecreaseSpeed();

	if (key == GLFW_KEY_UP)
		myChartEditor.MoveUp();

	if (key == GLFW_KEY_DOWN)
		myChartEditor.MoveDown();  


	if (key == GLFW_KEY_1)
		myChartEditor.SetEditMode(EditActionState::Select);

	if (key == GLFW_KEY_2)
		myChartEditor.SetEditMode(EditActionState::EditNotes);

	if (key == GLFW_KEY_3)
		myChartEditor.SetEditMode(EditActionState::EditHolds);

	if (key == GLFW_KEY_4)
		myChartEditor.SetEditMode(EditActionState::EditBPM);
}

void ofApp::keyReleased(ofKeyEventArgs& keyEvent)
{
	if (keyEvent.key == OF_KEY_CONTROL)
		myControlKey = false;
	

	if (keyEvent.key == OF_KEY_ALT)
		myAltKey = false;

	if (keyEvent.key == OF_KEY_SHIFT)
	{
		myChartEditor.DoShiftAction(false);
		myShiftKey = false;
	}

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

	myChartEditor.TryDropFilesAction(dragInfo.files[0]);
}