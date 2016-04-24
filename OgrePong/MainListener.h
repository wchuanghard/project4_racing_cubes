#ifndef __MainListener_h_
#define __MainListener_h_

#include "Ogre.h"
#include "OgreFrameListener.h"


// Forward declaration of classes
namespace Ogre
{
	class Overlay;
	class RenderWindow;
}

class InputHandler;
class AIManager;
class World;
class PongCamera;
class ProjectileManager;
class Tank;

class MainListener : public Ogre::FrameListener
{
public:
	MainListener(Ogre::RenderWindow *mainWindow, InputHandler *inputHandler, AIManager *aiManager, World *world, PongCamera *cam, ProjectileManager *pm, Tank *mT);

	bool frameStarted(const Ogre::FrameEvent &evt);


protected:
	InputHandler *mInputHandler;
	AIManager *mAIManager;
	World *mWorld;
	Tank *mTank;
    PongCamera *mPongCamera;
	ProjectileManager *mProjectileManager;
	Ogre::RenderWindow *mRenderWindow;
	int x;
private:
	Ogre::Overlay *mGameOverOverlay;
	void createGameOverOverlay();
};

#endif