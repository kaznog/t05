//
//  RandomTank.cpp
//  t05
//
//  Created by 野口一也 on 2014/06/18.
//
//

#include "RandomTank.h"
#include "HelloWorldScene.h"

RandomTank* RandomTank::initWithLayer(HelloWorld *layer, int type, int hp)
{
    auto tank = new RandomTank();
    if (tank && tank->initWithSpriteFrameName("tank" + std::to_string(type) + "_base.png")) {
        tank->autorelease();
        tank->initTank(layer, type, hp);
        
        tank->schedule(schedule_selector(RandomTank::move), 0.5f);
        return tank;
    }
    CC_SAFE_DELETE(tank);
    return nullptr;
}

bool RandomTank::shouldShoot()
{
    if (getPosition().getDistance(_layer->_tank->getPosition()) > 600.0f) {
        return false;
    }
    if (_timeSinceLastShot > _timeForNextShot) {
        _timeSinceLastShot = 0;
        _timeForNextShot = (CCRANDOM_0_1() * 3) + 1;
        shootToward(_layer->_tank->getPosition());
        return true;
    } else {
        return false;
    }
}

void RandomTank::move(float dt)
{
    if (_moving && arc4random() % 3 != 0) {
        return;
    }
    calcNextMove();
}

// From http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
bool RandomTank::clearPathFromTileCoord(cocos2d::Vec2 start, cocos2d::Vec2 end)
{
    int dx = abs(end.x - start.x);
    int dy = abs(end.y - start.y);
    int x = start.x;
    int y = start.y;
    int n = 1 + dx + dy;
    int x_inc = (end.x > start.x) ? 1 : -1;
    int y_inc = (end.y > start.y) ? 1 : -1;
    int error = dx - dy;
    dx *= 2;
    dy *= 2;
    
    for (; n > 0; --n) {
        if (_layer->isWallAtTileCoord(Vec2(x, y))) {
            return false;
        }
        if (error > 0) {
            x += x_inc;
            error -= dy;
        }
        else {
            y += y_inc;
            error += dx;
        }
    }
    return true;
}

void RandomTank::calcNextMove()
{
    bool moveOk = false;
    Vec2 start = _layer->tileCoordForPosition(getPosition());
    Vec2 end;
    
    while (!moveOk) {
        end = start;
        end.x += CCRANDOM_MINUS1_1() * ((arc4random() % 10) + 3);
        end.y += CCRANDOM_MINUS1_1() * ((arc4random() % 10) + 3);
        
        moveOk = clearPathFromTileCoord(start, end);
    }
    Vec2 _moveToWord = _layer->positionForTileCoord(end);
    _moving = true;
    moveToward(_moveToWord);
}
