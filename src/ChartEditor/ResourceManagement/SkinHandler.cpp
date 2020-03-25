#include "SkinHandler.h"

SkinHandler::SkinHandler()
{

}

SkinHandler::~SkinHandler()
{

}

void SkinHandler::LoadSkin(int aKeyMode)
{
	std::string path = "skin/" + std::to_string(aKeyMode) + "k/";

	for (int i = 0; i < aKeyMode; i++)
	{
		std::string noteImagePath = path + "column_" + std::to_string(i + 1) + ".png";
		EditorConfig::Skin::noteImages[i].loadImage(noteImagePath);
	}

	EditorConfig::Skin::holdBodyImage.loadImage(path + "holdbody.png");
	EditorConfig::Skin::holdCapImage.loadImage(path + "holdcap.png");

	EditorConfig::Skin::selectImage.loadImage(path + "selected.png");

	EditorConfig::fieldWidth = EditorConfig::Skin::noteImages[0].getWidth() * aKeyMode;
}
