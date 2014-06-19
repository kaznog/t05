#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    auto director = Director::getInstance();
    director->getOpenGLView()->setDesignResolutionSize(480, 320, ResolutionPolicy::SHOW_ALL);
    _winSize = director->getWinSize();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

//    auto closeItem = MenuItemImage::create(
//                                           "CloseNormal.png",
//                                           "CloseSelected.png",
//                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
//    
//	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
//                                origin.y + closeItem->getContentSize().height/2));
//
//    // create menu, it's an autorelease object
//    auto menu = Menu::create(closeItem, NULL);
//    menu->setPosition(Vec2::ZERO);
//    this->addChild(menu, 1);

    _tileMap = TMXTiledMap::create("tanks.tmx");
    this->addChild(_tileMap);
    _bgLayer = _tileMap->getLayer("Background");
    
    Vec2 spawnTileCoord = Vec2(4, 4);
    Vec2 spawnPos = positionForTileCoord(spawnTileCoord);
    setViewPointCenter(spawnPos);
    
    _batchNode = SpriteBatchNode::create("sprites.png");
    _tileMap->addChild(_batchNode);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprites.plist");
    
    _tank = Tank::initWithLayer(this, 1, 5);
    _tank->setPosition(spawnPos);
    _batchNode->addChild(_tank);
    
    for (int i = 0; i < 50; i++) {
        auto enemy = RandomTank::initWithLayer(this, 2, 2);
        Vec2 randSpot;
        bool inWall = true;
        while (inWall) {
            randSpot.x = CCRANDOM_0_1() * tileMapWidth();
            randSpot.y = CCRANDOM_0_1() * tileMapHeight();
            inWall = isWallAtPosition(randSpot);
        }
        enemy->setPosition(randSpot);
        _batchNode->addChild(enemy);
        _enemyTanks.push_back(enemy);
    }
    
    _explosion = ParticleSystemQuad::create("explosion.plist");
    _explosion->stopSystem();
    _tileMap->addChild(_explosion, 1);
    
    _explosion2 = ParticleSystemQuad::create("explosion2.plist");
    _explosion2->stopSystem();
    _tileMap->addChild(_explosion2, 1);
    
    _exit = Sprite::createWithSpriteFrameName("exit.png");
    Vec2 exitTileCoord = Vec2(98, 98);
    Vec2 exitTilePos = positionForTileCoord(exitTileCoord);
    _exit->setPosition(exitTilePos);
    _batchNode->addChild(_exit);
//    this->setScale(0.5f);
    this->enableTouchEvent(true);
    this->enableAccelerationEvent(true);
    this->scheduleUpdate();
    return true;
}

#pragma mark -
void HelloWorld::update(float dt)
{
    if (_gameOver) {
        return;
    }
    
    if (_exit->getBoundingBox().intersectsRect(_tank->getBoundingBox())) {
        this->endScene(KENDREASONWIN);
    }
    std::vector<Sprite*> childrenToRemove;
    childrenToRemove.clear();
    
    Vector<Node*> childrens = _batchNode->getChildren();
    for (Vector<Node*>::iterator it = childrens.begin(); it != childrens.end(); it++) {
        auto sprite = dynamic_cast<Sprite*>(*it);
        if (sprite && sprite->getTag() != 0) { // bullet
            if (isWallAtPosition(sprite->getPosition())) {
                childrenToRemove.push_back(sprite);
                continue;
            }
            if (sprite->getTag() == 1) {  // player bullet
                for (std::vector<RandomTank*>::iterator tankit = _enemyTanks.begin(); tankit != _enemyTanks.end(); tankit++) {
                    auto enemy = dynamic_cast<RandomTank*>(*tankit);
                    if (enemy && sprite->getBoundingBox().intersectsRect(enemy->getBoundingBox())) {
                        childrenToRemove.push_back(sprite);
                        enemy->_hp--;
                        if (enemy->_hp <= 0) {
                            _explosion->setPosition(enemy->getPosition());
                            _explosion->resetSystem();
                            std::vector<RandomTank*> tmplist;
                            for (std::vector<RandomTank*>::iterator tit = _enemyTanks.begin(); tit != _enemyTanks.end(); tit++) {
                                auto tank = dynamic_cast<RandomTank*>(*tit);
                                if (tank && tank != enemy) {
                                    tmplist.push_back(tank);
                                }
                            }
                            _enemyTanks.clear();
                            for (std::vector<RandomTank*>::iterator tit = tmplist.begin(); tit != tmplist.end(); tit++) {
                                auto tank = dynamic_cast<RandomTank*>(*tit);
                                if (tank) {
                                    _enemyTanks.push_back(tank);
                                }
                            }
                            tmplist.clear();
                            childrenToRemove.push_back(enemy);
                        } else {
                            
                        }
                    }
                }
            }
            if (sprite->getTag() == 2) {  // enemy bullet
                if (sprite->getBoundingBox().intersectsRect(_tank->getBoundingBox())) {
                    childrenToRemove.push_back(sprite);
                    _tank->_hp--;
                    if (_tank->_hp <= 0) {
                        _explosion->setPosition(_tank->getPosition());
                        _explosion->resetSystem();
                        endScene(KENDREASONLOSE);
                    } else {
                        _explosion2->setPosition(_tank->getPosition());
                        _explosion2->resetSystem();
                        
                    }
                }
            }
        }
    }
    for (std::vector<Sprite*>::iterator it = childrenToRemove.begin(); it != childrenToRemove.end(); it++) {
        auto child = dynamic_cast<Sprite*>(*it);
        child->removeFromParentAndCleanup(true);
    }
    childrenToRemove.clear();
    setViewPointCenter(_tank->getPosition());
}

#pragma mark -
#pragma mark Acceleration
void HelloWorld::enableAccelerationEvent(bool enabled)
{
    if (this->_accelerationListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(this->_accelerationListener);
        this->_accelerationListener = nullptr;
        Device::setAccelerometerEnabled(false);
    }
    Device::setAccelerometerEnabled(enabled);
    if (enabled) {
        Device::setAccelerometerInterval(0.1);
        this->_accelerationListener = EventListenerAcceleration::create(CC_CALLBACK_2(HelloWorld::OnAcceleration, this));
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(this->_accelerationListener, this);
    }
}

void HelloWorld::OnAcceleration(cocos2d::Acceleration *acc, cocos2d::Event *event)
{
    float accelX = acc->x;
    float accelY = acc->y;

    auto moveTo = _tank->getPosition();
    if (accelX < -0.1f) {
        moveTo.x -= 300.0f;
    } else if (accelX > 0.1f) {
        moveTo.x += 300.0f;
    }
    if (accelY < -0.1f) {
        moveTo.y -= 300.0f;
    } else if (accelY > 0.1f) {
        moveTo.y += 300.0f;
    }
    _tank->_moving = true;
    _tank->moveToward(moveTo);
    
//    log("accelX:%f", acc->x);
//    log("accelY:%f", acc->y);
}

#pragma mark -
#pragma mark touch events
void HelloWorld::enableTouchEvent(bool enabled)
{
    if (this->_touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(this->_touchListener);
        this->_touchListener = nullptr;
    }
    if (enabled) {
        this->_touchListener = EventListenerTouchOneByOne::create();
        _touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
        _touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
        _touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);
    }
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
//    log("onTouchBegan");
    auto location = _tileMap->convertToNodeSpace(touch->getLocation());
    _tank->_shooting = true;
    _tank->shootToward(location);
    
    _tank->_moving = true;
    _tank->moveToward(location);
    return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event)
{
//    log("onTouchMoved");
    auto location = _tileMap->convertToNodeSpace(touch->getLocation());
    _tank->_shooting = true;
    _tank->shootToward(location);

    _tank->_moving = true;
    _tank->moveToward(location);
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
    auto location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
}

void HelloWorld::onTouchCancelled(Touch *touch, Event *event)
{
}

#pragma mark -

bool HelloWorld::isPropAtTileCoord(std::string prop, Vec2 tileCoord, TMXLayer* layer)
{
    if (!isValidTileCoord(tileCoord)) {
        return false;
    }
    int tileGid = layer->getTileGIDAt(tileCoord);
    Value properties = _tileMap->getPropertiesForGID(tileGid);
    if(properties.getType() == Value::Type::NONE) {
        return false;
    }
    ValueMap map = properties.asValueMap();
    if (map.size() == 0) {
        return false;
    }
    if (map.find(prop) != map.end()) {
//        log("isWall!!");
        return true;
    }
    return false;
}

bool HelloWorld::isPropAtPosition(std::string prop, Vec2 position, TMXLayer* layer)
{
    auto tilecoord = tileCoordForPosition(position);
    return isPropAtTileCoord(prop, tilecoord, layer);
}

bool HelloWorld::isWallAtTileCoord(Vec2 tileCoord)
{
//    log("isWallAtTileCoord tileCoord.x:%f, tileCoord.y:%f", tileCoord.x, tileCoord.y);
    return isPropAtTileCoord("Wall", tileCoord, _bgLayer);
}

bool HelloWorld::isWallAtPosition(Vec2 position)
{
    auto tilecoord = tileCoordForPosition(position);
    
//    log("isWallAtPosition tileCoord.x:%f, tileCoord.y:%f", tilecoord.x, tilecoord.y);
    if (!isValidTileCoord(tilecoord)) {
        return true;
    }
    return isWallAtTileCoord(tilecoord);
}

bool HelloWorld::isWallAtRect(Rect rect)
{
    auto lower_left = Vec2(rect.origin.x, rect.origin.y);
    auto upper_left = Vec2(rect.origin.x, rect.origin.y + rect.size.height);
    auto lower_right = Vec2(rect.origin.x + rect.size.width, rect.origin.y);
    auto upper_right = Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    
    return isWallAtPosition(lower_left) || isWallAtPosition(upper_left) || isWallAtPosition(lower_right) || isWallAtPosition(upper_right);
}

#pragma mark -
float HelloWorld::tileMapHeight()
{
    return _tileMap->getMapSize().height * _tileMap->getTileSize().height;
}

float HelloWorld::tileMapWidth()
{
    return _tileMap->getMapSize().width * _tileMap->getTileSize().width;
}

bool HelloWorld::isValidPosition(cocos2d::Vec2 position)
{
    if (position.x < 0
    ||  position.y < 0
    ||  position.x > tileMapWidth()
    ||  position.y > tileMapHeight()) {
        return false;
    }
    return true;
}

bool HelloWorld::isValidTileCoord(cocos2d::Vec2 tileCoord)
{
    if (tileCoord.x < 0
    ||  tileCoord.y < 0
    ||  tileCoord.x >= _tileMap->getMapSize().width
    ||  tileCoord.y >= _tileMap->getMapSize().height) {
        return false;
    }
    return true;
}

Vec2 HelloWorld::tileCoordForPosition(cocos2d::Vec2 position)
{
    if (!isValidPosition(position)) {
        return Vec2(-1, -1);
    }
    int x = position.x / _tileMap->getTileSize().width;
    int y = (tileMapHeight() - position.y) / _tileMap->getTileSize().height;
    return Vec2(x, y);
}

Vec2 HelloWorld::positionForTileCoord(cocos2d::Vec2 tilecoord)
{
    int x = (tilecoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width/2;
    int y = tileMapHeight() - (tilecoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height/2;
    return Vec2(x, y);
}

void HelloWorld::setViewPointCenter(cocos2d::Vec2 position)
{
    float x = MAX(position.x, _winSize.width/2/this->getScale());
    float y = MAX(position.y, _winSize.height/2/this->getScale());
    x = MIN(x, tileMapWidth() - _winSize.width/2/this->getScale());
    y = MIN(y, tileMapHeight() - _winSize.height/2/this->getScale());
    Vec2 actualPosition = Vec2(x, y);
    
    Vec2 centerOfView = Vec2(_winSize.width/2, _winSize.height/2);
    Vec2 viewPoint = centerOfView - actualPosition;
    _tileMap->setPosition(viewPoint);
}

#pragma mark -

void HelloWorld::restartTapped(cocos2d::Ref *pSender)
{
    Director::getInstance()->replaceScene(TransitionZoomFlipX::create(0.5f, HelloWorld::createScene(), cocos2d::TransitionScene::Orientation::RIGHT_OVER));
}

void HelloWorld::endScene(EndReason reason)
{
    if (_gameOver) {
        return;
    }
    _gameOver = true;
    std::string message;
    if (reason == KENDREASONWIN) {
        message = "You Win!";
    } else if (reason == KENDREASONLOSE) {
        message = "You Lose :[";
    }
    
    auto label = LabelBMFont::create(message, "TanksFont.fnt");
    label->setScale(0.1f);
    label->setPosition(Vec2(_winSize.width/2, _winSize.height * 0.7f));
    this->addChild(label);
    
    auto restartlabel = LabelBMFont::create("Restart", "TanksFont.fnt");
    auto restartitem = MenuItemLabel::create(restartlabel, CC_CALLBACK_1(HelloWorld::restartTapped, this));
    restartitem->setScale(0.1f);
    restartitem->setPosition(Vec2(_winSize.width/2, _winSize.height * 0.3f));
    
    auto menu = Menu::create(restartitem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    
    restartitem->runAction(ScaleTo::create(0.5f, 4.0f));
    label->runAction(ScaleTo::create(0.5f, 4.0f));
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
