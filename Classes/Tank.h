//
//  Tank.h
//  t05
//
//  Created by 野口一也 on 2014/06/17.
//
//

#ifndef __t05__Tank__
#define __t05__Tank__

#include "cocos2d.h"
class HelloWorld;

USING_NS_CC;

class Tank : public cocos2d::Sprite
{
public:
    Tank();
    ~Tank();
    virtual void update(float dt);

    void initTank(HelloWorld* layer, int type, int hp);
    virtual void calcNextMove();
    void updateMove(float dt);
    static Tank* initWithLayer(HelloWorld* layer, int type, int hp);
    void moveToward(Vec2 targetPosition);
    void shootToward(Vec2 targetPosition);
    void shootNow();
    virtual bool shouldShoot();
    void updateShhot(float dt);
    
    bool _shooting;
    bool _moving;
    int _hp;
    int _type;
    HelloWorld* _layer;
    Vec2 _targetPosition;
    Vec2 _shootVector;
    double _timeSinceLastShot;
    Sprite* _turret;
};

#endif /* defined(__t05__Tank__) */
