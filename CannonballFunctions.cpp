#include "Global.h"


using namespace std;
const float ballspeed = 500;
const float modifier = 200;
const float modifierL = 1000;
const float modifierS = 50;
const float zero = 0;
char str1[256];

//by default ballspeed is positive----to the right
float Cannonball::AdjustingDirection(float ballspeedvalue, bool &cannonballdirectionvalue)
{
	if (cannonballdirectionvalue)//right
	{
		if (ballspeedvalue > 0)
		{
			return ballspeedvalue;
		}
		else
		{
			return -ballspeedvalue; //change the negative speed to positive
		}
	}
	else//left
	{
		if (ballspeedvalue < 0) 
		{
			return ballspeedvalue; 
		}
		else
		{
			return -ballspeedvalue; //change positive speed to negative
		}
	}
}

void Cannonball::CombinedMovementFunction(float& deltatime, bool& condition)
{
	//X axis
	prevcoordx = ReadX();
	prevcoordvelx = ReadVelX();
	float newvelx = ApproachFunction(zero , prevcoordvelx , deltatime * modifierS);
	if (newvelx < 0)
	{
		CannonballDirectionLR = false;
	}
	else if (newvelx > 0)
	{
		CannonballDirectionLR = true;
	}
	ChangeVelX(newvelx);
	MoveX(deltatime); //adding to x
	MapLimitX(condition);
	
	//check for collision with player
	/*
	vector<int> playersnumber = CollisionObjectSprite(playersarray); //return a list of indexes that collided with cannonball in the playersarray
	bool CollisionWithPlayer = false;
	if (playersnumber.size() > 0)
	{
		CollisionWithPlayer = true;
	}

	if (CollisionWithPlayer)
	{
		for (int i = 0; i < playersnumber.size(); i++)
		{
			float speed = ReadVelX();
			playersarray[playersnumber[i]]->ChangeVelX(speed);
		}
	}
	playersnumber.clear();

	*/
	if (CollisionTerrain(collisionmap)) // does not collide with anything
	{
		SetX(prevcoordx);
		CannonballDirectionLR = !CannonballDirectionLR; //changing to opposite direction
		ChangeVelX(AdjustingDirection(prevcoordvelx, CannonballDirectionLR));
		OutputDebugStringA("Collided with wall\n");
	}

	//Y axis, making cannonball bounce
	prevcoordy = ReadY();
	prevcoordvely = ReadVelY();//getting current vely
	float newvely = ApproachFunction(zero, prevcoordvely, deltatime * modifierS);//linear interpolating currentvely towards 0, currentvely decrease 
	if (newvely < 0)
	{
		CannonballDirectionUD = false;
	}
	else if (newvely > 0)
	{
		CannonballDirectionUD = true;
	}
	ChangeVelY(newvely);
	MoveY(deltatime);//adding to y, also adding to gravity to vely, vely increase
	MapLimitY(condition);
	if (CollisionTerrain(collisionmap)) //does not collide with anything
	{
		SetY(prevcoordy);
		CannonballDirectionUD = !CannonballDirectionUD; //changing to opposite direction
		ChangeVelY(AdjustingDirection(prevcoordvely, CannonballDirectionUD));
	}
}