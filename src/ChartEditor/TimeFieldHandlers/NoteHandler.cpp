#include "NoteHandler.h"

#include "ofWindowSettings.h"
#include "ofMain.h"

#include <string>

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

	myVisibleHolds.clear();
	myVisibleHoldsToRemove.clear();
}


std::vector<SelectableItem*>& NoteHandler::GetVisibleNotes()
{
	return myVisibleObjects;
}


void NoteHandler::DrawRoutine(SelectableItem* aTimeObject, float aTimePoint)
{
	int column = aTimeObject->noteData->column;

	aTimeObject->x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
	aTimeObject->y = ofGetWindowHeight() - aTimePoint;


	switch (aTimeObject->noteData->noteType)
	{
	case NoteType::HoldEnd:

		myHoldBodyImage.draw(aTimeObject->x, aTimeObject->y + myNoteImage[column].getHeight() / 2.f, myHoldBodyImage.getWidth(), abs(GetScreenTimePoint(aTimeObject->noteData->timePoint, 0) - GetScreenTimePoint(aTimeObject->noteData->relevantNote->timePoint, 0)));
		myHoldCapImage.draw(aTimeObject->x, aTimeObject->y - myHoldCapImage.getHeight() / 2.f);

		myNoteImage[column].draw(aTimeObject->x, aTimeObject->y + abs(GetScreenTimePoint(aTimeObject->noteData->timePoint, 0) - GetScreenTimePoint(aTimeObject->noteData->relevantNote->timePoint, 0)));


		break;

	case NoteType::HoldBegin:

		//myHoldBodyImage.draw(aTimeObject->x, aTimeObject->y + myNoteImage[column].getHeight() / 2.f, myHoldBodyImage.getWidth(), -abs(GetScreenTimePoint(aTimeObject->noteData->timePoint, 0) - GetScreenTimePoint(aTimeObject->noteData->relevantNote->timePoint, 0)));
		//myNoteImage[column].draw(aTimeObject->x, aTimeObject->y);

		myVisibleHolds[aTimeObject->noteData] = aTimeObject;

	case NoteType::Note:

		myNoteImage[column].draw(aTimeObject->x, aTimeObject->y);

		break;

	default:

		std::cout << "invalid notetype" << std::endl;
		
		break;
	}

	if (aTimeObject->selected)
		mySelectedImage.draw(aTimeObject->x, aTimeObject->y);
}

void NoteHandler::VisibleHoldDrawRoutine(double aTimePoint)
{
	for (auto& hold : myVisibleHolds)
	{
		float y = ofGetWindowHeight() - GetScreenTimePoint(hold.first->timePoint, aTimePoint);
		float yParent = ofGetWindowHeight() - GetScreenTimePoint(hold.first->relevantNote->timePoint, aTimePoint);

		if (yParent > ofGetWindowHeight() || y < 0)
		{
			myVisibleHoldsToRemove.push_back(myVisibleHolds.find(hold.first));
		}
		else
		{
			myHoldBodyImage.draw(hold.second->x, y + myNoteImage[hold.first->column].getHeight() / 2.f, myHoldBodyImage.getWidth(), -abs(GetScreenTimePoint(hold.second->noteData->timePoint, 0) - GetScreenTimePoint(hold.second->noteData->relevantNote->timePoint, 0)));
		}
	}

	for (auto& holdToRemove : myVisibleHoldsToRemove)
	{
		myVisibleHolds.erase(holdToRemove);
	}

	myVisibleHoldsToRemove.clear();
}

void NoteHandler::Draw(double aTimePoint)
{
	if (myObjectData == nullptr)
		return void();

	VisibleHoldDrawRoutine(aTimePoint);

	TimeFieldHandlerBase<SelectableItem>::Draw(aTimePoint);
	myHitLineImage.draw(ofGetWindowWidth() / 2 - myNoteImage[0].getWidth() * 2, ofGetWindowHeight() - EditorConfig::hitLinePosition /*+ myNoteImage[0].getHeight()*/ );
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
