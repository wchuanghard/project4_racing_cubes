// My header file.  This should go first!
#include "World.h"
#include "Properties.h"
// Ogre header files
#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreVector3.h"
#include "PongObject.h"
#include "MovingObject.h"
#include <math.h> 
#include "Windows.h"
#include "MMsystem.h"
#include <mciapi.h>

// IOS (Input system) header files

#include <ois/ois.h>

// Other input files for my project
#include "Camera.h"
#include "InputHandler.h"
#include "border.h"
#include "CollisionManager.h"

#include "OgreColourValue.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "RedBrick.h"
#include "slideredbrick.h"
#include "AIManager.h"
#include "PongManager.h"
#include "WorldManager.h"
#include <vector>
#include "VirtualClock.h"

 #define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}
World::World(Ogre::SceneManager *sceneManager, InputHandler *input, Tank *tank, VirtualClock *mVT)   : mSceneManager(sceneManager), mInputHandler(input), mTank(tank), mClock(mVT)
{
	createWorldState();
	ailevel = 0;
}

void World::createWorldState() {
	
	//mLevel = 1;
	newtime = 0;
	go==1;
	batch_loss = 0.0f;
	U_v=0.0f;
	AI_0_v=0.0f;
	//PlaySound(TEXT("Hitman.mp3"), NULL, SND_FILENAME);
	#pragma comment(lib, "Winmm.lib")
	mciSendString("open \"Hitman.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString("play mp3", NULL, 0, NULL);
	mciSendString("play mp3 repeat", NULL, 0, NULL);
	cllision = 0; //with collision
	uScore = 0;
	ai1Score = 0;
    mTankVelocity = Ogre::Vector3(0,0,0);
	state = 0;//pause
	
	Switch = 0;
	GlblTime = mClock->report();
	Mode = 1;//0 is prediction; 1 is training, and this value is only could be changed by designer manually 
	/*w1ID = "Trained_w1_binary";
	w2ID = "Trained_w2_binary";
	b1ID = "Trained_b1_binary";
	b2ID = "Trained_b2_binary";
	*/
	w1ID = "Trained_w1_readable.txt";
	w2ID = "Trained_w2_readable.txt";
	b1ID = "Trained_b1_readable.txt";
	b2ID = "Trained_b2_readable.txt";	//The following are the setup (initial conditions) for the first time training
	/*w1ID = "w1_readable.txt";
	w2ID = "w2_readable.txt";
	b1ID = "b1_readable.txt";
	b2ID = "b2_readable.txt";
	*/
	if(Mode==1)//read in the input parameters
	{
		ifstream fin_w1;
		fin_w1.open(w1ID,ios::binary);
		for (int i = 0; i < 9; i++)
		{
			// iterate over pixels in scanline
			for (int j = 0; j < 6; j++)
			{
				float my_int2_ai3;
				fin_w1.read((char*)&my_int2_ai3,sizeof(float));
				w1[i][j]=my_int2_ai3;
				//if(i%100==0)DBOUT(" my_int2_ai3: " <<  my_int2_ai3);
			}
		}
		fin_w1.close();
		ifstream fin_w2;

		fin_w2.open(w2ID,ios::binary);
		for (int i = 0; i < 6; i++)
		{
			// iterate over pixels in scanline
			for (int j = 0; j < 4; j++)
			{
				float my_int2_w2;
				fin_w2.read((char*)&my_int2_w2,sizeof(float));
				w2[i][j]=my_int2_w2;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
			}
		}
		fin_w2.close();
		ifstream fin_b1;

		fin_b1.open(b1ID,ios::binary);
		for (int i = 0; i < 6; i++)
		{
			// iterate over pixels in scanline
				float my_int2_b1;
				fin_b1.read((char*)&my_int2_b1,sizeof(float));
				b1[i]=my_int2_b1;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
		}
		fin_b1.close();
		ifstream fin_b2;
		fin_b2.open(b2ID,ios::binary);
		for (int i = 0; i < 4; i++)
		{
			// iterate over pixels in scanline
				float my_int2_b2;
				fin_b2.read((char*)&my_int2_b2,sizeof(float));
				b2[i]=my_int2_b2;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
		}
		fin_b2.close();
	}
	PROJECTILE_SPEED_V=100;
	PROJECTILE_SPEED_H=100;
	mScore = 0;
	mWorldManager = new WorldManager(this);
	initialStates();
	mUser = mUserRedBrick;
	mAIPlayer = mAIRedBrick;
	mBorder_4 = new Border_(mSceneManager, Borde_dim_0);
	mBorder_4->load();
	mBorder_1 = new Border_(mSceneManager, Borde_dim_0);
	mBorder_1->load();
	mBorder_3 = new Border_(mSceneManager, Borde_dim_0);
	mBorder_3->load();
	mBorder_2 = new Border_(mSceneManager, Borde_dim_0);
	mBorder_2->load();
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));
	mUserRedBrick = new RedBrick(mSceneManager, RB_DIM_0);
	mUserRedBrick->load();
	mAIRedBrick = new RedBrick(mSceneManager, RB_DIM_0);
	mAIRedBrick->load();
	createOverlay();
	tanks = new LinkedList();
	tanks->head = NULL;
	/* Initialize positions for an AI tank */
	spawnPoints = new Ogre::Vector3[50];
	spawnPoints[0] = Ogre::Vector3(900, 0, 10);
	spawnPoints[1] = Ogre::Vector3(700, 0, 200);
	spawnPoints[2] = Ogre::Vector3(510, 0, 400);
	spawnPoints[3] = Ogre::Vector3(3343, 0, 6540);
	spawnPoints[4] = Ogre::Vector3(42450, 0, 5550);
	spawnPoints[5] = Ogre::Vector3(5234, 0, 4560);
	spawnPoints[6] = Ogre::Vector3(6345, 0, 3570);
	spawnPoints[7] = Ogre::Vector3(7765, 0, 2580);
	spawnPoints[8] = Ogre::Vector3(8450, 0, 1590);
	spawnPoints[9] = Ogre::Vector3(93870, 0, 90);

	spawnPoints[10] = Ogre::Vector3(-120, 1230, 9500);
	spawnPoints[11] = Ogre::Vector3(-1243, 3210, 8500);
	spawnPoints[12] = Ogre::Vector3(-234, 2340, 7500);
	spawnPoints[13] = Ogre::Vector3(-30, 5340, 6500);
	spawnPoints[14] = Ogre::Vector3(-500, 6450, 5520);
	spawnPoints[15] = Ogre::Vector3(-300, 0, 4530);
	spawnPoints[16] = Ogre::Vector3(-300, 2430, 3540);
	spawnPoints[17] = Ogre::Vector3(-2300, 8670, 2500);
	spawnPoints[18] = Ogre::Vector3(-1300, 340, 1560);
	spawnPoints[19] = Ogre::Vector3(-800, 4320, 570);

	spawnPoints[20] = Ogre::Vector3(-1300, 10, 9100);
	spawnPoints[21] = Ogre::Vector3(-2300, 20, 8200);
	spawnPoints[22] = Ogre::Vector3(-3300, 30, 7300);
	spawnPoints[23] = Ogre::Vector3(-4300, 40, 9);
	spawnPoints[24] = Ogre::Vector3(-5300, 50, 5500);
	spawnPoints[25] = Ogre::Vector3(-6300, 60, 4600);
	spawnPoints[26] = Ogre::Vector3(-7300, 70, 3700);
	spawnPoints[27] = Ogre::Vector3(-8300, 80, 2800);
	spawnPoints[28] = Ogre::Vector3(-9300, 90, 900);
	spawnPoints[29] = Ogre::Vector3(-10300, 100, 1000);

	spawnPoints[30] = Ogre::Vector3(1300, 80, -500);
	spawnPoints[31] = Ogre::Vector3(2300, 70, -1500);
	spawnPoints[32] = Ogre::Vector3(3300, 4, -1000);
	spawnPoints[33] = Ogre::Vector3(4300, 30, -5000);
	spawnPoints[34] = Ogre::Vector3(5300, 50, -520);
	spawnPoints[35] = Ogre::Vector3(6300, 2, -530);
	spawnPoints[36] = Ogre::Vector3(7300, 20, -540);
	spawnPoints[37] = Ogre::Vector3(8300, 40, -550);
	spawnPoints[38] = Ogre::Vector3(9300, 0, -560);
	spawnPoints[39] = Ogre::Vector3(10300, 0, -570);

	spawnPoints[40] = Ogre::Vector3(300, -10, 9500);
	spawnPoints[41] = Ogre::Vector3(4300, -205, 8500);
	spawnPoints[42] = Ogre::Vector3(350, -303, 7500);
	spawnPoints[43] = Ogre::Vector3(7340, -440, 6100);
	spawnPoints[44] = Ogre::Vector3(5330, -530, 2500);
	spawnPoints[45] = Ogre::Vector3(3320, -650, 4300);
	spawnPoints[46] = Ogre::Vector3(4310, -720, 3400);
	spawnPoints[47] = Ogre::Vector3(6390, -860, 2500);
	spawnPoints[48] = Ogre::Vector3(1200, -940, 1600);
	spawnPoints[49] = Ogre::Vector3(1900, -100, 700);
	start = time(NULL);

	U_u = 0.0f;
	U_v = 0.0f;
	AI_0_u = 0.0f;
	AI_0_v = 0.0f;
	AI_1_u = 0.0f;
	AI_1_v = 0.0f;
	AI_2_u = 0.0f;
	AI_2_v = 0.0f;
    SPEED_COEFFICIENT = 10.0;
    SPEED_COEFFICIENT_AI_0 = 10.0;
    SPEED_COEFFICIENT_AI_1 = 10.0;
    SPEED_COEFFICIENT_AI_2 = 10.0;

	if(Mode==0){
	if(ailevel<1){
		string w1ID = "w1_readable.txt";
		ofstream outtxt_ai2( w1ID );
		for(int j=0; j< 6; j++)
		{
			for(int k=0; k< 9; k++)
			{
				w1[k][j]= 1.0f* ((double) rand() / (RAND_MAX));
				//write in text file 
			    outtxt_ai2 << w1[k][j] << flush;
			    outtxt_ai2 << ", "<< flush;
				
			}
			outtxt_ai2 << endl;
		}
		outtxt_ai2.close();

		string w2ID = "w2_readable.txt";
		ofstream outtxt_w2( w2ID );
		for(int j=0; j< 4; j++)
		{
			for(int k=0; k< 6; k++)
			{
				w2[k][j]= 1.0f* ((double) rand() / (RAND_MAX));
				outtxt_w2 << w2[k][j] << flush;
			    outtxt_w2 << ", "<< flush;
			}
			outtxt_w2 << endl;
		}
		outtxt_w2.close();

		string b1ID = "b1_readable.txt";
		ofstream outtxt_b1( b1ID );
		for(int k=0; k< 6; k++)
		{
			b1[k]= 1.0f* ((double) rand() / (RAND_MAX));
			outtxt_b1 << b1[k] << flush;
			outtxt_b1 << ", "<< flush;
			
		}
		outtxt_b1.close();

		string b2ID = "b2_readable.txt";
		ofstream outtxt_b2( b2ID );
		for(int k=0; k< 4; k++)
		{
			b2[k]= 1.0f* ((double) rand() / (RAND_MAX));
			outtxt_b2 << b2[k] << flush;
			outtxt_b2 << ", "<< flush;
			outtxt_b2 << endl;
		}		
		outtxt_b2.close();

	}
	else if(ailevel==1){
		ifstream fin_w1;
		fin_w1.open(w1ID,ios::binary);
		for (int i = 0; i < 9; i++)
		{
			// iterate over pixels in scanline
			for (int j = 0; j < 6; j++)
			{
				float my_int2_ai3;
				fin_w1.read((char*)&my_int2_ai3,sizeof(float));
				w1[i][j]=my_int2_ai3;
				//if(i%100==0)DBOUT(" my_int2_ai3: " <<  my_int2_ai3);
			}
		}
		fin_w1.close();

		ifstream fin_w2;
		fin_w2.open(w2ID,ios::binary);
		for (int i = 0; i < 6; i++)
		{
			// iterate over pixels in scanline
			for (int j = 0; j < 4; j++)
			{
				float my_int2_w2;
				fin_w2.read((char*)&my_int2_w2,sizeof(float));
				w2[i][j]=my_int2_w2;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
			}
		}
		fin_w2.close();

		ifstream fin_b1;
		fin_b1.open(b1ID,ios::binary);
		for (int i = 0; i < 6; i++)
		{
			// iterate over pixels in scanline
				float my_int2_b1;
				fin_b1.read((char*)&my_int2_b1,sizeof(float));
				b1[i]=my_int2_b1;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
		}
		fin_b1.close();
		ifstream fin_b2;
		fin_b2.open(b2ID,ios::binary);
		for (int i = 0; i < 4; i++)
		{
			// iterate over pixels in scanline
				float my_int2_b2;
				fin_b2.read((char*)&my_int2_b2,sizeof(float));
				b2[i]=my_int2_b2;
				//if(i%100==0)DBOUT(" my_int2_w2: " <<  my_int2_ai3);
		}
		fin_b2.close();

	}
	else if(ailevel==2){

	}
	}
}



//flying_or_diving
void
World::fly_or_dive_Tank(InputHandler *mInputHandler, const Ogre::Real& mTime)
{
	const float RADIANS_PER_SECOND = 1.2;
/*	if (mInputHandler->IsKeyDown(OIS::KC_W)) 
		mTank->mMainNode->translate(Ogre::Vector3(0,10,0)*mTime*10);
	else */mTank->mMainNode->translate(Ogre::Vector3(0,-10,0)*mTime*20);

    //mTank->mOBB->setPosition(mTank->mMainNode->getPosition());


}
//Rotating
void
World::yawTank(InputHandler *mInputHandler, const Ogre::Real& mTime)
{
	const float RADIANS_PER_SECOND = 1.2;
	if (mInputHandler->IsKeyDown(OIS::KC_LEFT))
    {
		mTank->mMainNode->yaw(Ogre::Radian(mTime * RADIANS_PER_SECOND));
        //mTank->mOBB->setOrientation(mTank->mMainNode->getOrientation());
    }
	else 
    {
        mTank->mMainNode->yaw(-Ogre::Radian(mTime * RADIANS_PER_SECOND));
        //mTank->mOBB->setOrientation(mTank->mMainNode->getOrientation());
    }
    

}
void
World::setIterator() 
{
	iterator = tanks->head;
}
void
World::Push(Tank::Node *node)
{
	node->next = tanks->head;
	tanks->head = node;
}

Ogre::Vector3 World::Normal(float u, float v){
	float kappa = 1;
	
	float A=kappa*(-sin(u)+ v*((-1/2)*sin(u/2)*cos(u)-cos(u/2)*sin(u)));
	float B=kappa*(cos(u)+ v*((-1/2)*sin(u/2)*sin(u)+cos(u/2)*cos(u)));
	float C=kappa*((v/2)*cos(u/2));
	float D=kappa*(cos(u/2)*cos(u));
	float E=kappa*(cos(u/2)*sin(u));
	float F=kappa*(sin(u/2));

	float N_x = B*F-C*E;
	float N_y = C*D-A*F;
	float N_z = A*E-B*D;
	Ogre::Vector3 N = Ogre::Vector3(N_x,N_y,N_z);
	return N;
}
Ogre::Vector3 World::Tangent(float u, float v){
	float kappa = 1;
	
	float A=kappa*(-sin(u)+ v*((-1/2)*sin(u/2)*cos(u)-cos(u/2)*sin(u)));
	float B=kappa*(cos(u)+ v*((-1/2)*sin(u/2)*sin(u)+cos(u/2)*cos(u)));
	float C=kappa*((v/2)*cos(u/2));
	Ogre::Vector3 T = Ogre::Vector3(A,B,C);
	return T;
}
Ogre::Vector3 World::Binormal(float u, float v){
	float kappa = 1;
	
	float D=kappa*(cos(u/2)*cos(u));
	float E=kappa*(cos(u/2)*sin(u));
	float F=kappa*(sin(u/2));
	Ogre::Vector3 B = Ogre::Vector3(D,E,F);
	return B;
}
Ogre::Matrix3 World::Orientation(Ogre::Vector3 xBasis, Ogre::Vector3 yBasis, Ogre::Vector3 zBasis){
    Ogre::Matrix3 rotate;
    rotate.FromAxes(xBasis, yBasis, zBasis);
	return rotate;
}

void World::Think(const Ogre::Real& mTime)
{
	updateLevel();


	GlblTime = mClock->report();

    //moveCubes2();
	Ogre::Vector3 position = SceneManager()->getSceneNode("U")->getPosition();
	float u = 2*asin(position.z/1000);
	Ogre::Vector3 N;
	float ftime = mClock->report(); //Ogre::ControllerManager::getSingleton().getElapsedTime();
    u=ftime;
	U_u=-ftime/SPEED_COEFFICIENT;// ORIGINALLY -ftime/10
    AI_0_u=-ftime/SPEED_COEFFICIENT_AI_0;// ORIGINALLY -ftime/10
    AI_1_u=-ftime/SPEED_COEFFICIENT_AI_1;// ORIGINALLY -ftime/10
	AI_2_u=-ftime/SPEED_COEFFICIENT_AI_2;// ORIGINALLY -ftime/10
	
	float u1 = u+U_u;
    u2 = u+AI_0_u;
    u3 = u+AI_1_u;
	u4 = u+AI_2_u;
    //For Debug use:
    //"AItank0" will make User cube Designer

    mTank->getMovingObject("U")->setPosition(Ogre::Vector3(1000*cos(u2)+1000*cos(u2/2)*cos(u2)*(AI_0_v),1000*sin(u2)+1000*cos(u2/2)*sin(u2)*(AI_0_v),1000*sin(u2/2)*(AI_0_v)));
    mTank->getMovingObject("U")->translate(Normal(u2, AI_0_v)*10);
    mTank->getMovingObject("U")->setOrientation(Orientation(Binormal(u2, AI_0_v),Normal(u2, AI_0_v),Tangent(u2,AI_0_v)));


    /*mTank->getMovingObject("AItank2")->setPosition(Ogre::Vector3(1000*cos(u2)+1000*cos(u2/2)*cos(u2)*(AI_0_v),1000*sin(u2)+1000*cos(u2/2)*sin(u2)*(AI_0_v),1000*sin(u2/2)*(AI_0_v)));
    mTank->getMovingObject("AItank2")->translate(Normal(u2, AI_0_v)*10);
    mTank->getMovingObject("AItank2")->setOrientation(Orientation(Binormal(u2, AI_0_v),Normal(u2, AI_0_v),Tangent(u2,AI_0_v)));
   
    mTank->getMovingObject("AItank3")->setPosition(Ogre::Vector3(1000*cos(u3)+1000*cos(u3/2)*cos(u3)*(AI_1_v),1000*sin(u3)+1000*cos(u3/2)*sin(u3)*(AI_1_v),1000*sin(u3/2)*(AI_1_v)));
    mTank->getMovingObject("AItank3")->translate(Normal(u3, AI_1_v)*10);
    mTank->getMovingObject("AItank3")->setOrientation(Orientation(Binormal(u3, AI_1_v),Normal(u3, AI_1_v),Tangent(u3,AI_1_v)));
	*/

    /*mTank->getMovingObject("U")->setPosition(Ogre::Vector3(1000*cos(u4)+1000*cos(u4/2)*cos(u4)*(AI_2_v),1000*sin(u4)+1000*cos(u4/2)*sin(u3)*(AI_2_v),1000*sin(u4/2)*(AI_2_v)));
    mTank->getMovingObject("U")->translate(Normal(u4, AI_2_v)*10);
    mTank->getMovingObject("U")->setOrientation(Orientation(Binormal(u4, AI_2_v),Normal(u4, AI_2_v),Tangent(u4,AI_2_v)));
    u+=2;*/

    ////////////////////////////////////////////////////////
    ////////////// START OF NOT "SMART" AI's////////////////
    ////////////////////////////////////////////////////////

	/*mTank->getMovingObject("AItank4")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.6),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.6),1000*sin(u/2)*(-0.6)));
	mTank->getMovingObject("AItank4")->translate(Normal(u, -0.6)*11);
	mTank->getMovingObject("AItank4")->setOrientation(Orientation(Binormal(u, -0.6),Normal(u, -0.6),Tangent(u,-0.6)));
	u+=5;
	mTank->getMovingObject("AItank5")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.8),1000*sin(u/2)*(-0.8)));
	mTank->getMovingObject("AItank5")->translate(Normal(u, -0.8)*11);
	mTank->getMovingObject("AItank5")->setOrientation(Orientation(Binormal(u, 1/7),Normal(u, -0.8),Tangent(u,-0.8)));
	u+=5;
	mTank->getMovingObject("AItank6")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(0.8),1000*sin(u/2)*(0.8)));
	mTank->getMovingObject("AItank6")->translate(Normal(u, 0.8)*11);
	mTank->getMovingObject("AItank6")->setOrientation(Orientation(Binormal(u, 0.8),Normal(u, 0.8),Tangent(u,0.8)));
	u+=5;
	mTank->getMovingObject("AItank7")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank7")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank7")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank8")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(0),1000*sin(u)+1000*cos(u/2)*sin(u)*(0),1000*sin(u/2)*(0)));
	mTank->getMovingObject("AItank8")->translate(Normal(u, 0)*11);
	mTank->getMovingObject("AItank8")->setOrientation(Orientation(Binormal(u, 0),Normal(u, 0),Tangent(u,0)));
	u+=5;
	mTank->getMovingObject("AItank9")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1),1000*sin(u/2)*(-1)));
	mTank->getMovingObject("AItank9")->translate(Normal(u, -1)*11);
	mTank->getMovingObject("AItank9")->setOrientation(Orientation(Binormal(u, -1),Normal(u, -1),Tangent(u,-1)));
	u+=5;
	mTank->getMovingObject("AItank10")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank10")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank10")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank11")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank11")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank11")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank12")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/14),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/14),1000*sin(u/2)*(1/14)));
	mTank->getMovingObject("AItank12")->translate(Normal(u, 1/14)*11);
	mTank->getMovingObject("AItank12")->setOrientation(Orientation(Binormal(u, 1/14),Normal(u, 1/14),Tangent(u,1/14)));
	u+=5;
	mTank->getMovingObject("AItank13")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/15),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/15),1000*sin(u/2)*(1/15)));
	mTank->getMovingObject("AItank13")->translate(Normal(u, 1/15)*11);
	mTank->getMovingObject("AItank13")->setOrientation(Orientation(Binormal(u, 1/15),Normal(u, 1/15),Tangent(u,1/15)));
	u+=5;
	mTank->getMovingObject("AItank14")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/16),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/16),1000*sin(u/2)*(1/16)));
	mTank->getMovingObject("AItank14")->translate(Normal(u, 1/16)*11);
	mTank->getMovingObject("AItank14")->setOrientation(Orientation(Binormal(u, 1/16),Normal(u, 1/16),Tangent(u,1/16)));
	u+=5;
	mTank->getMovingObject("AItank15")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/17),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/17),1000*sin(u/2)*(1/17)));
	mTank->getMovingObject("AItank15")->translate(Normal(u, 1/17)*11);
	mTank->getMovingObject("AItank15")->setOrientation(Orientation(Binormal(u, 1/17),Normal(u, 1/17),Tangent(u,1/17)));
	u+=5;
	mTank->getMovingObject("AItank16")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/18),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/18),1000*sin(u/2)*(1/18)));
	mTank->getMovingObject("AItank16")->translate(Normal(u, 1/18)*11);
	mTank->getMovingObject("AItank16")->setOrientation(Orientation(Binormal(u, 1/18),Normal(u, 1/18),Tangent(u,1/18)));
	u+=5;
	mTank->getMovingObject("AItank17")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/19),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/19),1000*sin(u/2)*(1/19)));
	mTank->getMovingObject("AItank17")->translate(Normal(u, 1/19)*11);
	mTank->getMovingObject("AItank17")->setOrientation(Orientation(Binormal(u, 1/19),Normal(u, 1/19),Tangent(u,1/19)));
	u+=5;
	mTank->getMovingObject("AItank18")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/20),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/20),1000*sin(u/2)*(1/20)));
	mTank->getMovingObject("AItank18")->translate(Normal(u, 1/20)*11);
	mTank->getMovingObject("AItank18")->setOrientation(Orientation(Binormal(u, 1/20),Normal(u, 1/20),Tangent(u,1/20)));
	u+=5;
	mTank->getMovingObject("AItank19")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/2),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/2),1000*sin(u/2)*(-1/2)));
	mTank->getMovingObject("AItank19")->translate(Normal(u, -1/2)*11);
	mTank->getMovingObject("AItank19")->setOrientation(Orientation(Binormal(u, -1/2),Normal(u, -1/2),Tangent(u,-1/2)));
	u+=5;
	mTank->getMovingObject("AItank20")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/3),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/3),1000*sin(u/2)*(-1/3)));
	mTank->getMovingObject("AItank20")->translate(Normal(u, -1/3)*11);
	mTank->getMovingObject("AItank20")->setOrientation(Orientation(Binormal(u, -1/3),Normal(u, -1/3),Tangent(u,-1/3)));
	*/

	/*mObj1->yaw(Ogre::Radian(0.5f * mTime));
    mObj2->pitch(Ogre::Radian(0.6f * mTime));
    mObj3->roll(Ogre::Radian(0.5f * mTime));
	mObj4->translate(mTime * Ogre::Vector3(-1, 0,0));
	*/



	FILE * pFile;
	char buffer [100];
   pFile = fopen ("w1.txt" , "r");
   if (pFile == NULL) 
   {

   }
   else
   {
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       fputs (buffer , stdout);
     }
     fclose (pFile);
   }
      pFile = fopen ("w2.txt" , "r");
   if (pFile == NULL) 
   {

   }
   else
   {
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       fputs (buffer , stdout);
     }
     fclose (pFile);
   }
      pFile = fopen ("b1.txt" , "r");
   if (pFile == NULL) 
   {

   }
   else
   {
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       fputs (buffer , stdout);
     }
     fclose (pFile);
   }
      pFile = fopen ("b2.txt" , "r");
   if (pFile == NULL) 
   {

   }
   else
   {
     while ( ! feof (pFile) )
     {
       if ( fgets (buffer , 100 , pFile) == NULL ) break;
       fputs (buffer , stdout);
     }
     fclose (pFile);
   }


	mWorldManager->doWork();
	t=mTime;


	int k = mClock->report();//Ogre::ControllerManager::getSingleton().getElapsedTime();
	int cd = 95-k;
	if (cd<5){
		Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	     mGameOverOverlay = om.getByName("GameOver");
		 mGameOverOverlay->show();
	}


	//mpong->move(mTime);
	const float TANK_SPEED = 50;
	end = time(NULL);
	diff = end - start;

    //LEFT,RIGHT,UP,DOWN MOVE DESIGNER MODE CUBE

	if (mInputHandler->IsKeyDown(OIS::KC_LEFT) || mInputHandler->IsKeyDown(OIS::KC_RIGHT))
    {
		yawTank(mInputHandler, mTime);
    }
    else if (mInputHandler->IsKeyDown(OIS::KC_UP))
	{
        Ogre::Vector3 mTankPosition = mTank->mMainNode->getPosition();
        //Ogre::Vector3 mTankFacing = mTank->mMainNode->getOrientation() * Ogre::Vector3::UNIT_Z;

        mTankVelocity = mTank->mMainNode->getFacing() * 300 * mTime;
        mTank->mMainNode->setPosition(mTankPosition + mTankVelocity);
        //DBOUT("tank velocity" << Ogre::StringConverter::toString(mTankVelocity).c_str());
        //mTank->mMainNode->setPosition(mTankPosition + mTankVelocity);

        //mTank->mMainNode->translate(Ogre::Vector3(0, 0, mTime * TANK_SPEED *20) * mTank->mMainNode->getFacing());
        //mTank->mOBB->setPosition(mTank->mMainNode->getPosition());

        
	}
	else if (mInputHandler->IsKeyDown(OIS::KC_DOWN))
	{
        Ogre::Vector3 mTankPosition = mTank->mMainNode->getPosition();
        mTankVelocity = mTank->mMainNode->getFacing() * 300 * -mTime;
        mTank->mMainNode->setPosition(mTankPosition + mTankVelocity);
        //mTank->mMainNode->translate(Ogre::Vector3(0, 0, -mTime*20 * TANK_SPEED) * mTank->mMainNode->getFacing());
        //mTank->mOBB->setPosition(mTank->mMainNode->getPosition());

	}
	if (mInputHandler->IsKeyDown(OIS::KC_Z))
	{
		fly_or_dive_Tank(mInputHandler, mTime);
                recordMove();

	}
	if (mInputHandler->IsKeyDown(OIS::KC_X))
	{
		fly_or_dive_Tank(mInputHandler, -mTime);
	}


    //J,L,I,K Move cube 1
	if (mInputHandler->IsKeyDown(OIS::KC_J))
	{
		if(AI_2_v<0.5){
			AI_2_v+=0.05f;
		}
	}
	if (mInputHandler->IsKeyDown(OIS::KC_L))
	{
		if(AI_2_v>-0.5){
			AI_2_v-=0.05f;
		}
	}
	
	if (mInputHandler->IsKeyDown(OIS::KC_K))
	{

			//U_u-=0.1f;
        //if (SPEED_COEFFICIENT <= 1.0 && SPEED_COEFFICIENT > 0) {
        //    SPEED_COEFFICIENT -= 0.1f;
        //}
        if (SPEED_COEFFICIENT_AI_2 > 1.0) {
            SPEED_COEFFICIENT_AI_2  -= 0.5f;
        }

	}


        //T,G,F,H Move cube 2
	if (mInputHandler->IsKeyDown(OIS::KC_F))
	{
		if(AI_0_v<0.5){
			AI_0_v+=0.1f;
		}
	}
	if (mInputHandler->IsKeyDown(OIS::KC_H))
	{
		if(AI_0_v>-0.5){
			AI_0_v-=0.1f;
		}
	}
	if (mInputHandler->IsKeyDown(OIS::KC_T))
	{
			//U_u+=0.1f;
        if (SPEED_COEFFICIENT_AI_0 < 20) {
            SPEED_COEFFICIENT_AI_0 += 0.5f;

        }
	}
	if (mInputHandler->IsKeyDown(OIS::KC_G))
	{

			//U_u-=0.1f;
        if (SPEED_COEFFICIENT_AI_0 > 1.0) {
            SPEED_COEFFICIENT_AI_0 -= 0.5f;
        }

	}

            //W,A,S,D Move cube 3
	else if (mInputHandler->IsKeyDown(OIS::KC_A))
	{
		if(AI_1_v<0.5){
			AI_1_v+=0.1f;
		}
	}
	else if (mInputHandler->IsKeyDown(OIS::KC_D))
	{
		if(AI_1_v>-0.5){
			AI_1_v-=0.1f;
		}
	}
		else if (mInputHandler->IsKeyDown(OIS::KC_W))
	{
			//U_u+=0.1f;
        if (SPEED_COEFFICIENT_AI_1 < 50) {
            SPEED_COEFFICIENT_AI_1 += 0.5f;

        }
	}
	else if (mInputHandler->IsKeyDown(OIS::KC_S))
	{

			//U_u-=0.1f;
        if (SPEED_COEFFICIENT_AI_1 > 1.0) {
            SPEED_COEFFICIENT_AI_1 -= 0.5f;
        }

	}
		if (mInputHandler->IsKeyDown(OIS::KC_I))
	{
			//U_u+=0.1f;
        if (SPEED_COEFFICIENT_AI_2  < 50) {
            SPEED_COEFFICIENT_AI_2  += 1.0f;
			
        }
		

	}

	if(mInputHandler->IsKeyDown(OIS::KC_8))
	{
		mClock->pause();
	}
	if(mInputHandler->IsKeyDown(OIS::KC_7))
	{
		cllision=1;
	}
	if(mInputHandler->IsKeyDown(OIS::KC_6))
	{
		cllision=0;
	}
	if(mInputHandler->IsKeyDown(OIS::KC_9))
	{
		mClock->start();
	}
	if(mInputHandler->IsKeyDown(OIS::KC_0))
	{
		mClock->reset();
	}
	int elp = mClock->report(); //Ogre::ControllerManager::getSingleton().getElapsedTime();

	if(elp%5==0){
		if (mInputHandler->IsKeyDown(OIS::KC_T)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[0][0]=1;
				}
				else
				{
					lab[0][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_H)){
			for(int i=0;i<4;i++)
			{
				if(i==1)
				{
					lab[0][1]=1;
				}
				else
				{
					lab[0][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_F)){
			for(int i=0;i<4;i++)
			{
				if(i==2)
				{
					lab[0][2]=1;
				}
				else
				{
					lab[0][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==3)
				{
					lab[0][3]=1;
				}
				else
				{
					lab[0][i]=0;
				}
			}
		}
		else {
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[0][0]=1;
				}
				else
				{
					lab[0][i]=0;
				}
			}
		}
	}
	if(elp%5==1){
		if (mInputHandler->IsKeyDown(OIS::KC_T)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[1][0]=1;
				}
				else
				{
					lab[1][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_H)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[1][1]=1;
				}
				else
				{
					lab[1][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_F)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[1][2]=1;
				}
				else
				{
					lab[1][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[1][3]=1;
				}
				else
				{
					lab[1][i]=0;
				}
			}
		}
		else {
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[1][0]=1;
				}
				else
				{
					lab[1][i]=0;
				}
			}
		}
	}
	if(elp%5==2){
		if (mInputHandler->IsKeyDown(OIS::KC_T)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[2][0]=1;
				}
				else
				{
					lab[2][i]=0;
				}
			}
		}
				if (mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[2][1]=1;
				}
				else
				{
					lab[2][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_F)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[2][2]=1;
				}
				else
				{
					lab[2][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[2][3]=1;
				}
				else
				{
					lab[2][i]=0;
				}
			}
		}
		else {
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[2][0]=1;
				}
				else
				{
					lab[2][i]=0;
				}
			}
		}
	}
	if(elp%5==3){
		if (mInputHandler->IsKeyDown(OIS::KC_T)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[3][0]=1;
				}
				else
				{
					lab[3][i]=0;
				}
			}
		}
	    else if (mInputHandler->IsKeyDown(OIS::KC_H)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[3][1]=1;
				}
				else
				{
					lab[3][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_F)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[3][2]=1;
				}
				else
				{
					lab[3][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[3][3]=1;
				}
				else
				{
					lab[3][i]=0;
				}
			}
		}
		else {
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[3][0]=1;
				}
				else
				{
					lab[3][i]=0;
				}
			}
		}
	}
	if(elp%5==4){
		if (mInputHandler->IsKeyDown(OIS::KC_T)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[4][0]=1;
				}
				else
				{
					lab[4][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_H)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[4][1]=1;
				}
				else
				{
					lab[4][i]=0;
				}
			}
		}
		else if (mInputHandler->IsKeyDown(OIS::KC_F)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[4][2]=1;
				}
				else
				{
					lab[4][i]=0;
				}
			}
		}
		else if(mInputHandler->IsKeyDown(OIS::KC_G)){
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[4][3]=1;
				}
				else
				{
					lab[4][i]=0;
				}
			}
		}
		else {
			for(int i=0;i<4;i++)
			{
				if(i==0)
				{
					lab[4][0]=1;
				}
				else
				{
					lab[4][i]=0;
				}
			}
		}
	}




}

World::~World() {
	delete mBorder_2;
	delete mBorder_3;
	delete mBorder_1;
	delete mBorder_4;

	delete mAIRedBrick;
	delete mUserRedBrick;
	delete mWorldManager;

}


void World::updateLevel() {
	mWorldManager->updateLevel();

	if(!flag()){
	}


}
bool World::flag() {
    return mLevel <-7;
}


void World::createOverlay() {
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mOverlay = om.getByName("Score");
	//mOverlay = om.getByName("Horizontal_Initial_Value");
	mOverlay->show();
}


void World::getEventFrom(PongManager *pongManager) {
	mWorldManager->getEvent(pongManager);
}

void World::initialStates() {
	mAIScore = 0;

	mLevel = 0;
	mUserScore = 0;
	mUser = mUserRedBrick;
	mAIPlayer = mAIRedBrick;
}

void World::recordMove() {
    string readID = "moves";
	string exd = ".txt";
	string readResult;          // string which will contain the result  
	ostringstream readconvert;
	//readconvert << k;      // insert the textual representation of 'Number' in the characters in the stream
	//readResult = readconvert.str(); // set 'Result' to the contents of the stream
	//readID.append(readResult);
	readID.append(exd);

    ofstream outtxt( readID, std::ios_base::app | std::ios_base::out );

    outtxt << "Hello World!ed";
    outtxt << endl; //End of Line

    outtxt.close();
}

void World::moveCubes() {

	Ogre::Vector3 position = SceneManager()->getSceneNode("U")->getPosition();
	float u = 2*asin(position.z/1000);
	Ogre::Vector3 N;
	float ftime = mClock->report(); //Ogre::ControllerManager::getSingleton().getElapsedTime();
    u=ftime;
	U_u=-ftime/SPEED_COEFFICIENT;// ORIGINALLY -ftime/10
    AI_0_u=-ftime/SPEED_COEFFICIENT_AI_0;// ORIGINALLY -ftime/10
    AI_1_u=-ftime/SPEED_COEFFICIENT_AI_1;// ORIGINALLY -ftime/10

	float u1 = u+U_u;
    float u2 = u+AI_0_u;
    float u3 = u+AI_1_u;
    //For Debug use:
    //"AItank0" will make User cube Designer

	mTank->SceneManager()->getSceneNode("U")->setPosition(1000*cos(u1)+1000*cos(u1/2)*cos(u1)*(U_v),1000*sin(u1)+1000*cos(u1/2)*sin(u1)*(U_v),1000*sin(u1/2)*(U_v));
	mTank->SceneManager()->getSceneNode("U")->translate(Normal(u1, U_v)*10);
	mTank->SceneManager()->getSceneNode("U")->setOrientation(Orientation(Binormal(u1, U_v),Normal(u1, U_v),Tangent(u1,U_v)));
    u+=5;
	mTank->SceneManager()->getSceneNode("AItank0")->setPosition(1000*cos(u2)+1000*cos(u2/2)*cos(u2)*(AI_0_v),1000*sin(u2)+1000*cos(u2/2)*sin(u2)*(AI_0_v),1000*sin(u2/2)*(AI_0_v));
	mTank->SceneManager()->getSceneNode("AItank0")->translate(Normal(u2, AI_0_v)*11);
	mTank->SceneManager()->getSceneNode("AItank0")->setOrientation(Orientation(Binormal(u2, AI_0_v),Normal(u2, AI_0_v),Tangent(u2,AI_0_v)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank1")->setPosition(1000*cos(u3)+1000*cos(u3/2)*cos(u3)*(AI_1_v),1000*sin(u3)+1000*cos(u3/2)*sin(u3)*(AI_1_v),1000*sin(u3/2)*(AI_1_v));
	mTank->SceneManager()->getSceneNode("AItank1")->translate(Normal(u3, AI_1_v)*11);
	mTank->SceneManager()->getSceneNode("AItank1")->setOrientation(Orientation(Binormal(u3, AI_1_v),Normal(u3, AI_1_v),Tangent(u3,AI_1_v)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank2")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/-100),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/-100),1000*sin(u/2)*(1/-100));
	mTank->SceneManager()->getSceneNode("AItank2")->translate(Normal(u, 1/-100)*11);
	mTank->SceneManager()->getSceneNode("AItank2")->setOrientation(Orientation(Binormal(u, 1/-100),Normal(u, 1/-100),Tangent(u,1/-100)));
	u+=5;

    ////////////////////////////////////////////////////////
    ////////////// START OF NOT "SMART" AI's////////////////
    ////////////////////////////////////////////////////////

	mTank->SceneManager()->getSceneNode("AItank4")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.6),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.6),1000*sin(u/2)*(-0.6));
	mTank->SceneManager()->getSceneNode("AItank4")->translate(Normal(u, -0.6)*11);
	mTank->SceneManager()->getSceneNode("AItank4")->setOrientation(Orientation(Binormal(u, -0.6),Normal(u, -0.6),Tangent(u,-0.6)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank5")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.8),1000*sin(u/2)*(-0.8));
	mTank->SceneManager()->getSceneNode("AItank5")->translate(Normal(u, -0.8)*11);
	mTank->SceneManager()->getSceneNode("AItank5")->setOrientation(Orientation(Binormal(u, 1/7),Normal(u, -0.8),Tangent(u,-0.8)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank6")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(0.8),1000*sin(u/2)*(0.8));
	mTank->SceneManager()->getSceneNode("AItank6")->translate(Normal(u, 0.8)*11);
	mTank->SceneManager()->getSceneNode("AItank6")->setOrientation(Orientation(Binormal(u, 0.8),Normal(u, 0.8),Tangent(u,0.8)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank7")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1));
	mTank->SceneManager()->getSceneNode("AItank7")->translate(Normal(u, 1)*11);
	mTank->SceneManager()->getSceneNode("AItank7")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank8")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(0),1000*sin(u)+1000*cos(u/2)*sin(u)*(0),1000*sin(u/2)*(0));
	mTank->SceneManager()->getSceneNode("AItank8")->translate(Normal(u, 0)*11);
	mTank->SceneManager()->getSceneNode("AItank8")->setOrientation(Orientation(Binormal(u, 0),Normal(u, 0),Tangent(u,0)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank9")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1),1000*sin(u/2)*(-1));
	mTank->SceneManager()->getSceneNode("AItank9")->translate(Normal(u, -1)*11);
	mTank->SceneManager()->getSceneNode("AItank9")->setOrientation(Orientation(Binormal(u, -1),Normal(u, -1),Tangent(u,-1)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank10")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1));
	mTank->SceneManager()->getSceneNode("AItank10")->translate(Normal(u, 1)*11);
	mTank->SceneManager()->getSceneNode("AItank10")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank11")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1));
	mTank->SceneManager()->getSceneNode("AItank11")->translate(Normal(u, 1)*11);
	mTank->SceneManager()->getSceneNode("AItank11")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank12")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/14),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/14),1000*sin(u/2)*(1/14));
	mTank->SceneManager()->getSceneNode("AItank12")->translate(Normal(u, 1/14)*11);
	mTank->SceneManager()->getSceneNode("AItank12")->setOrientation(Orientation(Binormal(u, 1/14),Normal(u, 1/14),Tangent(u,1/14)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank13")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/15),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/15),1000*sin(u/2)*(1/15));
	mTank->SceneManager()->getSceneNode("AItank13")->translate(Normal(u, 1/15)*11);
	mTank->SceneManager()->getSceneNode("AItank13")->setOrientation(Orientation(Binormal(u, 1/15),Normal(u, 1/15),Tangent(u,1/15)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank14")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/16),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/16),1000*sin(u/2)*(1/16));
	mTank->SceneManager()->getSceneNode("AItank14")->translate(Normal(u, 1/16)*11);
	mTank->SceneManager()->getSceneNode("AItank14")->setOrientation(Orientation(Binormal(u, 1/16),Normal(u, 1/16),Tangent(u,1/16)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank15")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/17),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/17),1000*sin(u/2)*(1/17));
	mTank->SceneManager()->getSceneNode("AItank15")->translate(Normal(u, 1/17)*11);
	mTank->SceneManager()->getSceneNode("AItank15")->setOrientation(Orientation(Binormal(u, 1/17),Normal(u, 1/17),Tangent(u,1/17)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank16")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/18),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/18),1000*sin(u/2)*(1/18));
	mTank->SceneManager()->getSceneNode("AItank16")->translate(Normal(u, 1/18)*11);
	mTank->SceneManager()->getSceneNode("AItank16")->setOrientation(Orientation(Binormal(u, 1/18),Normal(u, 1/18),Tangent(u,1/18)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank17")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/19),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/19),1000*sin(u/2)*(1/19));
	mTank->SceneManager()->getSceneNode("AItank17")->translate(Normal(u, 1/19)*11);
	mTank->SceneManager()->getSceneNode("AItank17")->setOrientation(Orientation(Binormal(u, 1/19),Normal(u, 1/19),Tangent(u,1/19)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank18")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/20),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/20),1000*sin(u/2)*(1/20));
	mTank->SceneManager()->getSceneNode("AItank18")->translate(Normal(u, 1/20)*11);
	mTank->SceneManager()->getSceneNode("AItank18")->setOrientation(Orientation(Binormal(u, 1/20),Normal(u, 1/20),Tangent(u,1/20)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank19")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/2),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/2),1000*sin(u/2)*(-1/2));
	mTank->SceneManager()->getSceneNode("AItank19")->translate(Normal(u, -1/2)*11);
	mTank->SceneManager()->getSceneNode("AItank19")->setOrientation(Orientation(Binormal(u, -1/2),Normal(u, -1/2),Tangent(u,-1/2)));
	u+=5;
	mTank->SceneManager()->getSceneNode("AItank20")->setPosition(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/3),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/3),1000*sin(u/2)*(-1/3));
	mTank->SceneManager()->getSceneNode("AItank20")->translate(Normal(u, -1/3)*11);
	mTank->SceneManager()->getSceneNode("AItank20")->setOrientation(Orientation(Binormal(u, -1/3),Normal(u, -1/3),Tangent(u,-1/3)));


	/*mObj1->yaw(Ogre::Radian(0.5f * mTime));
    mObj2->pitch(Ogre::Radian(0.6f * mTime));
    mObj3->roll(Ogre::Radian(0.5f * mTime));
	mObj4->translate(mTime * Ogre::Vector3(-1, 0,0));
	*/


}

void World::moveCubes2() {

	Ogre::Vector3 position = SceneManager()->getSceneNode("U")->getPosition();
	float u = 2*asin(position.z/1000);
	Ogre::Vector3 N;
	float ftime = mClock->report(); //Ogre::ControllerManager::getSingleton().getElapsedTime();
    u=ftime;
	U_u=-ftime/SPEED_COEFFICIENT;// ORIGINALLY -ftime/10
    AI_0_u=-ftime/SPEED_COEFFICIENT_AI_0;// ORIGINALLY -ftime/10
    AI_1_u=-ftime/SPEED_COEFFICIENT_AI_1;// ORIGINALLY -ftime/10

	float u1 = u+U_u;
    float u2 = u+AI_0_u;
    float u3 = u+AI_1_u;
    //For Debug use:
    //"AItank0" will make User cube Designer

    mTank->getMovingObject("U")->setPosition(Ogre::Vector3(1000*cos(u1)+1000*cos(u1/2)*cos(u1)*(U_v),1000*sin(u1)+1000*cos(u1/2)*sin(u1)*(U_v),1000*sin(u1/2)*(U_v)));
    mTank->getMovingObject("U")->translate(Normal(u1, U_v)*10);
    mTank->getMovingObject("U")->setOrientation(Orientation(Binormal(u1, U_v),Normal(u1, U_v),Tangent(u1,U_v)));
    u+=5;

    mTank->getMovingObject("AItank1")->setPosition(Ogre::Vector3(1000*cos(u2)+1000*cos(u2/2)*cos(u2)*(AI_0_v),1000*sin(u2)+1000*cos(u2/2)*sin(u2)*(AI_0_v),1000*sin(u2/2)*(AI_0_v)));
    mTank->getMovingObject("AItank1")->translate(Normal(u2, AI_0_v)*11);
    mTank->getMovingObject("AItank1")->setOrientation(Orientation(Binormal(u2, AI_0_v),Normal(u2, AI_0_v),Tangent(u2,AI_0_v)));
    u+=5;
   
    mTank->getMovingObject("AItank2")->setPosition(Ogre::Vector3(1000*cos(u3)+1000*cos(u3/2)*cos(u3)*(AI_1_v),1000*sin(u3)+1000*cos(u3/2)*sin(u3)*(AI_1_v),1000*sin(u3/2)*(AI_1_v)));
    mTank->getMovingObject("AItank2")->translate(Normal(u3, AI_1_v)*11);
    mTank->getMovingObject("AItank2")->setOrientation(Orientation(Binormal(u3, AI_1_v),Normal(u3, AI_1_v),Tangent(u3,AI_1_v)));
    u+=5;

    mTank->getMovingObject("AItank3")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/-100),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/-100),1000*sin(u/2)*(1/-100)));
    mTank->getMovingObject("AItank3")->translate((Normal(u, 1/-100)*11));
    mTank->getMovingObject("AItank3")->setOrientation(Orientation(Binormal(u, 1/-100),Normal(u, 1/-100),Tangent(u,1/-100)));
    u+=5;

    ////////////////////////////////////////////////////////
    ////////////// START OF NOT "SMART" AI's////////////////
    ////////////////////////////////////////////////////////

	mTank->getMovingObject("AItank4")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.6),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.6),1000*sin(u/2)*(-0.6)));
	mTank->getMovingObject("AItank4")->translate(Normal(u, -0.6)*11);
	mTank->getMovingObject("AItank4")->setOrientation(Orientation(Binormal(u, -0.6),Normal(u, -0.6),Tangent(u,-0.6)));
	u+=5;
	mTank->getMovingObject("AItank5")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(-0.8),1000*sin(u/2)*(-0.8)));
	mTank->getMovingObject("AItank5")->translate(Normal(u, -0.8)*11);
	mTank->getMovingObject("AItank5")->setOrientation(Orientation(Binormal(u, 1/7),Normal(u, -0.8),Tangent(u,-0.8)));
	u+=5;
	mTank->getMovingObject("AItank6")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(0.8),1000*sin(u)+1000*cos(u/2)*sin(u)*(0.8),1000*sin(u/2)*(0.8)));
	mTank->getMovingObject("AItank6")->translate(Normal(u, 0.8)*11);
	mTank->getMovingObject("AItank6")->setOrientation(Orientation(Binormal(u, 0.8),Normal(u, 0.8),Tangent(u,0.8)));
	u+=5;
	mTank->getMovingObject("AItank7")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank7")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank7")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank8")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(0),1000*sin(u)+1000*cos(u/2)*sin(u)*(0),1000*sin(u/2)*(0)));
	mTank->getMovingObject("AItank8")->translate(Normal(u, 0)*11);
	mTank->getMovingObject("AItank8")->setOrientation(Orientation(Binormal(u, 0),Normal(u, 0),Tangent(u,0)));
	u+=5;
	mTank->getMovingObject("AItank9")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1),1000*sin(u/2)*(-1)));
	mTank->getMovingObject("AItank9")->translate(Normal(u, -1)*11);
	mTank->getMovingObject("AItank9")->setOrientation(Orientation(Binormal(u, -1),Normal(u, -1),Tangent(u,-1)));
	u+=5;
	mTank->getMovingObject("AItank10")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank10")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank10")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank11")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1),1000*sin(u)+1000*cos(u/2)*sin(u)*(1),1000*sin(u/2)*(1)));
	mTank->getMovingObject("AItank11")->translate(Normal(u, 1)*11);
	mTank->getMovingObject("AItank11")->setOrientation(Orientation(Binormal(u, 1),Normal(u, 1),Tangent(u,1)));
	u+=5;
	mTank->getMovingObject("AItank12")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/14),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/14),1000*sin(u/2)*(1/14)));
	mTank->getMovingObject("AItank12")->translate(Normal(u, 1/14)*11);
	mTank->getMovingObject("AItank12")->setOrientation(Orientation(Binormal(u, 1/14),Normal(u, 1/14),Tangent(u,1/14)));
	u+=5;
	mTank->getMovingObject("AItank13")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/15),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/15),1000*sin(u/2)*(1/15)));
	mTank->getMovingObject("AItank13")->translate(Normal(u, 1/15)*11);
	mTank->getMovingObject("AItank13")->setOrientation(Orientation(Binormal(u, 1/15),Normal(u, 1/15),Tangent(u,1/15)));
	u+=5;
	mTank->getMovingObject("AItank14")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/16),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/16),1000*sin(u/2)*(1/16)));
	mTank->getMovingObject("AItank14")->translate(Normal(u, 1/16)*11);
	mTank->getMovingObject("AItank14")->setOrientation(Orientation(Binormal(u, 1/16),Normal(u, 1/16),Tangent(u,1/16)));
	u+=5;
	mTank->getMovingObject("AItank15")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/17),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/17),1000*sin(u/2)*(1/17)));
	mTank->getMovingObject("AItank15")->translate(Normal(u, 1/17)*11);
	mTank->getMovingObject("AItank15")->setOrientation(Orientation(Binormal(u, 1/17),Normal(u, 1/17),Tangent(u,1/17)));
	u+=5;
	mTank->getMovingObject("AItank16")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/18),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/18),1000*sin(u/2)*(1/18)));
	mTank->getMovingObject("AItank16")->translate(Normal(u, 1/18)*11);
	mTank->getMovingObject("AItank16")->setOrientation(Orientation(Binormal(u, 1/18),Normal(u, 1/18),Tangent(u,1/18)));
	u+=5;
	mTank->getMovingObject("AItank17")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/19),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/19),1000*sin(u/2)*(1/19)));
	mTank->getMovingObject("AItank17")->translate(Normal(u, 1/19)*11);
	mTank->getMovingObject("AItank17")->setOrientation(Orientation(Binormal(u, 1/19),Normal(u, 1/19),Tangent(u,1/19)));
	u+=5;
	mTank->getMovingObject("AItank18")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(1/20),1000*sin(u)+1000*cos(u/2)*sin(u)*(1/20),1000*sin(u/2)*(1/20)));
	mTank->getMovingObject("AItank18")->translate(Normal(u, 1/20)*11);
	mTank->getMovingObject("AItank18")->setOrientation(Orientation(Binormal(u, 1/20),Normal(u, 1/20),Tangent(u,1/20)));
	u+=5;
	mTank->getMovingObject("AItank19")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/2),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/2),1000*sin(u/2)*(-1/2)));
	mTank->getMovingObject("AItank19")->translate(Normal(u, -1/2)*11);
	mTank->getMovingObject("AItank19")->setOrientation(Orientation(Binormal(u, -1/2),Normal(u, -1/2),Tangent(u,-1/2)));
	u+=5;
	mTank->getMovingObject("AItank20")->setPosition(Ogre::Vector3(1000*cos(u)+1000*cos(u/2)*cos(u)*(-1/3),1000*sin(u)+1000*cos(u/2)*sin(u)*(-1/3),1000*sin(u/2)*(-1/3)));
	mTank->getMovingObject("AItank20")->translate(Normal(u, -1/3)*11);
	mTank->getMovingObject("AItank20")->setOrientation(Orientation(Binormal(u, -1/3),Normal(u, -1/3),Tangent(u,-1/3)));


	/*mObj1->yaw(Ogre::Radian(0.5f * mTime));
    mObj2->pitch(Ogre::Radian(0.6f * mTime));
    mObj3->roll(Ogre::Radian(0.5f * mTime));
	mObj4->translate(mTime * Ogre::Vector3(-1, 0,0));
	*/


}