#include "Global.h"

using namespace std;
//file for terrain generation
const int Xstart = 10, Xend = MAPTILEWIDTH - 10;
int SpawnY;

bool** Terrain(LPDIRECT3DSURFACE9 source, LPDIRECT3DSURFACE9 dest)
{
	bool Collision = TRUE;
	int row = MAPTILEHEIGHT;
	bool** array = new bool* [MAPTILEHEIGHT];
	int walltopY = 42; //total height is 96
	SpawnY = (walltopY - 11) * TILEHEIGHT;
	for (int y = 0; y < MAPTILEHEIGHT; y++)
	{
		array[y] = new bool[MAPTILEWIDTH];
		for (int x = 0; x < MAPTILEWIDTH; x++)
		{
			if ((y == walltopY - 10) && (x > Xstart && x <	MAPTILEWIDTH / 2 - 10))
			{
				DrawTiles(source, 32, 0, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else if ((y == walltopY - 10) && ((x > MAPTILEWIDTH / 2 + 10) && x < Xend))
			{
				DrawTiles(source, 32, 0, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			//drawing platform
			else if (y == walltopY && x == Xstart)
			{
				DrawTiles(source, 16, 0, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else if (y == walltopY && x == Xend)
			{
				DrawTiles(source, 48, 0, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else if (y == walltopY && (x > Xstart && x < Xend))
			{
				DrawTiles(source, 32, 0, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			//drawing walls
			else if (y > walltopY && x == Xstart)
			{
				DrawTiles(source, 16, 16, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else if (y > walltopY && x == Xend)
			{
				DrawTiles(source, 48, 16, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else if (y > walltopY && (x > Xstart && x < Xend))
			{
				DrawTiles(source, 32, 16, 16, 16, dest, x * TILEWIDTH, y * TILEHEIGHT);
				array[y][x] = TRUE;
			}
			else
			{
				array[y][x] = FALSE;
			}


		}

	}
	return array;

}