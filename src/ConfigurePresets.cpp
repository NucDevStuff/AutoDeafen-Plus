#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/CCSprite.hpp>

#include <iostream>
#include <fstream>

using namespace cocos2d;
using namespace geode::prelude;

std::string folder_name = "Deafen_Presets";
std::string levelName;

std::string myClipboard = "";

class LevelEditMenu {
public:
	void delete_callback(CCObject*);
	void copy_callback(CCObject*);
	void paste_callback(CCObject*);
};

void LevelEditMenu::paste_callback(CCObject*) {
	std::string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (myClipboard.length() > 0) {
        if (!std::filesystem::exists(folder_name)) { // If presets folder doesn't exist, create it
		    std::filesystem::create_directory(folder_name);
	    }

		geode::createQuickPopup(
            "Paste Preset",   // title
            "Are you sure you want to paste this AutoDeafen+ Preset?",   // content
            "Cancel", "Yes",      // buttons
            [](auto, bool btn2) {
                if (btn2) {
                    std::string dir = folder_name + "\\" "preset" + levelName + ".txt";

                    std::ofstream outFile;
                    outFile.open(dir);

                    outFile << myClipboard;

                    outFile.close();

                    auto flalert = FLAlertLayer::create("Paste Preset", "Preset successfully pasted.", "OK");
                    flalert->show();
                }
            }
	    );
	}
	else {
		auto flalert = FLAlertLayer::create("Error", "No preset copied to Clipboard.", "OK");
		flalert->show();
	}
}

void LevelEditMenu::copy_callback(CCObject*) {
	std::string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (std::filesystem::exists(dir)) {
		std::ifstream inFile;
		inFile.open(dir);

		std::string fileContents;

		inFile >> fileContents;

		inFile.close();

		myClipboard = fileContents;

		auto flalert = FLAlertLayer::create("Copy Preset", "Preset copied to clipboard!", "OK");
		flalert->show();
	}
	else {
		auto flalert = FLAlertLayer::create("Error", "No preset found.", "OK");
		flalert->show();
	}
}

void LevelEditMenu::delete_callback(CCObject*) {
	std::string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (std::filesystem::exists(dir)) {
		geode::createQuickPopup(
            "Delete Preset",
            "Are you sure you want to delete this AutoDeafen+ Preset?",   // content
            "Cancel", "Yes",      // buttons
            [](auto, bool btn2) {
                if (btn2) {
                    std::string dir = folder_name + "\\" "preset" + levelName + ".txt";

                    std::filesystem::remove(dir);
                }
            }
	    );
	}
	else {
		auto flalert = FLAlertLayer::create("Error", "No preset found.", "OK");
		flalert->show();
	}
}

class $modify(EditLevelLayer) {
    bool init(GJGameLevel* level) {
        levelName = level->m_levelName;

        bool result = EditLevelLayer::init(level);
        auto director = CCDirector::sharedDirector();
        auto size = director->getWinSize();

        CCSprite* delete_button = CCSprite::create("delete_button_ad+.png"_spr);
        CCSprite* copy_button = CCSprite::create("copy_button_ad+.png"_spr);
        CCSprite* paste_button = CCSprite::create("paste_button_ad+.png"_spr);

        auto deleteButton = CCMenuItemSpriteExtra::create(delete_button, this, menu_selector(LevelEditMenu::delete_callback));
        deleteButton->setPosition({ 1450, 180 });

        auto copyButton = CCMenuItemSpriteExtra::create(copy_button, this, menu_selector(LevelEditMenu::copy_callback));
        copyButton->setPosition({ 1450, -20 });

        auto pasteButton = CCMenuItemSpriteExtra::create(paste_button, this, menu_selector(LevelEditMenu::paste_callback));
        pasteButton->setPosition({ 1450, -220 });

        CCMenu* edit_menu = CCMenu::create();
        edit_menu->setPosition({ 0, 0 });
        edit_menu->setScale(.15f);
        edit_menu->addChild(deleteButton);
        edit_menu->addChild(copyButton);
        edit_menu->addChild(pasteButton);
        this->addChild(edit_menu);

        return true;
    }
};

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        levelName = std::to_string(level->m_levelID);

        bool result = LevelInfoLayer::init(level, false);
        auto director = CCDirector::sharedDirector();
        auto size = director->getWinSize();

        CCSprite* delete_button = CCSprite::create("delete_button_ad+.png"_spr);
        CCSprite* copy_button = CCSprite::create("copy_button_ad+.png"_spr);
        CCSprite* paste_button = CCSprite::create("paste_button_ad+.png"_spr);

        auto deleteButton = CCMenuItemSpriteExtra::create(delete_button, this, menu_selector(LevelEditMenu::delete_callback));
        deleteButton->setPosition({ 1450, -320 });

        auto copyButton = CCMenuItemSpriteExtra::create(copy_button, this, menu_selector(LevelEditMenu::copy_callback));
        copyButton->setPosition({ 1450, -520 });

        auto pasteButton = CCMenuItemSpriteExtra::create(paste_button, this, menu_selector(LevelEditMenu::paste_callback));
        pasteButton->setPosition({ 1450, -720 });

        CCMenu* edit_menu = CCMenu::create();
        edit_menu->setPosition({ 0, 0 });
        edit_menu->setScale(.15f);
        edit_menu->addChild(deleteButton);
        edit_menu->addChild(copyButton);
        edit_menu->addChild(pasteButton);
        this->addChild(edit_menu);

        return true;
    }
};