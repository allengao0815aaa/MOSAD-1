#pragma once
#include "cocos2d.h"

USING_NS_CC;

class HelloWorld : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	virtual void initSchedule();

	virtual void initMenuItem(std::string font, float fontSize);

	virtual void moveSprite(Ref *sender);

	virtual void updateSprite(float i);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);
private:
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* grade;
	int dtime;
	cocos2d::ProgressTimer* pT;
};
