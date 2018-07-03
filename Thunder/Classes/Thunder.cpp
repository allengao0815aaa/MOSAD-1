#include "Thunder.h"
#include "SimpleAudioEngine.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	char enemyPath[20];
	sprintf(enemyPath, "stone%d.png", 3 - stoneType);
	double width = visibleSize.width / 6, 
		height = visibleSize.height - 50;
	for (auto enemy : enemys) {
		enemy->setPositionY(enemy->getPositionY() - 50);
	}
	for (int i = 0; i < 5; ++i) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * (i + 1) - 80, height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
	stoneType = (++stoneType) % 3;
}

// 移动飞船
void Thunder::movePlane(char c) {
	float dx = (movekey - 66.5) * 5;
	Vec2 dest = Vec2(player->getPositionX() + dx, player->getPositionY());
	// normalize
	dest.x = dest.x > visibleSize.width - player->getContentSize().width / 2
		? visibleSize.width - player->getContentSize().width / 2 : dest.x;
	dest.x = dest.x < Director::getInstance()->getVisibleOrigin().x + player->getContentSize().width / 2
		? Director::getInstance()->getVisibleOrigin().x + player->getContentSize().width / 2: dest.x;

	player->setPosition(dest);
}

//发射子弹
void Thunder::fire() {
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	auto moveTo = MoveTo::create(3, Vec2(player->getPositionX(), visibleSize.height + bullet->getContentSize().height));
	bullet->runAction(Sequence::create(moveTo, CallFunc::create([=]() -> void {
		this->bullets.remove(bullet);
		this->removeChild(bullet);
	}), NULL));
}

// 切割爆炸动画帧
void Thunder::explosion() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explore.reserve(9);
	for (int i = 0; i < 9; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(190 * (i % 5), (i / 5) * 190, 190, 190)));
		explore.pushBack(frame);
	}
}

void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	EventCustom e1("meet");
	_eventDispatcher->dispatchEvent(&e1);

	if (this->getChildByName("over") != NULL && bullets.empty()) {
		unscheduleAllCallbacks();
	} else if (this->getChildByName("over") == NULL) {
		// 飞船移动
		if (isMove)
			this->movePlane(movekey);

		static int ct = 0;
		static int dir = 4;
		++ct;
		if (ct == 120)
			ct = 40, dir = -dir;
		else if (ct == 80) {
			dir = -dir;
			newEnemy();  // 陨石向下移动并生成新的一行(加分项)
		}
		else if (ct == 20)
			ct = 40, dir = -dir;

		//陨石左右移动
		for (Sprite* s : enemys) {
			if (s != NULL) {
				s->setPosition(s->getPosition() + Vec2(dir, 0));
			}
		}

		// 分发自定义事件

		EventCustom e2("deadmeet");
		_eventDispatcher->dispatchEvent(&e2);
	}
}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	for (auto enemy = enemys.begin(); enemy != enemys.end(); ) {
		auto flag = false;
		for (auto bullet = bullets.begin(); bullet != bullets.end(); ) {
			if ((*bullet)->getPosition().getDistance((*enemy)->getPosition()) < 25) {
				this->removeChild(*bullet, true);
				bullets.erase(bullet++);
				flag = true;
				break;
			} else {
				++bullet;
			}
		}
		if (flag) {
			Boom(*enemy);
			enemys.erase(enemy++);
		} else {
			++enemy;
		}
	}
}


void Thunder::Boom(Sprite* enemy) {
	SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
	auto animation = Animation::createWithSpriteFrames(explore, 0.1f);
	auto animate = Animate::create(animation);
	enemy->runAction(Sequence::create(animate, [=]() -> void {
		this->removeChild(enemy, true);
	}, NULL));

}

void Thunder::deadmeet(EventCustom* event) {
	for (auto enemy = enemys.begin(); enemy != enemys.end(); ++enemy) {
		if ((*enemy)->getPositionY() < 50 || 
			player->getBoundingBox().containsPoint((*enemy)->getPosition() - Vec2(0, 10))) {
			Boom(player);
			stopAc();
			break;
		}
	}
}

void Thunder::stopAc() {
	auto over = Sprite::create("gameOver.png");
	over->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	over->setName("over");
	this->addChild(over);
	//unscheduleAllCallbacks();
	_eventDispatcher->removeAllEventListeners();
}



// 添加自定义监听器
void Thunder::addCustomListener() {
	auto listener1 = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	auto listener2 = EventListenerCustom::create("deadmeet", CC_CALLBACK_1(Thunder::deadmeet, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
        isClick = true;
	else
		fire();
	return true;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	if (isClick) {
		float dest = touch->getDelta().x + player->getPositionX();
		if (dest < player->getContentSize().width / 2 + Director::getInstance()->getVisibleOrigin().x)
			dest = player->getContentSize().width / 2 + Director::getInstance()->getVisibleOrigin().x;
		else if (dest > visibleSize.width - player->getContentSize().width / 2)
			dest = visibleSize.width - player->getContentSize().width / 2;
		player->setPositionX(dest);

	}
}
