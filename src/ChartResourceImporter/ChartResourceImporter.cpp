#include "ChartResourceImporter.h"
#include "ofFileUtils.h"
#include "ofWindowSettings.h"
#include "ofMain.h"


#include <filesystem>
#include <sstream>

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
						chartData->song.load(aPath + "\\" + value);

						chartData->song.play();
						chartData->song.setPosition(0.9999999f);
						int ms = chartData->song.getPositionMS();
						chartData->song.setPosition(0);
						chartData->song.stop();

						chartData->songLength = ms;
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
					if (line == "[HitObjects]")
						break;

					std::stringstream timePointStream(line);

					int timePoint;
					float bpm;
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

					BPMData bpmData;
					bpmData.BPM = bpm;
					bpmData.timePoint = timePoint;

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
	
					NoteData note;

					note.timePoint = timePoint;
					note.column = column <= 64 ? 0 : (column <= 192 ? 1 : (column <= 320 ? 2 : (column <= 448 ? 3 : 3)));
					
					if (isLN == 128)
					{
						note.isHold = true;
						note.timePointEnd = timePointEnd;
						note.cullTimeOffset = timePointEnd;
					}

					chartData->noteData.push_back(note);
				}
			}				
		}
	}

	return &myCharts[aPath];
}
