#include "ChartResourceHandler.h"
#include "ofFileUtils.h"
#include "ofWindowSettings.h"
#include "ofMain.h"


#include <filesystem>
#include <sstream>
#include <algorithm>

ChartResourceHandler::ChartResourceHandler()
{

}

ChartResourceHandler::~ChartResourceHandler()
{
	for (auto& entry : myCharts)
		for (auto& chart : entry.second.charts)
			delete chart;
}

ChartSet* ChartResourceHandler::ImportChart(std::string aPath)
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
		chartData->filePath = directory.getPath(i);

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
						chartData->audioFileName = value;
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
						myCharts[aPath].songTitle = value;
					
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

					chartData->backgroundFileName = background;
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
					int meter, sampleSet, sampleIndex, volume, uninherited, effects;

					PARSE_COMMA_VALUE(timePointStream, timePoint);
					PARSE_COMMA_VALUE(timePointStream, bpm);
					PARSE_COMMA_VALUE(timePointStream, meter);
					PARSE_COMMA_VALUE(timePointStream, sampleSet);
					PARSE_COMMA_VALUE(timePointStream, sampleIndex);
					PARSE_COMMA_VALUE(timePointStream, volume);
					PARSE_COMMA_VALUE(timePointStream, uninherited);
					PARSE_COMMA_VALUE(timePointStream, effects);

					BPMData* bpmData = new BPMData();
					bpmData->BPMSaved = bpm;

					if (bpm < 0)
						continue;	

					bpm = 60000.0 / bpm;

					bpmData->BPM = bpm;
					bpmData->timePoint = int(timePoint);
					bpmData->meter = meter;
					bpmData->uninherited = uninherited;

					chartData->BPMPoints.push_back(bpmData);
				}
			}

			if (line == "[HitObjects]")
			{
				std::string noteLine;
				while (chartFile >> noteLine)
				{
					std::stringstream noteStream(noteLine);
					int column, y, timePoint, isLN, hitSound, timePointEnd;

					PARSE_COMMA_VALUE(noteStream, column);
					PARSE_COMMA_VALUE(noteStream, y);
					PARSE_COMMA_VALUE(noteStream, timePoint);
					PARSE_COMMA_VALUE(noteStream, isLN);
					PARSE_COMMA_VALUE(noteStream, hitSound);
					PARSE_COMMA_VALUE(noteStream, timePointEnd);
	
					if (isLN == 128)
					{
						NoteData* note = new NoteData();
						NoteData* holdNote = new NoteData();

						note->timePoint = timePoint;
						note->column = floor(float(column) * (4.f / 512.f));
						note->noteType = NoteType::HoldBegin;
						note->type = isLN;
						note->hitSound = hitSound;

						holdNote->timePoint = timePointEnd;
						holdNote->column	= note->column;
						holdNote->noteType  = NoteType::HoldEnd;
						holdNote->type = isLN;
						holdNote->hitSound = hitSound;

						note->relevantNote = holdNote;
						holdNote->relevantNote = note;

						chartData->noteData.push_back(note);
						chartData->noteData.push_back(holdNote);
					}
					else
					{
						NoteData* note = new NoteData();

						note->timePoint = timePoint;
						note->column = floor(float(column) * (4.f / 512.f));
						note->noteType = NoteType::Note;
						note->type = isLN;
						note->hitSound = hitSound;

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

void ChartResourceHandler::SaveChart(std::string aPath, ChartData* aChart)
{
	ChartSet& chartSet = myCharts[aPath];
	ChartData* chart = aChart;

	std::stringstream chartData;
	chartData << "osu file format v14" << std::endl;
	
	chartData << std::endl;
	chartData << "[General]" << std::endl;

	chartData << "AudioFilename: " << chart->audioFileName << std::endl;
	chartData << "AudioLeadIn: 0" << std::endl;
	chartData << "PreviewTime: 0" << std::endl;
	chartData << "Countdown: 0" << std::endl;
	chartData << "SampleSet: None" << std::endl;
	chartData << "StackLeniency: 0.7" << std::endl;
	chartData << "Mode: 3" << std::endl;
	chartData << "LetterboxInBreaks: 0" << std::endl;
	chartData << "SpecialStyle: 0" << std::endl;
	chartData << "WidescreenStoryboard: 0" << std::endl;

	chartData << std::endl;
	chartData << "[Editor]" << std::endl;

	chartData << "DistanceSpacing: 1" << std::endl;
	chartData << "BeatDivisor: 4" << std::endl;
	chartData << "GridSize: 16" << std::endl;
	chartData << "TimelineZoom: 1" << std::endl;
	
	chartData << std::endl;
	chartData << "[Metadata]" << std::endl;
	
	chartData << "Title:" << chartSet.songTitle << std::endl;
	chartData << "TitleUnicode:" << chartSet.songTitle << std::endl;
	chartData << "Artist:" << chartSet.artist << std::endl;
	chartData << "ArtistUnicode:" << chartSet.artist << std::endl;
	chartData << "Creator:untitled_chart_editor" << std::endl;
	chartData << "Version:insane" << std::endl;
	chartData << "Source:" << std::endl;
	chartData << "Tags:created by untitled_chart_editor" << std::endl;
	chartData << "BeatmapID:-1" << std::endl;
	chartData << "BeatmapSetID:-1" << std::endl;

	chartData << std::endl;
	chartData << "[Difficulty]" << std::endl;

	chartData << "HPDrainRate:8" << std::endl;
	chartData << "CircleSize:4" << std::endl;
	chartData << "OverallDifficulty:8" << std::endl;
	chartData << "ApproachRate:9" << std::endl;
	chartData << "SliderMultiplier:1.4" << std::endl;
	chartData << "SliderTickRate:1" << std::endl;

	chartData << std::endl;
	chartData << "[Events]" << std::endl;

	chartData << "0,0,\"" << chart->backgroundFileName << "\",0,0" << std::endl;

	chartData << std::endl;
	chartData << "[TimingPoints]" << std::endl;

	for (auto timingPoint : chart->BPMPoints)
	{
		chartData << timingPoint->timePoint << "," << 60000.0 / timingPoint->BPM << "," << timingPoint->meter << ",0,0,50," << timingPoint->uninherited << ",0" << std::endl;
	}

	chartData << std::endl;
	chartData << "[HitObjects]" << std::endl;

	for (auto note : chart->noteData)
	{
		int column = note->column == 0 ? 64 : note->column == 1 ? 192 : note->column == 2 ? 320 : note->column == 3 ? 448 : 0;

		switch (note->noteType)
		{
		case NoteType::Note:
			chartData << column << ",192," << note->timePoint << ",1," << "0,1:0:0:0" << std::endl;
			break;
		case NoteType::HoldBegin:
			chartData << column << ",192," << note->timePoint << ",128,1," << note->relevantNote->timePoint << ":0:0:0:0" << std::endl;
			break;

		default:
			break;
		}
	}

	std::ofstream chartFile(chart->filePath);
	if (chartFile.good())
	{
		chartFile.clear();
		chartFile << chartData.str();
		chartFile.close();
	}
}
