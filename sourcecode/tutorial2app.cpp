/////////////////////////////////////////////////////////////////
//
// tutorial2app.cpp
// Application class
//
/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Daniel Jeppsson
// All Rights Reserved. These instructions, statements, computer
// programs, and/or related material (collectively, the "Source")
// contain unpublished information propietary to Daniel Jeppsson
// which is protected by US federal copyright law and by 
// international treaties. This Source may NOT be disclosed to 
// third parties, or be copied or duplicated, in whole or in 
// part, without the written consent of Daniel Jeppsson.
//
/////////////////////////////////////////////////////////////////
// 
// Author: Daniel Jeppsson
// 
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Engine Includes
#include <djmodulemanager.h>
#include <djostime.h>
#include <dj2d.h>
#include <dj2dutil.h>
#include <djgamesounds.h>
#include <djfont.h>

/////////////////////////////////////////////////////////////////
// Game Includes
#include "tutorial2app.h"
#include "constants.h"
#include "player.h"

/////////////////////////////////////////////////////////////////
// Name of the game module
const char *GetGameModuleName()
{
	return "Tutorial2";
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Name of the game application
const char *GetGameObjectName()
{
	return "Tutorial2Application";
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
IMPORT_STATIC_MODULE(Tutorial2)
IMPORT_STATIC_MODULE(OpenGLRender)
#if defined (_AIRPLAY)
IMPORT_STATIC_MODULE(AirSound)
#else
IMPORT_STATIC_MODULE(OpenALSound)
#endif
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void DJSystem_AddStaticModules()
{
	ADD_STATIC_MODULE(OpenGLRender);
#if defined (_AIRPLAY)
	ADD_STATIC_MODULE(AirSound);
#else
	ADD_STATIC_MODULE(OpenALSound);
#endif
	ADD_STATIC_MODULE(Tutorial2);
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_ENUMERATE_INTERFACE()
ENUMERATE_INTERFACE(Tutorial2Application)
END_ENUMERATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_CREATE_INTERFACE()
CREATE_INTERFACE(Tutorial2Application)
END_CREATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_REGISTER_SYMBOL(Tutorial2)
REGISTER_INTERFACE(Tutorial2Application)
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
REGISTER_ENUMERATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
END_REGISTER_SYMBOL()
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Linked list of players
DJLinkedList<Player> g_players;
Player* g_PlayerBeingClicked = NULL;

void CreateNewPlayer();

/////////////////////////////////////////////////////////////////
// Application class
DJTutorial2Application::DJTutorial2Application()
{
	DJTrace("%s()", __FUNCTION__);
}

///

DJTutorial2Application::~DJTutorial2Application()
{
	DJTrace("%s()", __FUNCTION__);
}

///

djresult DJTutorial2Application::OnInit()
{
	DJTrace("%s()", __FUNCTION__);

	// Set screen orientation depending on device
#if defined(_DJIPHONE)
	theSpriteEngine.SetScreenOrientation(1);
#else
	theSpriteEngine.SetScreenOrientation(0);
#endif

	// Get size of screen and put in global variables
	pTheRenderDevice->GetScreenSize(g_nScreenWidth, g_nScreenHeight);

	// Randomize random seed by using system time
	djRandomSetSeed( (djuint32)(djGetSystemTimeFloat() * 65536.0f) );

	// Initialize sprite engine with the layers we want
	if (!theSpriteEngine.Init(LAYER_COUNT))
	{
		DJError("Failed to initialize the sprite engine!");
		return DJ_FAILURE;
	}

	// Load font
	g_pFont = (DJFont*)theResourceManager.GetResource( "font", DJResource::TYPE_FONT );
	if (g_pFont == NULL)
	{
		DJError("Failed to load the font!");
		return DJ_FAILURE;
	}

	// Create a flare sprite sheet
	g_pFlareSheet = new DJ2DGraphicsSheet;
	if (!g_pFlareSheet->Init("sprites/flare.tga"))
	{
		DJError("Failed to load flare sheet!");
		return DJ_FAILURE;
	}
	DJVector2 vHotspot = DJVector2(g_pFlareSheet->GetTexture()->GetWidth()/2, g_pFlareSheet->GetTexture()->GetHeight()/2);
	g_pFlareSheet->AutoGenFrames(0, 0, -1, -1, 1, 1, &vHotspot);
	theSpriteEngine.AddGraphicsSheet(g_pFlareSheet);

	g_pCharacterSheet = (DJ2DFrameGroupList*)theResourceManager.GetResource("sprites/characters.sheet", DJResource::TYPE_FRAMEGROUPLIST);
	if (g_pCharacterSheet == NULL)
	{
		DJError("Failed to load character sheet!");
		return DJ_FAILURE;
	}

	// Create background image sheet
	g_pBackgroundSheet = new DJ2DGraphicsSheet;
	if (!g_pBackgroundSheet->Init("backgrounds/background.tga"))
	{
		DJError("Failed to load background sheet!");
		return DJ_FAILURE;
	}
	g_pBackgroundSheet->AutoGenFrames(0, 0, -1, -1, 1, 1, NULL);
	theSpriteEngine.AddGraphicsSheet(g_pBackgroundSheet);

	// Set the background layer to draw the background image
	DJ2DLayer *pLayer = theSpriteEngine.GetLayer(LAYER_BACKGROUND);
	DJ2DGraphicsFrame *pFrame = g_pBackgroundSheet->GetFrame(0);
	pLayer->SetBackground(pFrame);
	pLayer->SetPosition(0,0);
	pLayer->SetScale(1,1);

	// Load sprite animations (run up, down, left and right)
	g_pAnimations[RED][0] = (DJ2DAnimation*)theResourceManager.GetResource("ranger_n", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[RED][1] = (DJ2DAnimation*)theResourceManager.GetResource("ranger_s", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[RED][2] = (DJ2DAnimation*)theResourceManager.GetResource("ranger_w", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[RED][3] = (DJ2DAnimation*)theResourceManager.GetResource("ranger_e", DJResource::TYPE_ANIMATION2D);

	g_pAnimations[BLACK][0] = (DJ2DAnimation*)theResourceManager.GetResource("lizardman_n", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[BLACK][1] = (DJ2DAnimation*)theResourceManager.GetResource("lizardman_s", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[BLACK][2] = (DJ2DAnimation*)theResourceManager.GetResource("lizardman_w", DJResource::TYPE_ANIMATION2D);
	g_pAnimations[BLACK][3] = (DJ2DAnimation*)theResourceManager.GetResource("lizardman_e", DJResource::TYPE_ANIMATION2D);

	// Set layer to sort sprites
	pLayer = theSpriteEngine.GetLayer(LAYER_SPRITES);
	//pLayer->AddLayerFlags(DJ2DLayer::flagLayerSorted);

	// Create 100 players
	for (int q=0; q<2; q++)
	{
		CreateNewPlayer();
	}

	// Set position of Cages
	cage1.topleft_x = 0;
	cage1.topleft_y = (float) g_nScreenHeight *3 / 8;
	cage1.width = (float) g_nScreenHeight / 4;
	cage1.height = (float) g_nScreenHeight / 4;

	cage2.topleft_x = g_nScreenWidth - (float) g_nScreenHeight / 4;
	cage2.topleft_y = (float) g_nScreenHeight *3 / 8;
	cage2.width = (float) g_nScreenHeight / 4;
	cage2.height = (float) g_nScreenHeight / 4;

	// Time 
	m_fTimer = TIME_CREATE_PLAYER;

	// Load sound effects using the DJEngine gamesound helper functions (internally uses DJSoundDevice)
	if (!djGameSoundsInit(l_szSoundFiles, SOUND_COUNT))
	{
		DJError("Failed to load sound effects!");
		return DJ_FAILURE;
	}

	return DJ_SUCCESS;
}

///

void DJTutorial2Application::OnTerm()
{
	DJTrace("%s()", __FUNCTION__);

	// Delete the players
	g_players.Clear(DJTRUE);

	g_pFlareSheet = NULL;		// No need to delete this since the sprite engine takes care of it (we added it above)
	g_pBackgroundSheet = NULL;	// No need to delete this since the sprite engine takes care of it (we added it above)

	// Terminate sprite engine
	theSpriteEngine.Term();

	// Terminate DJEngine gamesound helper function
	djGameSoundsTerm();

}

///

void DJTutorial2Application::OnUpdate()
{
	//DJTrace(__FUNCTION__"()");

	// Update player
	DJLinkedListIter<Player> iter(g_players);
	Player * pPlayer;
	while ( (pPlayer = iter.GetStep()) ) //Get current position and advance
	{
		pPlayer->OnUpdate();
	}

	// Update sprite engine
	theSpriteEngine.OnUpdate(GetDeltaAppTime());

	//
	m_fTimer -= GetDeltaAppTime();
	if (m_fTimer < 0) {
		m_fTimer = TIME_CREATE_PLAYER;
		//Create new player
		CreateNewPlayer();
	}
}

///

void DJTutorial2Application::OnPaint()
{
	//DJTrace(__FUNCTION__"()");

	// Set the clear color
	pTheRenderDevice->SetClearColor(DJColor(1,0,0,0));
	// Clear the screen
	pTheRenderDevice->Clear(DJRenderDevice::flagClearAll);
	// Disable the depth buffer (no need for it in 2D games usually)
	pTheRenderDevice->EnableZBuffer(0);

	// Set render context
	DJ2DRenderContext rc;
	rc.m_dwFlags = 0;
	rc.m_cModColor = DJColor(1,1,1,1);
	rc.m_cAddColor = DJColor(0,0,0,0);
	rc.m_mLayerTransform = DJMatrix2D::Identity();
	rc.m_pLayer = NULL;
	rc.m_mTransform = DJMatrix2D::Identity();

	// Render sprites and all layers
	theSpriteEngine.OnPaint(rc);

	// Setup screen for rendering text
	pTheRenderDevice->SetViewTransform(DJMatrix::Identity());
	pTheRenderDevice->SetPerspectiveOrtho(0,0,g_nScreenWidth,g_nScreenHeight,0.0f,100.0f);
	char buffer[100];
	djStringFormat(buffer, 100,  "Scores : %d", g_scores);
	g_pFont->DrawString(buffer , DJVector3(10,10,0), DJVector2(16,16), 0xFFFFFFFF);
}

void CreateNewPlayer() 
{
	int random = djRoundToInt(djRandomGetFloat());
	Player* pPlayer;
	if (random == RED) {
		pPlayer = new Player(RED);
	} else {
		pPlayer = new Player(BLACK);
	}
	g_players.AddLast(pPlayer);
}

void SetStopAnimation(DJLinkedList<Player> &_g_players, djbool b, float x=-1, float y=-1) {
	DJLinkedListIter<Player> iter(_g_players);
	Player *player;

	while (player = iter.GetStep()) {
		if (player->IsClicked(x,y)) {
			player->SetStopAnimation(b);
		}
	}
}


void mark_player_being_clicked( float x, float y ) 
{
	DJLinkedListIter<Player> iter(g_players);
	Player *player;

	while (player = iter.GetStep()) {
		if (player->IsClicked(x,y)) {
			g_PlayerBeingClicked = player;
			player->SetStopAnimation(DJTRUE);
			break;
		}
	}
}

djint32 DJTutorial2Application::OnTouchBegin(djint32 nID, float x, float y)
{
	DJInfo("Touch Begin: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchBegin(nID, x,y))
		return 1;

	if (nID ==0) {
		// Mark player being clicked
		mark_player_being_clicked(x,y);
	} else {
		//Play normally
		SetStopAnimation(g_players,DJFALSE);

	}

	return 0;
}

///

djint32 DJTutorial2Application::OnTouchMove(djint32 nID, float x, float y)
{
	DJInfo("Touch Move: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchMove(nID, x,y))
		return 1;

	if (g_PlayerBeingClicked == NULL) 
		return 0;

	// Move player
	g_PlayerBeingClicked->MoveTo(x,y);


	return 0;
}

///

djint32 DJTutorial2Application::OnTouchEnd(djint32 nID, float x, float y)
{
	DJInfo("Touch End: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchEnd(nID, x,y))
		return 1;

	if (g_PlayerBeingClicked == NULL) 
		return 0;

	if (nID ==0) {
		//Check if player being clicked is in cages
		if ((g_PlayerBeingClicked->GetType() == RED && g_PlayerBeingClicked->InCage(cage1)) || 
			(g_PlayerBeingClicked->GetType() == BLACK && g_PlayerBeingClicked->InCage(cage2))) {
			DJ2DLayer* pLayer = theSpriteEngine.GetLayer(LAYER_SPRITES);
			pLayer->RemoveNode(g_PlayerBeingClicked->m_pSprite);
			g_scores++;
		} else if (g_PlayerBeingClicked->InCage(cage1) || g_PlayerBeingClicked->InCage(cage2)) { // Wrong cage, move player to middle of screen
			g_PlayerBeingClicked->m_pSprite->SetPosition((float) g_nScreenWidth/2,(float) g_nScreenHeight/2);
		}

		g_PlayerBeingClicked = NULL;


	} else {

	}

	return 0;
}

///

djint32 DJTutorial2Application::OnAccelerate(float x, float y, float z)
{
	if (DJApplication::OnAccelerate(x,y,z))
		return 1;

	//DJInfo("%s %.2f %.2f %.2f", __FUNCTION__, x, y, z);
	return 0;
}

///

djint32 DJTutorial2Application::OnButtonDown(djint32 nKey)
{
	DJInfo("Button Down: %d", nKey);

	//nKey 1 2 3 4: Len Phai Xuong Trai

	if (DJApplication::OnButtonDown(nKey))
		return 1;

	return 0;
}

///

djint32 DJTutorial2Application::OnButtonUp(djint32 nKey)
{
	DJInfo("Button Up: %d", nKey);

	if (DJApplication::OnButtonUp(nKey))
		return 1;

	return 0;
}

///

void DJTutorial2Application::OnMessage(djuint32 nMessage, djuint32 nParam1, djuint32 nParam2)
{
	//DJTrace(__FUNCTION__"()");
}
// Application class
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
DJ_FILE_END();
/////////////////////////////////////////////////////////////////

