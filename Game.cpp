#include "Global.h"

using namespace std;
const string APPTITLE = "THE GAME";
const int SCREENW = 1024;
const int SCREENH = 768;
const int TILEWIDTH = 16; //in pixels
const int TILEHEIGHT = 16; //in pixels
const int WINDOWWIDTH = SCREENW;
const int WINDOWHEIGHT = SCREENH;
const int MAPWIDTH = SCREENW;
const int MAPHEIGHT = SCREENH;
const int MAPTILEWIDTH = MAPWIDTH / 16;
const int MAPTILEHEIGHT = MAPHEIGHT / 16;
const float gravity = 200;
const float friction = 50;

//game world dimensions

LARGE_INTEGER startingtime, endingtime, timestep, frequency;
bool** collisionmap;
float deltatime;
string gamestate;
vector<Sprite*> collidables;
vector<Cannonball*> cannonballobjects;
vector <Player*> playersarray;
char str[256];
Player* player;
Player* player2;
bool CountDownOver = false;
float countdowntimer;
string playerstring;


//font objs
LPD3DXFONT BITFONT80 = NULL;
LPD3DXFONT BITFONT48 = NULL;
LPD3DXFONT BITFONT20 = NULL;
vector<string> CountdownString;

LPDIRECT3DSURFACE9 gamemap = NULL;
LPDIRECT3DSURFACE9 spritesheet = NULL; //to load map tiles 
D3DCOLOR White = D3DCOLOR_XRGB(255,255,255);


bool Game_Init(HWND window)
{
	Direct3D_Init(window, SCREENW, SCREENH);

	DirectInput_Init(window);

	//get backbuffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//load map
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		MAPWIDTH, //width of surface
		MAPHEIGHT, //height of surface
		D3DFMT_X8R8G8B8, //surface format
		D3DPOOL_DEFAULT, //memory pool to use
		&gamemap, //pointer to the surface
		NULL);//reserved always NULL
	if (result != D3D_OK) 
	{ 
		OutputDebugStringA("surface failed");
		return false; 
	}
	if (!gamemap)
	{
		OutputDebugStringA("gamemap");  
		return false;
	}
	spritesheet = LoadSurface("files/dungeon.png");
	collisionmap = Terrain(spritesheet, gamemap);
	//Loading Player Texture and animation configurations
	player = new Player(SCREENW / 2 - 15 * TILEWIDTH, (float)SpawnY);
	player->LoadTexture(d3ddev, "files/dungeon.png", player->color);
	player->SetSourceCoords(368, 80);  
	player->delay = 100;
	player->endframe = 3;
	//intialising control keys
	player->LeftKey = DIK_A;
	player->RightKey = DIK_D;
	player->JumpKey = DIK_W;
	player->FireKey = DIK_S;
	collidables.push_back(player);
	playersarray.push_back(player);

	player2 = new Player(SCREENW / 2 + 15 * TILEWIDTH, (float)SpawnY);
	player2->LoadTexture(d3ddev, "files/dungeon.png", player2->color);
	player2->SetSourceCoords(368, 80);
	player2->delay = 100;
	player2->endframe = 3;
	//initialising control keys for player 2
	player2->LeftKey = DIK_LEFT;
	player2->RightKey = DIK_RIGHT;
	player2->JumpKey = DIK_UP;
	player2->FireKey = DIK_DOWN;
	collidables.push_back(player2);
	playersarray.push_back(player2);

	//Adding 8Bit Font
	AddFontResourceEx("files/8Bit.ttf",NULL,0);
	//Font name
	string FontName = "Press Start 2P";
	BITFONT80 = MakeFont(FontName, TILEHEIGHT * 5); //SIZE OF FONT IN SECOND PARAM
	if (BITFONT80 == NULL)
	{
		return false;
	}
	BITFONT48 = MakeFont(FontName, TILEHEIGHT * 3); //SIZE OF FONT IN SECOND PARAM
	if (BITFONT48 == NULL)
	{
		return false;
	}
	BITFONT20 = MakeFont(FontName, 20); //SIZE OF FONT IN SECOND PARAM
	if (BITFONT20 == NULL)
	{
		return false;
	}

	//variables initialisation
	gamestate = "idle";
	countdowntimer = 0;
	CountdownString = { "3","2","1" };
	CountDownOver = false;

	

	return true;
}


void Game_Run(HWND window)
{
	//make sure the Direct3D Device is valid
	if (!d3ddev)
	{
		OutputDebugStringA("Direct3D Device failed");
		return;
	}

	//update the input devices
	DirectInput_Update();

	//high resolution timer
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingtime);

	if (CountDownOver)
	{

		//cannonball movement
		for (int i = 0; i < (int)cannonballobjects.size(); i++)
		{
			cannonballobjects[i]->CombinedMovementFunction(deltatime, cannonballobjects[i]->deleteball);
		}


		//simulate player movement to check if x should be moved
		for (int i = 0; i < (int)playersarray.size(); i++)
		{
			playersarray[i]->CombinedMovementFunction(deltatime, playersarray[i]->dead);
		}
	}
	else
	{
		if (countdowntimer > 3)
		{
			CountDownOver = true;
		}
		else
		{
			countdowntimer += deltatime;
		}
	}

	//clear the backbuffer
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));
	//start rendering----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if (d3ddev->BeginScene() == D3D_OK)
	{
		for (int i = 0; i < (int)playersarray.size(); i++)
		{
			gamestate = playersarray[i]->ReadGameState();
			playersarray[i]->Animation(gamestate);
		}

		//start drawing
		d3ddev->StretchRect(gamemap, NULL, backbuffer, NULL, D3DTEXF_NONE);

		//Drawing Countdown Numbers
		fontspriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		double temp = floor((double)countdowntimer);
		if ((int)temp < 3)
		{
			vector<int> CountdownFont = FontCalculateVector(BITFONT80, SCREENW / 2, SCREENH / 2, CountdownString[(int)temp], White);
		}
		fontspriteobj->End();

		//Drawing Sprites
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		//Drawing Players
		for (int i = 0; i < (int)playersarray.size(); i++)
		{
			if (!playersarray[i]->dead)
			{
				playersarray[i]->Draw(spriteobj);
				sprintf_s(str, sizeof(str), "playerdirection: %d\n", playersarray[i]->ReadDirectionLR());
				OutputDebugStringA(str);
			}
			else
			{
				//TODO after one player die
				if (i == 0) //player 1 died
				{
					playerstring = "Player 1";
				}
				else //player 2 died
				{
					playerstring = "Player 2";
				}
				string VictoryMessage = playerstring + " Has Won!";
				string Restart = "Press Space to Restart\n";
				fontspriteobj->Begin(D3DXSPRITE_ALPHABLEND);
				vector<int> VictoryFont = FontCalculateVector(BITFONT48, SCREENW / 2, SCREENH / 2, VictoryMessage, White);
				FontCalculateVector(BITFONT20, SCREENW / 2, SCREENH / 2 + VictoryFont[1], Restart, White);
				fontspriteobj->End();
				CountDownOver = false;
				if (Key_Down(DIK_SPACE))
				{
					gamestart = false;
				}
			}
		}

		for (int i = 0; i < (int)cannonballobjects.size(); i++)
		{
			if (!cannonballobjects[i]->deleteball)
			{
				cannonballobjects[i]->Draw(spriteobj);
				float rotation = cannonballobjects[i]->ReadRotation();
				float scaling = cannonballobjects[i]->ReadScaling();
				sprintf_s(str, sizeof(str), "scaling:%f, rotation:%f, width:%d, height:%d, x:%f, y:%f, left:%d, top:%d frame:%d, direction: %d \n",
					scaling, rotation, cannonballobjects[i]->width, cannonballobjects[i]->height, cannonballobjects[i]->ReadX(), cannonballobjects[i]->ReadY(), cannonballobjects[i]->left, cannonballobjects[i]->top, cannonballobjects[i]->frame, cannonballobjects[i]->CannonballDirectionLR);
				OutputDebugStringA(str);
			}
			else
			{	
				free(cannonballobjects[i]); //free cannonball memory if it is out of map
				cannonballobjects[i] = 0; //reset pointer to 0
				cannonballobjects.erase(cannonballobjects.begin() + i);// erase the object pointer from the cannonballobjects array
			}
		}
		sprintf_s(str, sizeof(str), "Cannonballobjects.size: %d\n", (int)cannonballobjects.size());
		OutputDebugStringA(str);

		//stop drawing sprite
		if (spriteobj->End() != S_OK) OutputDebugStringA("spriteobj failed to end scene \n");

		//stop rendering
		if (d3ddev->EndScene() != D3D_OK) OutputDebugStringA("device failed to end scene \n");
		if (d3ddev->Present(NULL, NULL, NULL, NULL) != D3D_OK) OutputDebugStringA("device failed to present scene \n");
	}
	else
	{
		OutputDebugStringA("Failed to begin scene \n");
	}

	//escape key exits
	if (Key_Down(DIK_ESCAPE))
	{
		gameover = true;
		gamestart = false;
	}
	QueryPerformanceCounter(&endingtime);
	timestep.QuadPart = endingtime.QuadPart - startingtime.QuadPart;
	deltatime = (float)timestep.QuadPart / (float)frequency.QuadPart;

}

void Game_End()
{
	RemoveFontResourceEx("files/8Bit.ttf", NULL, 0);
	if (BITFONT80) BITFONT80->Release();
	if (BITFONT48) BITFONT48->Release();
	if (BITFONT20) BITFONT20->Release();
	spritesheet->Release();
	gamemap->Release();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
	//freeing space for collision map
	for (int i = 0; i < MAPTILEHEIGHT; i++)
	{
		delete[] collisionmap[i];
	}
	delete[] collisionmap;

}
