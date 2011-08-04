#ifndef CONSTANTS_H
#define CONSTANTS_H

//extern
enum
{
	// Layer for background image
	LAYER_BACKGROUND	= 0,
	// Layer for our sprites
	LAYER_SPRITES		= 1,
	// Layer for overlay and UI
	LAYER_OVERLAY		= 2,

	// Number of layers
	LAYER_COUNT			= 3,
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Sound enum
//extern
enum
{
	// ZAP sound effect
	SOUND_ZAP = 0,
	// Tick sound effect
	SOUND_TICK,

	// Number of sound effects
	SOUND_COUNT
};

struct Cage {
	float topleft_x;
	float topleft_y;
	float width;
	float height;
};

extern djuint32				g_nScreenWidth	;
// Screen height (set in OnInit function)
extern djuint32				g_nScreenHeight	;
extern DJ2DAnimation*			g_pAnimations[2][4];

extern Cage cage1, cage2;
#endif