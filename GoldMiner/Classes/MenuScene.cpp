#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto cave = Sprite::createWithSpriteFrameName("cave-0.png");
	cave->setPosition(Vec2(visibleSize.width / 2 + origin.x + 190, visibleSize.height / 2 + origin.y - 20));
	auto caveAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("caveAnimation"));
	cave->runAction(RepeatForever::create(caveAnimate));
	this->addChild(cave, 1);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto menu_start_gold = Sprite::create("menu-start-gold.png");
	menu_start_gold->setPosition(Vec2(visibleSize.width + origin.x - 200, origin.y + 130));
	this->addChild(menu_start_gold, 1);

	auto start = MenuItemImage::create("start-0.png", "start-1.png", [](Ref *sender) -> void {
		auto transitions = TransitionMoveInL::create(0.2f, GameScene::create());
		Director::getInstance()->replaceScene(transitions);
	});
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2(menu_start_gold->getPositionX(), menu_start_gold->getPositionY() + 50));
	this->addChild(menu, 2);

	auto title = Sprite::create("gold-miner-text.png");
	title->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 150));
	this->addChild(title, 2);

    return true;
}

