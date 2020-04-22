#pragma once

#include <vector>
#include <functional>

#include "imgui.h"

struct Tile
{
	std::string tag;
	std::function<void()> work;

	bool move = false;
	ImVec2 position;

	int index = -1;
};

class TileableGUI
{
public:

	TileableGUI();
	~TileableGUI();

	void RegisterTile(std::function<void()> aWork, std::string aTag, bool aActive); 

	void Init();
	void Update();

	static TileableGUI* GetInstance();

private:

	static TileableGUI* ourInstance;

	std::vector<Tile*> myInactiveTiles;
	std::vector<Tile*> myActiveTiles;

	ofVec2f myTileFieldPosition;
	ofVec2f myTileFieldDimensions;

	float myGaps = 8.f;
};

