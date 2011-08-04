#include "player.h"
#include "dj2d.h"
#include "djapp.h"
#include "constants.h"


Player::Player()
{
	//::Player(RED);
}

Player::Player(PLAYER_TYPE pt )
{
	type = pt;
	SetStopAnimation(DJFALSE);

	// Create a sprite we can later draw
	m_pSprite = new DJ2DSprite;
	// Make the sprite magnify with a blur instead of pixelated
	m_pSprite->SetFilter(DJ2DSprite::filterLinear);
	// Set the sprite to the center of the screen
	//m_pSprite->SetPosition((float)g_nScreenWidth * djRandomGetFloat(), (float)g_nScreenHeight * djRandomGetFloat());
	if (type == RED) { //RED is always from the middle of top
		m_pSprite->SetPosition((float)g_nScreenWidth/2,0);
	} else { // BLACK is always from the middle of bottom
		m_pSprite->SetPosition((float)g_nScreenWidth/2,	g_nScreenHeight);
	}

	// Set the sprite sorting value to the Y position of the sprite
	m_pSprite->SetSortValue(m_pSprite->GetPosition()[0]);

	// Randomize direction
	m_vDir = DJVector2(1,0); // Horizontal
	m_vDir = m_vDir.RotatedZ(djRandomGetFloat() * DJ_2PIf);

	// Choose the sprite animation that is mostly in the right direction
	if ((m_vDir % DJVector2(1,0) > 0.707f))
		m_nDir = 3;
	else if ((m_vDir % DJVector2(-1,0) > 0.707f))
		m_nDir = 2;
	else if ((m_vDir % DJVector2(0,1) > 0.707f))
		m_nDir = 1;
	else
		m_nDir = 0;

	// Randomize timer before changing direction
	m_fTimer = djRandomGetFloat() * 4.0f;

	// Play animation on the sprite
	m_pSprite->PlayAnimation(g_pAnimations[type][m_nDir]);
	// Add sprite to active node list in the sprite engine to automatically update the animations on it
	theSpriteEngine.AddActiveNode(m_pSprite);

	DJ2DLayer* pLayer = theSpriteEngine.GetLayer(LAYER_SPRITES);
	pLayer->AddNode(m_pSprite);
}

///

Player::~Player()
{
	// Remove sprite from the active list
	theSpriteEngine.RemoveActiveNode(m_pSprite);
	// Delete the sprite
	delete m_pSprite;
}

djbool between(float value, float min, float max) {
	return (min <= value && value <= max);
}

djbool Player::IsClicked(float x, float y )
{
	return between(x,m_pSprite->GetPosition()[0] - OFFSET_WIDTH, m_pSprite->GetPosition()[0] + OFFSET_WIDTH) && between(y,m_pSprite->GetPosition()[1] - OFFSET_HEIGHT,m_pSprite->GetPosition()[1] + OFFSET_HEIGHT) ;
}

///

void Player::OnUpdate()
{
	DJVector2 vPos = m_pSprite->GetPosition();

	// Move player in the direction we are walking
	vPos += m_vDir * pTheApp->GetDeltaAppTime() * 50.0f; // Move 100 pixel

	// Check that the player does not walk outside of the screen	
	if (vPos[0] < 0.0f)						{ vPos[0] = 0.0f; m_fTimer = 0.0f; }
	if (vPos[1] < 0.0f)						{ vPos[1] = 0.0f; m_fTimer = 0.0f; }
	if (vPos[0]  > (float)g_nScreenWidth)	{ vPos[0] = (float)g_nScreenWidth ; m_fTimer = 0.0f; }
	if (vPos[1]  > (float)g_nScreenHeight)	{ vPos[1] = (float)g_nScreenHeight ; m_fTimer = 0.0f; }

	//Check if player go into a cage
	if (InCage(cage1) || InCage(cage2)) {
		m_fTimer = 0.0f;
	}

	m_fTimer -= pTheApp->GetDeltaAppTime();

	// Time to change direction?
	if (m_fTimer <= 0.0f)
	{
		// Randomize direction
		m_vDir = DJVector2(1,0);
		m_vDir = m_vDir.RotatedZ(djRandomGetFloat() * DJ_2PIf);

		// Choose the sprite animation that is mostly in the right direction
		if ((m_vDir % DJVector2(1,0) > 0.707f))
			m_nDir = 3;
		else if ((m_vDir % DJVector2(-1,0) > 0.707f))
			m_nDir = 2;
		else if ((m_vDir % DJVector2(0,1) > 0.707f))
			m_nDir = 1;
		else
			m_nDir = 0;

		// Play animation on the sprite
		m_pSprite->PlayAnimation(g_pAnimations[type][m_nDir]);

		// Randomize timer before changing direction
		m_fTimer = djRandomGetFloat() * 4.0f;
	}

	// Set new position
	m_pSprite->SetPosition(vPos);

	// Set the sprite sorting value to the Y position of the sprite
	m_pSprite->SetSortValue(m_pSprite->GetPosition()[1]);
}

void Player::MoveTo(float x, float y)
{
	//m_vDir = (DJVector2(x,y) - m_pSprite->GetPosition()).Normalized();
	//OnUpdate();
	m_pSprite->SetPosition(x,y);
}

bool Player::InCage(Cage cage)
{
	return (between(m_pSprite->GetPosition()[0], cage.topleft_x, cage.topleft_x + cage.width) 
		&& between(m_pSprite->GetPosition()[1], cage.topleft_y, cage.topleft_y + cage.height));
}

/////////////////////////////////////////////////////////////////

