#ifndef PLAYER_H
#define PLAYER_H

#include "dj2d.h"
#include "djapp.h"
#include "constants.h"

const static float OFFSET_HEIGHT = 16; // From Hotspot to Edge
const static float OFFSET_WIDTH = 16;	// From Hotspot to Edge

enum PLAYER_TYPE {
	RED=0,
	BLACK=1,
};

class Player
{
protected:
	
	// Player direction vector
	DJVector2	m_vDir;
	// Current player direction
	int			m_nDir;
	// Timer before changing direction
	float		m_fTimer;

	// Stop animation
	djbool	m_stopAnimation;

	PLAYER_TYPE type;

public:
	// Player sprite
	DJ2DSprite* m_pSprite;
	// Constructor
	Player();

	Player(PLAYER_TYPE pt);
	// Destructor
	~Player();

	// Update player
	void OnUpdate();

	void SetStopAnimation(djbool b) {
		m_stopAnimation = b;
	}

	djbool IsStopAnimation() {
		return m_stopAnimation;
	}

	// Check if player is clicked
	djbool IsClicked(float x, float y);

	void MoveTo(float x, float y);

	PLAYER_TYPE GetType() const { return type; }
	void SetType(PLAYER_TYPE val) { type = val; }
	bool InCage(Cage cage );
};

#endif