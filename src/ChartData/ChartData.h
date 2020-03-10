#pragma once

#include "ofSoundPlayer.h"
#include "ofImage.h"

#include <vector>

enum class NoteType
{
	Note,
	HoldBegin,
	HoldEnd,

	Count
};


struct TimeFieldObject
{
	int timePoint = 0;
	int cullTimeOffset = -1;
	float visibleTimePoint = -1.f;
};

struct BPMData : public TimeFieldObject
{
	float BPM = 0.f;
};

struct NoteData : public TimeFieldObject
{
	int column = 0;	

	NoteType noteType = NoteType::Note;

	NoteData* relevantNote = nullptr;

	bool selected = false;

	int x = 0;
	int y = 0;
};

struct ChartData
{
	std::vector<NoteData*> noteData;
	
	int songLength;
	
	std::string song;
	ofImage background;

	std::string difficultyName;

	std::vector<BPMData*> BPMPoints;
};

struct ChartSet
{
	std::vector<ChartData*> charts;

	std::string artist;
	std::string songName;
	std::string charter;
};