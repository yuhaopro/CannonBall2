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