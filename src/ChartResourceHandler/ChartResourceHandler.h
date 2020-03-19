#pragma once

#include <string>
#include <unordered_map>

#include "../ChartData/ChartData.h"

#define PARSE_COMMA_VALUE(stringstream, target) stringstream >> target; if (stringstream.peek() == ',') stringstream.ignore()

class ChartResourceHandler
{
public:

	ChartResourceHandler();
	~ChartResourceHandler();

	ChartSet* ImportChart(std::string aPath);

	void RegisterChartSet(ChartSet* aChartSet);

	void SaveChart(std::string aFolderPath, std::string aPath, ChartData* aChart);
	void GenerateChartDifficulty(ChartSet* aChartSet, ChartData* aChart);

private:

	std::unordered_map<std::string, ChartSet> myChartSets;
};
