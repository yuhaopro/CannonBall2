#include "Global.h"

int error = 404;
using namespace std;
bool Sprite::LoadTexture(LPDIRECT3DDEVICE9 &d3ddev, string filename, D3DCOLOR color)
{
	//get width and height of image
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return false;

	//create texture by loading bitmap image file
	D3DXCreateTextureFromFileEx(
		d3ddev, //direct3d device
		filename.c_str(), //filename
		info.Width, //width of img
		info.Height, //height of img
		1, //mip map level
		D3DPOOL_DEFAULT, //type of surface
		D3DFMT_UNKNOWN, //surface format
		D3DPOOL_DEFAULT, //memory class for texture
		D3DX_DEFAULT, //image filter
		D3DX_DEFAULT, //mip filter
		color, //color key for transparency
		&info, //bitmap file info
		NULL, //color palatte
		&texture); //destination texture

	//make sure the bitmap texture was loaded correctly
	if (result != D3D_OK) return false;

	return true;
}

void Sprite::MoveX(float deltatime)
{
	x += velx * deltatime;
}

void Sprite::MoveY(float deltatime)
{
	y += deltatime * (vely  + (deltatime * (gravity) / 2));
	vely += gravity * deltatime;
}

float Sprite::ReadX()
{
	return x;
}

float Sprite::ReadY()
{
	return y;
}

void Sprite::SetX(float value)
{
	if (value > 0 && value < SCREENW)
	{
		x = value;
	}
}

void Sprite::SetY(float value)
{
	if (value > 0 && value < SCREENH)
	{
		y = value;
	}
}

void Sprite::ChangeVelX(float value)
{
	velx = value;
}

void Sprite::VectorSumVelX(float value, string typeofvelx)
{
	if (typeofvelx == "playerinput") //if receiving input from player
	{
		if (velx < playerXspeed && velx > -playerXspeed)
		{
			velx = value;
		}
		

		
	}
	else if (typeofvelx == "collisioninput") //if receiving input from other objects
	{
		velx = value;
	}

}

void Sprite::ChangeVelY(float value)
{
	vely = value;
}

void Sprite::VectorSumVelY(float value)
{
	vely += value;
}

float Sprite::ReadVelX()
{
	return velx;
}

float Sprite::ReadVelY()
{
	return vely;
}

void Sprite::SetSourceCoords(int tleft, int ttop)
{
	left = tleft;
	top = ttop;
}

void Sprite::SetRotation(float value)

{
	rotation = value;
}

void Sprite::SetScaling(float value)

{
	scaling = value;
}

float Sprite::ReadRotation()
{
	return rotation;
}

float Sprite::ReadScaling()
{
	return scaling;
}

void Sprite::MapLimitX(bool& condition)
{
	//character x boundaries
	if (x < 0)
	{
		x = 0;
		condition = true;
	}
	else if (x > (float)(MAPWIDTH - width))
	{
		x = (float)(MAPWIDTH - width);
		condition = true;
	}
}

void Sprite::MapLimitY(bool& condition)
{
	//character y boundaries
	if (y < 0)
	{
		y = 0;
		condition = true;
	}
	else if (y > (float)(MAPHEIGHT - height))
	{
		y = (float)(MAPHEIGHT - height);
		condition = true;
	}
}
bool Sprite::CollisionTerrain(bool** collisionmap)
{
	//rect1 represents the previous player hitbox 
	//find if current tile is collidable
	RECT dest;
	int tilecol = (int)(x / TILEWIDTH); //dependent on sprite position
	int tilerow = (int)(y / TILEHEIGHT); //depending on sprite position
	RECT rect1 = { (long)x, (long)y, rect1.left + TILEWIDTH, rect1.top + TILEHEIGHT }; //rectangle box for current player 
	//checking which quadrants are intersected by object and terrain
	for (int row = tilerow; row < tilerow + 2; row++) //looking for collidable tiles within +1/-1 of character y axis
	{
		for (int col = tilecol; col < tilecol + 2; col++)//looking for collidable tiles within +1/-1 of character x axis
		{
			if (col >= 0 && col < MAPTILEWIDTH && row >= 0 && row < MAPTILEHEIGHT) //prevent access violation from collisionmap
			{
				if (collisionmap[row][col])//if there are collidables within 
				{
					RECT rect2 = { col * TILEWIDTH, row * TILEHEIGHT,rect2.left + TILEWIDTH, rect2.top + TILEHEIGHT }; //rectangle box of the collidable tile
					if (IntersectRect(&dest, &rect1, &rect2))
					{
						return true;
					}
				}

			}
		}
	}
	return false;
}

bool Sprite::CollisionObject(vector<Sprite*> &object)
{
	float x = ReadX();
	float y = ReadY();
	RECT rect1 = { (LONG)x, (LONG)y, (LONG)(x + width), (LONG)(y + height) };
	RECT dest;
	for (int i = 0; i < (int)object.size(); i++)
	{	
		if (object[i] != this) //if collidable pointer not equals to object pointer
		{
			float xO = object[i]->ReadX();
			float yO = object[i]->ReadY();
			RECT rect2 = { (LONG)xO, (LONG)yO, (LONG)(xO + object[i]->width), (LONG)(yO + object[i]->height) };
			if (IntersectRect(&dest, &rect1, &rect2))
			{
				return true;
			}
		}
	}
	return false;
}

vector<int> Sprite::CollisionObjectSprite(vector<Player*>& object)
{
	vector<int> playernumbers;
	float x = ReadX();
	float y = ReadY();
	RECT rect1 = { (LONG)x, (LONG)y, (LONG)(x + width), (LONG)(y + height) };
	RECT dest;
	for (int i = 0; i < (int)object.size(); i++)
	{
		if (object[i] != this) //if collidable pointer not equals to object pointer
		{
			float xO = object[i]->ReadX();
			float yO = object[i]->ReadY();
			RECT rect2 = { (LONG)xO, (LONG)yO, (LONG)(xO + object[i]->width), (LONG)(yO + object[i]->height) };
			if (IntersectRect(&dest, &rect1, &rect2))
			{
				playernumbers.push_back(i);
			}
		}
	}
	return playernumbers;
}

void Sprite::Animation(string  &gamestate)
{
	if (gamestate == "idle")
	{
		//OutputDebugStringA("hello\n");
		if (frame != 0 && startframe != 0 && endframe != 3)
		{
			frame = 0;
			startframe = 0;
			endframe = 3;
		}
	}
	else if (gamestate == "run")
	{
		if (frame != 4 && startframe != 4 && endframe != 7)
		{
			frame = 4;
			startframe = 4;
			endframe = 7;
		}
	}

	if ((int)GetTickCount64() > (starttime + delay))
	{
		starttime = (int)GetTickCount64();
		frame++;
		if (frame > endframe) frame = startframe;
		if (frame < startframe) frame = endframe;
	}
}

void Sprite::Draw(LPD3DXSPRITE &spriteobj)
{
	if (!texture)
	{
		OutputDebugStringA("Texture failed to load");
		return;
	}
	//create matrix
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

	//calculate frame location in source image
	RECT rect;
	rect.left = left + (frame * width);
	rect.top = top;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	//draw sprite frame
	spriteobj->Draw(texture, &rect, NULL, NULL, color);
}

void Sprite::CombinedMovementFunction(float &deltatime, float &prevcoord, bool &condition)
{
	prevcoord = this->ReadX();
	this->MoveX(deltatime); //adding to x
	this->MapLimitX(condition);
	if (this->CollisionTerrain(collisionmap) || this->CollisionObject(collidables)) // does not collide with anything
	{
		SetX(prevcoord);
	}
	prevcoord = this->ReadY();
	this->MoveY(deltatime);//adding to y
	this->MapLimitY(condition);
	if (this->CollisionTerrain(collisionmap) || this->CollisionObject(collidables)) //does not collide with anything
	{
		SetY(prevcoord);
		ChangeVelY(0);
	}
}

float Sprite::ApproachFunction(float goal, float currentvelx, float deltatime)
{
	char str[256];
	bool flag = false; //if true, means direction is defined
	bool direction; //true for right, false for left
	float difference; //always positive value
	if ((goal < 0 && currentvelx > 0)) // both values are different signs
	{
		direction = false;
		currentvelx = 0;
		difference = -goal;
		flag = true;
	}
	else if (goal > 0 && currentvelx < 0)
	{
		direction = true;
		currentvelx = 0;
		difference = goal;
		flag = true;
	}
	else if (goal > currentvelx)
	{
		direction = true;
		difference = goal - currentvelx; // difference if both values are of the same signs
		flag = true;
	}
	else if (goal < currentvelx)
	{
		direction = false;
		difference = currentvelx - goal;
		flag = true;
	}


	if (flag)
	{
		if (direction) // going right
		{
			if (difference > deltatime)
			{
				return currentvelx + deltatime; // increasing velx
			}
		}
		else if (!direction) //going left
		{
			if (difference > deltatime)
			{
				return currentvelx - deltatime; //decreasing velx
			}
		}
	}

	return goal;
}

float Sprite::ApproachFunctionTest(float goal, float currentvelx, float deltatime)
{
	char str[256];
	bool flag = false; //if true, means direction is defined
	bool direction; //true for right, false for left
	float difference; //always positive value
	if ((goal < 0 && currentvelx > 0)) // both values are different signs
	{
		direction = false;
		difference = abs(goal) + currentvelx;
		flag = true;
	}
	else if (goal > 0 && currentvelx < 0)
	{
		direction = true;
		difference = goal + abs(currentvelx);
		flag = true;
	}
	else if (goal > currentvelx)
	{
		direction = true;
		difference = goal - currentvelx; // difference if both values are of the same signs
		flag = true;
	}
	else if (goal < currentvelx)
	{
		direction = false;
		difference = currentvelx - goal;
		flag = true;
	}

	if (flag)
	{
		if (direction) // going right
		{
			if (difference > deltatime)
			{
				return currentvelx + deltatime; // increasing velx
			}
		}
		else if (!direction) //going left
		{
			if (difference > deltatime)
			{
				return currentvelx - deltatime; //decreasing velx
			}
		}
	}
		
	return goal;
}

void Sprite::CollisionScenariosX(bool PlayerDirection, int cannonballindex)
{
	/*
		Scenario 1: Both Cannonball and Player moves towards each other
			Cannonball should take previous coords, Player should take previous coords

		Scenario 2: Player is not moving, but Cannonball moving towards player
			Cannonball should take previous coords, Player can also take previous coords(as previous coords will be the same current coords)

		Scenario 3: Cannonball is not moving, Player move to cannonball

		Scenario 4: Both Cannonball and Player moving in same direction
		Cannonball should take previous coords
	*/
	bool CannonballDirection = cannonballobjects[cannonballindex]->CannonballDirectionLR;

	if (CannonballDirection == PlayerDirection && cannonballobjects[cannonballindex]->ReadVelX() != 0) // Same direction each other
	{
		cannonballobjects[cannonballindex]->SetX(cannonballobjects[cannonballindex]->prevcoordx);
	}
	else //towards each other
	{
		cannonballobjects[cannonballindex]->SetX(cannonballobjects[cannonballindex]->prevcoordx);
		SetX(prevcoordx);
	}
	
}

void Sprite::CollisionScenariosY(bool PlayerDirection, int cannonballindex)
{
	/*
		Scenario 1: Both Cannonball and Player moves towards each other
			Cannonball should take previous coords, Player should take previous coords

		Scenario 2: Player is not moving, but Cannonball moving towards player
			Cannonball should take previous coords, Player can also take previous coords(as previous coords will be the same current coords)

		Scenario 3: Cannonball is not moving, Player move to cannonball

		Scenario 4: Both Cannonball and Player moving in same direction
		Cannonball should take previous coords
	*/
	bool CannonballDirection = cannonballobjects[cannonballindex]->CannonballDirectionUD;

	if (CannonballDirection == PlayerDirection && cannonballobjects[cannonballindex]->ReadVelY() != 0) // Same direction each other
	{
		cannonballobjects[cannonballindex]->SetY(cannonballobjects[cannonballindex]->prevcoordy);
	}
	else //towards each other
	{
		cannonballobjects[cannonballindex]->SetY(cannonballobjects[cannonballindex]->prevcoordy);
		SetY(prevcoordy);
	}

}