//--------------------------------------------------------------------
//
//	Sample.cpp
//
//	Sora Development
//

#include "Sample.h"
#include "System\D3D9\D9Texture.h"
#include "Utilities\Helper.h"
#include "Utilities\Math.h"
#include "resource\resource.h"
//--------------------------------------------------------------------

const float PADDLE_FORCE = 100.0f;
const float BULLET_FORCE = 300.0f;
const float	ROTATE_STEP_CW = 0.016f;
const float	ROTATE_STEP_CCW = -ROTATE_STEP_CW;
const int COMBO = 0;
//--------------------------------------------------------------------

MODE m_mode = BUMPER_CAR;
//--------------------------------------------------------------------

World::World()
	:	g_accelerating(false)
{
}
//--------------------------------------------------------------------

World::~World()
{
}
//--------------------------------------------------------------------

bool World::init()
{
	sora::generateSeed();

	SoraRC rc;
	GetClientRect(getWindowHandle(), &rc);
	g_worldBoundary.x = (float)(rc.right - rc.left);
	g_worldBoundary.y = (float)(rc.bottom - rc.top);

	m_texture.loadTextureFromResource(IDB_BMP);
	m_textureBullet.loadTextureFromResource(IDB_BMP3);
	m_spriteBullet.setTexture(&m_textureBullet);
	m_spriteBullet.setCenter(4.0f, 4.0f);

	m_players[P1].m_spriteGuard.setTexture(&m_texture);
	m_players[P1].m_spriteGuard.setColor(128, 0, 32);
	m_players[P1].m_agent.setPosition(g_worldBoundary.x * 0.25f, g_worldBoundary.y * 0.25f);
	m_players[P1].init();
	m_players[P1].setBullet(m_spriteBullet);

	m_players[P2].m_spriteGuard.setTexture(&m_texture);
	m_players[P2].m_spriteGuard.setColor(0, 128, 32);
	m_players[P2].m_agent.setPosition(g_worldBoundary.x * 0.75f, g_worldBoundary.y * 0.75f);
	m_players[P2].init();
	m_players[P2].setBullet(m_spriteBullet);

	m_texture2.loadTextureFromResource(IDB_BMP2);

	m_spriteObstacle;
	m_spriteObstacle.setTexture(&m_texture2);
	m_spriteObstacle.setCenter(16.0f, 16.0f);
	for (int i = 0; i < TOTAL_OBSTACLE; i++)
	{
		m_obstacles[i].setGfxSprite(m_spriteObstacle);
		m_obstacles[i].m_position.x = 100.0f;
		m_obstacles[i].setRadius(24.0f);
		placeObstacle(i);
	}

	CDXUTComboBox * comboBox = addComboBox(COMBO, 32, 32);
	comboBox->SetDropHeight(32);
	comboBox->AddItem("Bumper Car", (void*)BUMPER_CAR);
	comboBox->AddItem("Shooting Star", (void*)SHOOTING);

	return true;
}
//--------------------------------------------------------------------

void World::placeObstacle(int index)
{
	bool notPlaced = true;
	while (notPlaced)
	{
		bool noCollide = true;
		m_obstacles[index].m_position.x = (float)randomNumber(32, 760);
		m_obstacles[index].m_position.y = (float)randomNumber(32, 560);

		if (isCollide(m_obstacles[index], m_players[P1].m_agent))
			noCollide = false;
		if (isCollide(m_obstacles[index], m_players[P2].m_agent))
			noCollide = false;
		for (int i = 0; i < TOTAL_OBSTACLE; i++)
		{
			if (i == index) continue;
			if (isCollide(m_obstacles[index], m_obstacles[i]))
			{
				noCollide = false;
				break;
			}
		}
		if (noCollide)
			notPlaced = false;
	}
}
//--------------------------------------------------------------------

void World::render()
{
	if (m_mode == SHOOTING)
	{
		drawFormattedText(32, 500, "P1 Score : %d", m_players[P1].score);
		drawFormattedText(600, 500, "P2 Score : %d", m_players[P2].score);
	}
	
	for (int i = 0; i < TOTAL_OBSTACLE; i++)
	{
		if (m_obstacles[i].isAlive())
			m_obstacles[i].draw();
		else
		{
			placeObstacle(i);
			m_obstacles[i].alive();
		}
	}
	
	m_players[P1].draw();
	m_players[P2].draw();
}
//--------------------------------------------------------------------

void World::update()
{
	for (int i = P1; i < TOTAL_PLAYER; i++)
	{
		m_players[i].update(g_worldBoundary, m_obstacles);
	}

	if (isCollide(m_players[P1].m_agent, m_players[P2].m_agent))
	{
		m_players[P1].m_agent.bounce(m_players[P2].m_agent.m_heading);
		m_players[P2].m_agent.bounce(m_players[P1].m_agent.m_heading);
	}
}
//--------------------------------------------------------------------

void World::onKeyPressed(char key, bool isDown)
{
	if (isDown)
	{
		switch (key)
		{
		case '1':
			m_mode = BUMPER_CAR;
			break;

		case '2':
			m_mode = SHOOTING;
			break;

		case VK_UP:
			m_players[P2].m_input.keyDown(UP_KEY);
			break;

		case VK_DOWN:
			m_players[P2].m_input.keyDown(DOWN_KEY);
			break;

		case VK_LEFT:
			m_players[P2].m_input.keyDown(LEFT_KEY);
			//m_agent[P1].rotate(ROTATE_STEP_CCW);
			break;

		case VK_RIGHT:
			m_players[P2].m_input.keyDown(RIGHT_KEY);
			//m_agent[P1].rotate(ROTATE_STEP_CW);
			break;

		case VK_SPACE:
			m_players[P2].m_input.keyDown(A_KEY);
			break;

		case 'W':
			m_players[P1].m_input.keyDown(UP_KEY);
			break;

		case 'S':
			m_players[P1].m_input.keyDown(DOWN_KEY);
			break;

		case 'A':
			m_players[P1].m_input.keyDown(LEFT_KEY);
			//m_players[P1].m_agent.rotate(ROTATE_STEP_CCW);
			break;

		case 'D':
			m_players[P1].m_input.keyDown(RIGHT_KEY);
			//m_agent[P2].rotate(ROTATE_STEP_CW);
			break;

		case VK_RETURN:
			m_players[P1].m_input.keyDown(A_KEY);
			break;
		}
	}
	else
	{
		switch (key)
		{
		case VK_UP:
			m_players[P2].m_input.keyUp(UP_KEY);
			m_players[P2].m_agent.setStatus(DECELERATING);
			break;

		case VK_DOWN:
			m_players[P2].m_input.keyUp(DOWN_KEY);
			break;

		case VK_LEFT:
			m_players[P2].m_input.keyUp(LEFT_KEY);
			break;

		case VK_RIGHT:
			m_players[P2].m_input.keyUp(RIGHT_KEY);
			break;

		case VK_SPACE:
			m_players[P2].m_input.keyUp(A_KEY);
			break;

		case 'W':
			m_players[P1].m_input.keyUp(UP_KEY);
			m_players[P1].m_agent.setStatus(DECELERATING);
			break;

		case 'S':
			m_players[P1].m_input.keyUp(DOWN_KEY);
			break;

		case 'A':
			m_players[P1].m_input.keyUp(LEFT_KEY);
			break;

		case 'D':
			m_players[P1].m_input.keyUp(RIGHT_KEY);
			break;

		case VK_RETURN:
			m_players[P1].m_input.keyUp(A_KEY);
			break;
		}
	}
}
//--------------------------------------------------------------------

void CALLBACK World::guiEvent(UINT eventID, int controlID, CDXUTControl * control, void * userContext)
{
	switch (controlID)
	{
	case COMBO:
		DXUTComboBoxItem* item = ((CDXUTComboBox*)control)->GetSelectedItem();
		m_mode = (MODE)((int)item->pData);
		changeMode();
		break;
	}
}
//--------------------------------------------------------------------

void World::changeMode()
{
	if (m_mode == BUMPER_CAR)
	{
		for (int i = 0; i < TOTAL_PLAYER; i++)
		{
			for (int j = 0; j < TOTAL_BULLET; j++)
				m_players[i].m_bullets[j].setStatus(IDLE);
		}
	}
}
//--------------------------------------------------------------------

void Player::update(const D3DXVECTOR2 &world, Object * obstacles)
{
	if (m_input.isKeyHold(UP_KEY))
	{
		m_agent.accelerate(PADDLE_FORCE);
	}
	else if (m_input.isKeyHold(DOWN_KEY))
	{
		m_agent.breaking();
	}
	if (m_input.isKeyHold(LEFT_KEY))
	{
		m_agent.rotate(ROTATE_STEP_CCW);
	}
	else if (m_input.isKeyHold(RIGHT_KEY))
	{
		m_agent.rotate(ROTATE_STEP_CW);
	}

	if (m_agent.isMoving())
	{
		m_agent.moving(0.016f);
		m_agent.worldCollision(world);

		for (int j = 0; j < TOTAL_OBSTACLE; ++j)
		{
			if (obstacles[j].isAlive())
			{
				if (isCollide(m_agent, obstacles[j]))
				{
					if (m_mode == SHOOTING)
					{
						obstacles[j].dead();
						score++;
					}
					else
					{
						m_agent.m_velocity *= -0.9f;
					}
				}
			}
		}
	}

	if (m_mode == SHOOTING)
	{
		if (m_input.isKeyPressed(A_KEY))
		{
			for (int i = 0; i < TOTAL_BULLET; i++)
			{
				if (m_bullets[i].isIdle())
				{
					m_bullets[i].setStatus(ACCELERATING);
					m_bullets[i].m_position = m_agent.m_position;
					m_bullets[i].m_velocity.x = m_agent.m_heading.x * BULLET_FORCE;
					m_bullets[i].m_velocity.y = m_agent.m_heading.y * BULLET_FORCE;
					break;
				}
			}
		}

		for (int i = 0; i < TOTAL_BULLET; i++)
		{
			if (m_bullets[i].isMoving())
			{
				m_bullets[i].integrate(0.016f);

				if (isWorldCollide(m_bullets[i], world))
					m_bullets[i].setStatus(IDLE);
			}

			for (int j = 0; j < TOTAL_OBSTACLE; ++j)
			{
				if (obstacles[j].isAlive())
				{
					if (isCollide(m_bullets[i], obstacles[j]))
					{
						m_bullets[i].setStatus(IDLE);
						obstacles[j].dead();
						score++;
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------