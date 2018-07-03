#include "Player.h"

Player *Player::player = NULL;

int AccessDB() {
	sqlite3* pdb = NULL;
	std::string path = FileUtils::getInstance()->getWritablePath() + "data.db";
	int res = sqlite3_open(path.c_str(), &pdb);
	if (res == SQLITE_OK)
	{
		res = sqlite3_exec(pdb, "create table info(ID integer primary key autoincrement,score integer);", NULL, NULL, NULL);
		char **re;
		int r, c;
		int score;
		sqlite3_get_table(pdb, "select * from info;", &re, &r, &c, NULL);
		if (r != 1) 
		{
			res = sqlite3_exec(pdb, "insert into info values(0, 0);", NULL, NULL, NULL);
			score = 0;
		} 
		else
		{
			score = std::atoi(re[3]);
		}
		sqlite3_free_table(re);
		sqlite3_close(pdb);
		return score;
	}
}

Player::Player()
{
	initFrame();

	// init the sprite
	sprite = Sprite::createWithSpriteFrame(idle.at(0));

	hp = 100;
	prevDir = 'D';

	grade = AccessDB();
}

Player * Player::getInstance()
{
	if (player == NULL) {
		player = new Player();
	}
	return player;
}

Sprite * Player::getSprite()
{
	return sprite;
}

void Player::initFrame()
{
	//init the sprite frame
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));

	// idle
	idle.reserve(1);
	idle.pushBack(frame0);

	// attack animate
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// dead animate
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}

	// run animate
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
}

float Player::getHp()
{
	return hp;
}

float Player::damage(float i)
{
	hp = std::max(hp - i, 0.0f);
	if (hp == 0) 
	{
		die();
		return 0;
	}
	return hp;
}

float Player::heal(float i)
{
	hp = std::min(hp + i, 100.0f);
	return hp;
}

void Player::die()
{
	// dead animate
	auto danimation = Animation::createWithSpriteFrames(this->dead, 0.1f);
	auto danimate = Animate::create(danimation);
	// normal
	auto ianimation = Animation::createWithSpriteFrames(this->idle, 0.1f);
	auto ianimate = Animate::create(ianimation);
	this->sprite->stopAllActions();
	this->sprite->runAction(Sequence::create(ianimate, danimate, NULL));

	hp = 0;

	grade = -1;
	updateGrade();
}

void Player::move(char dir, Vec2 origin, Size visibleSize)
{
	if (sprite->getNumberOfRunningActions()) return;

	int dx = 0, dy = 0;

	// determine which direction
	switch (dir)
	{
	case 'W':
		dy += 20;
		break;
	case 'A':
		if (prevDir != 'A') 
		{
			sprite->setFlippedX(true);
		}
		dx -= 20;
		prevDir = 'A';
		break;
	case 'S':
		dy -= 20;
		break;
	case 'D':
		if (prevDir != 'D')
		{
			sprite->setFlippedX(false);
		}
		dx += 20;
		prevDir = 'D';
		break;
	default:
		break;
	}

	// dest position x
	auto x = sprite->getPositionX() + dx;
	if (x < origin.x + sprite->getContentSize().width / 2) {
		x = origin.x + sprite->getContentSize().width / 2;
	}
	else if (x > visibleSize.width - sprite->getContentSize().width / 2)
	{
		x = visibleSize.width - sprite->getContentSize().width / 2;
	}

	// dest position y
	auto y = sprite->getPositionY() + dy;
	if (y < origin.y + sprite->getContentSize().height / 2) {
		y = origin.y + sprite->getContentSize().height / 2;
	}
	else if (y > visibleSize.height - sprite->getContentSize().height / 2)
	{
		y = visibleSize.height - sprite->getContentSize().height / 2;
	}

	Vec2 dest(x, y);

	auto moveTo = MoveTo::create(0.4f, dest);

	// action
	auto animation = Animation::createWithSpriteFrames(run, 0.05f);
	auto animate = Animate::create(animation);
	sprite->runAction(animate);
	sprite->runAction(moveTo);
}

void Player::Attack()
{
	if (this->sprite->getNumberOfRunningActions()) return;
	// attack 
	auto aanimation = Animation::createWithSpriteFrames(this->attack, 0.05f);
	auto aanimate = Animate::create(aanimation);
	// normal
	auto ianimation = Animation::createWithSpriteFrames(this->idle, 0.1f);
	auto ianimate = Animate::create(ianimation);
	// action
	this->sprite->runAction(Sequence::create(aanimate, ianimate, NULL));
}

void Player::updateGrade()
{
	++grade;
	auto str = std::to_string(grade);
	sqlite3* pdb = NULL;
	std::string path = FileUtils::getInstance()->getWritablePath() + "data.db";
	int res = sqlite3_open(path.c_str(), &pdb);
	if (res == SQLITE_OK)
	{
		auto sql = "update info set score = " + str + "  where ID = 0;";
		res = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	}
	sqlite3_close(pdb);
}

int Player::getGrade() 
{
	return grade;
}
