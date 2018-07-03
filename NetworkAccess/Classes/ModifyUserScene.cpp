#include "ModifyUserScene.h"
#include "Utils.h"
#include "network\HttpClient.h"
#include "json\document.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * ModifyUserScene::createScene() {
  return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
  if (!Scene::init()) return false;
  
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
  if (postDeckButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + postDeckButton->getContentSize().height / 2;
    postDeckButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(postDeckButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  deckInput = TextField::create("Deck json here", "arial", 24);
  if (deckInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    deckInput->setPosition(Vec2(x, y));
    this->addChild(deckInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
	auto request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::PUT);
	request->setUrl("http://127.0.0.1:8000/users");

	auto str = deckInput->getString();

	str = "{\"deck\":" + str + "}";

	Document data;
	data.Parse(str.c_str());
	if (data.HasParseError() || !data["deck"].IsArray()) {
		messageBox->setString("format error");
		return;
	}

	request->setRequestData(str.c_str(), str.length());
	request->setResponseCallback([=](HttpClient *sender, HttpResponse *response) {
		if (!response) {
			messageBox->setString("fail to access server");
			return;
		}
		else if (!response->isSucceed()) {
			messageBox->setString("bad request");
			return;
		}
		auto buffer = response->getResponseData();
		// dont know why there are some useless words in the tail using data()
		std::string str;
		for (auto i : *buffer)
			str += i;

		Document doc;
		doc.Parse(str.c_str());
		CCLOG(serializeRapidjsonDocument(doc).c_str());
		std::string res;
		if (doc["status"] == true) {
			res += "success  ";
		}
		else {
			res += "fail  ";
		}
		messageBox->setString(res + doc["msg"].GetString());
	});
	HttpClient::getInstance()->send(request);
	request->release();
}
