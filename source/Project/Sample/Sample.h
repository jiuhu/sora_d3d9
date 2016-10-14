//--------------------------------------------------------------------
//
//	Sample.h
//
//	Sora Development
//

#ifndef _SORA_MOTION_H
#define _SORA_MOTION_H

#include "System\D3D9\D9System.h"
#include "Project\common\Agent.h"
//--------------------------------------------------------------------

using namespace sora;
//--------------------------------------------------------------------

enum MODE
{
	BUMPER_CAR = 0,
	SHOOTING,
};
//--------------------------------------------------------------------

static const int TOTAL_BULLET = 4;
static const int TOTAL_OBSTACLE = 8;
class Player
{
public:
	Agent m_agent;
	ColoredSprite m_spriteGuard;
	KeyInputController m_input;
	MovableObject m_bullets[TOTAL_BULLET];
	int score;
	void init()
	{
		m_spriteGuard.setCenter(32.0f, 32.0f);
		m_agent.setRadius(48.0f);
		m_agent.setGfxSprite(m_spriteGuard);
		score = 0;
	}
	//--------------------------------------------------------------------

	void setBullet(GfxSprite &bullet)
	{
		for (int i = 0; i < TOTAL_BULLET; ++i)
			m_bullets[i].setGfxSprite(bullet);
	}
	//--------------------------------------------------------------------

	void draw()
	{
		for (int i = 0; i < TOTAL_BULLET; ++i)
		{
			if (m_bullets[i].isMoving())
				m_bullets[i].draw();
		}
		m_agent.draw();
	}
	//--------------------------------------------------------------------

	void update(const D3DXVECTOR2 &world, Object * obstacles);
	//--------------------------------------------------------------------
};
//--------------------------------------------------------------------

class World : public D9System
{
public:

	World();
	~World();
	
	/*
		This function is called to initialize the World.
	*/
	bool init();

	/*
		This function is called to draw the scene.
	*/
	void render();

	void update();

	void onKeyPressed(char key, bool isDown);

	void CALLBACK guiEvent(UINT eventID, int controlID, CDXUTControl * control, void * userContext);

private:

	enum
	{
		P1 = 0,
		P2,

		TOTAL_PLAYER
	};
	//--------------------------------------------------------------------

	D9Texture m_texture;
	D9Texture m_textureBullet;
	GfxSprite m_spriteBullet;
	Player m_players[TOTAL_PLAYER];
	//--------------------------------------------------------------------

	D9Texture m_texture2;
	GfxSprite m_spriteObstacle;
	Object m_obstacles[TOTAL_OBSTACLE];
	//--------------------------------------------------------------------
	
	D3DXVECTOR2 g_worldBoundary;
	bool g_accelerating;
	//--------------------------------------------------------------------

	void placeObstacle(int index);
	void changeMode();
	//--------------------------------------------------------------------
};
//--------------------------------------------------------------------

#endif