#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "pch.h"
#include "PauseLayer.h"

using namespace std;

gd::CCTextInputNode* deafenAt;
gd::CCTextInputNode* undeafenAt;

string default_at = "";
string default_unat = "100";

string levelID;
int startPos;

string folderName_ = "Deafen_Presets";

bool menuOpen = false;

CCLayer* layer;

CCNode* sp_menu_sprites[20] = {};
CCMenu* sp_menu;

CCLabelBMFont* sp_menu_title;

float xOffset = 80;

string substring(string str, int start, int end) {
	return str.substr(start, end - start);
}

string readFromFile(bool deafen) {
	string presetName = "preset" + levelID + ".txt";

	if (filesystem::exists(folderName_ + "\\" + presetName)) {
		ifstream inFile;
		inFile.open(folderName_ + "\\" + presetName);

		string fileContents;

		inFile >> fileContents;

		if ((fileContents.find("%" + to_string(startPos) + "%")) != string::npos) {
			int deafenParams = fileContents.find("%" + to_string(startPos) + "%");

			int openParams = fileContents.find("{", deafenParams);
			int closeParams = fileContents.find("}", openParams);

			string subbedString = substring(fileContents, openParams + 1, closeParams);

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
	string presetName = "preset" + levelID + ".txt";

	ifstream inFile;
	inFile.open(folderName_ + "\\" + presetName);

	string fileContents;

	inFile >> fileContents;

	inFile.close();

	ofstream outFile;
	outFile.open(folderName_ + "\\" + presetName);

	outFile.clear();

	string deafAt = deafenAt->getString();
	string undeafAt = undeafenAt->getString();

	if ((fileContents.find("%" + to_string(startPos) + "%")) == string::npos) {
		if (deafAt.length() != 0 && undeafAt.length() != 0) {
			fileContents += ("%" + to_string(startPos) + "%{deafen=" + deafAt + ",undeafen=" + undeafAt + "}");
		}
		else {
			outFile << fileContents;
			outFile.close();

			return false;
		}
	}
	else {
		int deafenParams = fileContents.find("%" + to_string(startPos) + "%");

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

class SpMenu : public CCLayer {
public:
	void open_sp_callback(CCObject*);
	void close_sp_callbackBtn(CCObject*);
	void deleteSpConfirm(CCObject*);
};

class DeleteAlertInfo : public CCLayer, public gd::FLAlertLayerProtocol {
protected:
	virtual bool init();
	void FLAlert_Clicked(gd::FLAlertLayer*, bool btn2) override;
public:
	static DeleteAlertInfo* create();
};

DeleteAlertInfo* DeleteAlertInfo::create() { // doppler is gay
	auto ret = new DeleteAlertInfo();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool DeleteAlertInfo::init() {
	string spStr = to_string(startPos);

	string alertTitle = spStr + "% StartPos";

	auto flalert = gd::FLAlertLayer::create(this, alertTitle.c_str(), "Cancel", "Yes", "Are you sure you want to delete this StartPos' Deafen?");
	flalert->show();

	return true;
}

void DeleteAlertInfo::FLAlert_Clicked(gd::FLAlertLayer* alertLayer, bool btn2) {
	if (btn2) {
		// confirm code

		string presetName = "preset" + levelID + ".txt";

		ifstream inFile;
		inFile.open(folderName_ + "\\" + presetName);

		string fileContents;

		inFile >> fileContents;

		inFile.close();

		ofstream outFile;
		outFile.open(folderName_ + "\\" + presetName);

		outFile.clear();

		int deafenParams = fileContents.find("%" + to_string(startPos) + "%");

		ofstream console;
		console.open("Console.txt");

		console.clear();

		if (deafenParams != string::npos) {
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

void SavePreset() {
	if (!filesystem::exists(folderName_)) { // If presets folder doesn't exist, create it
		filesystem::create_directory(folderName_);
	}

	writeToFile();
}

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
	DeleteAlertInfo::create();
}

void SpMenu::open_sp_callback(CCObject*) { // open start pos menu
	if (!menuOpen) {
		menuOpen = true;
		gd::PlayLayer* playLayer = gd::PlayLayer::get();

		float levelLength = playLayer->m_endPortal->getPositionX();

		if (playLayer->m_startPos) {
			startPos = floor((playLayer->m_startPos->getStartPosition().x / levelLength) * 100);
		}
		else {
			startPos = 0;
		}

		string sp_str = to_string(startPos);
		string sp_title = "StartPos - " + sp_str + "%";

		sp_menu_title->setString(sp_title.c_str());

		string deafenAtString = readFromFile(true);

		deafenAt->setString(deafenAtString.c_str());

		string undeafenAtString = readFromFile(false);

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

bool __fastcall PauseLayer::hook(CCLayer* self) {
	PauseLayer::undeafen();

	menuOpen = false;

	layer = self;

	bool result = PauseLayer::init(self);
	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	CCSprite* button = CCSprite::create("startPos_button_ad+.png");
	button->setScale(0.3f);

	auto nextButton = gd::CCMenuItemSpriteExtra::create(button, self, menu_selector(SpMenu::open_sp_callback));

	CCMenu* menu = CCMenu::create();
	menu->setPosition({ 37, 37 });
	menu->addChild(nextButton);
	self->addChild(menu);

	auto level = gd::PlayLayer::get();

	levelID = to_string(level->m_level->m_nLevelID);

	if (levelID == "0") {
		levelID = level->m_level->m_sLevelName;
	}

	gd::PlayLayer* playLayer = gd::PlayLayer::get();

	float levelLength = playLayer->m_endPortal->getPositionX();

	if (playLayer->m_startPos) {
		startPos = floor((playLayer->m_startPos->getStartPosition().x / levelLength) * 100);
	}
	else {
		startPos = 0;
	}

	CCSprite* frame = CCSprite::create("GJ_square01-uhd.png");
	frame->setPosition({ xOffset, size.height * 0.7f });
	frame->setScale(2);
	frame->setZOrder(100);
	layer->addChild(frame);

	sp_menu_sprites[0] = frame;

	CCSprite* closeButton = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
	closeButton->setZOrder(100);
	closeButton->setPosition({ xOffset, (size.height * 0.7f) + 70 });

	sp_menu_sprites[1] = closeButton;

	string sp_str = to_string(startPos);

	string sp_title = "StartPos - " + sp_str + "%";

	auto title = CCLabelBMFont::create(sp_title.c_str(), "goldFont.fnt");
	title->setPosition({ xOffset, (size.height * 0.7f) + 60 });
	title->setScale(0.65f);
	title->setZOrder(101);
	layer->addChild(title);

	sp_menu_title = title;

	sp_menu_sprites[3] = title;

	auto deafenAtText = CCLabelBMFont::create("Deafen At:", "bigFont.fnt");
	deafenAtText->setPosition({ xOffset, (size.height * 0.7f) + 40 });
	deafenAtText->setScale(0.5f);
	deafenAtText->setZOrder(101);

	layer->addChild(deafenAtText);

	sp_menu_sprites[4] = deafenAtText;

	auto deafenAtInputFrame = CCSprite::create("GJ_button_01-uhd.png");
	deafenAtInputFrame->setPosition({ xOffset, (size.height * 0.7f) + 16 });
	deafenAtInputFrame->setScaleX(1);
	deafenAtInputFrame->setScaleY(.7f);
	deafenAtInputFrame->setZOrder(101);
	layer->addChild(deafenAtInputFrame);

	sp_menu_sprites[5] = deafenAtInputFrame;

	auto un_deafenAtText = CCLabelBMFont::create("Undeafen At:", "bigFont.fnt");
	un_deafenAtText->setPosition({ xOffset, (size.height * 0.7f) - 6 });
	un_deafenAtText->setScale(0.5f);
	un_deafenAtText->setZOrder(101);

	layer->addChild(un_deafenAtText);

	sp_menu_sprites[6] = un_deafenAtText;

	auto un_deafenAtInputFrame = CCSprite::create("GJ_button_01-uhd.png");
	un_deafenAtInputFrame->setPosition({ xOffset, (size.height * 0.7f) - 30 });
	un_deafenAtInputFrame->setScaleX(1);
	un_deafenAtInputFrame->setScaleY(.7f);
	un_deafenAtInputFrame->setZOrder(101);
	layer->addChild(un_deafenAtInputFrame);

	sp_menu_sprites[7] = un_deafenAtInputFrame;

	string deafenAtString = readFromFile(true);

	auto deafenAtInput = gd::CCTextInputNode::create("", nullptr, "bigFont.fnt", 35, 50);
	deafenAtInput->setPosition({ xOffset, (size.height * 0.7f) + 20 });
	deafenAtInput->setZOrder(101);
	deafenAtInput->setAllowedChars("0123456789.");
	deafenAtInput->setString(deafenAtString.c_str());

	sp_menu_sprites[8] = deafenAtInput;

	deafenAt = deafenAtInput;

	string undeafenAtString = readFromFile(false);

	auto un_deafenAtInput = gd::CCTextInputNode::create("", nullptr, "bigFont.fnt", 35, 50);
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

	auto closeBtn = gd::CCMenuItemSpriteExtra::create(closeButton, layer, menu_selector(SpMenu::close_sp_callbackBtn));
	closeBtn->setPosition({ 160, 300 });
	auto deleteBtn = gd::CCMenuItemSpriteExtra::create(deleteButton, layer, menu_selector(SpMenu::deleteSpConfirm));
	deleteBtn->setPosition({ 160, 200 });

	CCMenu* spMenu = CCMenu::create();
	spMenu->setPosition({ 0, 0 });
	spMenu->setZOrder(102);
	spMenu->setTouchPriority(100);
	spMenu->addChild(closeBtn);
	spMenu->addChild(deleteBtn);
	spMenu->addChild(deafenAtInput);
	spMenu->addChild(un_deafenAtInput);
	layer->addChild(spMenu);

	sp_menu = spMenu;

	spMenu->setVisible(false);
	
	for (CCNode* node : sp_menu_sprites) {
		if (node != nullptr) {
			node->setVisible(false);
		}
	}

	return result;
}