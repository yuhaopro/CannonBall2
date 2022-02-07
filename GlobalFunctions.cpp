#include "Global.h"

using namespace std;

//map functions
void DrawTilesC(LPDIRECT3DSURFACE9 source, int tilenum, int columns, int width, int height, LPDIRECT3DSURFACE9 dest, int destx, int desty)
{
	RECT rect1;
	rect1.left = (tilenum % columns) * width;
	rect1.top = (tilenum / columns) * height;
	rect1.right = rect1.left + width;
	rect1.bottom = rect1.top + height;

	RECT rect2;
	rect2.left = destx;
	rect2.top = desty;
	rect2.right = destx + width;
	rect2.bottom = desty + height;

	//draw the tile
	d3ddev->StretchRect(source, &rect1, dest, &rect2, D3DTEXF_NONE);
}
void DrawTiles(LPDIRECT3DSURFACE9 source, int left, int top, int width, int height, LPDIRECT3DSURFACE9 dest, int destx, int desty)
{
	RECT rect1;
	rect1.left = left;
	rect1.top = top;
	rect1.right = rect1.left + width;
	rect1.bottom = rect1.top + height;

	RECT rect2;
	rect2.left = destx;
	rect2.top = desty;
	rect2.right = destx + width;
	rect2.bottom = desty + height;

	//draw the tile
	d3ddev->StretchRect(source, &rect1, dest, &rect2, D3DTEXF_NONE);
}

void SettingSpriteObject(LPD3DXSPRITE &spriteobj,float scaling, float x, float y, float width, float height, float rotation)
{
	D3DXMATRIX mat;

	//create color 
	//D3DXCOLOR white = D3DCOLOR_XRGB(255, 255, 255);

	//create scale vector
	D3DXVECTOR2 scale(scaling, scaling);

	//create translate vector
	D3DXVECTOR2 trans(x, y);

	//set center by dividing width and height by 2
	D3DXVECTOR2 center((float)(width * scaling / 2), (float)(height * scaling / 2));

	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);
	//tell sprite object to use transform
	spriteobj->SetTransform(&mat);
}

int NumericLinearInterpolation(int goal, int current, float deltatime)
{
	if (current < goal)
	{
		return current + (int)deltatime;
	}
	else if (current > goal)
	{
		return current - (int)deltatime;
	}

	return goal;
}

void GameInitialization()
{
	for (int i = 0; i < (int)playersarray.size(); i++)
	{
		playersarray[i]->ChangeVelX(0);
		playersarray[i]->ChangeVelY(0);
		if (i == 0)
		{
			playersarray[i]->SetX((float)(SCREENW / 2 - 15 * TILEWIDTH));
		}
		else
		{
			playersarray[i]->SetX((float)(SCREENW / 2 + 15 * TILEWIDTH));
		}
		playersarray[i]->SetY((float)SpawnY);
		playersarray[i]->dead = false;
	}

	for (int i = 0; i < (int)cannonballobjects.size(); i++)
	{
		free(cannonballobjects[i]); //free cannonball memory if it is out of map
		cannonballobjects[i] = 0; //reset pointer to 0
		cannonballobjects.erase(cannonballobjects.begin() + i);// erase the object pointer from the cannonballobjects array
	}
	gamestate = "idle";
	countdowntimer = 0;
	CountDownOver = false;
	OutputDebugStringA("Game Initialized\n");
}