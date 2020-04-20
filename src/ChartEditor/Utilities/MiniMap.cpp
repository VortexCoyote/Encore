#include "MiniMap.h"

#include "imgui.h"
#include "ofMain.h"

#include "../SongHandler.h"
#include "../TimeFieldHandlers/NoteHandler.h"

MiniMap::MiniMap()
{

}

MiniMap::~MiniMap()
{

}

void MiniMap::Init(NoteHandler* aNoteHandler, SongHandler* aSongHandler)
{
	myNoteHandler = aNoteHandler;
	mySongHandler = aSongHandler;

	GenerateMiniMap();
}

void MiniMap::Update()
{
	//Old timeline dragger, left for debug purposes 
/*	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;

	bool open = true;
	ImGui::Begin("TL", &open, windowFlags);

	ImGui::SetWindowSize({ 0, float(ofGetWindowHeight()) - 16 });

	ImGui::SetWindowPos({ float(ofGetWindowWidth() - 32.f - 16.f - 4.f),  20 });

	if (ImGui::VSliderFloat("", { 32.f,  float(ofGetWindowHeight() - 40) }, &myTimeLine, 0.0f, 1.0f, ""))
	{
		mySongHandler->SetTimeNormalized(myTimeLine);
	}
	else
	{
		myTimeLine = mySongHandler->GetCurrentTimeS() / mySongHandler->GetSongLength();
	}

	ImGui::End();*/

	if (myNoteHandler->GetObjectData()->size() != mySavedCollectionSize)
		GenerateMiniMap();

	myDimensions = { myBufferWidth, float(ofGetWindowHeight()) - 40 };
	myPosition = { EditorConfig::leftSidePosition - EditorConfig::sideSpace - myBufferWidth - EditorConfig::MiniMap::spacingFromLeftSide, 28 };

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (ImGui::GetMousePos().x >= myPosition.x && ImGui::GetMousePos().y >= myPosition.y
		&&  ImGui::GetMousePos().x <= myPosition.x + myDimensions.x && ImGui::GetMousePos().y <= myPosition.y + myDimensions.y)
		{
			myHasClicked = true;
		}
	}

	if (myHasClicked == true)
	{
		mySongHandler->SetTimeNormalized(((myDimensions.y - ImGui::GetMousePos().y + myPosition.y)) / myDimensions.y);

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			myHasClicked = false;
	}
}

void MiniMap::Draw()
{
	float borderSize = 2;
	ofSetColor(255, 255, 255, 255);
	ofDrawRectangle(myPosition - ofVec2f(borderSize), myDimensions.x + borderSize * 2, myDimensions.y + borderSize * 2);

	ofSetColor(0, 0, 0, 255);
	ofDrawRectangle(myPosition, myDimensions.x, myDimensions.y);

	float songPixelLength = mySongHandler->GetSongLength() * 1000.0;

	float percentualDeltaMiniMap = myBufferHeight / myDimensions.y;

	float viewHeight = float(ofGetHeight()) * myMiniMapScale;

	float timeY = myPosition.y + myDimensions.y - (myDimensions.y * mySongHandler->GetTimeNormalized());
	float timeMiniMapY = timeY * percentualDeltaMiniMap;

	float visibleHeight = (viewHeight * (1.f / EditorConfig::scale));
	float hitlineOffset = EditorConfig::hitLinePosition * myMiniMapScale * (1.f / EditorConfig::scale);

	ofVec2f miniMapPos = ofVec2f(myPosition.x, myPosition.y * percentualDeltaMiniMap + timeY - timeMiniMapY);

	ofSetColor(255, 255, 255, 255);
	myMiniMapBuffer.getTexture().drawSubsection(myPosition.x, myPosition.y, myDimensions.x, myDimensions.y, 0, timeMiniMapY - timeY - (myPosition.y * percentualDeltaMiniMap -myPosition.y));

	ofSetColor(255, 255, 255, 128);
	ofDrawRectangle(myPosition.x, timeY - (visibleHeight - hitlineOffset), myDimensions.x, visibleHeight);

	ofSetColor(255, 255, 255, 255);
	ofDrawLine(myPosition.x, timeY, myPosition.x + myDimensions.x, timeY);
}

void MiniMap::GenerateMiniMap()
{
	myBufferHeight = mySongHandler->GetSongLength() * 1000.0 * myMiniMapScale;
	myBufferWidth = myNoteWidth * EditorConfig::keyAmount + EditorConfig::keyAmount * myNotePadding;

	myMiniMapBuffer.allocate(myBufferWidth, myBufferHeight);
	myMiniMapBuffer.begin();

	ofClear(255, 255, 255, 0);

	for (auto object : *(myNoteHandler->GetObjectData()))
	{

	switch (object->noteType)
		{
		case NoteType::HoldBegin:
			{
				ofSetColor(255, 25, 96, 255);

				float x = myNoteWidth * object->column + object->column * myNotePadding + myNotePadding * 0.5f;
				float y = myBufferHeight - object->timePoint * myMiniMapScale;
				float height = (myBufferHeight - object->relevantNote->timePoint * myMiniMapScale) - y;

				ofDrawRectangle(x, y, myNoteWidth, height);
			}
			break;

		case NoteType::Note:
		{
			ofSetColor(25, 255, 96, 255);

			float x = myNoteWidth * object->column + object->column * myNotePadding + myNotePadding * 0.5f;
			float y = myBufferHeight - object->timePoint * myMiniMapScale;

			ofDrawRectangle(x, y - myNoteHeight, myNoteWidth, myNoteHeight);
		}
			break;
		default:
			break;
		}


	}

	ofSetColor(255, 255, 255, 255);

	myMiniMapBuffer.end();

	mySavedCollectionSize = myNoteHandler->GetObjectData()->size();
}
