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
	if (myCurrentTime >= GetSongLength())
		SetPause(true);

	ShowPlaybackRateControls();

	BASS_Update(myStreamHandle);

	//myCurrentTime = BASS_ChannelBytes2Seconds(myStreamHandle, BASS_ChannelGetPosition(myStreamHandle, BASS_POS_BYTE));
	TryTimingSync();
}

void SongHandler::DrawWaveForm()
{
	if (myWaveFormData == nullptr)
		return void();

	int scaledSliceSize = float(myWaveFormSliceSize) * EditorConfig::scale;

	std::vector<int> indexes;

	int previousIndex = -1;

	for (int y = ofGetWindowHeight() * 2; y >= -ofGetWindowHeight(); y -= scaledSliceSize)
	{
		int newY = y;

		newY -= (EditorConfig::hitLinePosition);
		newY = int(float(newY) / EditorConfig::scale);

		int timePointMS = newY + ((myCurrentTime * 1000.0) + 0.5);

		int index = (timePointMS / myWaveFormSliceSize);
		if (index >= 0 && index < myWaveFormStructure.size() && index != previousIndex)
		{
			DrawWaveFormSliceAtIndex(index);
		}

		previousIndex = index;
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

void SongHandler::DrawWaveFormSliceAtIndex(int aIndex)
{
	int deltaSliceTime = (myWaveFormStructure[aIndex].timeMS + myWaveFormSliceSize) - GetCurrentTimeMS();

	int textureHeight = float(myWaveFormSliceSize) * EditorConfig::scale;
	int drawY = ofGetWindowHeight() - EditorConfig::hitLinePosition - float(deltaSliceTime) * EditorConfig::scale;

	myWaveFormStructure[aIndex].surface->draw(ofGetWindowWidth() / 2 - 128, drawY, 256, textureHeight);
}

void SongHandler::GenerateWaveForm(std::string aPath)
{
	HSTREAM decoder = BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE); // create a stream from the file

	if (myWaveFormData != nullptr)
	{
		delete myWaveFormData;
		myWaveFormData = nullptr;
	}

	for (auto slice : myWaveFormStructure )
	{
		delete slice.surface;
	}

	myWaveFormStructure.clear();

	mySongByteLength = BASS_ChannelGetLength(decoder, BASS_POS_BYTE); // get byte length
	myWaveFormData = (float*)std::malloc(mySongByteLength); // allocate a buffer for the data
	mySongByteLength = BASS_ChannelGetData(decoder, myWaveFormData, mySongByteLength); // decode the stream into the buffer

	if (myWaveFormData == nullptr)
		return void();
	
	int songLengthMS = BASS_ChannelBytes2Seconds(myStreamHandle, mySongByteLength) * 1000;
	int size = songLengthMS / myWaveFormSliceSize;

	for (int t = 0; t < songLengthMS; t += myWaveFormSliceSize)
	{
		ofFbo* surface = new ofFbo();
		surface->allocate(256, myWaveFormSliceSize);

		surface->begin();
		ofClear(255, 255, 255, 0);


		bool first = true;
		int time = -1;

		for (int y = 0; y < myWaveFormSliceSize; y += 1)
		{
			int newY = y;

			newY -= (EditorConfig::hitLinePosition);
			newY = int(float(newY) / EditorConfig::scale);

			int timePointMS = newY + t;

			if (first == true)
			{
				time = timePointMS;
				first = false;
			}

			if (timePointMS <= BASS_ChannelBytes2Seconds(myStreamHandle, mySongByteLength) * 1000 && timePointMS >= 0)
			{
				size_t index = BASS_ChannelSeconds2Bytes(myStreamHandle, double(timePointMS) / 1000.0) / 2;

				if (index < mySongByteLength / sizeof(int))
				{
					float width1 = abs(myWaveFormData[index]);
					float width2 = abs(myWaveFormData[index + 1]);

					ofSetColor(255, 255, 0, 128);
					ofDrawLine(128, myWaveFormSliceSize - y, 128 + width1 * 128.f, myWaveFormSliceSize - y);

					ofSetColor(255, 255, 0, 128);
					ofDrawLine(128, myWaveFormSliceSize - y, 128 - width2 * 128.f, myWaveFormSliceSize - y);

					ofSetColor(0, 255, 255, 192);
					ofDrawLine(128, myWaveFormSliceSize - y, 128 + width1 * 64.f, myWaveFormSliceSize - y);

					ofSetColor(0, 255, 255, 192);
					ofDrawLine(128, myWaveFormSliceSize - y, 128 - width2 * 64.f, myWaveFormSliceSize - y);

					ofSetColor(255, 255, 255, 255);
				}
			}
		}
		surface->end();

		myWaveFormStructure.push_back({ surface, time });
	}

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
