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
	int BPMSaved = -1;
	float beatLength = 0.f;
	int meter = -1;
	int uninherited = -1;

	bool pinControl = false;
};

struct NoteData : public TimeFieldObject
{
	int column = 0;	

	NoteType noteType = NoteType::Note;

	NoteData* relevantNote = nullptr;

	bool selected = false;

	int x = 0;
	int y = 0;

	int type = -1;
	int hitSound = 0;
};

struct ChartData
{
	std::vector<NoteData*> noteData;
	
	int songLength;
	
	std::string song = "";
	std::string audioFileName = "";
	std::string backgroundFileName = "";
	std::string filePath = "";

	ofImage background;

	std::string difficultyName;

	std::vector<BPMData*> BPMPoints;
};

struct ChartSet
{
	std::vector<ChartData*> charts;

	std::string artist = "";
	std::string songTitle = "";
	std::string charter = "";

	std::string audioFileName = "";
	std::string audioFilePath = "";
};