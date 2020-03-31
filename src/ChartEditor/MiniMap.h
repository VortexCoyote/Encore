#pragma once

#include <vector>

#include "ofFbo.h"
#include "ofVec2f.h"
#include "ofRectangle.h"

class SongHandler;
class NoteHandler;
class MiniMap
{
public:
	
	MiniMap();
	~MiniMap();

	void Init(NoteHandler* aNoteHandler, SongHandler* aSongHandler);

	void Update();
	void Draw();

private:

	void GenerateMiniMap();

	NoteHandler* myNoteHandler;
	SongHandler* mySongHandler;

	ofFbo myMiniMapBuffer;

	bool myHasClicked = false;

	float myTimeLine = 0.f;
	
	ofVec2f myPosition;
	ofVec2f myDimensions;

	const float myNoteWidth = 6.f;
	const float myNoteHeight = 2.f;

	const float myNotePadding = 2.f;
	const float myMiniMapScale = 0.0185f;

	float myBufferHeight = 0.f;
	float myBufferWidth = 0.f;

	int mySavedCollectionSize;
};