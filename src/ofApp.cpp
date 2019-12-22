#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(1000);
	ofSetVerticalSync(false);
	ofSetBackgroundColor({0, 0, 0, 255});


	myGui.setup();
	myChartEditor.Load();
}

//--------------------------------------------------------------
void ofApp::update() {

	myGui.begin();

	myChartEditor.Update();
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
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

	myChartEditor.SetMousePosition(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	if (button == 0)
	{
		myChartEditor.TrySelectItem(x, y);
	}
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

	myChartEditor.RescaleBackground();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
