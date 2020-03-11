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

void NoteHandler::Init(std::vector<NoteData*>* aObjectData)
{
	TimeFieldHandlerBase<NoteData>::Init(aObjectData);

	myVisibleHolds.clear();
	myVisibleHoldsToRemove.clear();
}


std::vector<NoteData*>& NoteHandler::GetVisibleNotes()
{
	return myVisibleObjects;
}

NoteData* NoteHandler::GetHoveredNote(int aX, int aY)
{
	auto& visibleNotes = GetVisibleNotes();

	for (auto note : visibleNotes)
	{
		switch (note->noteType)
		{
		case NoteType::HoldBegin:
		{
			int column = note->column;
			float x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
			float y = ofGetWindowHeight() - note->visibleTimePoint;

			if (aX > x && aX < x + 64 &&
				aY > y && aY < y + 64)
			{
				return note;
			}
		}
			break;

		case NoteType::HoldEnd:
		case NoteType::Note:

			if (aX > note->x && aX < note->x + 64 &&
				aY > note->y && aY < note->y + 64)
			{
				return note;
			}
		
			break;

		default:

			std::cout << "invalid notetype" << std::endl;

			break;
		}
	}

	return nullptr;
}

void NoteHandler::RemoveVisibleHold(NoteData* aNote)
{
	auto note = myVisibleHolds.find(aNote);

	if (note != myVisibleHolds.end())
		myVisibleHolds.erase(note);
}


void NoteHandler::DrawRoutine(NoteData* aTimeObject, float aTimePoint)
{
	int column = aTimeObject->column;

	aTimeObject->x = ofGetWindowWidth() / 2 - myNoteImage[column].getWidth() * 2 + myNoteImage[column].getWidth() * column;
	aTimeObject->y = ofGetWindowHeight() - int(aTimePoint + 0.5f);

	switch (aTimeObject->noteType)
	{
	case NoteType::HoldEnd:

		aTimeObject->relevantNote->x = aTimeObject->x;	
		aTimeObject->relevantNote->y = aTimeObject->y;	

		myVisibleHolds[aTimeObject->relevantNote] = aTimeObject->relevantNote;

		break;

	case NoteType::HoldBegin:

		myVisibleHolds[aTimeObject] = aTimeObject;

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

		myHoldCapImage.draw(hold.first->x, yParent - myHoldCapImage.getHeight() / 2.f);
		
		myHoldBodyImage.draw(hold.second->x, y + myNoteImage[hold.first->column].getHeight() / 2.f, myHoldBodyImage.getWidth(), (GetScreenTimePoint(hold.second->timePoint, 0) - GetScreenTimePoint(hold.second->relevantNote->timePoint, 0)));
		
		if (yParent > ofGetWindowHeight() || y < 0)
		{
			myVisibleHoldsToRemove.push_back(myVisibleHolds.find(hold.first));
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

	TimeFieldHandlerBase<NoteData>::Draw(aTimePoint);
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
