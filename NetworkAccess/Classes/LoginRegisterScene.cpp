#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;
using namespace rapidjson;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
	auto request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl("http://127.0.0.1:8000/auth");
	getRequest(request);
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
	auto request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl("http://127.0.0.1:8000/users");
	getRequest(request);
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::getRequest(HttpRequest *request)
{
	Document data;
	data.SetObject();
	rapidjson::Value username(usernameInput->getString().c_str(), data.GetAllocator());
	rapidjson::Value password(passwordInput->getString().c_str(), data.GetAllocator());
	data.AddMember("username", username, data.GetAllocator());
	data.AddMember("password", password, data.GetAllocator());
	auto str = serializeRapidjsonDocument(data);
	//std::string data = "{\n \"username\":\"" + usernameInput->getString() + "\",\n \"password\":\"" + passwordInput->getString() + "\"\n}";
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
		} else {
			res += "fail  ";
		}
		messageBox->setString(res + doc["msg"].GetString());
	});
}
