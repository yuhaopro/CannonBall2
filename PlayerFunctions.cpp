#include "Global.h"
using namespace std;

//const player speed variables
const float playerXspeed = 100;
const float playerJumpspeed = 100;
const float playerFireJumpspeed = 300;
string playerinput = "playerinput";
string collisioninput = "collisioninput";
void Player::PlayerControl() // how movement of player is controlled
{
	if (Key_Down(LeftKey))
	{
		VectorSumVelX(-playerXspeed, playerinput);
		gamestate = "run";
		directionLR = false;
	}
	else if (Key_Down(RightKey))
	{
		VectorSumVelX(playerXspeed, playerinput);
		gamestate = "run";
		directionLR = true;
	}
	else
	{
		gamestate = "idle";
	}

	if (Key_Down(JumpKey))  //JUMP mechanic	
	{
		if (!cooldownJump) //jump not on cooldown
		{
			ChangeVelY(-playerJumpspeed);
			if (Key_Down(FireKey)) //fire while jumping
			{
				if (!cooldownFire)
				{
					ChangeVelY(-playerFireJumpspeed);
				}
			}
			gamestate = "jump";
			directionUD = false;
			cooldownJump = true;
		}
	}

	if (Key_Down(FireKey)) //fire something
	{
		if (!cooldownFire) //fire not on cooldown
		{
			OutputDebugStringA("Fire\n");
			Fire();
			gamestate = "attack";
			cooldownFire = true;
			sprintf_s(str, sizeof(str), "cooldownfire: %d \n", cooldownFire);
			OutputDebugStringA(str);
		}
	}
	Cooldown(cooldownJump, cooldowntimerJump);
	Cooldown(cooldownFire, cooldowntimerFire);
}

string Player::ReadGameState()
{
	return gamestate;
}

bool Player::ReadDirectionLR()
{
	return directionLR;
}

bool Player::ReadDirectionUD()
{
	return directionUD;
}

void Player::Fire()
{
	//check for space to fire ball
	float temp = ReadX();
	vector <int> tempplayers = CollisionObjectSprite(playersarray);
	sprintf_s(str, sizeof(str), "Player Colliding with player: %d\n", (int)tempplayers.size());
	OutputDebugStringA(str);
	if (directionLR) //player direction the right
	{
		SetX(temp + TILEWIDTH);
		if (!CollisionTerrain(collisionmap) && (int)CollisionObjectSprite(playersarray).size() == 0) //nothing on the right
		{
			SetX(temp);
			cannonballCreation();
		}

	}
	else //player direction the left
	{
		SetX(temp - TILEWIDTH);
		if (!CollisionTerrain(collisionmap) && (int)CollisionObjectSprite(playersarray).size() == 0) //nothing on the left
		{
			SetX(temp);
			cannonballCreation();
		}
	}
	SetX(temp);
}

void Player::cannonballCreation()
{
	//This will add cannonball to the heap using malloc 
	//since malloc doesnt call the constructor, we need to manually construct the object
	cannonballobjects.push_back((Cannonball*)malloc(sizeof(Cannonball)));

	cannonballobjects.back()->deleteball = false;
	bool check = cannonballobjects.back()->LoadTexture(d3ddev, "files/cannonball.png", cannonballobjects.back()->color);
	if (!check)
	{
		OutputDebugStringA("Texture failed\n");
	}
	else
	{
		OutputDebugStringA("Texture succeeded\n");
	}
	//intialising cannonball object same as Sprite constructor
	cannonballobjects.back()->CannonballDirectionLR = directionLR;
	cannonballobjects.back()->CannonballDirectionUD = true;
	if (directionLR)
	{
		//RIGHT
		cannonballobjects.back()->SetX(ReadX() + TILEWIDTH);
	}
	else
	{
		//LEFT
		cannonballobjects.back()->SetX(ReadX() - TILEWIDTH);
	}

	cannonballobjects.back()->SetY(ReadY());
	cannonballobjects.back()->ChangeVelX(cannonballobjects.back()->AdjustingDirection(ballspeed, directionLR)); //changing signage of velx of cannonball based on the direction which it is facing
	cannonballobjects.back()->ChangeVelY(0);
	cannonballobjects.back()->starttime = 0;
	cannonballobjects.back()->SetRotation(0);
	cannonballobjects.back()->SetScaling(1.0);
	cannonballobjects.back()->width = cannonballobjects.back()->height = 16;
	cannonballobjects.back()->SetSourceCoords(0, 0);
	cannonballobjects.back()->frame = 0;
	cannonballobjects.back()->startframe = 0;
	cannonballobjects.back()->endframe = 0;
	cannonballobjects.back()->delay = 0;
	cannonballobjects.back()->color = D3DCOLOR_XRGB(255, 255, 255);
	collidables.push_back(cannonballobjects.back());

	//play firesound
	soundsystem->SoundPlay(sound);
}
//cooldown to be called whenever its true
void Player::Cooldown(bool& cooldown, int &timer)
{
	if (cooldown)
	{
		if ((int)GetTickCount64() > timer + cooldownduration)
		{
			timer = (int)GetTickCount64();
			cooldown = false;
		}
	}
	else
	{
		timer = (int)GetTickCount64();
	}
}

void Player::SetCooldownDuration(int milliseconds)
{
	cooldownduration = milliseconds;
}

vector<int> Player::CollisionObjectCannonball(vector<Cannonball*> &object)
{
	vector<int> cannonballNumber;
	float x = ReadX();
	float y = ReadY();
	RECT rect1 = { (LONG)x, (LONG)y, (LONG)(x + width), (LONG)(y + height) };
	RECT dest;
	for (int i = 0; i < (int)object.size(); i++)
	{	
		float xO = object[i]->ReadX();
		float yO = object[i]->ReadY();
		RECT rect2 = { (LONG)xO, (LONG)yO, (LONG)(xO + object[i]->width), (LONG)(yO + object[i]->height) };
		if (IntersectRect(&dest, &rect1, &rect2))
		{
			cannonballNumber.push_back(i);
		}
	}
	return cannonballNumber;
}

void Player::CannonballCollisionAgainX()
{

	vector<int> tempvector = CollisionObjectCannonball(cannonballobjects);
	bool collision = false;
	if ((int)tempvector.size() > 0)
	{
		collision = true;
	}
	else
	{
		return; // break recursion if no more collision with player and cannonballobjects
	}

	for (int i = 0; i < (int)tempvector.size(); i++)
	{
		cannonballobjects[tempvector[i]]->SetX(cannonballobjects[tempvector[i]]->prevcoordx);
	}
	tempvector.clear();
}

void Player::CannonballCollisionAgainY()
{

	vector<int> tempvector = CollisionObjectCannonball(cannonballobjects);
	bool collision = false;
	if ((int)tempvector.size() > 0)
	{
		collision = true;
	}
	else
	{
		return; // break recursion if no more collision with player and cannonballobjects
	}

	for (int i = 0; i < (int)tempvector.size(); i++)
	{
		cannonballobjects[tempvector[i]]->SetY(cannonballobjects[tempvector[i]]->prevcoordy);
	}
	tempvector.clear();
}


bool Player::CollisionCannonballCheck()
{
	CannonballNumber = CollisionObjectCannonball(cannonballobjects);
	if (CannonballNumber.size() > 0)
	{
		return true;
	}
	return false;
}

void Player::CombinedMovementFunction(float& deltatime, bool& condition)
{
	//X AXIS-------------------------------------------------------------------
	//simulate player movement to check if x should be moved
	prevcoordx = ReadX();
	prevcoordvelx = ReadVelX();
	sprintf_s(str, sizeof(str), "prevcoordvelx: %f, prevcoordx: %f\n", prevcoordvelx, prevcoordx);
	OutputDebugStringA(str);
	float newvelx = ApproachFunction(zero, prevcoordvelx, deltatime * modifierS);
	if (newvelx < 0)
	{
		directionLR = false;
	}
	else if (newvelx > 0)
	{
		directionLR = true;
	}
	ChangeVelX(newvelx);
	MoveX(deltatime);//adding to x
	MapLimitX(condition);
	PlayerControl();
	//player collide with terrain condition
	if (CollisionTerrain(collisionmap)) 
	{
		SetX(prevcoordx);
		ChangeVelX(0); // this prevents the player from skipping over walls due to high velx
		if (CollisionCannonballCheck()) //at least one cannonball collision
		{
			for (int i = 0; i < (int)CannonballNumber.size(); i++)
			{
				//changing each cannonball direction after checking cannonball collided with player collided with wall
				CollisionScenariosX(directionLR, CannonballNumber[i]);
				//cannonball change direction and take opposite velx
				cannonballobjects[CannonballNumber[i]]->CannonballDirectionLR = !cannonballobjects[CannonballNumber[i]]->CannonballDirectionLR;
				directionLR = cannonballobjects[CannonballNumber[i]]->CannonballDirectionLR;
				cannonballobjects[CannonballNumber[i]]->ChangeVelX(cannonballobjects[CannonballNumber[i]]->AdjustingDirection(cannonballobjects[CannonballNumber[i]]->prevcoordvelx, cannonballobjects[CannonballNumber[i]]->CannonballDirectionLR));
				OutputDebugStringA("Collided Wall and Cannonball\n");
			}
		}
	}
	//player collide with cannonball condition
	else if (CollisionCannonballCheck()) 
	{
		for (int i = 0; i < (int)CannonballNumber.size(); i++)
		{
			float speed;
			speed = cannonballobjects[CannonballNumber[i]]->ReadVelX();
			VectorSumVelX(speed, collisioninput);
			sprintf_s(str, sizeof(str), "Speed: %f\n",speed);
			OutputDebugStringA(str);
			CollisionScenariosX(directionLR, CannonballNumber[i]);
		}
		CannonballCollisionAgainX(); //double check collision again to make sure setting player back does not collide with other balls

	}
	sprintf_s(str, sizeof(str), "NEWvelx: %f, NEWx: %f\n", ReadVelX(), ReadX());
	OutputDebugStringA(str);
	CannonballNumber.clear();

	//Y AXIS-----------------------------------------------------------------------

	prevcoordy = ReadY();
	prevcoordvely = ReadVelY();
	float newvely = ApproachFunction(zero, prevcoordvely, deltatime * modifierS);
	//change directionUD of player base on vely signage
	if (newvely < 0)
	{
		directionUD = false;
	}
	else if(newvely > 0)
	{
		directionUD = true;
	}
	ChangeVelY(newvely);
	MoveY(deltatime); //adding to Y
	MapLimitY(condition);

	if (CollisionTerrain(collisionmap)) //collide with terrain
	{
		SetY(prevcoordy);
		ChangeVelY(0);
		if (CollisionCannonballCheck())
		{
			for (int i = 0; i < (int)CannonballNumber.size(); i++)
			{
				//changing each cannonball direction after checking cannonball collided with player collided with wall
				CollisionScenariosY(directionUD, CannonballNumber[i]);
				//player direction will face down
				directionUD = cannonballobjects[CannonballNumber[i]]->CannonballDirectionUD;
				//cannonball change direction and take opposite velx
				cannonballobjects[CannonballNumber[i]]->CannonballDirectionUD = !cannonballobjects[CannonballNumber[i]]->CannonballDirectionUD;
				cannonballobjects[CannonballNumber[i]]->ChangeVelY(cannonballobjects[CannonballNumber[i]]->AdjustingDirection(prevcoordvely, cannonballobjects[CannonballNumber[i]]->CannonballDirectionUD));
			}
		}
		
	}
	else if (CollisionCannonballCheck())
	{
		
		for (int i = 0; i < (int)CannonballNumber.size(); i++)
		{
			float speed;
			speed = cannonballobjects[CannonballNumber[i]]->ReadVelY(); //cannonball speed                              
			ChangeVelY(speed); //set player speed to cannonball speed upon contact
			CollisionScenariosY(directionUD, CannonballNumber[i]);
		}
		CannonballCollisionAgainY();
		
	}
	CannonballNumber.clear();
}