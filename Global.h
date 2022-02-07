#pragma once
//header files
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#define _CRTDBG_MAP_ALLOC
#define turninto(varname) #varname
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <InitGuid.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <synchapi.h>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <crtdbg.h>
#include <math.h>
#include <xaudio2.h>
#include <comdef.h>
#include "Classes.h"

//libraries
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxguid.lib")

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif


using namespace std;
extern bool** collisionmap;
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern const int MAPWIDTH;
extern const int MAPHEIGHT;
extern const int MAPTILEWIDTH;
extern const int MAPTILEHEIGHT;
extern const int TILEWIDTH;
extern const int TILEHEIGHT;
extern bool gameover;
extern bool gamestart;
extern string gamestate;
extern const float gravity;
extern int error;
extern const float zero;
extern char str[256];
extern const float ballspeed;
extern const float modifier;
extern const float modifierS;
extern const float modifierL;
extern const float playerXspeed;
extern const float playerJumpspeed;
extern const float playerFireJumpspeed;
extern string playerinput;
extern string collisioninput;
extern vector<Sprite*> collidables;
extern vector<Cannonball*> cannonballobjects;
extern vector<Player*> playersarray;
extern vector<char> Countdown;
extern D3DCOLOR White;
extern LARGE_INTEGER startingtime, endingtime, timestep, frequency;
extern float deltatime;
extern int SpawnY;

//DIRECT3D objects
extern D3DDISPLAYMODE dm;
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteobj;
extern LPD3DXSPRITE fontspriteobj;
extern LPD3DXFONT BITFONT80;
extern LPD3DXFONT BITFONT48;
extern LPD3DXFONT BITFONT20;
extern vector<string> CountdownString;
extern float countdowntimer;
extern bool CountDownOver;
extern SoundSystem* soundsystem;
extern Sound* sound;

//DIRECT3D functions
bool Direct3D_Init(HWND hwnd, int width, int height);
void Direct3D_Shutdown();


//DIRECTINPUT Objects, Devices, and States
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;

//DIRECTINPUT functions
bool DirectInput_Init(HWND hwnd);
void DirectInput_Update();
void DirectInput_Shutdown();
int Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();

//GlobalFunctions
void DrawTiles(LPDIRECT3DSURFACE9 source, int left, int top, int width, int height, LPDIRECT3DSURFACE9 dest, int destx, int desty);
LPDIRECT3DSURFACE9 LoadSurface(string filename);
LPD3DXFONT MakeFont(string name, int size);
vector<int> FontCalculateVector(LPD3DXFONT font,int x, int y, string text, D3DCOLOR white);
void SettingSpriteObject(LPD3DXSPRITE& spriteobj, float scaling, float x, float y, float width, float height, float rotation);
int NumericLinearInterpolation(int goal, int current, float deltatime);
void GameInitialization();

//game functions
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();

//menu functions
void Menu(HWND window);
void MenuExit();

//map functions
bool** Terrain(LPDIRECT3DSURFACE9 source, LPDIRECT3DSURFACE9 dest);



