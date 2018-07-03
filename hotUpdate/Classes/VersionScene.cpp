#include "VersionScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;

cocos2d::Scene * VersionScene::createScene() {
  return VersionScene::create();
}

bool VersionScene::init() {
  if (!Scene::init()) return false;

  //load(names, deck);
  //save("winged kuriboh", "winged_kuriboh.jpg");
  //version = loadVersion();

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto upgradeButton = MenuItemFont::create("Upgrade", CC_CALLBACK_1(VersionScene::upgradeVersion, this));
  if (upgradeButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + upgradeButton->getContentSize().height / 2;
    upgradeButton->setPosition(Vec2(x, y));
  }

  auto leftButton = MenuItemFont::create("<-", [this](Ref *sender) { this->switchPicture(true); });
  if (leftButton) {
    float x = origin.x + visibleSize.width / 2 - 100;
    float y = origin.y + leftButton->getContentSize().height / 2 + 100;
    leftButton->setPosition(Vec2(x, y));
  }

  auto rightButton = MenuItemFont::create("->", [this](Ref *sender) { this->switchPicture(false); });
  if (rightButton) {
    float x = origin.x + visibleSize.width / 2 + 100;
    float y = origin.y + rightButton->getContentSize().height / 2 + 100;
    rightButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(upgradeButton, backButton, leftButton, rightButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  versionInput = TextField::create("Version Here", "arial", 24);
  if (versionInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 80.0f;
    versionInput->setPosition(Vec2(x, y));
    this->addChild(versionInput, 1);
  }

  picture = Sprite::create("winged_kuriboh.jpg");
  if (picture) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2 - 20;
    picture->setPosition(Vec2(x, y));
	if (std::find(deck.begin(), deck.end(), std::string("winged_kuriboh.jpg")) == deck.end()) {
		deck.push_back("winged_kuriboh.jpg");
	}
    this->addChild(picture);
  }

  pictureName = Label::create();
  if (pictureName) {
	pictureName->setString("winged kuriboh");
	if (std::find(names.begin(), names.end(), std::string("winged kuriboh")) == names.end()) {
		names.push_back("winged kuriboh");
	}
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + 70.0f;
    pictureName->setPosition(Vec2(x, y));
    this->addChild(pictureName);
  }

  return true;
}

void VersionScene::upgradeVersion(Ref *sender) {
	auto nextversion = versionInput->getString();
	auto request = new HttpRequest();
	if (nextversion != "") {
		request->setUrl("https://easy-mock.com/mock/5b06434130326c5f05624462/api/assets?current_version="
			+ version + "&to_version=" + nextversion);
	} else {
		request->setUrl("https://easy-mock.com/mock/5b06434130326c5f05624462/api/assets?current_version="
			+ version);
	}

	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback([=](HttpClient *client, HttpResponse *response) {
		if (!response || !response->isSucceed()) {
			return;
		}
		auto raw = response->getResponseData();
		// CCLOG(raw->data());
		std::string res("");
		for (auto c : *raw) {
			res += c;
		}
		//std::string res = raw->data();
		//res[raw->size()] = '\0';
		rapidjson::Document doc;
		doc.Parse(res.c_str());
		//CCLOG(serializeRapidjsonDocument(doc).c_str());
		if (doc["status"].GetBool() == false) {
			return;
		}
		upgradeWithJson(doc);
	});
	HttpClient::getInstance()->send(request);
	request->release();
}

void VersionScene::switchPicture(bool isLeft) {
	if (isLeft && currentPicIndex > 0) --currentPicIndex;
	else if (!isLeft && currentPicIndex < deck.size() - 1) ++currentPicIndex;
	picture->setTexture(deck.at(currentPicIndex));
	pictureName->setString(names.at(currentPicIndex));
}

void VersionScene::upgradeWithJson(const rapidjson::Document &doc) {
	auto data = doc.FindMember("data");
	if (data != doc.MemberEnd()) {
		auto nv = data->value["new_version"].GetString();
		if (atof(nv) <= atof(version.c_str())) {
			return;
		}
		version = nv;
		saveVersion(version);
		auto assets = data->value.FindMember("assets");
		if (assets != data->value.MemberEnd()) {
			auto cards = assets->value.FindMember("cards");
			if (cards != assets->value.MemberEnd()) {
				for (auto itr = cards->value.MemberBegin();
					itr != cards->value.MemberEnd(); ++itr) {
					auto filename = itr->value["filename"].GetString();
					auto url = itr->value["url"].GetString();
					download(itr->name.GetString(), filename, url);
				}
			}
		}
	}
}

void VersionScene::download(std::string name, std::string filename, std::string url) {
	auto request = new HttpRequest();
	request->setUrl(url.c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback([=](HttpClient *client, HttpResponse *response) {
		if (!response || !response->isSucceed()) {
			return;
		}
		auto buffer = response->getResponseData();
		auto path = FileUtils::getInstance()->getWritablePath();
		auto fullpath = path + filename;
		writeFile(fullpath.c_str(), buffer->data(), buffer->size());
		this->deck.push_back(fullpath);
		this->names.push_back(name);
		save(name, fullpath);
	});
	HttpClient::getInstance()->send(request);
	request->release();
}