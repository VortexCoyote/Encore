#include "ofApp.h"

#include "BASS\bass.h"
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
}

//--------------------------------------------------------------
void ofApp::update() {

	myGui.begin();
	
	myChartEditor.Update();

	myChartEditor.SetMousePosition(mouseX, mouseY);
}

//--------------------------------------------------------------
void ofApp::draw() {

	myChartEditor.Draw();

	myGui.end();
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

	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.DoLeftClickDragAction(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.DoRightClickDragAction(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.DoLeftClickPressedAction(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.DoRightClickPressedAction(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

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
	int key = keyEvent.keycode;

#define KEY_PLUS 45
#define KEY_MINUS 47

	if (key == GLFW_KEY_SPACE)
		myChartEditor.TogglePlaying();


	if (keyEvent.hasModifier(OF_KEY_CONTROL))
	{

		if (key == KEY_PLUS)
			myChartEditor.IncreaseSnapDivision();

		if (key == KEY_MINUS)
			myChartEditor.DecreaseSnapDivision();
	}
	else
	{

		if (key == KEY_PLUS)
			myChartEditor.ZoomIn();

		if (key == KEY_MINUS)
			myChartEditor.ZoomOut();
	}


	if (key == GLFW_KEY_RIGHT)
		myChartEditor.IncreaseSpeed();

	if (key == GLFW_KEY_LEFT)
		myChartEditor.DecreaseSpeed();

	if (key == GLFW_KEY_UP)
		myChartEditor.MoveUp();

	if (key == GLFW_KEY_DOWN)
		myChartEditor.MoveDown();


	if (key == GLFW_KEY_1)
		myChartEditor.SetEditMode(0);

	if (key == GLFW_KEY_2)
		myChartEditor.SetEditMode(1);

	if (key == GLFW_KEY_3)
		myChartEditor.SetEditMode(2);

}

void ofApp::keyReleased(ofKeyEventArgs& keyEvent)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
