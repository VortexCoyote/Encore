#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

#include "ChartEditor/ChartEditor.h"



class ofApp : public ofBaseApp
{
public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void mouseScrolled(int x, int y, float scrollX, float scrollY);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	virtual void keyPressed(ofKeyEventArgs& keyEvent) override;
	virtual void keyReleased(ofKeyEventArgs& keyEvent) override;

private:
	
	ofxImGui::Gui myGui;

	bool myControlKey = false;
	bool myAltKey = false;
	bool myShiftKey = false;

	ChartEditor myChartEditor;
};
