#include "cocos2d.h"
#include "Monster.h"
#include "sqlite3.h"
#pragma once

USING_NS_CC;

class Player
{
public:
	static Player* getInstance();

	Sprite* getSprite();

	void initFrame();

	float getHp();

	float damage(float i);

	float heal(float i);

	void die();

	void move(char dir, Vec2 origin, Size visibleSize);

	void Attack();

	void updateGrade();

	int getGrade();
private:
	Player();
	static Player *player;

	float hp;
	char prevDir;
	int grade;

	cocos2d::Sprite* sprite;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
};

