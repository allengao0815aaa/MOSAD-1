#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	auto ccLayer = CCLayerColor::create(ccc4(229, 249, 255, 255));

	scene->addChild(ccLayer);

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto title = Label::createWithSystemFont("Hello World", "Consolas", 24);

	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - title->getContentSize().height));
	title->setTextColor(Color4B::BLACK);

	this->addChild(title, 1);

	auto strings = Dictionary::createWithContentsOfFile("zhcn.xml");
	const char *str = (dynamic_cast<String*>(strings->objectForKey("name")))->getCString();
	auto name = Label::createWithSystemFont(str, "Consolas", 20);
	name->setName("name");

	auto posX = title->getPositionX();
	auto posY = title->getPositionY() - title->getContentSize().height / 2 - 10 - name->getContentSize().height / 2;

	name->setPosition(Vec2(posX, posY));
	name->setTextColor(Color4B::BLACK);

	this->addChild(name, 2);

	auto img = Sprite::create("img.png");

	img->setPosition(Vec2(name->getPositionX(),
		name->getPositionY() - 30 - img->getContentSize().height / 2));

	this->addChild(img, -1);

	Vector<MenuItem*> menus;
	auto closeItem = MenuItemImage::create(
		"close.png",
		"close.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2 + 5));

	auto changeItem = MenuItemImage::create("change.png", "change.png", [this](Ref *sender) -> void {
		auto name = this->getChildByName("name");
		if (name != NULL && name->isVisible())
		{
			name->setVisible(false);
		}
		else
		{
			name->setVisible(true);
		}
	});

	changeItem->setPosition(Vec2(origin.x + visibleSize.width - changeItem->getContentSize().width / 2,
		origin.y + changeItem->getContentSize().height / 2 + closeItem->getContentSize().height + 20));

	menus.pushBack(changeItem);
	menus.pushBack(closeItem);

	auto menu = Menu::createWithArray(menus);
	menu->setPosition(Vec2::ZERO);

	this->addChild(menu, 1);

	return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}