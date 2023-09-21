// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "PauseLayer.h"
#include "MenuLayer.h"
#include <fstream>

#include "extensions2.h"

using namespace std;

bool toggled = true;

bool deafened = false;

MegaHackExt::Window* window;

MegaHackExt::Button* cancelButton;

string folderName = "Deafen_Presets";

class PlayThread : public CCObject {
public:
    void RunDeafen(float);
};

string substring_(string str, int start, int end) {
	return str.substr(start, end - start);
}

void PlayKeybind() {
    // press and hold
    keybd_event(0xA1, 0, 0x0000, 0);
    keybd_event(0x76, 0, 0x0000, 0);

    // release
    keybd_event(0x76, 0, 0x0002, 0);
    keybd_event(0xA1, 0, 0x0002, 0);
}

bool __fastcall PauseLayer::undeafen() {
    if (deafened) {
        PlayKeybind();

        deafened = false;
    }

    return false;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void PlayThread::RunDeafen(float) {
    if (toggled) {
        gd::GameManager* gm = gd::GameManager::sharedState();

        if (gm->m_pPlayLayer != nullptr) {
            gd::PlayLayer* playLayer = gd::PlayLayer::get();

            string levelID = to_string(playLayer->m_level->m_nLevelID);

            if (levelID == "0") {
                levelID = playLayer->m_level->m_sLevelName;
            }

            float levelLength = playLayer->m_endPortal->getPositionX();

            int startPos;

            if (playLayer->m_startPos) {
                startPos = floor((playLayer->m_startPos->getStartPosition().x / levelLength) * 100);
            }
            else {
                startPos = 0;
            }

            if (filesystem::exists(folderName + "\\" + "preset" + levelID + ".txt")) {
                ifstream inFile(folderName + "\\" + "preset" + levelID + ".txt");

                string contents;

                inFile >> contents;

                inFile.close();

                if ((contents.find("%" + to_string(startPos) + "%")) != string::npos) {
                    float currentPercent = (playLayer->m_pPlayer1->getPositionX() / levelLength) * 100;

                    int deafenParams = contents.find("%" + to_string(startPos) + "%");

                    int openParams = contents.find("{", deafenParams);
                    int closeParams = contents.find("}", openParams);

                    string subbedString = substring_(contents, openParams + 1, closeParams);

                    if (is_number(substring_(subbedString, 7, subbedString.find(","))) && is_number(substring_(subbedString, subbedString.find(",") + 10, subbedString.length()))) {
                        float deafenAt = stof(substring_(subbedString, 7, subbedString.find(",")));
                        float undeafenAt = stof(substring_(subbedString, subbedString.find(",") + 10, subbedString.length()));

                        if (currentPercent >= deafenAt && currentPercent < undeafenAt && (!playLayer->m_bIsPaused) && (!playLayer->m_isDead) && (!playLayer->m_hasCompletedLevel)) {
                            if (!deafened && !playLayer->m_isPracticeMode) {
                                PlayKeybind();

                                deafened = true;
                            }
                        }

                        if (((currentPercent < deafenAt) && deafened) || (currentPercent >= undeafenAt) || playLayer->m_isDead || playLayer->m_hasCompletedLevel) {
                            if (deafened && !playLayer->m_isPracticeMode) {
                                PlayKeybind();

                                deafened = false;
                            }
                        }
                    }
                }
            }
        }
    }
}

DWORD WINAPI my_thread(void* hModule) {
    if (MH_Initialize() != MH_OK) {
        //This line will dettach your DLL when executed. Remove if needed
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(hModule), 0);
    }
    //0x1E4620
    MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x1E4620), PauseLayer::hook, reinterpret_cast<void**>(&PauseLayer::init));
    MH_EnableHook(MH_ALL_HOOKS);

    //MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x1907B0), MenuLayer::hook, reinterpret_cast<void**>(&MenuLayer::init));
    //MH_EnableHook(MH_ALL_HOOKS);

    using namespace MegaHackExt;

    Window* window = Window::Create("AutoDeafen+");

    CheckBox* checkBox = CheckBox::Create("Toggle");
    checkBox->set(true);
    checkBox->setCallback([](CheckBox* obj, bool b) {toggled = b; });

    window->add(checkBox);

    Client::commit(window);
    
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(PlayThread::RunDeafen), CCDirector::sharedDirector(), 0, false);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

