#include "CSCIx229.h"
int incatapult(double movex, double movey, double movez)
{
	double boxminx=boundingbox.trebuchetbox[0]-boundingbox.trebuchetbox[3];
	double boxmaxx=boundingbox.trebuchetbox[0]+boundingbox.trebuchetbox[3];
	double boxminy=boundingbox.trebuchetbox[1]-boundingbox.trebuchetbox[4];
	double boxmaxy=boundingbox.trebuchetbox[1]+boundingbox.trebuchetbox[4];
	double boxminz=boundingbox.trebuchetbox[2]-boundingbox.trebuchetbox[5];
	double boxmaxz=boundingbox.trebuchetbox[2]+boundingbox.trebuchetbox[5]; 
	//printf("minx:%.2f, miny:%.2f, minz:%.2f maxx:%.2f, maxy:%.2f, maxz:%.2f",boxminx,boxminy,boxminz,boxmaxx,boxmaxy,boxmaxz);
	return (movex >= boxminx && movex <= boxmaxx) &&
    (movey >= boxminy && movey <= boxmaxy) &&
    (movez >= boxminz && movez <= boxmaxz);
}

int AABB(double movex, double movey, double movez)
{
	int inobject=0;
	if(incatapult(movex, movey, movez))
	{
		inobject=1;
	}
	return inobject;
}
