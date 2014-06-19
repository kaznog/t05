//
//  Tank.cpp
//  t05
//
//  Created by 野口一也 on 2014/06/17.
//
//

#include "Tank.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Tank::Tank()
: _moving(false)
, _shooting(false)
, _targetPosition(Vec2::ZERO)
, _shootVector(Vec2::ZERO)
{
    
}

Tank::~Tank()
{
    
}

void Tank::initTank(HelloWorld *layer, int type, int hp)
{
    _layer = layer;
    _type = type;
    _hp = hp;
    scheduleUpdateWithPriority(-1);
    _turret = Sprite::createWithSpriteFrameName("tank" + std::to_string(type) + "_turret.png");
    _turret->setAnchorPoint(Vec2(0.5f, 0.25f));
    _turret->setPosition(Vec2(getContentSize().width/2, getContentSize().height/2));
    addChild(_turret);
}

Tank* Tank::initWithLayer(HelloWorld *layer, int type, int hp)
{
    std::string tankname = "tank" + std::to_string(type) + "_base.png";
    Tank* sprite = new Tank();
    if (sprite && sprite->initWithSpriteFrameName(tankname)) {
        sprite->autorelease();
        sprite->initTank(layer, type, hp);
//        sprite->_layer = layer;
//        sprite->_type = type;
//        sprite->_hp = hp;
//        sprite->scheduleUpdateWithPriority(-1);
//        sprite->_turret = Sprite::createWithSpriteFrameName("tank" + std::to_string(type) + "_turret.png");
//        sprite->_turret->setAnchorPoint(Vec2(0.5f, 0.25f));
//        sprite->_turret->setPosition(Vec2(sprite->getContentSize().width/2, sprite->getContentSize().height/2));
//        sprite->addChild(sprite->_turret);
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void Tank::update(float dt)
{
    updateMove(dt);
    updateShhot(dt);
}

void Tank::calcNextMove()
{
    
}

void Tank::updateMove(float dt)
{
    if (!_moving) {
        return;
    }
    Vec2 Offset = _targetPosition - this->getPosition();
    float MIN_OFFSET = 10.0f;
    if (Offset.getLength() < MIN_OFFSET) {
        return;
    }
    Vec2 targetVector = Offset.getNormalized();
    
    float POINTS_PER_SECOND = 150.0f;
    Vec2 targetPerSecond = targetVector * POINTS_PER_SECOND;
    
    Vec2 actualTarget = this->getPosition() + (targetPerSecond * dt);
    
    Vec2 oldPosition = this->getPosition();
    this->setPosition(actualTarget);
    
    if (_layer->isWallAtRect(this->getBoundingBox())) {
//        log("Tank updatemove isWallAtRect");
        setPosition(oldPosition);
        calcNextMove();
    }
}

void Tank::moveToward(cocos2d::Vec2 targetPosition)
{
    _targetPosition = targetPosition;
}

void Tank::shootToward(cocos2d::Vec2 targetPosition)
{
    auto offset = targetPosition - this->getPosition();
    float MIN_OFFSET = 10.0f;
    if (offset.getLength() < MIN_OFFSET) {
        return;
    }
    _shootVector = offset.getNormalized();
}

void Tank::shootNow()
{
    float angle = _shootVector.getAngle();
    _turret->setRotation((-1 * CC_RADIANS_TO_DEGREES(angle)) + 90.0f);
    
    float mapMax = MAX(_layer->tileMapWidth(), _layer->tileMapHeight());
    auto actualVector = _shootVector * mapMax;
    
    float POINT_PER_SECOND = 300.0f;
    float duration = mapMax / POINT_PER_SECOND;
    
    auto bullet = Sprite::createWithSpriteFrameName("tank" + std::to_string(_type) + "_bullet.png");
    bullet->setTag(_type);
    bullet->setPosition(getPosition() + (_shootVector * _turret->getContentSize().height));
    
    auto move = MoveBy::create(duration, actualVector);
    auto call = CallFunc::create([bullet]() {
        bullet->removeFromParentAndCleanup(true);
    });
    bullet->runAction(Sequence::create(move, call, NULL));
    _layer->_batchNode->addChild(bullet);
}

bool Tank::shouldShoot()
{
    if (!_shooting) {
        return false;
    }
    
    double SECS_BETWEEN_SHOTS = 0.25f;
    if (_timeSinceLastShot > SECS_BETWEEN_SHOTS) {
        _timeSinceLastShot = 0.0f;
        return true;
    } else {
        return false;
    }
}

void Tank::updateShhot(float dt)
{
    _timeSinceLastShot += dt;
    if (shouldShoot()) {
        shootNow();
    }
}