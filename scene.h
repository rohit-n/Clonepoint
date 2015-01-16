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

#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "entity.h"
#include "locator.h"
#include "map.h"
#include "intersect.h"

#define MAX_SAVES 3

class Scene;
typedef void (Scene::*interpFunc) (Entity*, vec2f, vec2f, int*, bool*, bool*, vec2f*);

enum TraceDirection
{
	TDHorizontal = 0,
	TDVertical
};

enum UsableEnts
{
	UESwitch = 0,
	UETerminal,
	UEElevator,
	UECircuitBox,
	NumUsableEnts
};

enum SavedGameEntityType
{
	SGET_Unknown = 0,
	SGET_Player,
	SGET_Door,
	SGET_Enemy,
	SGET_Objective,
	SGET_CircuitBox,
	SGET_LightFixture,
	SGET_Glass,
	SGET_Alarm,
	SGET_ElevatorDoor
};

struct SavedGameHeader
{
	char filename[32];
	int numLinks;
	int numEnts;
	bool playerFiredShot;
	unsigned int timeToSniper;
	unsigned int numPlayerBullets;
	unsigned int playerEnergy;
};

struct SavedGameLink
{
	vec2f start;
	vec2f end;
};

struct SavedGameEntityState
{
	SavedGameEntityType type;
	vec2f position;
	vec2f velocity;

	//Living Entity
	Direction dir;
	int stairTimer;
	bool onGround;
	bool alive;

	//Door
	int timeToClose; //for trap doors only.

	//Enemy
	GuardState gs;
	TargetType targetType;
	AlertType alertType;
	vec2f targetLocation;

	//Player
	AttachType at;

	//Generic

	//for player: index of attached collision volume.
	//for guard: index of targeted light switch.
	//for door: index of this door in level entity vector.
	//for main computer: index of this objective in level entity vector.
	//for glass collision volumes: index of this level collision volume vector.
	//for circuit box: index of this circuit box in level entity vector.
	int index1;
	//for player: index of entered elevator.
	//for guard: index of light to activate (if off).
	int index2;
	//for player: number of incomplete objectives.
	//for guard: index of this guard in level entity vector.
	int index3;

	//for guard: reaction time before shooting.
	int index4;

	//for guard: index of secondary target entity.
	int index5;

	//to save on space, b1 is used for several
	//different bools of linkable entities, and also
	//for whether an enemy is patrolling on spawn.
	bool b1;

	//enemy: canSeePlayer
	bool b2;

	//enemy
	EnemyShotResolve esr;
};

class Scene
{
public:
	Scene();
	~Scene();
	void loadMap(const char* filename, bool savegame);
	void reloadMap();
	void update(unsigned int dT);
	void updateEnemy(Enemy* enemy, unsigned int dT);
	void updateMotionScanner(MotionScanner* scanner);
	void updateSecurityCamera(SecurityCamera* camera);
	bool handleMotionScannerWithEnt(MotionScanner* scanner, LivingEntity* le);
	void updatePowerSocket(PowerSocket* socket);

	void updateCamera();
	void setCameraDims(int w, int h);

	void movePlayer(bool left, bool right, bool down, bool up);
	std::shared_ptr<Map> getMap();
	vec2f getPlayerPosition();
	vec2f getMouseDragPosition();
	Rect getCamera();
	Player* getPlayer();
	vec2f getTrajPointAt(int i);
	size_t getNumberOfTrajPoints();
	void calculateJumpTrajectory(int x, int y, unsigned int dT);
	bool inCrosslinkMode();
	bool isPlayerSelecting();
	void setPlayerSelecting(bool b);
	void toggleCrosslinkMode();
	void handleClick(int mx, int my, unsigned int dT);
	void handleLeftClickRelease(int mx, int my);

	void addNoise(int x, int y, int radius, bool alertGuards, AlertType atype, Alarm* alarm);
	//Nonlinkable entities that player can interact with
	LinkableEntity* getLinker();

	void warpPlayerTo(int mx, int my);
	void endLevel();
	bool isLevelOver();
	bool isDetachmentDelayed();
	void resetDelayDetachment();
	bool isElevatorLocked();
	void resetElevatorLock();
	void handleStairCollision(LivingEntity* le);
	void computeVisibility(FieldOfView* fov);
	void updateVisibility(FieldOfView* fov, int angle1, int angle2);
	void updateOverlappingFOVs(Door* door); //called when door is opened or closed.
	void calculatePlayerVisibility();
	bool isPlayerInFOV(FieldOfView* fov);
	void calculateStrongestLight(Enemy* enemy);

	Door* getDoorOfCollisionVolume(CollisionVolume* vol);
	void calculateOverlappingLightsOfDoor(Door* door);

	bool isCircuitUnlocked(Circuit c);
	void getCircuitColor(Circuit c, float &r, float &g, float &b);
	void crosslinkPan(int mouseX, int mouseY);

	void handleVaultDoorClose(Door* door);

	bool isPlayerInLight();
	void addPlayerJumpPower(float f);
	void subPlayerJumpChargeTime(int dec);
	Particle* getParticleAt(size_t i);
	size_t getNumberOfParticles();

	void handleMouse(unsigned int mx, unsigned int my);
	MouseOverObject getObjectMousedOver();
	float getLightEnteredAlpha();
	bool hasPlayerFiredShot();
	int getTimeToSniper();
	unsigned int getNumPlayerBullets();
	unsigned int getPlayerEnergy();
	int getObjectivesIncompleteTime();
	void setTimeToSniper(int time);
	void setNumPlayerBullets(unsigned int bullets);
	void setPlayerEnergy(unsigned int energy);

	//scene_guards.cpp
	void traceEnemyFOV(Enemy* enemy);
	void handleEnemyLightOff(Enemy* enemy);
	void handleEnemyPathfind(Enemy* enemy);
	void enemyTryOpenDoor(Enemy* enemy, Door* door);
	// void handlePathfindEnd(Enemy* enemy);
	bool stairsReachableToEnemy(Enemy* enemy, Stairs* stairs);
	bool switchReachableToEnemy(Enemy* enemy, LightSwitch* ls);

	//scene_physics.cpp
	void handleMapCollisions(LivingEntity* ent, unsigned int dT);
	void handleParticles();
	void glassShatter(CollisionVolume* vol, vec2f velocity);
	bool checkIfEntOnGround(LivingEntity* ent);
	void checkPlayerClimbFinish();
	void checkPlayerClimbDownFinish();
	void checkPlayerCeilingEnd();
	void tryPlayerAttachSide();
	void handlePlayerFrob(bool up);
	void handlePlayerPounceEnemy(Enemy* enemy, unsigned int dT);
	void handlePlayerShotWhileAttached();

	//scene_saved_game.cpp
	void loadGame(const char* filename);
	void saveGame(const char* filename);
	void getLinks(std::vector<SavedGameLink>* container);
	void getSGESs(std::vector<SavedGameEntityState>* container);
	void updateSaves(unsigned int dT);
	bool isLoadMenuVisible();
	void showLoadMenu(bool b);
	int getSaveTimeAt(unsigned int index);
	void loadAutosave(unsigned int index);
	void setTimeAt(unsigned int index, unsigned int time);
	int getSecondsSince(unsigned int index);
	vec2f getLaserEnd();

	//scene_trace.cpp
	bool isTraceBlocked(Entity* source, vec2f a, vec2f b, vec2f* c, int* volIndex, bool* isDoor, float vertEpsilon, interpFunc func);
	bool traceInDirection(Entity* source, vec2f a, vec2f b, vec2f* end, bool downOrRight, TraceDirection direction, int* volIndex, bool* isDoor, interpFunc func);
	void traceBullet(LivingEntity* entity, vec2f target, GunShotTraceType gstt, bool fired);
	void handleTraceHit(vec2f* oldv3interp, vec2f* v3interp, vec2f* c, CollisionVolume* volume);
	bool handleTraceHitMapBounds(vec2f* v3interp, vec2f* c);
	void bulletFiredFromEnemy(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void bulletFiredFromEnemyInvoluntary(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void bulletFiredFromPlayer(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void traceLaserSight(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void traceGuardSight(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void interpStandard(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end);
	void breakOnGlass(vec2f interpolatedPoint, vec2f start, bool* stop);
	void killEnemies(Entity* source, vec2f interpolatedPoint, bool* stop);
	bool isMouseCursorSeen(int mx, int my);
private:
	void makeInitialLinks();
	void attemptLinkBetween(vec2f p1, vec2f p2);
	void attemptLightLinkBetween(vec2f p1, vec2f p2);
	void addParticle(float x, float y, float vx, float vy);
	void playerJump();
	void setNewLinkAt(int mx, int my);
	vec2f _mouseDragPos; //used to draw link from an linkableEnd to the mouse pointer when being moved by player.
	std::unique_ptr<Player> _player;
	std::shared_ptr<Map> _currentMap;
	size_t _numCollideVols;
	Rect _camera;
	vec2f _trajPoints[200];
	size_t _stopTrajPoint;
	bool _crosslink; //are we in crosslink mode?
	bool _selecting; //is the user currently setting a link?

	//Nonlinkable entities that player can interact with...besides stairs.
	LinkableEntity* _linker; //pointer to linkableEntity that player is currently choosing a target for.
	LightSwitch* _switcher; //pointer to light switch that player may overlap.
	MainComputer* _computer; //pointer to main objective computer that player may overlap.
	ElevatorDoor* _elevator;
	CircuitBox* _circuitBox;

	vec2f _jumpImpulse;
	vec2f _laserEnd;

	//is the player overlapping with any of these?
	bool _playerOverlappingEnts[NumUsableEnts];

	unsigned int _totalObjectives; //starts out as the number of MainComputers in currentMap. Decreases as player deactivates them. Used to check for level end condition.
	Rect _endRect; //player ends the level by completing all objectives and colliding with this.

	//used to iterate linkable entities only.
	std::vector<std::shared_ptr<LinkableEntity> >::iterator linkBegin;
	std::vector<std::shared_ptr<LinkableEntity> >::iterator linkEnd;
	std::vector<std::shared_ptr<LinkableEntity> >::iterator linkIter;

	//statistics
	unsigned int _numEnemies;
	int _jumpPowerTimer; //time that LMB is held for calculating jump power. Starts at time depending on player's jump level, then decreases to zero.
	float _jumpPower; //ranges from 0 to 1.
	float _playerJumpPower;
	int _timeToFullChargeJump;
	bool _levelOver; //set to true when player intersects objectives met and objectivesIncomplete is 0. Used to switch game state.
	bool _moveDelay; //set to true when player switches attachment from side to ceiling or vice-versa. Set to false when movement keys are released. Made to ensure player does not immediate detach.
	bool _elevatorLocked;

	//used to simplify movement code.
	AttachType _testAttach;

	//unused variables for calls to trace functions.
	vec2f _unused_c;
	int _unused_index;
	bool _unused_bool;

	bool _circuitUnlocked[4];

	std::vector<std::unique_ptr<Particle> > _particles;

	MouseOverObject _mousedOverObject;
	int objectivesNotCompleteTimer;

	float _lightEnteredAlpha;
	bool _playerShotFired;
	int _timeToSniper;
	unsigned int _numPlayerBullets;
	unsigned int _playerEnergy;

	//saving
	char _mapFilename[32];
	unsigned int _currentSave;
	int _saveTimer;
	char _saveFilename[16];
	bool _loadMenuVisible;
	int _saveTimeSince[MAX_SAVES];
};
#endif