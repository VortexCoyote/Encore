#pragma once

#include <vector>
#include <functional>

struct Tile
{
	std::function<void()> work;
};

class TileableGUI
{
public:

	TileableGUI();
	~TileableGUI();

private:

	std::vector<Tile> mySlots;
};

