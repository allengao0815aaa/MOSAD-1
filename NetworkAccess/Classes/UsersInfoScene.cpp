#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
  }

  messageBox = Label::create("", "arial", 25);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
	auto request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setUrl("http://127.0.0.1:8000/users?limit=" + limitInput->getString());
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
		res += doc["msg"].GetString();
		res = res + "\n";
		if (doc["status"] == true) {
			char each[100]{ 0 };
			const rapidjson::Value& data = doc["data"];
			// each user
			for (rapidjson::SizeType i = 0; i < data.Size(); ++i) {
				sprintf(each, "%s: %s\ndeck: \n", "\nusername: ", data[i]["username"].GetString());
				res += each;
				const rapidjson::Value& deck = data[i]["deck"];
				// each deck
				for (rapidjson::SizeType j = 0; j < deck.Size(); ++j) {
					sprintf(each, "  %d:\n", j);
					res += each;
					// each card
					for (rapidjson::Value::ConstMemberIterator k = deck[j].MemberBegin(); k != deck[j].MemberEnd(); ++k) {
						sprintf(each, "    %s: %d\n", k->name.GetString(), k->value.GetInt());
						res += each;
					}
				}
			}
		}
		messageBox->setString(res);
	});

	HttpClient::getInstance()->send(request);
	request->release();
}
