#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	startScreen(400,300,"startScreen.bmp",0),
	m_ball(50, 32, 16, 16, CColor::Black(), 0)
	// to initialise more sprites here use a comma-separated list
{
	
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	// do not run game logic when in menu mode
	if (IsMenuMode()) return;

	// TODO: add the game update code here
	

	if (m_state == AIRBORNE)
		player.Accelerate(0, -50);
	Uint32 t = GetTime();

	crystal.Update(t);
	
	// Run and Stand
	if (m_state == STANDING || m_state == RUNNING)
	{
		player.SetVelocity(0, 0);
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
		{
			player.Accelerate(-300, 0);
			if (m_state != RUNNING || m_side != LEFT)
				player.SetAnimation("run_left");
			m_state = RUNNING;
			m_side = LEFT;
			running.Resume();
		}
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
		{
			player.Accelerate(300, 0);
			if (m_state != RUNNING || m_side != RIGHT)
				player.SetAnimation("run_right");
			m_state = RUNNING;
			m_side = RIGHT;
			running.Resume();
		}
		else
		{
			if (m_state == RUNNING)
				player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			m_state = STANDING;
			
		}

	}
	m_ball.Update(GetTime());

	// Pre-Update Position
	CVector v0 = player.GetPos();

	// Updates
	for (CSprite* pSprite : m_sprites)
		pSprite->Update(t);
	player.Update(t);


	for (CSprite* pEnemy : m_sprites)
		if ((string)pEnemy->GetProperty("tag") == "enemy")
			for (CSprite* pCollider : m_sprites)
			{
				if ((string)pCollider->GetProperty("tag") == "left_enemy_collider" && pEnemy->HitTest(pCollider, 0))
				{
					pEnemy->SetVelocity(-pEnemy->GetVelocity());
					pEnemy->SetOmega(-pEnemy->GetOmega());
				}
				else
					if ((string)pCollider->GetProperty("tag") == "right_enemy_collider" && pEnemy->HitTest(pCollider, 0))
					{
						pEnemy->SetVelocity(-pEnemy->GetVelocity());
						pEnemy->SetOmega(-pEnemy->GetOmega());
					}
			}

	bool bTouchingPlatform = false;
	int h = player.GetHeight() / 2 - 1;
	for (CSprite* pSprite : m_sprites)
		if (player.HitTest(pSprite, 0))
		{
			// platforms and walls
			if ((string)pSprite->GetProperty("tag") == "platform")
			{
				if (v0.m_y >= pSprite->GetTop() + h)
				{
					player.SetVelocity(0, 0);
					player.SetY(pSprite->GetTop() + h);
					bTouchingPlatform = true;
				}
				else if (v0.m_x >= pSprite->GetRight() + h)
				{
					player.SetX(pSprite->GetRight() + h);
				}
				else if (v0.m_x >= pSprite->GetLeft() - h)
				{
					player.SetX(pSprite->GetLeft() - h);

				}
			}
		}
	for (CSprite* pSprite : m_sprites)
		if (player.HitTest(pSprite, 0))
		{
			if ((string)pSprite->GetProperty("tag") == "platformfloor")
			{
				health--;
				player.SetPos(40, 200);
			}

		}

	for (CSprite* pSprite : m_sprites)
		if (player.HitTest(pSprite, 0))
		{
			if ((string)pSprite->GetProperty("tag") == "hazard")
			{
				player.SetPos(55, 200);
				health--;
			}
		}

	for (CSprite* pSprite : m_sprites)
		if (player.HitTest(pSprite, 0))
		{
			if ((string)pSprite->GetProperty("tag") == "enemy")
			{
				player.SetPos(55, 200);
				health--;
			}
		}

	if (health == 0) {
		GameOver();
	}

	if (m_state == AIRBORNE && bTouchingPlatform)
	{
		// just landed
		m_state = STANDING;
		player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
	}
	if (m_state != AIRBORNE && !bTouchingPlatform)
	{
		// just taken off
		m_state = AIRBORNE;
		player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}
	

	if (crystalpoint == 0) {
		crystal.LoadImage("crystal.png", CColor::Black());
		crystal.SetImage("crystal.png");
		crystal.SetPosition(1535, 510);
	}
	if (player.HitTest(&crystal)) 
	{
		crystalpoint++;
		if (crystalpoint == 1) 
		{
			crystal.SetPosition(9999, 999);
		}
	}

	int s = player.GetHeight() / 2 - 1;

	for (CSprite* pSprite : m_sprites)
		if (m_ball.HitTest(pSprite, 0))
		{
			// platforms and walls
			if ((string)pSprite->GetProperty("tag") == "platform")
			{
				if (v0.m_y >= pSprite->GetTop() + s)
				{
					CVector v = m_ball.GetVelocity();
					v.m_y = -0.8 * v.m_y;
					m_ball.SetVelocity(v);
					m_ball.SetY(pSprite->GetTop() + s);
					bTouchingPlatform = true;
				}
				else if (v0.m_x >= pSprite->GetRight() + s)
				{
					m_ball.SetX(pSprite->GetRight() + s);
					CVector v = m_ball.GetVelocity();
					v.m_y = -0.8 * v.m_y;
					m_ball.SetVelocity(v);
				}
				else if (v0.m_x >= pSprite->GetLeft() - s)
				{
					m_ball.SetX(pSprite->GetLeft() - s);
					CVector v = m_ball.GetVelocity();
					v.m_y = -0.8 * v.m_y;
					m_ball.SetVelocity(v);

				}
			}
		}

	CVector v = m_ball.GetVelocity(); //Accelerated Motion (gravity)
	v.m_y -= 5;
	m_ball.SetVelocity(v);

	CVector g(0, 14); // Accelerated motion pt1
	m_ball.Accelerate(g);

	CVector f(0, -100); // Accelerated motion pt2
	m_ball.SetMass(5);
	m_ball.ApplyForce(f);


	float speed = m_ball.GetSpeed(); // set speed
	speed += 1;
	m_ball.SetSpeed(speed);

	enemy.Update(t);

	if(m_ball.HitTest(&enemy))
	{
		enemy.SetPos(-9000, 90000);
		m_ball.SetPos(-9000, 90000);

	}
}



void CMyGame::OnDraw(CGraphics* g)
{
	// draw menu when in menu mode
	if (IsMenuMode())
	{
		startScreen.Draw(g);

		return;
	}


	// ----- scrolling -------------------------------
    // game world (background image) is of size 2400x600
	int leftScreenLimit = 300;
	int rightScreenLimit = 1900; // 2400-800+300
	// do not scroll if left to the left limit
	if (player.GetX() < leftScreenLimit)
	{
		g->SetScrollPos(0, 0);  
	}
    // we scroll the whole game world according to the player position
	// unless we reached the left or right screen limits
	else if (player.GetX() >= leftScreenLimit && player.GetX() <= rightScreenLimit)
	{
		g->SetScrollPos(leftScreenLimit - player.GetX(), 0);
	}
	// we stop scrolling with the player once we have reached the right limit
	// and remain in that scrolling position at the right limit
	else if (player.GetX() > rightScreenLimit)
	{
		g->SetScrollPos(leftScreenLimit-rightScreenLimit, 0);
	}

	background.Draw(g);
	crystal.Draw(g);
	npc.Draw(g);
	player.Draw(g);
	gate.Draw(g);

	for (CSprite* pSprite : m_sprites)
		if ((string)pSprite->GetProperty("tag") != "right_enemy_collider" && (string)pSprite->GetProperty("tag") != "left_enemy_collider")
			pSprite->Draw(g);

	

	bool show = 0;

	if (player.HitTest(&npc))
	{
		show = 1;
		if (player.HitTest(&npc)) 
		{
			*g << font(12) << color(CColor::Red()) << xy(1250, 200) << "Make sure to collect the crystal before reaching the end.";
		}
	}
	
	if (show == 0) 
	{
		*g << font(12) << color(CColor::Red()) << xy(1250, 200) << "Come to me for vital information.";
	}

	 // don't scroll the overlay screen
	 g->SetScrollPos(0, 0);

	 *g << font(18) << color(CColor::Red()) << xy(10, 570) << "health: " << health;
	 *g << font(18) << color(CColor::Green()) << xy(10, 545) << "Crystals: " << crystalpoint;


	 if (IsGameOverMode() && health == 0)  
		 *g << font(40) << color(CColor::Green()) << xy(400, 300) << "Game Over";
	


	 if(crystalpoint == 1 && player.HitTest(&gate))
	 {
		 GameOver();
		 *g << font(40) << color(CColor::Green()) << xy(400, 300) << "Victory";
	 }

	
	m_ball.Draw(g);
	enemy.Draw(g);

}



/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{   
	// loading the game map
	background.LoadImage("background.bmp", "map1", CColor::Black());
	background.SetImage("map1"); background.SetPosition(1200, 300);

	gate.LoadImage("gate.png", CColor::Black());
	gate.SetImage("gate.png");
	gate.SetPosition(1860, 210);
	
	npc.LoadImage("npc.png", CColor::Black());
	npc.SetImage("npc.png");
	npc.SetPosition(1250, 180);
	
	// configuring the animations for the player sprite
	player.LoadImage("explorer.png", "stand_right", 6, 6, 0, 3, CColor::Black());
	player.LoadImage("explorer.png", "stand_left", 6, 6, 0, 2, CColor::Black());
	player.AddImage("explorer.png", "run_right", 6, 6, 0, 5, 5, 5, CColor::Black());
	player.AddImage("explorer.png", "run_left", 6, 6, 5, 4, 0, 4, CColor::Black());
	player.LoadImage("explorer.png", "jump_right", 6, 6, 0, 1, CColor::Black());
	player.LoadImage("explorer.png", "jump_left", 6, 6, 1, 0, CColor::Black());

	forest.Play("forest.wav", -1);
	running.Play("running.wav" - 1);

	enemy.LoadImage("enemy.bmp", CColor::Black());
	enemy.SetImage("enemy.bmp");
	enemy.SetPos(250, 170);
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{   

	
}




// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	player.SetPos(40, 580);
	player.SetImage("stand_right");
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{	
	for (CSprite* pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	CSprite* pSprite;

	crystalpoint = 0;
	health = 3;

	switch (nLevel)
	{
	case 1:
		player.SetPos(40, 200);
		player.SetImage("stand_right");

		pSprite = new CSpriteRect(950, -10, 1900, 19, CColor::Black(), CColor::Blue(), GetTime());
		pSprite->SetProperty("tag", "platformfloor");
		m_sprites.push_back(pSprite); //floor

		pSprite = new CSpriteRect(-9, 300, 15, 600, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // left wall

		pSprite = new CSpriteRect(150, 150, 300, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // top platform start

		pSprite = new CSpriteRect(480, 240, 175, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // top platform second

		pSprite = new CSprite(490, 260, "spikes.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite); // spike first

		pSprite = new CSpriteRect(625, 85, 150, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // leading towards npc platform

		pSprite = new CSpriteRect(490, 140, 100, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); //under platform npc

		pSprite = new CSpriteRect(795, 155, 85, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // leading towards next area after taking npc

		pSprite = new CSpriteRect(900, 350, 15, 100, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // pillar first

		pSprite = new CSpriteRect(1125, 155, 300, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);		

		pSprite = new CSprite(1115, 175, "spikes.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(900, 240, 150, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1400, 320, 600, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(1515, 340, "spikes.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1350, 490, 15, 100, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite); // pillar second

		pSprite = new CSpriteRect(1450, 400, 100, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1530, 480, 50, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1830, 160, 120, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(965, 175, 20, 20, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "left_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1285, 175, 20, 20, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "right_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1090, 340, 20, 20, CColor::Red(), GetTime());
		pSprite->SetProperty("tag", "left_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(1710, 340, 20, 20, CColor::Red(), GetTime());
		pSprite->SetProperty("tag", "right_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(540, 105, 20, 20, CColor::Red(), GetTime());
		pSprite->SetProperty("tag", "right_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(710, 105, 20, 20, CColor::Red(), GetTime());
		pSprite->SetProperty("tag", "left_enemy_collider");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(680, 100, "boulder.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(150, 0);
		pSprite->SetOmega(3.82 * 150);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(1125, 340, "boulder.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 150);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(1000, 170, "boulder.png", CColor::Black(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		break;
	}
	player.SetImage("stand_right");
	player.SetVelocity(0, 0);
	m_state = STANDING;
	m_side = RIGHT;
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
	
}

void CMyGame::victoryMode()
{
}
/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	if ((sym == SDLK_w || sym == SDLK_UP) && (m_state == STANDING || m_state == RUNNING))
	{
		player.Accelerate(0, 800);
		m_state = AIRBORNE;
		player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}
	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
		player.Accelerate(-300, 0);
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
		player.Accelerate(300, 0);

	
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	


	
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	// start game with a left mouse button click
	if (IsMenuMode()) StartGame();
	


}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
		// calculate ball's initial position
	CVector vec(0, 15);
	player.LtoG(vec);

	// calculate ball's initial angle

	// shoot!
	m_ball.SetPosition(vec);
	m_ball.SetDirection(75);
	m_ball.SetSpeed(300);
	
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
