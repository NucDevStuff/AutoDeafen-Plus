#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace cocos2d;

bool running = false;

bool deafened = false;

extern bool paused;

std::string folderName = "Deafen_Presets";

class PlayThread {
public:
    void RunDeafen(float);
};

std::string substring_(std::string str, int start, int end) {
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

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void PlayThread::RunDeafen(float) {
    GameManager* gm = GameManager::sharedState();

    if (gm->m_playLayer != nullptr && !PlayLayer::get()->m_player1->m_isPlatformer) {
        PlayLayer* playLayer = PlayLayer::get();

        std::string levelID = std::to_string(playLayer->m_level->m_levelID);

        if (levelID == "0") {
            levelID = playLayer->m_level->m_levelName;
        }

		PlayerObject* plr = playLayer->m_player1;

        if (playLayer->m_endPortal) {
            float levelLength = playLayer->m_endPortal->getPositionX();

            int startPos = 0;

            if (!plr->m_startPosition.isZero()) {
                startPos = floor((plr->m_startPosition.x / levelLength) * 100);
            }

            if (std::filesystem::exists(folderName + "\\" + "preset" + levelID + ".txt")) {
                std::ifstream inFile(folderName + "\\" + "preset" + levelID + ".txt");

                std::string contents;

                inFile >> contents;

                inFile.close();

                if ((contents.find("%" + std::to_string(startPos) + "%")) != std::string::npos) {
                    float currentPercent = (playLayer->m_player1->getPositionX() / levelLength) * 100;

                    int deafenParams = contents.find("%" + std::to_string(startPos) + "%");

                    int openParams = contents.find("{", deafenParams);
                    int closeParams = contents.find("}", openParams);

                    std::string subbedString = substring_(contents, openParams + 1, closeParams);

                    if (is_number(substring_(subbedString, 7, subbedString.find(","))) && is_number(substring_(subbedString, subbedString.find(",") + 10, subbedString.length()))) {
                        float deafenAt = stof(substring_(subbedString, 7, subbedString.find(",")));
                        float undeafenAt = stof(substring_(subbedString, subbedString.find(",") + 10, subbedString.length()));

                        if (currentPercent >= deafenAt && currentPercent < undeafenAt && (!paused) && (!playLayer->m_player1->m_isDead)) {
                            if (!deafened && !playLayer->m_isPracticeMode) {
                                PlayKeybind();

                                deafened = true;
                            }
                        }

                        if (((currentPercent < deafenAt) && deafened) || (currentPercent >= undeafenAt) || paused || playLayer->m_player1->m_isDead) {
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

class $modify(MenuLayer) {
    bool init() {
        MenuLayer::init();

        if (!running) {
            running = true;

            //auto alert = FLAlertLayer::create("Reminder", "EXPIRATION DATE IS TOP 1", "TRUEEEEE");
            //alert->m_scene = this;
            //alert->show();

            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(PlayThread::RunDeafen), CCDirector::sharedDirector(), 0, false);
        }

        return true;
    }
};

