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

protected:

	virtual void DrawRoutine(T* aTimeObject, float aTimePoint) = 0;

	float GetScreenTimePoint(int aTimePointMS, double aCurrentTimePointS);
	int GetTimeFromScreenPoint(float aScreenPoint, double aCurrentTimePointS);

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

		for (unsigned int rewindNoteIndex = myLastObjectIndex; rewindNoteIndex >= 0; rewindNoteIndex--)
		{
			float rewindNoteTimePoint = GetScreenTimePoint((*myObjectData)[rewindNoteIndex]->timePoint, aTimePoint);

			if (rewindNoteTimePoint < 0 || rewindNoteIndex == 0)
			{
				myLastObjectIndex = rewindNoteIndex;
				break;
			}
		}
	}

	for (unsigned int noteIndex = myLastObjectIndex; noteIndex < myObjectData->size(); noteIndex++)
	{
		float noteTimePoint = GetScreenTimePoint((*myObjectData)[noteIndex]->timePoint, aTimePoint);

		//render all visible notes
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
				float visibleNoteTimePoint = GetScreenTimePoint((*myObjectData)[visibleNoteIndex]->timePoint, aTimePoint);				

				if (visibleNoteTimePoint <= ofGetScreenHeight())
				{
					T* item = (*myObjectData)[visibleNoteIndex];

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
float TimeFieldHandlerBase<T>::GetScreenTimePoint(int aTimePointMS, double aCurrentTimePointS)
{
	float timePoint = float(aTimePointMS) - (aCurrentTimePointS * 1000.f);

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

	int timePointMS = aScreenPoint + (aCurrentTimePointS * 1000.f);

	return timePointMS;
}
