/////////////////////////////////////////////////////////////////
//
// tutorial2app.h
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


#ifndef _TUTORIAL2APP_H_
#define _TUTORIAL2APP_H_


#include <djapp.h>
#include <dj2d.h>
#include "constants.h"
#include "djuinode.h"

/////////////////////////////////////////////////////////////////
DJ_FILE_START();
//////

enum
{
	MENU_MAIN,
	MENU_SETTINGS,
	MENU_INGAME,
	MENU_LEVELSELECT,
	MENU_HUD,

	MENU_COUNT
};
enum
{
	GS_PRELOAD = 0,
	GS_PRELOGO,
	GS_LOGO_1,
	GS_LOAD_GAME,
	GS_MENU,
	GS_LOAD_LEVEL,
	GS_UNLOAD_LEVEL,
	GS_CREDITS,
	GS_INGAME,
	GS_INGAME_MENU,
	GS_MODELVIEW,
};


/////////////////////////////////////////////////////////////////
class DJTutorial2Application : public DJApplication, public DJUIEventListener
{
	IMPLEMENT_INTERFACE(Tutorial2Application, Application)

public:

	/////////////////////////////////////////////////////////////////
	// CONSTRUCTION/DESTRUCTION

	// Constructor
	DJTutorial2Application( );
	// Destructor
	virtual ~DJTutorial2Application( );

	// Application initialization
	virtual djresult OnInit( );

	// Application termination
	virtual void OnTerm( );

	/////////////////////////////////////////////////////////////////
	// OPERATIONS

	// Called once per frame from main loop
	virtual void OnUpdate( );

	// Called once per frame to render the application
	virtual void OnPaint( );

	/////////////////////////////////////////////////////////////////
	// EVENT HANDLERS

	// Touch begin callback
	virtual djint32 OnTouchBegin( djint32 nID, float x, float y );
	// Touch move callback
	virtual djint32 OnTouchMove( djint32 nID, float x, float y );
	// Touch end callback
	virtual djint32 OnTouchEnd( djint32 nID, float x, float y );
	// Accelerator event callback
	virtual djint32 OnAccelerate( float x, float y, float z );
	// Button Down
	virtual djint32 OnButtonDown(djint32 nKey);
	// Button Up
	virtual djint32 OnButtonUp(djint32 nKey);

	// Handle incoming system event
	virtual void OnMessage( djuint32 nMessage, djuint32 nParam1 = 0, djuint32 nParam2 = 0 );
	void GotoMenu( djuint32 MENU_LEVELSELECT );
	void PaintMenu();
	void PaintIngame();
	void UpdateIngame();
	void UpdateMenu();

	virtual djbool OnUIEvent( DJUINode *pNode, const DJUIEvent &ev );
	
};
/////////////////////////////////////////////////////////////////

// Globals
// Screen width (set in OnInit function)
djuint32				g_nScreenWidth	;
// Screen height (set in OnInit function)
djuint32				g_nScreenHeight			= 0;
djuint32					g_nHUDWidth			= 0;
djuint32					g_nHUDHeight		= 0;
// Sheet of graphics for the flare sprite
DJ2DGraphicsSheet*		g_pFlareSheet			= NULL;
DJUINode*	m_pMenus[MENU_COUNT];
djint32 g_GameState = GS_MENU;
DJViewport					g_UIViewport;
// Sheet of graphics for the background
DJ2DGraphicsSheet*		g_pBackgroundSheet		= NULL;
// Utility sheet class to simplify loading sprites
DJ2DFrameGroupList*		g_pCharacterSheet		= NULL;
// Animations
DJ2DAnimation*			g_pAnimations[2][4]		= {{NULL, NULL, NULL, NULL},{NULL, NULL, NULL, NULL}};
// A font to use for drawing text
DJFont*					g_pFont					= NULL;

djuint g_scores;
// Timer before creating new player
#define TIME_CREATE_PLAYER 2;
float	m_fTimer;

Cage cage1, cage2;

// List of sounds to load
static const char *l_szSoundFiles[SOUND_COUNT] =
{
	"sound/zap1.wav",
	"sound/tick.wav",
};

/////////////////////////////////////////////////////////////////
DJ_FILE_END();
/////////////////////////////////////////////////////////////////

#endif // _TUTORIAL2_H_
