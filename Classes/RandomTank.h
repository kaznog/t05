//
//  RandomTank.h
//  t05
//
//  Created by 野口一也 on 2014/06/18.
//
//

#ifndef __t05__RandomTank__
#define __t05__RandomTank__

#include "Tank.h"

class RandomTank: public Tank
{
public:
    static RandomTank* initWithLayer(HelloWorld* layer, int type, int hp);
    
    virtual bool shouldShoot();
    virtual void calcNextMove();
    void move(float dt);
private:
    bool clearPathFromTileCoord(Vec2 start, Vec2 end);
    double _timeForNextShot;
};

#endif /* defined(__t05__RandomTank__) */
