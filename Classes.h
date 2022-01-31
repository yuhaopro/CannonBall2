#pragma once
#include "Global.h"
using namespace std;

class Cannonball;
class Player;

class Sprite
{
private:
	float x, y, velx, vely, scaling, rotation;
		 //source coords from spritesheet
public:
	float prevcoordvelx, prevcoordvely, prevcoordx, prevcoordy;
	int left, top;
	int width, height;
	int frame, startframe, endframe, starttime, delay;
	LPDIRECT3DTEXTURE9 texture;
	D3DCOLOR color;
public:
	bool LoadTexture(LPDIRECT3DDEVICE9 &d3ddev, string filename, D3DCOLOR color);
	void MoveX(float deltatime);
	void MoveY(float deltatime);
	float ReadX();
	float ReadY();
	void SetX(float value);
	void SetY(float value);
	void VectorSumVelX(float value, string typeofvel);
	void VectorSumVelY(float value);
	void ChangeVelX(float value);
	void ChangeVelY(float value);
	float ReadVelX();
	float ReadVelY();
	void SetSourceCoords(int left, int top);
	void SetRotation(float value);
	void SetScaling(float value);
	float ReadScaling();
	float ReadRotation();
	void MapLimitX(bool& condition);
	void MapLimitY(bool& condition);
	bool CollisionTerrain(bool** collisionmap);
	bool CollisionObject(vector<Sprite*> &collidables);
	vector<int> CollisionObjectSprite(vector<Player*>& collidables);
	void Animation(string &gamestatevalue);
	void Draw(LPD3DXSPRITE &spriteobj);
	void CombinedMovementFunction(float &deltatime, float &prevcoord, bool &condition);
	float ApproachFunction(float goal, float currentvelx, float deltatime);
	float ApproachFunctionTest(float goal, float currentvelx, float deltatime);
	void CollisionScenariosX(bool PlayerDirection, int cannonballindex);
	void CollisionScenariosY(bool PlayerDirection, int cannonballindex);
	Sprite(float xvalue, float yvalue)
	{
		x = xvalue;
		y = yvalue;
		velx = 0;
		vely = 0;
		prevcoordx = x;
		prevcoordy = y;
		prevcoordvelx = 0;
		prevcoordvely = 0;
		starttime = 0;
		scaling = 1.0f;
		rotation = 0.0f;
		width = height = 16;
		left = 0;
		top = 0;
		frame = 0;
		startframe = 0;
		endframe = 0;
		delay = 0;
		color = D3DCOLOR_XRGB(255, 255, 255);
	}
	~Sprite()
	{
		texture->Release();
	}
};

class Player : public Sprite
{
private:
	int cooldownduration;
	bool directionLR;
	bool directionUD;
	string gamestate;
public:
	int LeftKey, RightKey, JumpKey, FireKey;
	int cooldowntimerJump, cooldowntimerFire;
	bool cooldownJump;
	bool cooldownFire;
	bool dead;
	bool CannonballCollision;
	vector<int> CannonballNumber;
public:
	Player(float xvalue, float yvalue) : 
		Sprite(xvalue, yvalue)
	{
		cooldowntimerJump = 0;
		cooldowntimerFire = 0;
		cooldownduration = 2000;
		directionLR = true;
		directionUD = true;
		CannonballCollision = false;
		cooldownJump = false;
		cooldownFire = false;
		gamestate = "idle";
		dead = false;
	}
	void PlayerControl();
	string ReadGameState();
	void Fire();
	bool ReadDirectionLR();
	bool ReadDirectionUD();
	void Cooldown(bool& cooldown, int &timer);
	void SetCooldownDuration(int milliseconds);
	void cannonballCreation();
	vector<int> CollisionObjectCannonball(vector<Cannonball*> &object);
	void CannonballCollisionAgainX();
	void CannonballCollisionAgainY();
	void CombinedMovementFunction(float& deltatime, bool& condition);
	bool CollisionCannonballCheck();

	~Player()
	{
	}
};

class Cannonball : public Sprite
{
public:
	bool CannonballDirectionLR;// True for Right, False for Left
	bool CannonballDirectionUD;// True for Down, False for Up
	bool deleteball;
public:
	float AdjustingDirection(float ballspeedvalue, bool& cannonballdirectionvalue);
	void CombinedMovementFunction(float& deltatime, bool& condition);
	Cannonball(float valuex, float valuey) : Sprite(valuex, valuey) 
	{
		CannonballDirectionLR = false;
		CannonballDirectionUD = true;
		deleteball = true;
	}

	~Cannonball() {}
};

