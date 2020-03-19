#pragma once

#include "../../ChartData/ChartData.h"
#include "../EditorConfig.h"


#include "ofMain.h"

template<class T = int>
class TimeFieldHandlerBase
{
public:

	TimeFieldHandlerBase();
	~TimeFieldHandlerBase();

	virtual void Init(std::vector<T*>* aObjectData);
	virtual void Draw(double aTimePoint);

	double GetScreenTimePoint(int aTimePointMS, double aCurrentTimePointS);
	int GetTimeFromScreenPoint(float aScreenPoint, double aCurrentTimePointS);

protected:

	virtual void DrawRoutine(T* aTimeObject, float aTimePoint) = 0;

	unsigned int myLastObjectIndex = 0;
	double		 myLastTimePoint = 0.0;

	std::vector<T*>* myObjectData = nullptr;
	std::vector<T*> myVisibleObjects;
};

template<class T>
TimeFieldHandlerBase<T>::TimeFieldHandlerBase()
{

}

template<class T>
TimeFieldHandlerBase<T>::~TimeFieldHandlerBase()
{

}

template<class T>
inline void TimeFieldHandlerBase<T>::Init(std::vector<T*>* aObjectData)
{
	myObjectData = aObjectData;

	myLastObjectIndex = 0;
	myLastTimePoint = 0.0;
}

template<class T>
inline void TimeFieldHandlerBase<T>::Draw(double aTimePoint)
{
	//handle rewinds
	if (aTimePoint < myLastTimePoint)
	{
		//std::cout << "yaaah, it's rewind time" << std::endl;

		for (unsigned int rewindNoteIndex = myLastObjectIndex; rewindNoteIndex >= 0 && myObjectData->size() > 0; rewindNoteIndex--)
		{
			double rewindNoteTimePoint = GetScreenTimePoint((*myObjectData)[rewindNoteIndex]->timePoint, aTimePoint);

			if (rewindNoteTimePoint < 0 || rewindNoteIndex == 0)
			{
				myLastObjectIndex = rewindNoteIndex;
				break;
			}
		}
	}

	for (unsigned int noteIndex = myLastObjectIndex; noteIndex < myObjectData->size(); noteIndex++)
	{
		double noteTimePoint = GetScreenTimePoint((*myObjectData)[noteIndex]->timePoint, aTimePoint);

		//render all visible notesb
		if (noteTimePoint >= 0)
		{
			myLastObjectIndex = noteIndex;
			myVisibleObjects.clear();

			if (noteIndex > 0)
			{
				T* itemBack = (*myObjectData)[noteIndex - 1];
				myVisibleObjects.push_back(itemBack);
			}

			for (unsigned int visibleNoteIndex = noteIndex; visibleNoteIndex < myObjectData->size(); visibleNoteIndex++)
			{
				double visibleNoteTimePoint = GetScreenTimePoint((*myObjectData)[visibleNoteIndex]->timePoint, aTimePoint);

				if (visibleNoteTimePoint <= ofGetScreenHeight())
				{
					T* item = (*myObjectData)[visibleNoteIndex];

					item->visibleTimePoint = visibleNoteTimePoint;
					DrawRoutine(item, visibleNoteTimePoint);
					
					myVisibleObjects.push_back(item);
				}
				else
				{
					T* itemFront = (*myObjectData)[visibleNoteIndex];
					myVisibleObjects.push_back(itemFront);

					break;
				}
			}
			break;
		}
	}

	myLastTimePoint = aTimePoint;
}

template<class T>
double TimeFieldHandlerBase<T>::GetScreenTimePoint(int aTimePointMS, double aCurrentTimePointS)
{
	double timePoint = double(aTimePointMS) - (aCurrentTimePointS * 1000.0);

	timePoint *= EditorConfig::scale;
	timePoint += (EditorConfig::hitLinePosition);

	timePoint += 64.f;

	return timePoint; 
}

template<class T>
inline int TimeFieldHandlerBase<T>::GetTimeFromScreenPoint(float aScreenPoint, double aCurrentTimePointS)
{
	aScreenPoint -= 64.f;

	aScreenPoint -= (EditorConfig::hitLinePosition);
	aScreenPoint /= EditorConfig::scale;

	int timePointMS = aScreenPoint + (aCurrentTimePointS * 1000.0);

	return timePointMS;
}
