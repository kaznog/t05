#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Tank.h"
#include "RandomTank.h"

USING_NS_CC;

typedef enum {
    KENDREASONWIN,
    KENDREASONLOSE
} EndReason;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    virtual void update(float dt);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    /** Acceleration listeer **/
    void enableAccelerationEvent(bool enabled);
    CC_SYNTHESIZE(cocos2d::EventListenerAcceleration*, _accelerationListener, AccelerationListener);
    void OnAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* event);
    /** Touch listener */
    void enableTouchEvent(bool enabled);
    CC_SYNTHESIZE(EventListenerTouchOneByOne*, _touchListener, TouchListener);
    virtual bool onTouchBegan(Touch *touch, Event *event);
    virtual void onTouchMoved(Touch *touch, Event *event);
    virtual void onTouchEnded(Touch *touch, Event *event);
    virtual void onTouchCancelled(Touch *touch, Event *event);

    bool isPropAtTileCoord(std::string prop, Vec2 tileCoord, TMXLayer* layer);
    bool isPropAtPosition(std::string prop, Vec2 position, TMXLayer* layer);
    bool isWallAtTileCoord(Vec2 tileCoord);
    bool isWallAtPosition(Vec2 position);
    bool isWallAtRect(Rect rect);
    
    void setViewPointCenter(Vec2 position);
    Vec2 positionForTileCoord(Vec2 tilecoord);
    Vec2 tileCoordForPosition(Vec2 position);
    bool isValidPosition(Vec2 position);
    bool isValidTileCoord(Vec2 tileCoord);
    float tileMapHeight();
    float tileMapWidth();
    Size _winSize;
    TMXTiledMap* _tileMap;
    TMXLayer* _bgLayer;
    
    SpriteBatchNode* _batchNode;
    
    Tank* _tank;
    ParticleSystemQuad* _explosion;
    ParticleSystemQuad* _explosion2;
    Sprite* _exit;
    bool _gameOver;
    
    void restartTapped(cocos2d::Ref* pSender);
    void endScene(EndReason reason);
private:
    std::vector<RandomTank*> _enemyTanks;
};

#endif // __HELLOWORLD_SCENE_H__
