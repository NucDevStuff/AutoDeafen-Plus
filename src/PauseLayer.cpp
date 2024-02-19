
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/CCSprite.hpp>

#include <iostream>
#include <fstream>

using namespace geode::prelude;

CCTextInputNode* deafenAt;
CCTextInputNode* undeafenAt;

std::string default_at = "";
std::string default_unat = "100";

std::string levelID;
int startPos;

std::string folderName_ = "Deafen_Presets";

bool menuOpen = false;

bool paused = false;

CCNode* sp_menu_sprites[20] = {};
CCMenu* sp_menu;

CCNode* scene;

CCLabelBMFont* sp_menu_title;

float xOffset = 80;

std::string substring(std::string str, int start, int end) {
	return str.substr(start, end - start);
}

std::string readFromFile(bool deafen) {
	std::string presetName = "preset" + levelID + ".txt";

	if (std::filesystem::exists(folderName_ + "\\" + presetName)) {
		std::ifstream inFile;
		inFile.open(folderName_ + "\\" + presetName);

		std::string fileContents;

		inFile >> fileContents;

		if ((fileContents.find("%" + std::to_string(startPos) + "%")) != std::string::npos) {
			int deafenParams = fileContents.find("%" + std::to_string(startPos) + "%");

			int openParams = fileContents.find("{", deafenParams);
			int closeParams = fileContents.find("}", openParams);

			std::string subbedString = substring(fileContents, openParams + 1, closeParams);

			if (deafen) {
				return substring(subbedString, 7, subbedString.find(","));
			}
			else {
				return substring(subbedString, subbedString.find(",") + 10, subbedString.length());
			}
		}

		inFile.close();
	}

	if (deafen) {
		return default_at;
	}
	else {
		return default_unat;
	}
}

bool writeToFile() {
	std::string presetName = "preset" + levelID + ".txt";

	std::ifstream inFile;
	inFile.open(folderName_ + "\\" + presetName);

	std::string fileContents;

	inFile >> fileContents;

	inFile.close();

	std::ofstream outFile;
	outFile.open(folderName_ + "\\" + presetName);

	outFile.clear();

	std::string deafAt = deafenAt->getString();
	std::string undeafAt = undeafenAt->getString();

	if ((fileContents.find("%" + std::to_string(startPos) + "%")) == std::string::npos) {
		if (deafAt.length() != 0 && undeafAt.length() != 0) {
			fileContents += ("%" + std::to_string(startPos) + "%{deafen=" + deafAt + ",undeafen=" + undeafAt + "}");
		}
		else {
			outFile << fileContents;
			outFile.close();

			return false;
		}
	}
	else {
		int deafenParams = fileContents.find("%" + std::to_string(startPos) + "%");
		// hi quad
		int openParams = fileContents.find("{", deafenParams);
		int closeParams = fileContents.find("}", openParams);

		if (deafAt.length() != 0 && undeafAt.length() != 0) {
			fileContents = fileContents.substr(0, openParams + 1) + "deafen=" + deafAt + ",undeafen=" + undeafAt + substring(fileContents, closeParams, fileContents.length());
		}
		else {
			fileContents = fileContents.substr(0, deafenParams - 1) + substring(fileContents, closeParams + 1, fileContents.length());
		}
	}

	outFile << fileContents;
	outFile.close();

	return true;
}

void SavePreset() {
	if (!std::filesystem::exists(folderName_)) { // If presets folder doesn't exist, create it
		std::filesystem::create_directory(folderName_);
	}

	writeToFile();
}

class SpMenu : public CCLayer {
public:
	void open_sp_callback(CCObject*);
	void close_sp_callbackBtn(CCObject*);
	void deleteSpConfirm(CCObject*);
};

void close_sp_callback() { // close start pos menu
	menuOpen = false;

	SavePreset();

	for (CCNode* node : sp_menu_sprites) {
		if (node != nullptr) {
			node->setVisible(false);
		}
	}

	sp_menu->setVisible(false);
}

void SpMenu::close_sp_callbackBtn(CCObject*) { // close start pos menu
	close_sp_callback();
}

void SpMenu::deleteSpConfirm(CCObject*) {
	std::string spStr = std::to_string(startPos);

	std::string alertTitle = spStr + "% StartPos";

	geode::createQuickPopup(
    	alertTitle.c_str(),   // title
    	"Are you sure you want to delete this StartPos' Deafen?",   // content
    	"Cancel", "Yes",      // buttons
    	[](auto, bool btn2) {
        	if (btn2) {
            	// confirm code

				std::string presetName = "preset" + levelID + ".txt";

				std::ifstream inFile;
				inFile.open(folderName_ + "\\" + presetName);

				std::string fileContents;

				inFile >> fileContents;

				inFile.close();

				std::ofstream outFile;
				outFile.open(folderName_ + "\\" + presetName);

				outFile.clear();

				int deafenParams = fileContents.find("%" + std::to_string(startPos) + "%");

				std::ofstream console;
				console.open("Console.txt");

				console.clear();

				if (deafenParams != std::string::npos) {
					int openParams = fileContents.find("{", deafenParams);
					int closeParams = fileContents.find("}", openParams);

					console << substring(fileContents, closeParams + 1, fileContents.length());

					fileContents = fileContents.substr(0, deafenParams) + substring(fileContents, closeParams + 1, fileContents.length());
				}

				console.close();

				outFile << fileContents;
				outFile.close();

				menuOpen = false;

				for (CCNode* node : sp_menu_sprites) {
					if (node != nullptr) {
						node->setVisible(false);
					}
				}

				sp_menu->setVisible(false);
        	}
    	}
	);
}

void SpMenu::open_sp_callback(CCObject*) { // open start pos menu
	if (!menuOpen) {
		menuOpen = true;

		PlayLayer* playLayer = PlayLayer::get();

		PlayerObject* plr = playLayer->m_player1;

		float levelLength = playLayer->m_endPortal->getPositionX();

		if (!plr->m_startPosition.isZero()) {
			startPos = floor((plr->m_startPosition.x / levelLength) * 100);
		}
		else {
			startPos = 0;
		}

		std::string sp_str = std::to_string(startPos);
		std::string sp_title = "StartPos - " + sp_str + "%";

		sp_menu_title->setString(sp_title.c_str());

		std::string deafenAtString = readFromFile(true);

		deafenAt->setString(deafenAtString.c_str());

		std::string undeafenAtString = readFromFile(false);

		undeafenAt->setString(undeafenAtString.c_str());

		for (CCNode* node : sp_menu_sprites) {
			if (node != nullptr) {
				node->setVisible(true);
			}
		}

		sp_menu->setVisible(true);
	}
	else {
		close_sp_callback();
	}
}

class $modify(PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		paused = true;

		scene = this;

		auto director = CCDirector::sharedDirector();
		auto size = director->getWinSize();

		auto spr = CCSprite::create("startPos_button_ad+.png");
		spr->setScale(0.3f);
	
		auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpMenu::open_sp_callback));
		CCMenu* menu = CCMenu::create();
		menu->setPosition({ 37, 37 });
		menu->addChild(btn);

		this->addChild(menu);

		auto level = PlayLayer::get();

		if (!level->m_player1->m_isPlatformer) {
			levelID = std::to_string(level->m_level->m_levelID);

			if (levelID == "0") {
				levelID = level->m_level->m_levelName;
			}

			PlayLayer* playLayer = PlayLayer::get();

			PlayerObject* plr = playLayer->m_player1;

			float levelLength = playLayer->m_endPortal->getPositionX();

			if (!plr->m_startPosition.isZero()) {
				startPos = floor((plr->m_startPosition.x / levelLength) * 100);
			}
			else {
				startPos = 0;
			}
			
			CCSprite* frame = CCSprite::create("GJ_square01-uhd.png");
			frame->setPosition({ xOffset, size.height * 0.7f });
			frame->setScale(2);
			frame->setZOrder(100);
			this->addChild(frame);

			sp_menu_sprites[0] = frame;

			CCSprite* closeButton = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
			closeButton->setZOrder(100);
			closeButton->setPosition({ xOffset, (size.height * 0.7f) + 70 });

			sp_menu_sprites[1] = closeButton;

			std::string sp_str = std::to_string(startPos);

			std::string sp_title = "StartPos - " + sp_str + "%";

			auto title = CCLabelBMFont::create(sp_title.c_str(), "goldFont.fnt");
			title->setPosition({ xOffset, (size.height * 0.7f) + 60 });
			title->setScale(0.65f);
			title->setZOrder(101);
			this->addChild(title);

			sp_menu_title = title;

			sp_menu_sprites[3] = title;

			auto deafenAtText = CCLabelBMFont::create("Deafen At:", "bigFont.fnt");
			deafenAtText->setPosition({ xOffset, (size.height * 0.7f) + 40 });
			deafenAtText->setScale(0.5f);
			deafenAtText->setZOrder(101);

			this->addChild(deafenAtText);

			sp_menu_sprites[4] = deafenAtText;

			auto deafenAtInputFrame = CCSprite::create("GJ_button_01-uhd.png");
			deafenAtInputFrame->setPosition({ xOffset, (size.height * 0.7f) + 16 });
			deafenAtInputFrame->setScaleX(1);
			deafenAtInputFrame->setScaleY(.7f);
			deafenAtInputFrame->setZOrder(101);
			this->addChild(deafenAtInputFrame);

			sp_menu_sprites[5] = deafenAtInputFrame;

			auto un_deafenAtText = CCLabelBMFont::create("Undeafen At:", "bigFont.fnt");
			un_deafenAtText->setPosition({ xOffset, (size.height * 0.7f) - 6 });
			un_deafenAtText->setScale(0.5f);
			un_deafenAtText->setZOrder(101);

			this->addChild(un_deafenAtText);

			sp_menu_sprites[6] = un_deafenAtText;

			auto un_deafenAtInputFrame = CCSprite::create("GJ_button_01-uhd.png");
			un_deafenAtInputFrame->setPosition({ xOffset, (size.height * 0.7f) - 30 });
			un_deafenAtInputFrame->setScaleX(1);
			un_deafenAtInputFrame->setScaleY(.7f);
			un_deafenAtInputFrame->setZOrder(101);
			this->addChild(un_deafenAtInputFrame);

			sp_menu_sprites[7] = un_deafenAtInputFrame;

			std::string deafenAtString = readFromFile(true);

			auto deafenAtInput = CCTextInputNode::create(35, 50, "", "bigFont.fnt");
			deafenAtInput->setPosition({ xOffset, (size.height * 0.7f) + 20 });
			deafenAtInput->setZOrder(101);
			deafenAtInput->setAllowedChars("0123456789.");
			deafenAtInput->setString(deafenAtString.c_str());

			sp_menu_sprites[8] = deafenAtInput;

			deafenAt = deafenAtInput;

			std::string undeafenAtString = readFromFile(false);

			auto un_deafenAtInput = CCTextInputNode::create(35, 50, "", "bigFont.fnt");
			un_deafenAtInput->setPosition({ xOffset, (size.height * 0.7f) - 26 });
			un_deafenAtInput->setZOrder(101);
			un_deafenAtInput->setAllowedChars("0123456789.");
			un_deafenAtInput->setString(undeafenAtString.c_str());

			sp_menu_sprites[9] = un_deafenAtInput;

			undeafenAt = un_deafenAtInput;

			auto deleteButton = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
			deleteButton->setScale(0.8f);
			deleteButton->setZOrder(101);

			sp_menu_sprites[10] = deleteButton;

			auto closeBtn = CCMenuItemSpriteExtra::create(closeButton, this, menu_selector(SpMenu::close_sp_callbackBtn));
			closeBtn->setPosition({ 160, 300 });
			auto deleteBtn = CCMenuItemSpriteExtra::create(deleteButton, this, menu_selector(SpMenu::deleteSpConfirm));
			deleteBtn->setPosition({ 160, 200 });

			CCMenu* spMenu = CCMenu::create();
			spMenu->setPosition({ 0, 0 });
			spMenu->setZOrder(102);
			spMenu->setTouchPriority(100);
			spMenu->addChild(closeBtn);
			spMenu->addChild(deleteBtn);
			spMenu->addChild(deafenAtInput);
			spMenu->addChild(un_deafenAtInput);
			this->addChild(spMenu);

			sp_menu = spMenu;

			spMenu->setVisible(false);

			for (CCNode* node : sp_menu_sprites) {
				if (node != nullptr) {
					node->setVisible(false);
				}
			}
		}
	}

	void onQuit(CCObject* sender) {
		PauseLayer::onQuit(sender);

        paused = false;
    }

	void onResume(CCObject* sender) {
		PauseLayer::onResume(sender);

        paused = false;
    }
};
