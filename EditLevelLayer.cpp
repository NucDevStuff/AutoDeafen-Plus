#include <fstream>

#include "pch.h"
#include "EditLevelLayer.h"
#include "LevelEditLayer.h"

using namespace std;

string folder_name = "Deafen_Presets";

string levelName;

string clipboard = "";

class LevelEditMenu : public CCLayer {
public:
	void delete_callback(CCObject*);
	void copy_callback(CCObject*);
	void paste_callback(CCObject*);
};

class PasteAlertInfo : public CCLayer, public gd::FLAlertLayerProtocol {
protected:
	virtual bool init();
	void FLAlert_Clicked(gd::FLAlertLayer*, bool btn2) override;
public:
	static PasteAlertInfo* create();
};

PasteAlertInfo* PasteAlertInfo::create() {
	auto ret = new PasteAlertInfo();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool PasteAlertInfo::init() {
	auto flalert = gd::FLAlertLayer::create(this, "Paste Preset", "Cancel", "Yes", "Are you sure you want to paste this AutoDeafen+ Preset?");
	flalert->show();

	return true;
}

void PasteAlertInfo::FLAlert_Clicked(gd::FLAlertLayer* alertLayer, bool btn2) {
	if (btn2) {
		string dir = folder_name + "\\" "preset" + levelName + ".txt";

		ofstream outFile;
		outFile.open(dir);

		outFile << clipboard;

		outFile.close();

		auto flalert = gd::FLAlertLayer::create(nullptr, "Paste Preset", "OK", nullptr, "Preset successfuly pasted.");
		flalert->show();
	}
}

class DeletePresetAlertInfo : public CCLayer, public gd::FLAlertLayerProtocol {
protected:
	virtual bool init();
	void FLAlert_Clicked(gd::FLAlertLayer*, bool btn2) override;
public:
	static DeletePresetAlertInfo* create();
};

DeletePresetAlertInfo* DeletePresetAlertInfo::create() {
	auto ret = new DeletePresetAlertInfo();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool DeletePresetAlertInfo::init() {
	auto flalert = gd::FLAlertLayer::create(this, "Delete Preset", "Cancel", "Yes", "Are you sure you want to delete this AutoDeafen+ Preset?");
	flalert->show();

	return true;
}

void DeletePresetAlertInfo::FLAlert_Clicked(gd::FLAlertLayer* alertLayer, bool btn2) {
	if (btn2) {
		string dir = folder_name + "\\" "preset" + levelName + ".txt";

		filesystem::remove(dir);
	}
}

void LevelEditMenu::paste_callback(CCObject*) {
	string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (clipboard.length() > 0) {
		PasteAlertInfo::create();
	}
	else {
		auto flalert = gd::FLAlertLayer::create(nullptr, "Error", "OK", nullptr, "No preset copied to clipboard.");
		flalert->show();
	}
}

void LevelEditMenu::copy_callback(CCObject*) {
	string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (filesystem::exists(dir)) {
		ifstream inFile;
		inFile.open(dir);

		string fileContents;

		inFile >> fileContents;

		inFile.close();

		clipboard = fileContents;

		auto flalert = gd::FLAlertLayer::create(nullptr, "Copy Preset", "OK", nullptr, "Preset copied to clipboard!");
		flalert->show();
	}
	else {
		auto flalert = gd::FLAlertLayer::create(nullptr, "Error", "OK", nullptr, "No preset found.");
		flalert->show();
	}
}

void LevelEditMenu::delete_callback(CCObject*) {
	string dir = folder_name + "\\" "preset" + levelName + ".txt";

	if (filesystem::exists(dir)) {
		DeletePresetAlertInfo::create();
	}
	else {
		auto flalert = gd::FLAlertLayer::create(nullptr, "Error", "OK", nullptr, "No preset found.");
		flalert->show();
	}
}

bool __fastcall EditLevelLayer::hook(CCLayer* self, void*, gd::GJGameLevel* level) {
	levelName = level->m_sLevelName;

	bool result = EditLevelLayer::init(self, level);
	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	CCSprite* delete_button = CCSprite::create("delete_button_ad+.png");
	CCSprite* copy_button = CCSprite::create("copy_button_ad+.png");
	CCSprite* paste_button = CCSprite::create("paste_button_ad+.png");

	auto deleteButton = gd::CCMenuItemSpriteExtra::create(delete_button, self, menu_selector(LevelEditMenu::delete_callback));
	deleteButton->setPosition({ 1450, 180 });

	auto copyButton = gd::CCMenuItemSpriteExtra::create(copy_button, self, menu_selector(LevelEditMenu::copy_callback));
	copyButton->setPosition({ 1450, -20 });

	auto pasteButton = gd::CCMenuItemSpriteExtra::create(paste_button, self, menu_selector(LevelEditMenu::paste_callback));
	pasteButton->setPosition({ 1450, -220 });

	CCMenu* edit_menu = CCMenu::create();
	edit_menu->setPosition({ 0, 0 });
	edit_menu->setScale(.15f);
	edit_menu->addChild(deleteButton);
	edit_menu->addChild(copyButton);
	edit_menu->addChild(pasteButton);
	self->addChild(edit_menu);

	return true;
}

bool __fastcall LevelEditLayer::hook(CCLayer* self, void*, gd::GJGameLevel* level) {
	levelName = to_string(level->m_nLevelID);

	bool result = LevelEditLayer::init(self, level);
	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	CCSprite* delete_button = CCSprite::create("delete_button_ad+.png");
	CCSprite* copy_button = CCSprite::create("copy_button_ad+.png");
	CCSprite* paste_button = CCSprite::create("paste_button_ad+.png");

	auto deleteButton = gd::CCMenuItemSpriteExtra::create(delete_button, self, menu_selector(LevelEditMenu::delete_callback));
	deleteButton->setPosition({ 1450, -320 });

	auto copyButton = gd::CCMenuItemSpriteExtra::create(copy_button, self, menu_selector(LevelEditMenu::copy_callback));
	copyButton->setPosition({ 1450, -520 });

	auto pasteButton = gd::CCMenuItemSpriteExtra::create(paste_button, self, menu_selector(LevelEditMenu::paste_callback));
	pasteButton->setPosition({ 1450, -720 });

	CCMenu* edit_menu = CCMenu::create();
	edit_menu->setPosition({ 0, 0 });
	edit_menu->setScale(.15f);
	edit_menu->addChild(deleteButton);
	edit_menu->addChild(copyButton);
	edit_menu->addChild(pasteButton);
	self->addChild(edit_menu);

	return true;
}