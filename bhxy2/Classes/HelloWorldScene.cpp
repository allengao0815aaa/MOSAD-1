#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#include "Player.h"

#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
	return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}
    
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	TMXTiledMap *tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);

	auto player = Player::getInstance();
	auto sprite = player->getSprite();

	sprite->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(sprite, 3);

	// hp bar
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	std::string font("fonts/arial.ttf");
	float fontSize = 36;

	initMenuItem(font, fontSize);

	auto factory = Factory::getInstance();
	for (size_t i = 0; i < 5; ++i)
	{
		auto monster = factory->createMonster();
		Vec2 point(RandomHelper::random_real(origin.x, visibleSize.width), RandomHelper::random_real(origin.y, visibleSize.height));
		monster->setPosition(Vec2(point));
		this->addChild(monster);
	}

	grade = Label::createWithTTF(std::to_string(player->getGrade()), font, fontSize);
	grade->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 130));
	this->addChild(grade, 2);

	// time
	dtime = 100;
	time = Label::createWithTTF(std::to_string(dtime), font, fontSize);
	time->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 80));
	this->addChild(time, 2);
	
	initSchedule();

	return true;
}

void HelloWorld::initSchedule() 
{
	// timer
	schedule([this](float i) -> void {
		if (--this->dtime >= 0) {
			this->time->setString(std::to_string(this->dtime));

		}
		if (this->dtime == 0)
		{
			// stop timer
			unschedule("timer");

			if (Player::getInstance()->getHp() != 0)
			{
				// hp bar
				auto progress = ProgressFromTo::create(0.5f, this->pT->getPercentage(), 0);
				this->pT->runAction(progress);

				Player::getInstance()->die();
				this->grade->setString(std::to_string(Player::getInstance()->getGrade()));
			}
		}
	}, 1, "timer");

	schedule([this](float i) -> void {
		auto player = Player::getInstance();
		if (this->dtime == 0 || player->getHp() == 0)
		{
			unscheduleAllCallbacks();
			return;
		}
		auto factory = Factory::getInstance();
		auto hitSprite = factory->collider(player->getSprite()->getBoundingBox());
		if (hitSprite != NULL)
		{
			factory->removeMonster(hitSprite);

			auto monster = factory->createMonster();
			Vec2 point(RandomHelper::random_real(this->origin.x, this->visibleSize.width),
				RandomHelper::random_real(this->origin.y, this->visibleSize.height));
			monster->setPosition(Vec2(point));
			this->addChild(monster);

			float hp = player->damage(25);
			auto progress = ProgressFromTo::create(0.5f, this->pT->getPercentage(), hp);
			this->pT->runAction(progress);
			this->grade->setString(std::to_string(player->getGrade()));
		}
	}, 0.1, "check");

	schedule(CC_CALLBACK_1(HelloWorld::updateSprite, this), 3, "monsterTimer");
}

void HelloWorld::initMenuItem(std::string font, float fontSize)
{
	// left menu
	auto Wb = MenuItemLabel::create(Label::createWithTTF("W", font, fontSize), CC_CALLBACK_1(HelloWorld::moveSprite, this));
	Wb->setPosition(Vec2(origin.x + Wb->getContentSize().width * 1.5, origin.y + Wb->getContentSize().height * 1.5));

	auto Ab = MenuItemLabel::create(Label::createWithTTF("A", font, fontSize), CC_CALLBACK_1(HelloWorld::moveSprite, this));
	Ab->setPosition(Vec2(origin.x + Wb->getContentSize().width / 2, origin.y + Wb->getContentSize().height / 2));

	auto Sb = MenuItemLabel::create(Label::createWithTTF("S", font, fontSize), CC_CALLBACK_1(HelloWorld::moveSprite, this));
	Sb->setPosition(Vec2(origin.x + Wb->getContentSize().width * 1.5, origin.y + Wb->getContentSize().height / 2));

	auto Db = MenuItemLabel::create(Label::createWithTTF("D", font, fontSize), CC_CALLBACK_1(HelloWorld::moveSprite, this));
	Db->setPosition(Vec2(origin.x + Wb->getContentSize().width * 2.5, origin.y + Wb->getContentSize().height / 2));

	auto leftMenu = Menu::createWithArray(Vector<MenuItem *>{ Wb, Ab, Sb, Db });
	leftMenu->setPosition(Vec2(origin.x, origin.y));
	this->addChild(leftMenu, 1);

	// right menu
	auto Xb = MenuItemLabel::create(Label::createWithTTF("X", font, fontSize), [this](Ref *sender) -> void {
		auto player = Player::getInstance();

		if (player->getHp() == 0 || this->dtime == 0) return;

		player->Attack();

		auto factory = Factory::getInstance();
		auto box = player->getSprite()->getBoundingBox();
		auto monster = factory->collider(Rect(box.getMinX() - 40, box.getMinY(),
			box.getMaxX() - box.getMinX() + 80, box.getMaxY() - box.getMinY()));

		if (monster != NULL)
		{
			factory->removeMonster(monster);
			monster = factory->createMonster();
			Vec2 point(RandomHelper::random_real(origin.x, visibleSize.width), RandomHelper::random_real(origin.y, visibleSize.height));
			monster->setPosition(Vec2(point));
			this->addChild(monster);

			float hp = player->heal(25);
			auto progress = ProgressFromTo::create(0.3f, this->pT->getPercentage(), hp);
			this->pT->runAction(progress);

			player->updateGrade();
			this->grade->setString(std::to_string(player->getGrade()));
		}
	});
	Xb->setPosition(Vec2(origin.x - Xb->getContentSize().width, origin.y + Xb->getContentSize().height * 1.5));

	auto Yb = MenuItemLabel::create(Label::createWithTTF("Y", font, fontSize), [this](Ref *sender) -> void {
		auto player = Player::getInstance();
		if (player->getHp() == 0 || this->dtime == 0) return;

		player->die();
		this->grade->setString(std::to_string(Player::getInstance()->getGrade()));

		//hp bar
		auto progress = ProgressFromTo::create(0.3f, this->pT->getPercentage(), 0);
		this->pT->runAction(progress);
	});
	Yb->setPosition(Vec2(origin.x - Yb->getContentSize().width * 2, origin.y + Yb->getContentSize().height / 2));

	auto rightMenu = Menu::createWithArray(Vector<MenuItem *>{ Xb, Yb });
	rightMenu->setPosition(Vec2(visibleSize.width, origin.y));
	this->addChild(rightMenu, 1);
}

void HelloWorld::updateSprite(float i) {
	auto factory = Factory::getInstance();
	auto player = Player::getInstance();

	if (player->getHp() == 0 || this->dtime == 0) 
	{
		unschedule("monsterTimer");
		return;
	}

	factory->moveMonster(player->getSprite()->getPosition(), 0.5);

}

void HelloWorld::moveSprite(Ref *sender)
{
	auto player = Player::getInstance();
	auto sprite = player->getSprite();

	if (player->getHp() == 0 || this->dtime == 0) return;

	MenuItemLabel *item = dynamic_cast<MenuItemLabel *>(sender);

	player->move(item->getString()[0], origin, visibleSize);
}