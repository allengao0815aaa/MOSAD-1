#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(true);

	// Debug ģʽ
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = HitBrick::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	layer->setJoint();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();


	auto edgeSp = Sprite::create();  //����һ������
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 1.0f), 3);  //edgebox�ǲ��ܸ�����ײӰ���һ�ָ��壬����������������������ı߽�
	edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //λ����������Ļ����
	edgeSp->setTag(5);
	edgeSp->setPhysicsBody(boundBody);
	addChild(edgeSp);


	preloadMusic(); // Ԥ������Ч

	addSprite();    // ��ӱ����͸��־���
	addListener();  // ��Ӽ����� 
	addPlayer();    // ��������
	BrickGeneraetd();  // ����ש��


	schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

	onBall = true;
	spFactor = 10;
	onMove = 0;
	return true;
}

// �ؽ����ӣ��̶��������
void HitBrick::setJoint() {
	//joint1 = PhysicsJointDistance::construct(player->getPhysicsBody(), ball->getPhysicsBody(), 
	//	Vec2(-player->getContentSize().width / 2 * 0.1f, 0), Vec2(0, 0));
	//auto joint2 = PhysicsJointDistance::construct(player->getPhysicsBody(), ball->getPhysicsBody(), 
	//	Vec2(player->getContentSize().width / 2 * 0.1f, 0), Vec2(0, 0));
	//auto joint3 = PhysicsJointDistance::construct(player->getPhysicsBody(), ball->getPhysicsBody(),
	//	Vec2(0, 0), Vec2(0, 0));
	//m_world->addJoint(joint1);
	//m_world->addJoint(joint2);
	//m_world->addJoint(joint3);
	joint1 = PhysicsJointPin::construct(player->getPhysicsBody(), ball->getPhysicsBody(), ball->getPosition());
	m_world->addJoint(joint1);
}



// Ԥ������Ч
void HitBrick::preloadMusic() {
	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("gameover.mp3");
	sae->preloadBackgroundMusic("bgm.mp3");
	sae->playBackgroundMusic("bgm.mp3", true);
}

// ��ӱ����͸��־���
void HitBrick::addSprite() {
	// add background
	auto bgSprite = Sprite::create("bg.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);


	// add ship
	ship = Sprite::create("ship.png");
	ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
	ship->setPosition(visibleSize.width / 2, 0);
	auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
	shipbody->setContactTestBitmask(0xFFFFFFFF);
	shipbody->setDynamic(false);
	ship->setPhysicsBody(shipbody);
	ship->setTag(1);
	this->addChild(ship, 1);

	// add sun and cloud
	auto sunSprite = Sprite::create("sun.png");
	sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
	this->addChild(sunSprite);
	auto cloudSprite1 = Sprite::create("cloud.png");
	cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite1);
	auto cloudSprite2 = Sprite::create("cloud.png");
	cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite2);
}

// ��Ӽ�����
void HitBrick::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// ������ɫ
void HitBrick::addPlayer() {

	player = Sprite::create("bar.png");
	int xpos = visibleSize.width / 2;

	player->setScale(0.1f, 0.1f);
	player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height * 0.1f));
	auto playerBody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(FLT_MAX, 1.0f, 0.0f));
	playerBody->setContactTestBitmask(0xFFFFFFFF);
	playerBody->setGravityEnable(false);
	player->setPhysicsBody(playerBody);
	this->addChild(player, 2);

	ball = Sprite::create("ball.png");
	ball->setPosition(Vec2(xpos, 
		player->getPosition().y + (ball->getContentSize().height + player->getContentSize().height) / 2 * 0.1f));
	ball->setScale(0.1f, 0.1f);
	auto ballBody = PhysicsBody::createCircle(ball->getContentSize().width / 2, PhysicsMaterial(0, 1.0f, 0.0f));
	ballBody->setContactTestBitmask(0xFFFFFFFF);
	ballBody->setGravityEnable(false);
	ballBody->setVelocityLimit(900);
	ball->setPhysicsBody(ballBody);
	addChild(ball, 3);

	tail = ParticleSystemQuad::create("tail.plist");
	tail->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
	tail->setPosition(ball->getPosition());
	addChild(tail, 2);
}

// ʵ�ּ򵥵�����Ч��
void HitBrick::update(float dt) {
	if (onMove) {
		auto width = player->getContentSize().width / 2;
		if ((onMove == -1 && player->getPosition().x <= Director::getInstance()->getVisibleOrigin().x + width * 0.1 + 30)
			|| (onMove == 1 && player->getPosition().x >= visibleSize.width - width * 0.1 - 30)) {
			player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		}
	}
	static int ct = 0;
	if (spHolded) {
		++ct;
		if (ct == 5) {
			ct = 0;
			spFactor *= 1.1f;
		}
	}

	if (!onBall && ball->getPhysicsBody()->getVelocity().x == 0) {
		ball->getPhysicsBody()->setVelocity(Vec2(random(-10, 10), ball->getPhysicsBody()->getVelocity().y));
	}

	tail->setPosition(ball->getPosition());
}

void HitBrick::BrickGeneraetd() {

	for (int i = 0; i < 3; i++) {
		float cw = 43.0f / 2;
		while (cw <= visibleSize.width) {
			auto box = Sprite::create("box.png");
			box->setAnchorPoint(Vec2(0.5f, 0.5f));
			box->setPosition(cw, visibleSize.height - (i + 0.5) * box->getContentSize().height);
			cw += box->getContentSize().width;
			auto boxBody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(0.1f, 1.0f, 0.0f));
			boxBody->setContactTestBitmask(0xFFFFFFFF);
			boxBody->setDynamic(false);
			box->setTag(2);
			box->setPhysicsBody(boxBody);
			addChild(box);
		}
	}
}


// ����
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		onMove = -1;
		player->getPhysicsBody()->setVelocity(Vec2(-800, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		onMove = 1;
		player->getPhysicsBody()->setVelocity(Vec2(800, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // ��ʼ����
		if (onBall)
			spHolded = true;
		break;
	default:
		break;
	}
}

// �ͷŰ���
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	Vec2 dir(random(-10, 10), 10);
	dir.y = sqrt(500 - dir.x * dir.x);
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		onMove = 0;
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // ����������С����
		if (onBall) {
			//auto j = ball->getPhysicsBody()->getJoints();
			//for (auto joint: j) {
			//	joint->removeFormWorld();
			//}
			joint1->removeFormWorld();
			ball->getPhysicsBody()->setVelocity(dir * spFactor);
			spHolded = false;
			spFactor = 0;
			onBall = false;
		}
		break;
	default:
		break;
	}
}

void HitBrick::hit(Sprite *brick) {
	static int killed = 0;
	if (killed == 3) {
		ball->getPhysicsBody()->setVelocity(ball->getPhysicsBody()->getVelocity() * 1.2f);
		killed = 0;
	} else {
		++killed;
	}

	auto explosion = ParticleSystemQuad::create("hit.plist");
	explosion->setPosition(brick->getPosition());
	explosion->setScale(0.5f);
	addChild(explosion);
	brick->removeFromParentAndCleanup(true);
}

// ��ײ���
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
	auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
	auto sp1 = (Sprite*)c1->getBody()->getNode(), sp2 = (Sprite*)c2->getBody()->getNode();
	
	if (!sp1 || !sp2)
		return false;
	if (sp1->getTag() == 2 && sp2 == ball) {
		hit(sp1);
	} else if (sp2->getTag() == 2 && sp1 == ball) {
		hit(sp2);
	} else if ((sp1 == ball && sp2->getTag() == 1) || (sp2 == ball && sp1->getTag() == 1)) {
		GameOver();
	}
	return true;
}


void HitBrick::GameOver() {
	//ball->getPhysicsBody()->setEnabled(false);
	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("����", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("�˳�", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// ���������水ť��Ӧ����
void HitBrick::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(HitBrick::createScene());
}

// �˳�
void HitBrick::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
