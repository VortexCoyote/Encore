#pragma once

#include "ofImage.h"

namespace EditorConfig
{
	extern float hitLinePosition;
	extern float scale;
	extern float leftSidePosition;
	extern float sideSpace;

	extern int keyAmount;

	extern int fieldWidth;
	extern int fieldWidthHalf;

	namespace MiniMap
	{
		extern float spacingFromLeftSide;
	}

	namespace Skin
	{
		extern ofImage noteImages[16];
		extern ofImage holdBodyImage;
		extern ofImage holdCapImage;

		extern ofImage selectImage;
	}
};