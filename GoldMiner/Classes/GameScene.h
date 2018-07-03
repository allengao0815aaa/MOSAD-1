#pragma once
#include <stdio.h>
#include "cocos2d.h"
USING_NS_CC;

class GameScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);

	virtual void shootMenuCallback(Ref *render);

	virtual void mouseMove(Vec2 dest);

	CREATE_FUNC(GameScene);

private:
	Sprite *mouse;

	Sprite *stone;

	Label *shootLabel;

	Layer *mouseLayer;

	Layer *stoneLayer;

	bool onRun = false;
};