#include "Global.h"

using namespace std;

//Direct3D variables
D3DDISPLAYMODE dm;
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteobj = NULL;
LPD3DXSPRITE fontspriteobj = NULL;

//Direct Input variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];

D3DPRESENT_PARAMETERS d3dpp;


bool Direct3D_Init(HWND window, int width, int height)
{
	//intialisation of Direct3D--------------------------------------------------------------------------------
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		MessageBox(window, "Error intialising Direct3D", "Error", MB_OK);
		return false;
	}

	//set Direct3Dpresentation parameters
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.hDeviceWindow = window;

	//create Direct3D device--------------------------------------------------------------------------------
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);
	if (!d3ddev)
	{
		MessageBox(window, "Error Creating Direct3D", "Error", MB_OK);
		return false;
	}

	//get a pointer to the backbuffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//create player sprite object
	D3DXCreateSprite(d3ddev, &spriteobj);
	D3DXCreateSprite(d3ddev, &fontspriteobj);


	return true;
}

void Direct3D_Shutdown()
{
	if (spriteobj) spriteobj->Release();
	if (backbuffer) backbuffer->Release();
	if (d3ddev)
	{
		d3ddev->Release();
	}
	if (d3d)
	{
		d3d->Release();
	}
}

void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	//get width and height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	//create rects for drawing
	RECT source_rect{ 0, 0, (LONG)desc.Width, (LONG)desc.Height };
	RECT dest_rect = { (LONG)x, (LONG)y, (LONG)x + (LONG)desc.Width, (LONG)y + (LONG)desc.Height };


	//draw the source surface onto the dest
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}

//Loads a bitmap file into a surface--------------------------------------------------------------------------------
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;


	//create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width, //width of surface
		info.Height, //height of surface
		D3DFMT_X8R8G8B8, //surface format
		D3DPOOL_DEFAULT, //memory pool to use
		&image, //pointer to the surface
		NULL);//reserved always NULL
	if (result != D3D_OK) return NULL;


	//load surface from file
	result = D3DXLoadSurfaceFromFile(
		image, // destination surface
		NULL, //destination palette
		NULL, //destination rectangle
		filename.c_str(), //source filename
		NULL, //source rectangle
		D3DX_DEFAULT, //control how image is filtered
		D3DCOLOR_XRGB(0, 0, 0), //for transparency(0 for none)
		NULL); //source image info
	if (result != D3D_OK) return NULL;

	return image;
}

//DirectInput intialisation--------------------------------------------------------------------------------
bool DirectInput_Init(HWND hwnd)
{
	//Initialising Direct Input
	HRESULT directinput = DirectInput8Create(
		GetModuleHandle(NULL), //get program instance
		DIRECTINPUT_VERSION, //get direct input version
		IID_IDirectInput8, //unique identifier for version of direct input to use
		(void**)&dinput, //double pointer for Direct Input Object pointer
		NULL); //Always NULL

	//Creating DirectInput Object for Keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	//Initialising Keyboard, setting the device to use the keyboard format
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);

	//Setting cooperation level, to know when should the keyboard be active (eg.when game is in background,our game will thus not respond to the keyboard presses if it was set to set to )
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	//acquiring keyboard, to use the device
	dikeyboard->Acquire();

	//Creating DirectInput Object for Mouse
	HRESULT result = dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	//Initialising Mouse, setting the device to use the mouse format
	result = dimouse->SetDataFormat(&c_dfDIMouse);
	//Setting cooperation level, to know when should the mouse be active (eg.when game is in background,our game will thus not respond to the mouse presses if it was set to set to )
	result = dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//acquiring mouse, to use the device
	result = dimouse->Acquire();
	if (result != S_OK) return false;
	d3ddev->ShowCursor(false);

	return true;
}

//DirectInput Update
void DirectInput_Update()
{
	//update keyboard
	dikeyboard->GetDeviceState(sizeof(keys), keys);

	//update mouse
	dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

	dikeyboard->Acquire(); //Reacquiring device if every loop if device is not acquired
	dimouse->Acquire();

}
//return mouse x movement
int Mouse_X()
{
	return mouse_state.lX;
}

//return mouse y movement
int Mouse_Y()
{
	return mouse_state.lY;
}

//return mouse button state
int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

//return key press state
int Key_Down(int key)
{
	return (keys[key] & 0x80);
}

//DirectInput shutdown
void DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}

	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}

//Get Bitmap size (width and height to contain the image)
D3DXVECTOR2 GetBitmapSize(string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);

	return size;
}

//font functions-------------------------------------------------------------------------------------
LPD3DXFONT MakeFont(string name, int size)
{
	LPD3DXFONT font;
	D3DXFONT_DESC desc =
	{
		size, //size of font
		0, //width
		0, //weight
		0, //miplevels
		false, //italics
		DEFAULT_CHARSET, //charset
		OUT_TT_PRECIS, //output precision
		CLIP_DEFAULT_PRECIS,//quality
		DEFAULT_PITCH, //pitch and family
		"Press Start 2P" //font name
	};
	strcpy_s(desc.FaceName, name.c_str());
	HRESULT result = D3DXCreateFontIndirect(d3ddev, &desc, &font);
	if (result != S_OK)
	{
		return NULL;
	}
	return font;

}

vector<int> FontCalculateVector(LPD3DXFONT font,int x, int y, string text, D3DCOLOR white)
{
	//figure out text boundary 
	int width;
	float scaling = 1.0f;
	float rotation = 0.0f;
	
	RECT rect = { x, y, 0, 0 };

	/*
	* USAGE OF DT_CALCRECT FLAG	
		Determines the width and height of the rectangle. 
		If there are multiple lines of text, DrawText uses the width of the rectangle pointed to by the pRect parameter and extends the base of the rectangle to bound the last line of text. 
		If there is only one line of text,
		DrawText modifies the right side of the rectangle so that it bounds the last character in the line. 
		In either case, DrawText returns the height of the formatted text but does not draw the text.
	*/
	int height = (int)font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, white);  //Essentially extends the rectangle to cover the last char of the string 

	if (rect.left > rect.right)
	{
		width = rect.left - rect.right;
	}
	else
	{
		width = rect.right - rect.left;
	}
	// Drawing in center instead of top left coords for rect
	rect.left = x -width / 2;
	sprintf_s(str, sizeof(str), "StringWidth: %d, StringHeight: %d\n", width, height);
	OutputDebugStringA(str);

	rect.top = y -height / 2;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;


	SettingSpriteObject(fontspriteobj, scaling, 0, 0, 0, 0, rotation);
	font->DrawText(fontspriteobj, text.c_str(), text.length(), &rect, DT_CENTER, white);
	vector<int> Fontvector;
	Fontvector.push_back(width);
	Fontvector.push_back(height);
	return Fontvector;
}
