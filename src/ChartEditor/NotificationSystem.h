#pragma once

#include <string>
#include <vector>

#include "ofRectangle.h"

#define PUSH_NOTIFICATION(message) NotificationSystem::GetInstance()->SubmitMessage(message)

#ifdef _DEBUG
#define PUSH_NOTIFICATION_DEBUG(message) NotificationSystem::GetInstance()->SubmitMessage(std::string("[DEBUG] ") + message)
#else
#define PUSH_NOTIFICATION_DEBUG(message)
#endif

namespace StringHelpfunctions
{
    ofRectangle getBitmapStringBoundingBox(std::string text);
}


struct Notification
{
	std::string message;
	float timer = 5.f;

	float LerpY = 0.f;
	float LerpX = 0.f;
};

class NotificationSystem
{
public:

	NotificationSystem();
	~NotificationSystem();

	static NotificationSystem* GetInstance();

	void SubmitMessage(std::string aMessage);

	void Update();
	void Draw();

private:

	static NotificationSystem* ourInstance;

	std::vector<Notification> mySubmittedMessages;

	float myTimeLimit = 3.5f;
};
