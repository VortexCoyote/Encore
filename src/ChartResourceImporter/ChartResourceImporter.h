#pragma once

#include <string>
#include <unordered_map>

#include "../ChartData/ChartData.h"

#define PARSE_COMMA_VALUE(stringstream, target) stringstream >> target; if (stringstream.peek() == ',') stringstream.ignore()

class ChartResourceImporter
{
public:

	ChartResourceImporter();
	~ChartResourceImporter();

	ChartSet* ImportChart(std::string aPath);

private:

	std::unordered_map<std::string, ChartSet> myCharts;
};
