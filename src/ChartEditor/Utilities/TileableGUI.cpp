#include "TileableGUI.h"

#include "ofMain.h"

TileableGUI* TileableGUI::ourInstance = nullptr;

TileableGUI::TileableGUI()
{

}

TileableGUI::~TileableGUI()
{

}

void TileableGUI::RegisterTile(std::function<void()> aWork, std::string aTag, bool aActive)
{
	Tile* tile = new Tile();
	
	tile->tag = aTag;
	tile->work = aWork;

	if (aActive == true)
	{
		myActiveTiles.push_back(tile);
		
	}
	else
	{
		myInactiveTiles.push_back(tile);
	}
}

void TileableGUI::Init()
{

}

void TileableGUI::Update()
{
	myTileFieldDimensions = { 256, ofGetWindowHeight() - 32.f - 8.f};
	myTileFieldPosition = { ofGetWindowWidth() - 256 - 16.f, 32.f };

	float tileSize = myTileFieldDimensions.y /float( myActiveTiles.size());

	Tile* movingTile = nullptr;
	
	int index = 0;
	for (auto tile : myActiveTiles)
	{
		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
		windowFlags |= ImGuiWindowFlags_NoTitleBar;

		bool open = true;

		ImGui::SetNextWindowSize({ myTileFieldDimensions.x, tileSize - myGaps });
		ImGui::Begin(tile->tag.c_str(), &open, windowFlags);

		if (tile->position.x != ImGui::GetWindowPos().x || tile->position.y != ImGui::GetWindowPos().y && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			tile->move = true;
		
		tile->move = (tile->move == true && ImGui::IsMouseDown(ImGuiMouseButton_Left));
		
		if (tile->move == true)
			movingTile = tile;

		tile->work();
		
		tile->position = ImGui::GetWindowPos();
		ImGui::SetWindowPos({ myTileFieldPosition.x, myTileFieldPosition.y + tileSize * float(index) });
		ImGui::End();


		tile->index = index;
		index++;
	}

	if (movingTile != nullptr)
	{
		int newIndex = ofClamp(movingTile->position.y, myTileFieldPosition.y, myTileFieldPosition.y + myTileFieldDimensions.y - myGaps) / tileSize;
		int oldIndex = movingTile->index;

		if (oldIndex > newIndex)
			std::rotate(myActiveTiles.rend() - oldIndex - 1, myActiveTiles.rend() - oldIndex, myActiveTiles.rend() - newIndex);
		else
			std::rotate(myActiveTiles.begin() + oldIndex, myActiveTiles.begin() + oldIndex + 1, myActiveTiles.begin() + newIndex + 1);

		movingTile->index = newIndex;
	}
}

TileableGUI* TileableGUI::GetInstance()
{
	if (ourInstance == nullptr)
		ourInstance = new TileableGUI();

	return ourInstance;
}
