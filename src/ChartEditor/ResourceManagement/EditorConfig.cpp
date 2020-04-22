#include "EditorConfig.h"

namespace EditorConfig
{
	float hitLinePosition = 216.f;
	float scale = 1.f;

	float leftSidePositionFromCenter = -364.f;
	float leftSidePosition = -1.f;
	float sideSpace = 32.f;

	int keyAmount = -1;

	int fieldWidth = -1;
	int fieldWidthHalf = -1;

	namespace MiniMap
	{
		float spacingFromLeftSide = 16.f;
	}

	namespace Skin
	{
		ofImage noteImages[16];
		ofImage holdBodyImage;
		ofImage holdCapImage;

		ofImage selectImage;
	}
};