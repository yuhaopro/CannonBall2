#include "Global.h"
string PressPlay = "Press Space \n To Play!";
int opaqueslider = 255;
int MaxAlpha = 200;
int MinAlpha = 0;
int modifierXS = 250;
int Goal = 0;
bool GameInitialized = false;

void Menu(HWND window)
{
	if (!GameInitialized)
	{
		GameInitialization();
		GameInitialized = true;
	}
	DirectInput_Update();
	//high resolution timer
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingtime);

	if (Key_Down(DIK_SPACE))
	{
		gamestart = true;
		GameInitialized = false;
		MenuExit();
		return;
	}

	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));
	if (d3ddev->BeginScene() == D3D_OK)
	{
		/*
			Drawing Text on Screen
		*/
		fontspriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		D3DCOLOR FontColor = D3DCOLOR_RGBA(255, 255, 255, opaqueslider);
		if (opaqueslider == MinAlpha)
		{
			Goal = MaxAlpha;
		}
		else if (opaqueslider == MaxAlpha)
		{
			Goal = MinAlpha;
		}
		opaqueslider = NumericLinearInterpolation(Goal, opaqueslider, deltatime * modifierXS);
		if (opaqueslider > MaxAlpha)
		{
			opaqueslider = MaxAlpha;
		}
		else if (opaqueslider < MinAlpha)
		{
			opaqueslider = MinAlpha;
		}
		FontCalculateVector(BITFONT48, SCREENW / 2, SCREENH / 2, PressPlay, FontColor);
		fontspriteobj->End();

		//Drawing End
		if (d3ddev->EndScene() != D3D_OK) OutputDebugStringA("device failed to end scene \n");
		if (d3ddev->Present(NULL, NULL, NULL, NULL) != D3D_OK) OutputDebugStringA("device failed to present scene \n");
	}
	else
	{
		OutputDebugStringA("Failed to begin Menu Scene \n");
	}
	QueryPerformanceCounter(&endingtime);
	timestep.QuadPart = endingtime.QuadPart - startingtime.QuadPart;
	deltatime = (float)timestep.QuadPart / (float)frequency.QuadPart;
}

void MenuExit()
{
	
}