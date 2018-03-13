/*
 *
 *  Demonstrates how to draw a cockpit view for a 3D scene.
 *
 *  Key bindings:
 *  m          Toggle cockpit mode
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  l		   Enable lighting	
 *  ESC        Exit
 */
#include "CSCIx229.h"
int axes=0;       //  Display axes
int mode=0;       //  perspective or first person
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=25;       //  Field of view
double asp=1;     //  Aspect ratio
double dim=10.0;   //  Size of world

//lighting parameters
int movelight =1; // toggle for idlily moving light
int diffuse=50;
int specular=20;
int ambient=20;
int emission=30;
int light=1; //enable or disable light
double zh=0; //lighting angle of rotation
double distance  =   2;  // Light distance
float ylight  =   0;  // Elevation of light
int local     =   0;  // Local Viewer Model
float shiny   =   1;  // Shininess (value)
int shininess =   3;  // Shininess (power of two)

//terrain parameter
float z[65][65];
float zmag=.1;
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location

//bounding boxes
int showboxes=0;

//texture parameters
int cockpit;
int land;
int wood;
int wood2;
int metal;
int water;
//movement parameters
double dirx=0;
double diry=0;
double dirz=0;
double movex=2;
double movey=2;
double movez=5;
double moveth=0;// angle for the planar movement
double moveph=0;// angle for the spherical cords


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void Vertex(double th,double ph)
{
	double x=Sin(th)*Cos(ph);
	double y= Sin(ph);
	double z= Cos(th)*Cos(ph);
	glNormal3d(x,y,z);
	glVertex3d(x , y , z);
}
double *normalplane(double *p1,double *p2, double *p3,double *norm)
{
	double a1[3]={0,0,0};
	double a2[3]={0,0,0};
	a1[0]=p2[0]-p1[0];
	a1[1]=p2[1]-p1[1];
	a1[2]=p2[2]-p1[2];
	a2[0]=p3[0]-p1[0];
	a2[1]=p3[1]-p1[1];
	a2[2]=p3[2]-p1[2];
	//printf("x:%.2f, y:%.2f, z:%.2f",a1[0],a1[1],a1[2]);
	norm[0]=(a1[1]*a2[2])-(a2[1]*a1[2]);
	norm[1]=-(a1[0]*a2[2])+(a2[0]*a1[2]);
	norm[2]=(a1[0]*a2[1])-(a2[0]*a1[1]);
	return norm;
}
static void sphere1(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   float yellow[] = {0,1.0,0.0,1.0};
   float Emission[]  = {0.01*emission,0.005*emission,0.0,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  yellow ball
   glColor3f(1,1,0);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
} 
static void cylinder(double x,double y,double z, double r,double d, double th, double ph)
{
   int i,k;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(r,r,d);
   //  Head & Tail
   glColor3f(1,1,1);
   for (i=1;i>=-1;i-=2)
   {
      glNormal3f(0,0,i);
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5,0.5);
      glVertex3f(0.0,0.0,i);
      for (k=0;k<=360;k+=10)
      {
         glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
         glVertex3f(Cos(k),Sin(k),i);
      }
      glEnd();
   }
   //  Edge
   glBegin(GL_QUAD_STRIP);
   for (k=0;k<=360;k+=10)
   {
      glNormal3f(Cos(k),Sin(k),0);
      glTexCoord2f(0,0.01*k); glVertex3f(Cos(k),Sin(k),+1);
      glTexCoord2f(1,0.01*k); glVertex3f(Cos(k),Sin(k),-1);
   }
   glEnd();
   //  Undo transformations
   glPopMatrix();
}
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, double ph)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1, 1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1, 1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1, 1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(1.0,1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(-1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.5,0.5);
   glTexCoord2f(1.0,1.0);glVertex3f(+1,-1,+1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(-1,-1,+1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(-1,+1,+1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,+1,+1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,+1,+1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-1, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,-1,+1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}
void weight(double x, double y, double z, double dx, double dy, double dz, double th, double ph)
{
	glPushMatrix();
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,0,0,1);
    glScaled(dx,dy,dz);
	glBegin(GL_QUADS);
	//bottom of weight
	glColor3f(1,1,1);
	glNormal3f(0,-1,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.1,0.0,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.1,0.0,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.1,0.0,-0.1);
	
	// angled bottoms
	glColor3f(1,1,1);
	double n1[3]={0,0,0};
	double p1[3]={0.1,0.0,0.1};
	double p2[3]={0.3,0.1,0.1};
	double p3[3]={0.3,0.1,-0.1};
	normalplane(p1,p2,p3,n1);
	glNormal3f(-n1[0],-n1[1],-n1[2]);
	glTexCoord2f(1.0,1.0);glVertex3f(0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.1,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(0.3,0.1,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.1,0.0,-0.1);
	
	glColor3f(1,1,1);
	double n2[3]={0,0,0};
	double p4[3]={-0.1,0.0,0.1};
	double p5[3]={-0.3,0.1,0.1};
	double p6[3]={-0.3,0.1,-0.1};
	normalplane(p4,p5,p6,n2);
	glNormal3f(n2[0],n2[1],n2[2]);
	glTexCoord2f(1.0,1.0);glVertex3f(-0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.3,0.1,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.1,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.1,0.0,-0.1);
	
	//top
	glColor3f(1,1,1);
	glNormal3f(0,1,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.3,0.25,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.25,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.3,0.25,0.1);
	//high sides
	glColor3f(1,1,1);
	glNormal3f(-1,0,0);
	glTexCoord2f(1.0,1.0);glVertex3f(-0.3,0.1,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.3,0.1,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.3,0.25,0.1);
	
	glColor3f(1,1,1);
	glNormal3f(1,0,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.3,0.1,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.1,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.3,0.25,0.1);
	glEnd();
	//code to check normal vector correctness
	//glBegin(GL_LINES);
	//glColor3f(1,0,0);
	//glVertex3d(p4[0],p4[1],p4[2]);
	//glVertex3d(p4[0]+10*n2[0],p4[1]+10*n2[1],p4[2]+10*n2[2]);
	//glEnd();
	
	
	//odd sides
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	glNormal3f(0,0,1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, .1);
	glTexCoord2f(-.3,.25);glVertex3f(-.3, .25, .1);
	glTexCoord2f(-.3,.1);glVertex3f(-.3, .1 , .1);
	glTexCoord2f(-1.0,0);glVertex3f(-.1,  0 , .1);
	glTexCoord2f(.1,0);glVertex3f( .1,  0 , .1);
	glTexCoord2f(.3,.1);glVertex3f( .3, .1 , .1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, .1);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	glNormal3f(0,0,-1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, -.1);
	glTexCoord2f(-.3,.25);glVertex3f(-.3, .25, -.1);
	glTexCoord2f(-.3,.1);glVertex3f(-.3, .1 , -.1);
	glTexCoord2f(-.1,0);glVertex3f(-.1,  0 , -.1);
	glTexCoord2f(.1,0);glVertex3f( .1,  0 , -.1);
	glTexCoord2f(.3,.1);glVertex3f( .3, .1 , -.1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, -.1);
	glEnd();
	glPopMatrix();
	
}
void terrain(float zmag,int texture)
{
	  int i,j;
	  double z0 = (zmin+zmax)/2;
	  glColor3f(1,1,1);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);
      glBindTexture(GL_TEXTURE_2D,texture);
      for (i=0;i<64;i++)
         for (j=0;j<64;j++)
         {
            float x=16*i-512;
            float y=16*j-512;
            glBegin(GL_QUADS);
            glTexCoord2f((i+0)/64.,(j+0)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+0]-z0)-2.8,y+0);
            glTexCoord2f((i+1)/64.,(j+0)/64.); glVertex3d(x+16,zmag*(z[i+1][j+0]-z0)-2.8,y+0);
            glTexCoord2f((i+1)/64.,(j+1)/64.); glVertex3d(x+16,zmag*(z[i+1][j+1]-z0)-2.8,y+16);
            glTexCoord2f((i+0)/64.,(j+1)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+1]-z0)-2.8,y+16);
            glEnd();
         }
    glDisable(GL_TEXTURE_2D);
}
void tree(double x, double y, double z, double dx, double dy, double dz, double th)
{
	//generate object last due to transperancy property of foilage
	
} 
void Trebuchet(double x, double y,double z,double dx, double dy, double dz, double th)
{ 
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,wood2);
	glPushMatrix();
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glColor3f(1,1,1);
    //base
	cube(0.5,0.0,0.5,2.0,0.1,0.1,0,0);
	cube(0.5,0.0,-0.5,  2.0,0.1,0.1,  0,0);
	//arm support
	cube(1.0,1.1, 0.5,  0.1,1.1,0.1,  0,0);
	cube(1.0,1.1,-0.5,  0.1,1.1,0.1,  0,0);
	
	cube(1.4,0.75, 0.5,  0.09,0.80,0.1,  0,30);
	cube(0.6,0.75, 0.5,  0.09,0.80,0.1,  0,-30);
	cube(1.4,0.75, -0.5,  0.09,0.80,0.1,  0,30);
	cube(0.6,0.75, -0.5,  0.09,0.80,0.1,  0,-30);
	//gear support
	cube(-1.0,0.25, 0.5,  0.1,0.25,0.1,  0,0);
	cube(-1.0,0.25,-0.5,  0.1,0.25,0.1,  0,0);
	//gears
	cylinder(-1.0,0.40,-0.7,.2,.05,0,0);
	cylinder(-1.0,0.40, 0.7,.2,.05,0,0);
	//rotation arm
	cylinder(-1.0,0.40, 0.0,.05,0.7,0,0);
	//arms for gears
	glBindTexture(GL_TEXTURE_2D,metal);
	cylinder(-1.0,0.40,-0.7,.05,.30,90,0);
	cylinder(-1.0,0.40, 0.7,.05,.30,90,0);
	cylinder(-1.0,0.40,-0.7,.05,.30,90,90);
	cylinder(-1.0,0.40, 0.7,.05,.30,90,90);
	glBindTexture(GL_TEXTURE_2D,wood);
	//trebuchet arm
	cylinder(1.0,2.0,0.0,.08,.8,0,0);
	cylinder(1.0,2.0,0.0,.08,1.5,90,90);
	// counterweight
	glBindTexture(GL_TEXTURE_2D,wood2);
	weight(1.0,.1,0,3,3,3,0,0);
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	 
	boundingbox.trebuchetbox[0]=x+dx*(.5); //actual center
	boundingbox.trebuchetbox[1]=y+dy*(1.5);//actual center
	boundingbox.trebuchetbox[2]=z+dz*(0);//actual center
	boundingbox.trebuchetbox[3]=dx*(2.2); 
	boundingbox.trebuchetbox[4]=dy*(2.2); 
	boundingbox.trebuchetbox[5]=dz*(.9); 
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective
   if (mode==0)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  first person view 
   else if(mode==1)
   {
      dirx = +dim*Sin(th)*Cos(ph);
      diry = fmod(+dim*Sin(ph),89);
      dirz = -dim*Cos(th)*Cos(ph);
      gluLookAt(movex,movey,movez , dirx+movex,diry+movey,dirz+movez , 0,Cos(ph),0);
   }
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Sin(zh),ylight,distance*Cos(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        sphere1(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else 
      glDisable(GL_LIGHTING);
		
   //code for rendering scene
   terrain(0.1,land);
   terrain(0.0,water);
   //cylinder(0.5,0.5,0.5,0.1,1.0,0.0,10);
   Trebuchet(0.15,0.15,0.05,.75,.75,.75,0);
   Trebuchet(3.15,0.15,3.05,.75,.75,.75,0);
   Trebuchet(-3.15,0.15,3.05,.75,.75,.75,0);
   //diagnose boundingboxes
   if(showboxes)
   {
      cube(boundingbox.trebuchetbox[0],boundingbox.trebuchetbox[1],boundingbox.trebuchetbox[2],boundingbox.trebuchetbox[3],boundingbox.trebuchetbox[4],boundingbox.trebuchetbox[5],0,0);
   }
   //weight(0,0,0,1,1,1,0,0);
   glDisable(GL_LIGHTING);
   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Disable Z-buffering in OpenGL
   glDisable(GL_DEPTH_TEST);
   //  Draw cockpit
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d, move cords=(%f,%f,%Ff)",th,ph,dim,fov,movex, movey, movez);
   glWindowPos2i(5,25);
   Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim,mode,dirx,diry,dirz,movex,movey,movez,th,ph);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
   {
      movex=2;
      movey=2;
      movez=5;
      th = ph = 0;
   }
   //  Toggle axes
   else if (ch == '1')
      light = fmod(light+1,2);
   else if (ch == '2')
      axes = fmod(axes+1,2);
   //  Switch display mode
  else if (ch == '3')
      mode = fmod(mode+1,2);
  else if (ch == '4')
      movelight = fmod(movelight+1,2);
  else if (ch == '5')
      showboxes = fmod(showboxes+1,2);
  else if (ch == 'q')
      zh -= 5;
  else if (ch == 'Q')
      zh += 5;
  else if (ch == '8' && fov>0)
      fov -= 1;
  else if (ch == '9' && fov<180)
      fov += 1;
  //  Light elevation
  else if (ch=='[')
	  ylight -= 0.1;
  else if (ch==']')
      ylight += 0.1;
  //  Light distance
  else if (ch=='k' && distance >0)
      distance -= 0.1;
  else if (ch=='l')
		distance += 0.1;
  else if (ch == 'Z' && ambient>0)
	ambient-=1;
  else if (ch == 'z' && ambient<100)
	ambient+=1;
  else if (ch == 'X' && diffuse>0)
	diffuse-=1;
  else if (ch == 'x' && diffuse<100)
	diffuse+=1;
  else if (ch == 'C' && specular>0)
	specular-=1;
  else if (ch == 'c' && specular<100)
	specular+=1;
  else if (ch == 'V' && emission>0)
	emission-=1;
  else if (ch == 'v' && emission<100)
	emission+=1;		
  else if (ch=='B' && shininess>-1)
	shininess -= 1;
  else if (ch=='b' && shininess<7)
	shininess += 1;
  else if (ch == 'w' && AABB(movex+.1*dirx,movey+.1*diry+.1*dirx,movez+.1*dirz)== 0)
  {  
     movex= movex+.1*dirx ;
     movey= movey+.1*diry;
     movez= movez+.1*dirz ;
  }
  else if (ch == 's' && AABB(movex-.1*dirx, movey-.1*diry, movez-.1*dirz)== 0)
  {
     movex= movex-.1*dirx ;
     movey= movey-.1*diry ;
     movez= movez-.1*dirz ;
  }
	
  //  Translate shininess power to value (-1 => 0)
  shiny = shininess<0 ? 0 : pow(2.0,shininess);
  //  Reproject
  Project(fov,asp,dim,mode,dirx,diry,dirz,movex,movey,movez,th,ph);
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim,mode,dirx,diry,dirz,movex,movey,movez,th,ph);
}
//void mouse()
//{
 //  if(1){}
//   else if(1){}
//   glutPostRedisplay();
//}
void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/3000.0;
   //  Calculate spin angle 90 degrees/second
   moveth = fmod(90*t,360);
   //moveph = fmod(90*t,360);
   if(movelight==1)
		zh = fmod(90*t,360.0);
   //moveph = fmod(90*t,360);
   //  Request display update
   glutPostRedisplay();
}
/*
 *  Start up GLUT and tell it what to do
 */
void ReadDEM(char* file)
{
   int i,j;
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);
   for (j=0;j<=64;j++)
      for (i=0;i<=64;i++)
      {
         if (fscanf(f,"%f",&z[i][j])!=1) Fatal("Error reading saddleback.dem\n");
         if (z[i][j] < zmin) zmin = z[i][j];
         if (z[i][j] > zmax) zmax = z[i][j];
      }
   fclose(f);
}
int main(int argc,char* argv[])
{
   //initialize struct bounding boxes
   
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Project by Dmitriy Tarasov");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //glutMouseFunc(mouse);
   glutIdleFunc(idle);
   //  Load textures
   land=LoadTexBMP("grass.bmp");
   wood=LoadTexBMP("wood.bmp");
   wood2=LoadTexBMP("wood2.bmp");
   metal=LoadTexBMP("metal.bmp");
   water=LoadTexBMP("water.bmp");
   
   ReadDEM("saddleback.dem");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
