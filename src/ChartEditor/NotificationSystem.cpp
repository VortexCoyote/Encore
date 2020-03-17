#include "NotificationSystem.h"

#include "ofGraphics.h"
#include "ofMain.h"

#include "imgui.h"

NotificationSystem* NotificationSystem::ourInstance;

NotificationSystem::NotificationSystem()
{
	mySubmittedMessages.reserve(100);
}

NotificationSystem::~NotificationSystem()
{

}

NotificationSystem* NotificationSystem::GetInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new NotificationSystem();
	}

	return ourInstance;
}

void NotificationSystem::SubmitMessage(std::string aMessage, ofColor aColor)
{
	mySubmittedMessages.push_back({ aMessage, myTimeLimit, 32.f, float(-StringHelpfunctions::getBitmapStringBoundingBox(aMessage).width - 8), aColor});
}

void NotificationSystem::SubmitMessage(std::string aMessage)
{
	mySubmittedMessages.push_back({ aMessage, myTimeLimit, 32.f, float(-StringHelpfunctions::getBitmapStringBoundingBox(aMessage).width - 8), ofColor(255, 255, 255, 255) });
}

void NotificationSystem::Update()
{
	for (int i = mySubmittedMessages.size() - 1; i >= 0; i--)
	{
		mySubmittedMessages[i].LerpX = ofLerp(mySubmittedMessages[i].LerpX, 8, 15.f * ofGetLastFrameTime());
		mySubmittedMessages[i].LerpY = ofLerp(mySubmittedMessages[i].LerpY, float(34 + (mySubmittedMessages.size() - 1 - i) * 24), 5.f  * ofGetLastFrameTime());

		mySubmittedMessages[i].timer -= ofGetLastFrameTime();
		if (mySubmittedMessages[i].timer <= 0.f)
		{
			mySubmittedMessages.erase(mySubmittedMessages.begin() + i);
		}
	}
}

void NotificationSystem::Draw()
{
	//for (int i = mySubmittedMessages.size() - 1; i >= 0; i--)
	for (int i = 0; i < mySubmittedMessages.size(); i++)
	{
		ofRectangle stringDimension = StringHelpfunctions::getBitmapStringBoundingBox(mySubmittedMessages[i].message);;

		ofSetColor(0, 0, 0, 200 * (float(mySubmittedMessages[i].timer) / myTimeLimit));
		ofDrawRectangle(mySubmittedMessages[i].LerpX, mySubmittedMessages[i].LerpY - stringDimension.height, 1.f, stringDimension.width, stringDimension.height + 4);
		
		auto color = mySubmittedMessages[i].color;

		ofSetColor(color.r, color.g, color.b, color.a * (float(mySubmittedMessages[i].timer) / myTimeLimit));

		ofDrawBitmapString(mySubmittedMessages[i].message, mySubmittedMessages[i].LerpX, mySubmittedMessages[i].LerpY);

		ofSetColor(255, 255, 255, 255);

	}
}

ofRectangle StringHelpfunctions::getBitmapStringBoundingBox(std::string text)
{
	vector<string> lines = ofSplitString(text, "\n");
	int maxLineLength = 0;
	for (int i = 0; i < (int)lines.size(); i++) {

		const string& line(lines[i]);
		int currentLineLength = 0;
		for (int j = 0; j < (int)line.size(); j++) {
			if (line[j] == '\t') {
				currentLineLength += 8 - (currentLineLength % 8);
			}
			else {
				currentLineLength++;
			}
		}
		maxLineLength = std::max(maxLineLength, currentLineLength);
	}

	int padding = 4;
	int fontSize = 8;

	float leading = 1.7;

	int height = lines.size() * fontSize * leading - 1;
	int width = maxLineLength * fontSize;

	return ofRectangle(0, 0, width, height);
}
