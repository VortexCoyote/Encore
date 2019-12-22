#include "NoteHandler.h"

#include "ofWindowSettings.h"
#include "ofMain.h"


NoteHandler::NoteHandler()
{
	myNoteImage[0].loadImage("images/arrows/left.png");
	myNoteImage[1].loadImage("images/arrows/down.png");
	myNoteImage[2].loadImage("images/arrows/up.png");
	myNoteImage[3].loadImage("images/arrows/right.png");
	
	myHoldCapImage.loadImage("images/holdcap.png");
	myHoldBodyImage.loadImage("images/holdbody.png");
	
	mySelectedImage.loadImage("images/selected.png");
	
	myHitLineImage.loadImage("images/hitline.png");

	myVisibleObjects.reserve(100000);

	myPreviewBuffer.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);
}

NoteHandler::~NoteHandler()
{

}

void NoteHandler::Init(std::vector<SelectableItem>* aObjectData)
{
	TimeFieldHandlerBase<SelectableItem>::Init(aObjectData);

	myLastLNIndex = 0;
	myLastLNTimePoint = 0.0;
}

std::vector<SelectableItem*>& NoteHandler::GetVisibleNotes()
{
	return myVisibleObjects;
}

void NoteHandler::ClearLNs()
{
	myLNs.clear();
}

void NoteHandler::AddLN(SelectableItem aLN)
{
	myLNs.push_back(aLN);
}

void NoteHandler::DrawRoutine(SelectableItem* aTimeObject, float aTimePoint)
{
	int column = aTimeObject->noteData->column;

	aTimeObject->x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
	aTimeObject->y = ofGetWindowHeight() - aTimePoint;

	myNoteImage[column].draw(aTimeObject->x, aTimeObject->y);

	if (aTimeObject->selected)
		mySelectedImage.draw(aTimeObject->x, aTimeObject->y);
}

void NoteHandler::Draw(double aTimePoint)
{
	if (myObjectData == nullptr)
		return void();

	//handle LN rewinds
	if (aTimePoint < myLastLNTimePoint)
	{
		for (unsigned int rewindLNIndex = myLastLNIndex; rewindLNIndex >= 0; rewindLNIndex--)
		{
			float LNTimePointEnd = GetScreenTimePoint(myLNs[rewindLNIndex].noteData->timePointEnd, aTimePoint);

			if (LNTimePointEnd < 0 || rewindLNIndex == 0)
			{
				myLastLNIndex = rewindLNIndex;
				break;
			}
		}
	}

	//Handle LNs 
	for (unsigned int LNIndex = myLastLNIndex; LNIndex < myLNs.size(); LNIndex++)
	{
		float LNTimePoint = GetScreenTimePoint(myLNs[LNIndex].timePoint,				 aTimePoint);
		float LNTimePointEnd = GetScreenTimePoint(myLNs[LNIndex].noteData->timePointEnd, aTimePoint);

		//render all visible LNs
		if (LNTimePoint >= 0 || LNTimePointEnd >= 0)
		{
			myLastLNIndex = LNIndex;

			for (unsigned int visibleLNIndex = LNIndex; visibleLNIndex < myLNs.size(); visibleLNIndex++)
			{
				float visibleLNTimePoint = GetScreenTimePoint(myLNs[visibleLNIndex].timePoint,				    aTimePoint);
				float visibleLNTimePointEnd = GetScreenTimePoint(myLNs[visibleLNIndex].noteData->timePointEnd,  aTimePoint);

				if (visibleLNTimePoint <= ofGetScreenHeight() || visibleLNTimePointEnd <= ofGetScreenHeight())
				{
					int column = myLNs[visibleLNIndex].noteData->column;

					myLNs[visibleLNIndex].x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
					myLNs[visibleLNIndex].y = ofGetWindowHeight() - visibleLNTimePoint;

					myHoldBodyImage.draw(myLNs[visibleLNIndex].x, myLNs[visibleLNIndex].y + myNoteImage[0].getHeight() / 2.f, myHoldBodyImage.getWidth(), visibleLNTimePoint - visibleLNTimePointEnd);
					myHoldCapImage.draw(myLNs[visibleLNIndex].x, myLNs[visibleLNIndex].y + myNoteImage[0].getHeight() / 2.f + visibleLNTimePoint - visibleLNTimePointEnd - myHoldCapImage.getHeight());
				}
				else
				{
					break;
				}
			}
			break;
		}
	}


	TimeFieldHandlerBase<SelectableItem>::Draw(aTimePoint);
	myHitLineImage.draw(ofGetWindowWidth() / 2 - myNoteImage[0].getWidth() * 2, ofGetWindowHeight() - EditorConfig::hitLinePosition /*+ myNoteImage[0].getHeight()*/);

	myLastLNTimePoint = aTimePoint;
}


void NoteHandler::DrawPreviewBox(double aTimePoint, float aMouseY)
{
/*	myPreviewBuffer.begin();
	
	ofClear(0, 0, 0, 0);

	int   oldNoteIndex = myLastObjectIndex;
	float oldTimePoint = myLastTimePoint;
	std::vector<SelectableItem*> oldVisibleObjects = myVisibleObjects;

	TimeFieldHandlerBase<SelectableItem>::Draw(aTimePoint);

	myLastObjectIndex = oldNoteIndex;
	myLastTimePoint = oldTimePoint;
	myVisibleObjects = oldVisibleObjects;

	myPreviewBuffer.end();

	int width  = ofGetWindowWidth()  / 2.8;
	int height = ofGetWindowHeight() / 2.8;
	
	myPreviewBuffer.draw({ ofGetWindowWidth() - width + 64, aMouseY - height / 2}, width, height);*/
}

void NoteHandler::DrawNoteFieldBackground()
{
	ofSetColor(0, 0, 0, 216);
	ofDrawRectangle(ofGetWindowWidth() / 2 - myNoteImage[0].getWidth() * 2, 0.f, 1.f, myNoteImage[0].getWidth() * 4, ofGetWindowHeight());
	ofSetColor(255, 255, 255, 255);
}
