/*
Copyright 2013-2015 Rohit Nirmal

This file is part of Clonepoint.

Clonepoint is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Clonepoint is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Clonepoint.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENTITY_H
#define ENTITY_H
#include <string> //for AudioManager::playSound
#include "vec.h"
#include "global.h"
#include "sprite.h"
#include "locator.h"

#define ENEMY_FOV_RADIUS_SNIPER 1024
#define ENEMY_FOV_RADIUS_LIT 512
#define ENEMY_FOV_RADIUS_DARK 64
#define ENEMY_FOV_HALFANGLE 45
#define ENEMY_FOV_HALFANGLE_SEEN 80

#define GRAVITY 0.025

enum StairTraversal
{
	NotMoving = 0,
	MovingUp,
	MovingDown
};

enum GuardState
{
	IDLE = 0,
	IDLE_CAUTION, //just ended investigation - wait a few seconds to switch to patrolling.
	PATROLLING, //normal speed.
	INVESTIGATING, //running toward point of interest - either noise or player's last position
	AIMING,
	USING_SWITCH,
	PINNED,
	FALLING,
	KNOCKED_OUT
};

enum AttachType
{
	NotAttached = 0,
	LeftSide,
	RightSide,
	Ceiling
};

enum AlertType
{
	ALERT_NONE = 0,
	ALERT_LOOK,
	ALERT_WALK,
	ALERT_RUN
};

enum TargetType
{
	TARGET_NONE = 0,
	TARGET_NOISE,
	TARGET_PLAYER,
	TARGET_LIGHTSWITCH,
	TARGET_ALARM
};

enum FOVType
{
	FOV_LIGHT = 0,
	FOV_CAMERA
};

enum DoorType
{
	Door_Normal = 0,
	Door_Trap,
	Door_Vault
};

enum EnemyType
{
	Enemy_Guard = 0,
	Enemy_Enforcer,
	Enemy_Professional,
	Enemy_Sniper
};

struct Acceleration
{
	bool accelerating;
	float target;
	float accel;
};

enum GunShotTraceType
{
	Shot_None = 0, //used only for EnemyShotResolve.
	Shot_FromPlayer,
	Shot_FromEnemyVoluntary,
	Shot_FromEnemyInvoluntary
};

struct EnemyShotResolve
{
	int timeSinceShot;
	GunShotTraceType shotType;
};

void setAccel(Acceleration* accel, bool accelerating, float start, float target);

class LightFixture;

class FieldOfView
{
public:
	FieldOfView(float x, float y, float radius, int direction, int halfSize, bool active, FOVType type);
	~FieldOfView();
	vec2f getPosition();
	float getRadius();
	int getDirection();
	int getSize();
	void addVertex(float x, float y);
	void changeVertex(unsigned int i, float x, float y); //for calculating light when door opens or closes.
	float* getVertData();
	int getNumberOfVerts();
	void moveTo(float x, float y);
	void clearVerts();
	void getColors(float* r, float* g, float* b);
	Rect getCollisionRect();
	FOVType getType();
	float _red;
	float _green;
	float _blue;
	bool _active;
	LightFixture* _fixture;
private:
	vec2f _position;
	float _radius;
	int _direction;
	int _halfSize;
	std::vector<float> _verts;
	FOVType _type;
};

struct LightAndAngles
{
	FieldOfView* fov;
	int angle1;
	int angle2;
};

class Entity
{
public:
	Entity(float x, float y);
	Entity(float x, float y, unsigned int sprite);
	virtual ~Entity();
	vec2f getPosition();
	vec2f getVelocity();
	float getRotation();
	void setPosition(float x, float y);
	void setCollisionRectPosition(float x, float y);
	void setCollisionRectDims(float w, float h, int entDim);
	void setCollisionRectDimsAndPosition(float x, float y, float w, float h, int entDim);
	void setPosX(float x);
	void setPosY(float y);
	void setPosition(vec2f newPos);
	void setVelocity(float x, float y);
	void setVelX(float x);
	void setVelY(float y);
	virtual void update(unsigned int dT);
	Rect getCollisionRect();
	vec2f getCollisionRectPosition();
	vec2f getCollisionRectCenterPosition();
	void updateCollisionRectPosition();
	unsigned int getCurrentSprite();
	void changeToStaticSprite(unsigned int sprite);
	void changeAnimationSequence(AnimationSequence* sequence);
	void changeAnimationSequenceForce(AnimationSequence* sequence);
	bool _highlighted;
protected:
	vec2f _position;
	vec2f _offset;
	vec2f _velocity;
	float _rotation;
	Rect _collisionRect;
	unsigned int _sprite;
	AnimationSequence* _activeSequence; //what animation is this entity currently running?
	float _animDT;
	unsigned int _currentSequenceIndex;
	bool _currentAnimFinished;
};

class Particle : public Entity //probably used only for glass shards.
{
public:
	Particle(float x, float y, unsigned int sprite);
	void update(unsigned int dT);
	bool _alive;
};

class TutorialMark : public Entity
{
public:
	TutorialMark(float x, float y, StringMessage ts);
	StringMessage _ts;
};

class Stairwell;

class Stairs : public Entity
{
public:
	Stairs(float x, float y);
	void registerStairwell(Stairwell* well);
	Stairs* getUpstairs();
	Stairs* getDownstairs();
	void setUpstairs(Stairs* target);
	void setDownstairs(Stairs* target);
private:
	Stairs* _upstairs;
	Stairs* _downstairs;
	Stairwell* _well;
};

class Stairwell
{
public:
	Stairwell(int x);
	~Stairwell();
	void addStairs(Stairs* stairs);
	int getX();
	void setDirections(unsigned int yMax);
private:
	std::vector<Stairs*> _stairs;
	int _x;
};

class ElevatorShaft;
class ElevatorSwitch;

class ElevatorDoor : public Entity
{
public:
	ElevatorDoor(float x, float y);
	bool isOpen();
	void open(bool animate);
	void close( bool animate);
	void update(unsigned int dT);
	bool isOpening();
	bool isClosing();
	ElevatorShaft* _shaft;
	ElevatorSwitch* _switch;
private:
	bool _open;
};

class ElevatorShaft
{
public:
	ElevatorShaft(int x);
	~ElevatorShaft();
	void addDoor(ElevatorDoor* ed);
	void update();
	int containsDoor(ElevatorDoor* door);
	void setOpenDoor(ElevatorDoor* door, bool animate);
	void setOpenDoorFirst();
	void setTarget(ElevatorDoor* target);
	vec2f getElevatorPosition();
	Rect getRect();
	ElevatorDoor* getOpenDoor();
	ElevatorDoor* getTarget();
	void calculateDoorOrders(int yMax);
	ElevatorDoor* getDoorAbove(ElevatorDoor* door);
	ElevatorDoor* getDoorBelow(ElevatorDoor* door);
	int getDoorIndexOrdered(int index);
	float _yVel;
	bool _moving;
	int _x;
private:
	std::vector<ElevatorDoor*> _doors;
	std::vector<int> _order;
	Rect _rect;
	vec2f _elevatorPosition; //where bounding box is drawn.
	ElevatorDoor* _target;
	ElevatorDoor* _openDoor;
	bool _waitingForClose;
	Acceleration _acceleration;
};

class MainComputer : public Entity
{
public:
	MainComputer(float x, float y, bool active);
	bool isActive();
	void setActive(bool b);
private:
	bool _active;
};

class CircuitBox : public Entity
{
public:
	CircuitBox(float x, float y, Circuit circuit);
	bool isHacked();
	void setHacked(bool hacked, bool playSound);
	Circuit getCircuit();
private:
	Circuit _circuit;
	bool _hacked;
};

class LivingEntity : public Entity
{
public:
	LivingEntity(float x, float y, Direction startingDir);
	void update(unsigned int dT);
	void setAlive(bool b);
	bool isAlive();
	virtual void landOnGround();
	void setStairMovement(StairTraversal st);
	StairTraversal getStairTraversal();
	virtual void arriveAtStairs(Stairs* st);
	Direction getDirection();
	void reverseDirection();
	bool isMovingThroughStairs();
	int getStairTimer();
	void setDirection(Direction dir);
	Stairs* getStairsEntered();
	virtual bool isAnimatingThroughStairs() = 0;
	Acceleration* getAccelerationStruct();
	bool isPositionBehind(float x);
	bool _onGround;
	Stairs* _overlappingStairs;
protected:
	bool _alive;
	bool _fixDirection;
	bool _affectedByGravity;
	Direction _dir;
	int _stairTimer;
	StairTraversal _traversal;
	Stairs* _stairsEntered;
	Acceleration _acceleration;
private:
};

class LinkableEntity : public Entity
{
public:
	LinkableEntity(float x, float y, Circuit c);
	Circuit getCircuitType();
	LinkableEntity* getTarget();
	void link(LinkableEntity* target, bool playSound);
	virtual void unlink();
	virtual void activate(LinkableEntity* activator);
	bool hasCycle();
protected:
	Circuit _circuit;
	LinkableEntity* _other;
private:
};

class LightSwitch : public LinkableEntity
{
public:
	LightSwitch(float x, float y, Circuit c, bool hs); //hs = isHandScanner
	void activate(LinkableEntity* activator);
	bool isHandScanner();
private:
	bool _isHandScanner;
};

class ElevatorSwitch : public LinkableEntity
{
public:
	ElevatorSwitch(float x, float y, Circuit c);
	void activate(LinkableEntity* activator);
	void activateTarget();
	void changeSprite(unsigned int sprite);
	ElevatorDoor* _door;
};

class Door : public LinkableEntity
{
public:
	Door(float x, float y, Circuit c, bool open, DoorType type);
	CollisionVolume* getCollisionVolume();
	CollisionVolume* getCollisionVolume2();
	void update(unsigned int dT);
	void updateCollisionVolume();
	void activate(LinkableEntity* activator);
	bool isOpened();
	void open(Direction opendir);
	void close(Direction opendir);
	void openSound(Direction opendir);
	void closeSound(Direction opendir);
	void addOverlappingLight(FieldOfView* fov, int angle1, int angle2);
	size_t getNumberOfOverlappingLights();
	void setOpenDirection(Direction opendir);
	Direction getOpenDirection();
	FieldOfView* getLightAndAnglesAt(int i, int* angle1, int* angle2);
	DoorType getType();
	int getTimeToClose(); //for save games.
	bool dirty;
private:
	DoorType _type;
	int _timeToClose; //for trap and vault doors only.
	bool _opened;
	CollisionVolume* _cvol;
	CollisionVolume* _cvol2;
	std::vector<LightAndAngles> _overlappingLights; //all lights here update when opened or closed.
	Direction _opendir;
};

class MotionScanner : public LinkableEntity
{
public:
	MotionScanner(float x, float y, Circuit c);
	Entity* getTrespasser();
	void setTrespasser(Entity* ent);
	void resetTrespasser();
	bool _trespassed; //set to true when an entity (probably living) has overlapped collisionRect. Do not activate() while true, to prevent multiple activations.
private:
	Entity* _trespasser;
};

class SecurityCamera : public LinkableEntity
{
public:
	SecurityCamera(float x, float y, Circuit c, Direction dir, FieldOfView* fov);
	void activate(LinkableEntity* activator);
	FieldOfView* getFOV();
	bool _trespassed; //set to true when an entity (probably living) has overlapped collisionRect. Do not activate() while true, to prevent multiple activations.
private:
	Direction _direction;
	FieldOfView* _fov;
};

class LightFixture : public LinkableEntity
{
public:
	LightFixture(float x, float y, Circuit c, bool switchedOn);
	~LightFixture();
	void activate(LinkableEntity* activator);
	void toggleAllFOVs();
	void setSwitchedOn(bool sw);
	bool isSwitchedOn();
	void addFOV(FieldOfView* fov);
private:
	bool _switchedOn;
	std::vector<FieldOfView*> _lights;
};

class PowerSocket : public LinkableEntity
{
public:
	PowerSocket(float x, float y, Circuit c);
	void activate(LinkableEntity* activator);
	void deactivate();
	bool isLive();
private:
	bool _live;
};

class SoundDetector : public LinkableEntity
{
public:
	SoundDetector(float x, float y, Circuit c);
	void unlink();
	void activate(LinkableEntity* activator);
private:
	bool _soundedAlarm;
};

class Alarm : public LinkableEntity
{
public:
	Alarm(float x, float y, Circuit c);
	void activate(LinkableEntity* activator);
	void deactivate();
	void setAnimating(bool b);
	bool isActivated();
	bool _sounded;
};

class Enemy;

class EnemyGun : public LinkableEntity
{
public:
	EnemyGun(float x, float y, Circuit c);
	void activate(LinkableEntity* activator);
	void fire(GunShotTraceType gstt);
	void update(unsigned int dT);
	Enemy* _enemy;
};

class Enemy : public LivingEntity
{
public:
	Enemy(float x, float y, Direction startingDir, bool startPatrol, EnemyType type);
	void update(unsigned int dT);
	void setPosition(float x, float y);
	void landOnGround();
	void seePlayer(float x, float y);
	void alertToPosition(float x, float y, AlertType aType, TargetType tType);
	bool hasSeenPlayer();
	void loseSightOfPlayer(bool wentInElevator, vec2f newTarget);
	bool canSeePlayer();
	void changeState(GuardState state);
	GuardState getState();
	AlertType getAlertType();
	vec2f getTarget();
	void setDesiredStairsAndDirection(Stairs* sw, StairTraversal st);
	bool goingForStairs();
	void setSecondaryTarget(Entity* ent);
	Entity* getSecondaryTarget();
	unsigned int getNumSwitchAttempts();
	void resetSwitchAttempts();
	void setStrongestLight(FieldOfView* fov);
	void setLightToActivate(FieldOfView* toActivate); //used for loading saved games only.
	void loseStrongestLight(); //called only when strongestLight is suddenly set to inactive.
	FieldOfView* getStrongestLight();
	FieldOfView* getLightToActivate();
	TargetType getTargetType();
	void forgetTarget();
	void setLinkableTarget(LinkableEntity* ls);
	LinkableEntity* getTargetSwitch();

	bool ignoringFall();
	void setIgnoreFall(bool b);
	void setReadyToShoot(bool b);
	void resetReactionTime();
	void setReactionTime(int time);
	int getReactionTime();
	EnemyType getType();
	void setHeldAtGunpoint(bool b);
	bool isHeldAtGunpoint();
	void setResolve(int timeToReact, GunShotTraceType gstt);
	void setSprite();
	void arriveAtStairs(Stairs* st);
	bool isAnimatingThroughStairs();
	void setStairMovement(StairTraversal st);

	//save game related only.
	bool isWaitingForAlert();
	void setWaitingForAlert(bool b);
	void setCanSeePlayer(bool b);

	bool _scene_trace_bullet;
	vec2f _shootTarget; //usually set to _target, unless gun is activated by something else...
	void _fireWeapon(GunShotTraceType gstt); //ugh.
	EnemyShotResolve getResolve();
	EnemyGun* _gun;

private:
	int _reactionTime; //time between seeing player and shooting.
	int _timeToResetReactionTime;
	EnemyShotResolve _resolve;
	bool _playerInSight;
	bool _hasSeenPlayer; //used to detect how many guards in a level has seen the player for statistical purposes.
	vec2f _target; //try to move here when INVESTIGATING.
	GuardState _state;
	bool _fullyPunched; //used to play punch sound once in punching animation.
	StairTraversal _desiredStairDirection;
	int _timeToPatrol;
	int _timeToTurn1;
	int _timeToTurn2;
	int _timeToHitSwitch;
	int _timeToAlert;
	bool _waitingForAlert;
	LinkableEntity* _targetSwitch;
	Entity* _secondaryTarget; //a handscanner or stairs.
	unsigned int _numSwitchAttempts;
	FieldOfView* _lightToActivate; //former strongest light that was just turned off. Try to "activate" again.
	FieldOfView* _strongestLight;
	TargetType _targetType;
	AlertType _alertType;
	bool _ignoreFall;
	bool _readyToShoot;
	bool _heldAtGunpoint;
	EnemyType _type;
};

class Player : public LivingEntity
{
public:
	Player(float x, float y, Direction startingDir);
	~Player();
	void update(unsigned int dT);
	void setVelX(float x);
	void setVelY(float y);
	void die();
	void landOnGround();
	void jump(float x, float y);
	bool isJumping();
	bool isPinning();
	bool isHacking();
	bool isAttachingDown();
	bool isGoingUpRoof();
	void pinEnemy(Enemy* enemy);
	void hackTerminal(MainComputer* computer);
	void punchPinnedEnemy();
	unsigned int getNumPunches();
	void releasePin();
	Enemy* getPinnedEnemy();
	AttachType getAttachType();
	AttachType getLastAttachType();
	void attachToVolume(CollisionVolume* volume, AttachType at);
	void detach();
	CollisionVolume* getAttachedVolume();
	bool isInElevator();
	void enterElevator(ElevatorDoor* door);
	void switchElevator(ElevatorDoor* door);
	void leaveElevator();
	ElevatorDoor* getElevatorDoor();
	void setStairMovement(StairTraversal st);
	void arriveAtStairs(Stairs* st);
	bool isAnimatingThroughStairs();
	int getArmRotation();
	void setArmRotation(int rotation);
	void setAimingGun(bool b);
	bool isAimingGun();
	unsigned int _lightVisibility; //between 0 and 100.
	unsigned int _numHackedTerminals;
	int _armRotation;
private:
	bool _jumping;
	bool _pinning;
	unsigned int _numPunches;
	bool _inElevator;
	AttachType _attach;
	AttachType _lastAttach;
	ElevatorDoor* _door;
	Enemy* _pinnedEnemy;
	MainComputer* _hackedTerminal;
	CollisionVolume* _attachedVolume; //The Collision Volume the player is attached to.
	bool _aimingGun;

	//deleted by AnimationManager.
	AnimationSequence* _runningSequence;
	AnimationSequence* _climbingUpSequence;
	AnimationSequence* _ceilingSequence;
	AnimationSequence* _hackingSequence;
	AnimationSequence* _attachDownSequence;
	AnimationSequence* _reachRoofSequence;
	AnimationSequence* _attachToCeiling;
	AnimationSequence* _attachFromCeiling;
	AnimationSequence* _enterStairs;
	AnimationSequence* _exitStairs;
	AnimationSequence* _walkingSequence;
};
#endif