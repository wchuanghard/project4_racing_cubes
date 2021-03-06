#ifndef __OgrePong_h_
#define __OgrePong_h_

// Including header files within header files is frowned upon,
// but this one is requried for Ogre Strings.
#include "OgrePrerequisites.h"
#include "OgreWindowEventUtilities.h"
#include "VirtualClock.h"
namespace Ogre {
    class Root;
    class RenderWindow;
    class Camera;
    class SceneManager;
	class OverlaySystem;
}

class Tank;
class ProjectileManager;
class AIManager;
class InputHandler;
class World;
class PongCamera;
class MainListener;
class PongManager;
class DebugInterface;
class MenuInterface;
class VirtualClock;

class OgrePong :  public Ogre::WindowEventListener
{
public:
    OgrePong();
    ~OgrePong();
	VirtualClock *mGT;
    // Do all the application setup
    bool setup(void);

    // Start run
    void go(void);
	

protected:


    //  Load all the requrired resources (Mostly reading the file paths
    //  of various resources from the .cfg files)
    void setupResources(void);

    // Invoke the startup window for all of the Ogre settings (resolution, etc)
    bool configure(void);

    // Create all of the required classes and do setup for all non-rendering tasks
    void createScene(void);

    // Free up all memory & resources created in createScene
    void destroyScene(void);

    // Create the rendering camera (separate from the game camera -- the game camera
    //   contains the logic of how the camera should be moved, the rendering camera
    //   is used by the rendering system to render the scene.  So, the game camera 
    //   decides where the camera should be, and then makes calls to the rendering camera
    //   to move the camera
	void createCamera(void);
	void createRearCamera(void);
	void createViewports(void);
	void createCamera_ai1(void);
	void createCamera_ai2(void);
	void createCamera_ai3(void);

    void createCameraMenu(void);
    void setupMenuViewport();

    // The FrameListener is what receives callbacks from the rendering loop to handle 
    //  game logic
	void createFrameListener(void);
	
	MainListener *mPongFrameListener;
    DebugInterface *mDebugInterface;
    MenuInterface *mMenuInterface;
	Tank *mTank;
	ProjectileManager *mProjectileManager;
	AIManager *mAIManager;
	InputHandler *mInputHandler;
	World *mWorld;
    PongCamera *mPongCamera;
	PongCamera *mPongCameraRear;
	PongCamera *mPongCamera_ai1;
	PongCamera *mPongCamera_ai2;
	PongCamera *mPongCamera_ai3;
    PongCamera *mPongCameraMenu;
	PongManager *mPongMag;


    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
	Ogre::Camera* mRCamera;
	Ogre::Camera* mCamera_ai1;
	Ogre::Camera* mCamera_ai2;
	Ogre::Camera* mCamera_ai3;
    Ogre::Camera* mCameraMenu;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
	Ogre::RenderWindow* mWindow_ai1;
    Ogre::String mResourcePath;
	Ogre::OverlaySystem *mOverlaySystem;

 
    

};

#endif // #ifndef __OgrePong_h_