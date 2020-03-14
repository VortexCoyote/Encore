#include "SongHandler.h"

#include "ofSoundPlayer.h"
#include "ofSoundUtils.h"
#include "ofMain.h"

#include "imgui.h"

#include <Windows.h>

#include "EditorConfig.h"
#include "NotificationSystem.h"

SongHandler::SongHandler()
{

}

SongHandler::~SongHandler()
{

}

void SongHandler::Init(std::string aPath, double aSyncThreshold)
{
	mySyncThreshold = aSyncThreshold;

	BASS_Free();
	BASS_Init(myDevice, myFreq, 0, 0, NULL);

	myStreamHandle = BASS_FX_TempoCreate(BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_STREAM_PRESCAN), BASS_FX_FREESOURCE);


	auto error = BASS_ErrorGetCode();
	if (error != 0)
	{
		std::cout << BASS_ErrorGetCode() << std::endl;
	}

	GenerateWaveForm(aPath);

	BASS_ChannelPlay(myStreamHandle, FALSE);
	BASS_ChannelPause(myStreamHandle);

	myIsPausing = true;
}

void SongHandler::Update()
{
	/*if (GetAsyncKeyState(VK_DOWN))
	{
		myCurrentTime -= 0.01;
		SetTimeS(myCurrentTime);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		myCurrentTime += 0.01;
		SetTimeS(myCurrentTime);
	}*/

	if (myCurrentTime >= GetSongLength())
		SetPause(true);
	

	ShowPlaybackRateControls();

	BASS_Update(myStreamHandle);

	//myCurrentTime = BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetPosition(myStreamHandle, BASS_POS_BYTE));
	TryTimingSync();
}

void SongHandler::DrawWaveForm()
{
	for (int y = 0; y < ofGetWindowHeight(); y += 1)
	{
		int newY = y;

		newY -= (EditorConfig::hitLinePosition);
		newY = int(float(newY) / EditorConfig::scale);

		int timePointMS = newY + (myCurrentTime * 1000.0);

		if (timePointMS <= BASS_ChannelBytes2Seconds(myStreamHandle, mySongByteLength) * 1000 && timePointMS >= 0)
		{
			size_t index = BASS_ChannelSeconds2Bytes(myStreamHandle, double(timePointMS) / 1000.0) / 2;

			if (index < mySongByteLength / sizeof(int))
			{
				float width1 = myWaveFormData[index];
				float width2 = myWaveFormData[index + 1];

				float color = abs(width1) * 255;
				ofSetColor(255 - color, 255, color, 128);
				ofDrawLine(ofGetWindowWidth() / 2, ofGetWindowHeight() - y, ofGetWindowWidth() / 2 + width1 * 128.f, ofGetWindowHeight() - y);

				color = abs(width2) * 255;
				ofSetColor(255 - color, 255, color, 128);
				ofDrawLine(ofGetWindowWidth() / 2, ofGetWindowHeight() - y, ofGetWindowWidth() / 2 - width2 * 128.f, ofGetWindowHeight() - y);

				ofSetColor(255, 255, 255, 255);
			}
		}
	}
}

void SongHandler::TogglePause()
{
	myIsPausing = !myIsPausing;
	
	if (myIsPausing)
		BASS_ChannelPause(myStreamHandle);
	else
		BASS_ChannelPlay(myStreamHandle, FALSE);

	myCurrentTime = GetRealCurrentTimeS();
}


void SongHandler::SetPause(bool aPause)
{
	myIsPausing = aPause;

	if (aPause)
		BASS_ChannelPause(myStreamHandle);
	else
		BASS_ChannelPlay(myStreamHandle, FALSE);

	myCurrentTime = GetRealCurrentTimeS();
}

void SongHandler::SetTimeS(double aTimeS)
{
	myCurrentTime = aTimeS;
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, myCurrentTime), BASS_POS_BYTE);
}

void SongHandler::SetTimeNormalized(float aNormalizedTime)
{
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, aNormalizedTime * GetSongLength()), BASS_POS_BYTE);
	myCurrentTime = GetRealCurrentTimeS();
}

void SongHandler::ResetSpeed()
{
	mySpeed = 1.f;

	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(myStreamHandle, &info);

	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_FREQ, float(info.freq) * mySpeed);
	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_TEMPO, 0);
}

void SongHandler::SongJumpAmount(double aSongJumpAmount)
{
	myCurrentTime += aSongJumpAmount;
	
	BASS_ChannelSetPosition(myStreamHandle, BASS_ChannelSeconds2Bytes(myStreamHandle, myCurrentTime), BASS_POS_BYTE);
}

double SongHandler::GetCurrentTimeS()
{
	return myCurrentTime;
}

int SongHandler::GetCurrentTimeMS()
{
	return int(myCurrentTime * 1000.0);
}

double SongHandler::GetTimeNormalized()
{
	return myCurrentTime / GetSongLength();
}

double SongHandler::GetSongLength()
{
	return BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetLength(myStreamHandle, BASS_POS_BYTE));
}

void SongHandler::StopSong()
{
	BASS_ChannelStop(myStreamHandle);
}

void SongHandler::IncreaseSpeed()
{
	mySpeed += 0.05f;
	
	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(myStreamHandle, &info);
	
	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_FREQ, float(info.freq) * mySpeed);
	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_TEMPO, 0);
	
	PUSH_NOTIFICATION(std::string("Playback Speed: ") + std::to_string(mySpeed));
}

void SongHandler::DecreaseSpeed()
{
	mySpeed -= 0.05f;

	if (mySpeed < 0.05f)
		mySpeed = 0.05f;

	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(myStreamHandle, &info);

	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_FREQ, float(info.freq) * mySpeed);
	BASS_ChannelSetAttribute(myStreamHandle, BASS_ATTRIB_TEMPO, 0);

	PUSH_NOTIFICATION(std::string("Playback Speed: ") + std::to_string(mySpeed));
}

void SongHandler::GenerateWaveForm(std::string aPath)
{
	HSTREAM decoder = BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE); // create a stream from the file

	mySongByteLength = BASS_ChannelGetLength(decoder, BASS_POS_BYTE); // get byte length
	myWaveFormData = (float*)malloc(mySongByteLength); // allocate a buffer for the data
	mySongByteLength = BASS_ChannelGetData(decoder, myWaveFormData, mySongByteLength); // decode the stream into the buffer
}

void SongHandler::ShowPlaybackRateControls()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;

	bool open = true;

	ImGui::Begin("Playback Rate", &open, windowFlags);

	ImGui::Text("Playback Rate:");
	ImGui::SameLine();

	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << mySpeed << "x";
	std::string s = stream.str();

	ImGui::Text(s.c_str());


	if (ImGui::Button("- 0.05x"))
		DecreaseSpeed();

	ImGui::SameLine();

	if (ImGui::Button("+ 0.05x"))
		IncreaseSpeed();

	ImGui::SetWindowPos({ ofGetWindowWidth() - ImGui::GetWindowWidth() - 128, ofGetWindowHeight() - ImGui::GetWindowHeight() - 8 });

	ImGui::End();
}

void SongHandler::TryTimingSync()
{
	// **all prints here are left for debug purposes** //

	if (myIsPausing == false)
	{
		myCurrentTime += ofGetLastFrameTime() * double(mySpeed);

		double currentSongTime = GetRealCurrentTimeS();

		double syncAdjustment = (currentSongTime - myCurrentTime);

		if (abs(syncAdjustment) > mySyncThreshold)
		{
			myCurrentTime += syncAdjustment;
			
			PUSH_NOTIFICATION_DEBUG(std::to_string(int(syncAdjustment * 1000.0 + 0.5)) + "ms SYNCED!");
		}
		else
		{
			//std::cout << syncAdjustment << std::endl;
		}	
	}
}

double SongHandler::GetRealCurrentTimeS()
{
	return BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetPosition(myStreamHandle, BASS_POS_BYTE));
}
