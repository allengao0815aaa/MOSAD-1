#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	return Scene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	stoneLayer = Layer::create();
	stoneLayer->setPosition(origin.x, origin.y);
	this->addChild(stoneLayer, 1);

	stone = Sprite::createWithSpriteFrameName("stone-1.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(stone, 2);

	mouseLayer = Layer::create();
	mouseLayer->setPosition(origin.x, origin.y + visibleSize.height / 2);
	this->addChild(mouseLayer, 3);

	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	mouse->setPosition(Vec2(visibleSize.width / 2 + origin.x, 0));
	mouseLayer->addChild(mouse, 4);

	shootLabel = Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 50);
	auto menuLabel = MenuItemLabel::create(shootLabel, CC_CALLBACK_1(GameScene::shootMenuCallback, this));
	auto menu = Menu::create(menuLabel, NULL);
	menu->setPosition(origin.x + visibleSize.width - 100, stone->getPositionY());
	this->addChild(menu, 4);

	//EventListenerTouchOneByOne *shoot_listener = EventListenerTouchOneByOne::create();
	//shoot_listener->setSwallowTouches(true);
	//shoot_listener->onTouchBegan = CC_CALLBACK_2(GameScene::shootMenuCallback, this);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(shoot_listener, shootLabel);

	return true;
}



bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {
	auto location = touch->getLocation();
	if (onRun /*|| location.y > 430*/) {
		return false;
	}

	onRun = true;

	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	cheese->setName("cheese");
	this->addChild(cheese, 2);

	// move
	auto mouseMove = MoveTo::create(2, this->mouse->getParent()->convertToNodeSpace(location));
	auto mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	// animation
	auto repeat = RepeatForever::create(mouseAnimate);
	mouse->runAction(repeat);

	auto callback = CallFunc::create([this, repeat]() {
		this->removeChildByName("cheese");
		this->mouse->stopAction(repeat);
		this->onRun = false;
	});

	auto seq = Sequence::create(mouseMove, callback, NULL);
	mouse->runAction(seq);
	return true;
}

void GameScene::shootMenuCallback(Ref *render)
{
	//auto location = touch->getLocation();
	//auto relativeLocation = this->stone->getParent()->convertToNodeSpace(location);
	//if (!(this->shootLabel->getBoundingBox().containsPoint(location))) {
	//	return false;
	//}

	if (onRun) {
		return;
	}
	onRun = true;

	auto shoot = Sprite::createWithSpriteFrameName("stone-0.png");
	shoot->setPosition(this->stone->getPosition());
	shoot->setName("shoot");
	this->addChild(shoot, 1);

	auto mousePosition = this->mouse->getParent()->convertToWorldSpace(this->mouse->getPosition());
	auto shootTo = MoveTo::create(1, mousePosition);

	auto callback = CallFunc::create([this, mousePosition]() -> void {
		this->removeChildByName("shoot");

		auto dia = Sprite::createWithSpriteFrameName("diamond-0.png");
		dia->setPosition(mousePosition);
		Animate* diaAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamond"));
		dia->runAction(RepeatForever::create(diaAnimate));
		this->addChild(dia, 3);

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		auto dest = Vec2(RandomHelper::random_real(origin.x + 100, visibleSize.width - 100),
			RandomHelper::random_real(origin.y + 100, origin.y + 400) );

		this->mouseMove(dest);
	});

	shoot->runAction(Sequence::create(shootTo, callback, NULL));

}

void GameScene::mouseMove(Vec2 dest) {
	auto mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	// animation
	auto repeat = RepeatForever::create(mouseAnimate);
	repeat->setTag(1);
	this->mouse->runAction(repeat);

	auto move = MoveTo::create(1, this->mouse->getParent()->convertToNodeSpace(dest));

	auto callback = CallFunc::create([this]() -> void {
		this->mouse->stopActionByTag(1);
		this->onRun = false;
	});

	auto seq = Sequence::create(move, callback, NULL);
	this->mouse->runAction(seq);
}