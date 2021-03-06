#ifndef __World_h_
#define __World_h_

#include "Tank.h"
#include <time.h>
#include "VirtualClock.h"
// Forward delcarations of Ogre classes.  Note the Ogre namespace!
namespace Ogre {
    class SceneNode;
    class SceneManager;
	class Vector3;
	class Overlay;
	class AxisAlignedBox;
}

// Forward delcarations of our own classes
class PongManager;
class InputHandler;
class RedBrick;
enum slideredbrick;
class WorldManager;
class MovingObject;
class PongCamera;
class InputHandler;
class Tank;
class ProjectileManager;
class Border_;
class VirtualClock;
class AIManager;

class World
{
public:
	int ais;
	float newtime;
	void World::createWorldState() ;
	int go;
	float batch_loss;
	int uScore;
	int ai1Score;
	int cllision;
	//Global time
	float GlblTime;
	int state;
	//neural network
	int ailevel;
	float dloss_dw1[10][7];
	float sum_dloss_dz[5];
	float sum_dloss_da[7];
	float dloss_da[7][6];
	float dy_da[6][7];
	float dloss_dy[6][7];
	float dloss_dw2[7][5];
	float y_t[7][6];
	float dloss_dz[6][5];
	float p[6][5];
	float M[6];
	float z[6][5];
	float z_1[6][7];
	float y[6][7];
	float softsum[6];
	float loss[6];
	string w1ID;
	string w2ID;
	string b1ID;
	string b2ID;
	int ai1[6][10];
	int ai2[6][10];
	int ai3[6][10];
	int lab[6][5];
	float x[6][10]; 
	float w1[10][7]; 
	float w2[7][5]; 
	float b1[7]; 
	float b2[5];
	//cotrol
	float U_u;
	float U_v;
	float u2;
	float u3;
	float u4;
	float AI_0_u;
	float AI_0_v;
	float AI_1_u;
	float AI_1_v;
	float AI_2_u;
	float AI_2_v;

    float SPEED_COEFFICIENT;

    float SPEED_COEFFICIENT_AI_0;
    float SPEED_COEFFICIENT_AI_1;
    float SPEED_COEFFICIENT_AI_2;

	Ogre::Camera* mCamera;
	Ogre::Camera* mRCamera;
	Ogre::Vector3 World::Normal(float u, float v);
	Ogre::Vector3 World::Tangent(float u, float v);
	Ogre::Vector3 World::Binormal(float u, float v);
	Ogre::Matrix3 World::Orientation(Ogre::Vector3 xBasis, Ogre::Vector3 yBasis, Ogre::Vector3 zBasis);
	int Switch;
	int Mode;
	World(Ogre::SceneManager *sceneManager, InputHandler *input, Tank *tank, VirtualClock *mVT);
	~World();
	MovingObject *mObj1;
    MovingObject *mObj2;
    MovingObject *mObj3;
	MovingObject *mObj4;

    MovingObject *mCrossHair;

    //InputHandler *mInputHandler;
	Ogre::Real t;
	Border_* getBorder_2() const { return mBorder_2; }
	Border_* getBorder_3() const { return mBorder_3; }
	Border_* getBorder_1() const { return mBorder_1; }
	Border_* getBorder_4() const { return mBorder_4; }
	//    void think(const Ogre::Real& time);

	Tank* getpong() const { return mpong; }
	Tank* getpong1() const { return mpong1; }
	Tank* getpong2() const { return mpong2; }
	Tank* getpong3() const { return mpong3; }
	Tank* getpong4() const { return mpong4; }
	Tank* getpong5() const { return mpong5; }
	Tank* getpong6() const { return mpong6; }
	void getEventFrom(PongManager *pongManager);
	RedBrick* getAIRedBrick() const { return mAIRedBrick; }
	RedBrick* getUserRedBrick() const { return mUserRedBrick; }
	RedBrick* getAIPlayer() const { return mAIPlayer; }
	int getUserScore() const { return mUserScore; }
	void setUserScore(int score) { mUserScore = score; }
	RedBrick* getUser() const { return mUser; }
	void reset();

	int getAIPlayerScore() const { return mAIScore; }
	void setAIPlayerScore(int score) { mAIScore = score; }
	void updateLevel();
	bool flag();
	int getLevel() { return mLevel; }
	
	int mLevel;
	void World::Think(const Ogre::Real& mTime);
    //void Think(float mTime);
	void yawTank(InputHandler *mInputHandler, const Ogre::Real& mTime);
	void fly_or_dive_Tank(InputHandler *mInputHandler, const Ogre::Real& mTime);
	
	void addTank(Tank *t) {mTank = t; }
	void addProjectile(ProjectileManager *pm) { mProjectileManager = pm; }
	void setIterator();
    void recordMove();

    void moveCubes();
    void moveCubes2();

    Tank::Node *getIterator() {return iterator;}

    VirtualClock* getClock() {return mClock; }

	
	//Tank *getHumanTank() { return mHumanPlayer; }
	
	void Push(Tank::Node *node);

	/* Linked list structure that will contain AI tank nodes */
	struct LinkedList {
		Tank::Node *head;
	} mList;

	/* List of AI tanks */
	LinkedList *tanks;

	Ogre::SceneManager *SceneManager() { return mSceneManager; }
	Tank* getHumanPlayer() const { return mHuman; }
    Ogre::Vector3 *spawnPoints;
	Tank *mHuman;
	float Hpoint_x;
	/*Ogre::Real Hpoint_y;
	Ogre::Real Hpoint_z;
	Ogre::Real AIpoint_x;
	Ogre::Real AIpoint_y;

	Ogre::Real AIpoint_z;*/
	time_t start, end, diff;
	ProjectileManager *mProjectileManager;
	float PROJECTILE_SPEED_H;
	float PROJECTILE_SPEED_V;
private:

	Ogre::Overlay *mGameOverOverlay;
	void createGameOverOverlay();
    Ogre::Vector3 mTankVelocity;
	Border_* mBorder_2;
	Border_* mBorder_3;
	Border_* mBorder_4;
	Border_* mBorder_1;
	AIManager *mAIManager;
	void createModifiers();
	int mScore;
	Ogre::Overlay* mOverlay;
	Tank* mpong;
	Tank* mpong1;
	Tank* mpong2;
	Tank* mpong3;
	Tank* mpong4;
	Tank* mpong5;
	Tank* mpong6;
	RedBrick* mUserRedBrick;
	RedBrick* mAIRedBrick;
	RedBrick* mAIPlayer;
	RedBrick* mUser;
	WorldManager *mWorldManager;
	int mUserScore;
	int mAIScore;
	void createOverlay();
	void initialStates();
protected:
	Ogre::SceneManager* getSceneManager() { return mSceneManager; }
	
	Ogre::SceneManager *mSceneManager;
	InputHandler *mInputHandler;
	VirtualClock *mClock;
	Tank *mTank;
	
	
	//Ogre::SceneNode *mFloorNode;

	/* Used to iterate through AI tank list to see if any user tank collides with another tank */
	Tank::Node *iterator;

	
};

#endif