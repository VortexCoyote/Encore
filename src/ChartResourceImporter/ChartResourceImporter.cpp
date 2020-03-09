#include "ChartResourceImporter.h"
#include "ofFileUtils.h"
#include "ofWindowSettings.h"
#include "ofMain.h"


#include <filesystem>
#include <sstream>
#include <algorithm>

ChartResourceImporter::ChartResourceImporter()
{

}

ChartResourceImporter::~ChartResourceImporter()
{
	for (auto& entry : myCharts)
		for (auto& chart : entry.second.charts)
			delete chart;
}

ChartSet* ChartResourceImporter::ImportChart(std::string aPath)
{
	if (myCharts.find(aPath) != myCharts.end())
		return &myCharts[aPath];

	ofDirectory directory(aPath);

	directory.allowExt("osu");
	directory.listDir();

	//loop through all chart files
	for (auto i = 0; i < directory.size(); i++)
	{
		std::ifstream chartFile(directory.getPath(i));

		myCharts[aPath].charts.push_back(new ChartData());
		ChartData* chartData = myCharts[aPath].charts.back();

		chartData->noteData.reserve(100000);

		std::string line;
		while (std::getline(chartFile, line))
		{
			if (line == "[General]")
			{
				std::string metadataLine;

				while (std::getline(chartFile, metadataLine))
				{
					if (metadataLine.empty())
						break;

					std::string meta;
					std::string value;

					int pointer = 0;

					while (metadataLine[pointer] != ':')
						meta += metadataLine[pointer++];

					pointer++;
					pointer++;

					while (metadataLine[pointer] != '\0')
						value += metadataLine[pointer++];

					if (meta == "AudioFilename")
					{
						std::string songPath = aPath + "\\" + value;
						chartData->song = songPath;
					}
				}
			}

			if (line == "[Metadata]")
			{
				std::string metadataLine;

				while(std::getline(chartFile, metadataLine))
				{
					if (metadataLine.empty()) 
						break;	
										
					std::string meta;
					std::string value;

					int pointer = 0;

					while (metadataLine[pointer] != ':')
						meta += metadataLine[pointer++];
					
					pointer++;

					while (metadataLine[pointer] != '\0')
						value += metadataLine[pointer++];
					
					if (meta == "Title")
						myCharts[aPath].songName = value;
					
					if (meta == "Artist")
						myCharts[aPath].artist = value;
					
					if (meta == "Version")
						chartData->difficultyName = value;
					
					if (meta == "Creator")
						myCharts[aPath].charter = value;
				}
			}

			if (line == "[Events]")
			{
				std::string timePointLine;

				while (std::getline(chartFile, timePointLine))
				{
					if (timePointLine[0] == '/' && timePointLine[1] == '/')
						continue;
					
					std::string background;
					int charIndex = 0;

					while (timePointLine[charIndex++] != '"');

					do					
						background += timePointLine[charIndex];
					while (timePointLine[++charIndex] != '"');
					
					std::string backgroundPath = aPath + "\\" + background;

					chartData->background.load(backgroundPath);

					float procentualChange = float(ofGetWindowHeight()) / float(chartData->background.getHeight());

					chartData->background.resize(int(float(chartData->background.getWidth()) * procentualChange), int(float(chartData->background.getHeight()) * procentualChange));

					break;
				}
			}

			if (line == "[TimingPoints]")
			{
				std::string timePointLine;
				while (chartFile >> line)
				{
					if (line == "[HitObjects]" || line == "[Colours]")
						break;

					std::stringstream timePointStream(line);

					float timePoint, bpm;
					int filler1, filler2, filler3, filler4, filler5, filler6;

					PARSE_COMMA_VALUE(timePointStream, timePoint);
					PARSE_COMMA_VALUE(timePointStream, bpm);
					PARSE_COMMA_VALUE(timePointStream, filler1);
					PARSE_COMMA_VALUE(timePointStream, filler2);
					PARSE_COMMA_VALUE(timePointStream, filler3);
					PARSE_COMMA_VALUE(timePointStream, filler4);
					PARSE_COMMA_VALUE(timePointStream, filler5);
					PARSE_COMMA_VALUE(timePointStream, filler6);

					if (bpm < 0)
						continue;	

					bpm = 60000.0 / bpm;

					BPMData* bpmData = new BPMData();
					bpmData->BPM = bpm;
					bpmData->timePoint = int(timePoint);

					chartData->BPMPoints.push_back(bpmData);
				}
			}

			if (line == "[HitObjects]")
			{
				std::string noteLine;
				while (chartFile >> noteLine)
				{
					std::stringstream noteStream(noteLine);
					int column, filler, timePoint, isLN, filler2, timePointEnd;

					PARSE_COMMA_VALUE(noteStream, column);
					PARSE_COMMA_VALUE(noteStream, filler);
					PARSE_COMMA_VALUE(noteStream, timePoint);
					PARSE_COMMA_VALUE(noteStream, isLN);
					PARSE_COMMA_VALUE(noteStream, filler2);
					PARSE_COMMA_VALUE(noteStream, timePointEnd);
	

					if (isLN == 128)
					{
						NoteData* note = new NoteData();
						NoteData* holdNote = new NoteData();

						note->timePoint = timePoint;
						note->column = column <= 64 ? 0 : (column <= 192 ? 1 : (column <= 320 ? 2 : (column <= 448 ? 3 : 3)));
						note->noteType = NoteType::HoldBegin;
						note->self = note;

						holdNote->timePoint = timePointEnd;
						holdNote->column	= note->column;
						holdNote->noteType  = NoteType::HoldEnd;
						holdNote->self = holdNote;

						note->relevantNote = holdNote;
						holdNote->relevantNote = note;

						chartData->noteData.push_back(note);
						chartData->noteData.push_back(holdNote);
					}
					else
					{
						NoteData* note = new NoteData();

						note->timePoint = timePoint;
						note->column = column <= 64 ? 0 : (column <= 192 ? 1 : (column <= 320 ? 2 : (column <= 448 ? 3 : 3)));
						note->noteType = NoteType::Note;
						note->self = note;

						chartData->noteData.push_back(note);
					}
				}
			}				
		}

		std::sort(chartData->noteData.begin(), chartData->noteData.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs->timePoint < rhs->timePoint;
		});

	}

	return &myCharts[aPath];
}
