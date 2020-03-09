#include "ofApp.h"

#include "BASS\bass.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(500);
	ofSetVerticalSync(false);
	ofSetBackgroundColor({0, 0, 0, 255});

	myGui.setup();
	myChartEditor.Load();
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

	if (key == ' ')
		myChartEditor.TogglePlaying();

	if (key == '+')
		myChartEditor.ZoomIn();

	if (key == '-')
		myChartEditor.ZoomOut();

	if (key == OF_KEY_RIGHT)
		myChartEditor.IncreaseSpeed();

	if (key == OF_KEY_LEFT)
		myChartEditor.DecreaseSpeed();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	if (button == OF_MOUSE_BUTTON_LEFT)
		myChartEditor.TryPlaceNote(x, y);

	if (button == OF_MOUSE_BUTTON_RIGHT)
		myChartEditor.TryDeleteNote(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

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

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
