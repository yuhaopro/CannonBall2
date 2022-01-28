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

LARGE_INTEGER startingtime, endingtime, timestep;
LARGE_INTEGER frequency;
bool** collisionmap;
ULONGLONG Wsleeptime = 2000, Wtimer = 0, Stimer = 0;
float deltatime;
string gamestate = "idle";
vector<Sprite*> collidables;
vector<Cannonball*> cannonballobjects;
vector <Player*> playersarray;
char str[256];
Player* player;
Player* player2;


//font objsS
LPD3DXFONT ARIAL32 = NULL;
LPD3DXFONT TIMESNEWROMAN20 = NULL;
string boop = "Welcome to Game Progamming";

LPDIRECT3DSURFACE9 gamemap = NULL;
LPDIRECT3DSURFACE9 spritesheet = NULL; //to load map tiles 



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
	player = new Player(SCREENW / 2, SCREENH / 2);
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

	player2 = new Player(SCREENW / 2, SCREENH / 2);
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
	//high resolution timer
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingtime);

	//update the input devices
	DirectInput_Update();

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
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		
		for (int i = 0; i < (int)playersarray.size(); i++)
		{
			if (!playersarray[i]->dead)
			{
				playersarray[i]->Draw(spriteobj);
				sprintf_s(str, sizeof(str), "playerdirection: %d\n", playersarray[i]->ReadDirectionLR());
				OutputDebugStringA(str);
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
	if (Key_Down(DIK_SPACE) || Key_Down(DIK_ESCAPE))
	{
		gameover = true;
	}
	QueryPerformanceCounter(&endingtime);
	timestep.QuadPart = endingtime.QuadPart - startingtime.QuadPart;
	deltatime = (float)timestep.QuadPart / (float)frequency.QuadPart;

}

void Game_End()
{
	if (ARIAL32) ARIAL32->Release();
	if (TIMESNEWROMAN20) TIMESNEWROMAN20->Release();
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
