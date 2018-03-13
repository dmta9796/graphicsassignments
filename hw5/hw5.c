/*
 *  Coordinates
 *
 *  Display 2, 3 and 4 dimensional coordinates in 3D.
 *
 *  Key bindings:
 *  +/-    Increase/decrease z or w
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int axes=0;       //  Display axes
int pmode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=4.0;   //  Size of world
double moveth=0;// angle for the planar movement of rocket
double moveph=0;// angle for the spherical cords of rocket
int mode=0;     // model archtiype/ model scene
char* pmodestr="Othogonal";
double dirx=0;
double diry=0;
double dirz=0;
double movex=0;
double movey=0;
double movez=0;

//Lighing Parameters
int diffuse=50;
int specular=20;
int ambient=10;
int emission=30;
int light=1; //enable or disable light
double zh=0; //lighting angle of rotation
double distance  =   2;  // Light distance
float ylight  =   0;  // Elevation of light
int local     =   0;  // Local Viewer Model
float shiny   =   1;  // Shininess (value)
int shininess =   3;  // Shininess (power of two)
int movelight =   1;  // moving light source

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}
/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (pmode==0)
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);

   //  Orthogonal projection
   else
      gluPerspective(fov,asp,dim/10,10*dim); //first person navigation implies perspective
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}
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
static void sphere2(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   float blue[] = {0,0,0.0,1.0};
   float Emission[]  = {0.01*emission,0.005*emission,0.0,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  yellow ball
   //glColor3f(0,0,1);
   glBlendColor(0,0,1,1.0);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,blue);
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
void fin()
{
	double n1[3]={0,0,0};
	double n2[3]={0,0,0};
	double n3[3]={0,0,0};
	double n4[3]={0,0,0};
	double n5[3]={0,0,0};
	double p1[3]={0.1,0.0,-0.2};
	double p2[3]={0.1,-0.1,-0.4};
	double p3[3]={0.2,0.0,-0.4};
	double p4[3]={0.1,0.1,-0.4};
	double p5[3]={0.2,0.0,-0.6};
	normalplane(p2,p3,p1,n1);
	normalplane(p3,p4,p1,n2);
	normalplane(p4,p3,p5,n3);
	normalplane(p2,p4,p5,n4);
	normalplane(p3,p2,p5,n5);
	//print statements to check normals
	//printf("n%d is x:%.2f, y:%.2f, z:%.2f\n",1,n1[0],n1[1],n1[2]);
	//printf("n%d is x:%.2f, y:%.2f, z:%.2f\n",2,n2[0],n2[1],n2[2]);
	//printf("n%d is x:%.2f, y:%.2f, z:%.2f\n",3,n3[0],n3[1],n3[2]);
	//printf("n%d is x:%.2f, y:%.2f, z:%.2f\n",4,n4[0],n4[1],n4[2]);
	//printf("n%d is x:%.2f, y:%.2f, z:%.2f\n",5,n5[0],n5[1],n5[2]);
	
	glBegin(GL_TRIANGLES);	
	glColor3f(0,1,0);
	glNormal3f(n1[0],n1[1],n1[2]);
	glVertex3d(0.1,0.0,-0.2);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.2,0.0,-0.4);
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3f(0,1,0);
	glNormal3d(n2[0],n2[1],n2[2]);
	glVertex3d(0.1,0.0,-0.2);
	glVertex3d(0.2,0.0,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glEnd();
	
	//code to check normal vector correctness
	//glBegin(GL_LINES);
	//glColor3f(1,0,0);
	//glVertex3d(p3[0],p3[1],p3[2]);
	//glVertex3d(p3[0]+10*n5[0],p3[1]+10*n5[1],p3[2]+10*n5[2]);
	//glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3f(0,1,0);
	glNormal3d(n3[0],n3[1],n3[2]);
	glVertex3d(0.2,0.0,-0.6);
	glVertex3d(0.2,0.0,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glEnd();
	//interior back fin part
	glBegin(GL_TRIANGLES);
	glColor3f(0,1,0);
	glNormal3d(n4[0],n4[1],n4[2]);
	glVertex3d(0.2,0.0,-0.6);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glEnd();
	//far outside left fin part
	glBegin(GL_TRIANGLES);
	glColor3f(0,1,0);
	glNormal3d(n5[0],n5[1],n5[2]);
	glVertex3d(0.2,0.0,-0.6);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.2,0.0,-0.4);
	glEnd();
}
void front()
{
	double n[3]={0,0,0};
	double p1[3]={0.0,0.0,0.75};
	double p2[3]={0.1,0.1,0.4};
	double p3[3]={-0.1,0.1,0.4};
	normalplane(p1,p2,p3,n);
	glBegin(GL_TRIANGLES);
	glColor3f(1,0,1);
	glNormal3d(n[0],n[1],n[2]);
	glVertex3d(0.0,0.0,0.75);
	glVertex3d(0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,0.4);
	glEnd();
}
void rocketbody()
{	
	//side 
	glBegin(GL_POLYGON);
	glColor3f(0,1,1);
	glNormal3d(1,0,0);
	glVertex3d(0.1,0.1,0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,0.4);
	glEnd();
	//side 2
	glBegin(GL_POLYGON);
	glColor3f(0,1,1);
	glNormal3d(-1,0,0);
	glVertex3d(-0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,-0.1,0.4);
	glEnd();
	//bottom
	glBegin(GL_POLYGON);
	glColor3f(0,1,1);
	glNormal3d(0,-1,0);
	glVertex3d(-0.1,-0.1,0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,0.4);
	glEnd();
	//top
	glBegin(GL_POLYGON);
	glColor3f(0,1,1);
	glNormal3d(0,1,0);
	glVertex3d(-0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,0.1,0.4);
	glEnd();
	//back
	glBegin(GL_POLYGON);
	glColor3f(0,1,1);
	glNormal3d(0,0,-1);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glEnd();
}
void rocket()
{
    //Setspecular color to white
	float white[] = {1,1,1,1};
	//Set emmision to black
	float black[] = {0,0,0,1};
	//  Save transformation
	glPushMatrix();
	glMaterialf(GL_FRONT,GL_SHININESS,shiny);
	glMaterialfv(GL_FRONT,GL_SPECULAR,white);
	glMaterialfv(GL_FRONT,GL_EMISSION,black);
	rocketbody();
	fin();
	front();
	glPushMatrix();
	glRotatef(90,0,0,1);
	fin();
	front();
	glPopMatrix();
	glPushMatrix();
	glRotatef(180,0,0,1);
	fin();
	front();
	glPopMatrix();
	glPushMatrix();
	glRotatef(270,0,0,1);
	fin();
	front();
	glPopMatrix();
}
void spaceplane()
{
	//Setspecular color to white
	float white[] = {1,1,1,1};
	//Set emmision to black
	float black[] = {0,0,0,1};
	//  Save transformation
	glPushMatrix();
	glMaterialf(GL_FRONT,GL_SHININESS,shiny);
	glMaterialfv(GL_FRONT,GL_SPECULAR,white);
	glMaterialfv(GL_FRONT,GL_EMISSION,black);
	rocketbody();
	glPushMatrix();
	glScalef(10,1,3);
	glTranslatef(-.09,0,.275);
	fin();
	glPopMatrix();
	glPushMatrix();
	front();
	glPopMatrix();
	glPushMatrix();
	glRotatef(90,0,0,1);
	fin();
	front();
	glPopMatrix();
	glPushMatrix();
	glScalef(10,1,3);
	glRotatef(180,0,0,1);
	glTranslatef(-.09,0,.275);
	fin();
	glPopMatrix();
	glPushMatrix();
	glRotatef(180,0,0,1);
	front();
	glPopMatrix();
	glPushMatrix();
	glRotatef(270,0,0,1);
	front();
	glPopMatrix();
}
void scenerender()
{
	   glPushMatrix();
	   glTranslatef(2*Cos(moveth),0,2*Sin(moveth));
	   glRotatef(moveth,0,-1,0);
	   glRotatef(8*moveth,0,0,1);
	   glScalef(.5,.5,.75);
	   rocket();
	   glPopMatrix();
	   glPushMatrix();
	   glTranslatef(1.5*Cos(moveth),0,-1.5*Sin(moveth));
	   glRotatef(180+moveth,0,1,0);
	   glScalef(.5,.5,.3);
	   rocket();
	   glPopMatrix();
	   glPushMatrix();
	   glTranslatef(1.2*Cos(moveth),2*Sin(moveth),0);
	   glRotatef(moveth,0,0,1);
	   glRotatef(90,-1,0,0);
	   glScalef(.3,.3,.2);
	   rocket();
	   glPopMatrix();
	   glPushMatrix();
	   glTranslatef(0,0,1.2*Sin(moveth));
	   glTranslatef(1.2*Cos(moveth),1.2*Sin(moveth),0);
	   glRotatef(moveth,0,-1,1);
	   glRotatef(90,0,-1,0);
	   glScalef(.5,.5,.1);
	   rocket();
	   glPopMatrix();
	   glPushMatrix();
	   glRotatef(moveth,0,1,0);
	   sphere1(0,0,0,1);
	   sphere2(0,0,0,1.2);
	   glPopMatrix();
}
/*
 *  Display the scene
 */
void display()
{
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   if(pmode==0)
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   if (pmode==1)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else if(pmode==2)
   {
      dirx = +dim*Sin(th)*Cos(ph);
      diry = fmod(+dim*Sin(ph),89);
      dirz = -dim*Cos(th)*Cos(ph);
      gluLookAt(movex,movey,movez , dirx+movex,diry+movey,dirz+movez , 0,Cos(ph),0);
   }
      //  Light switch
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
   //  Draw 10 pixel yellow points
   //glPointSize(10);
   if(mode==0)
   {
	   glPushMatrix();
	   glTranslatef(.2,0,.2);
	   spaceplane();
	   glPopMatrix();
   }
   if(mode==1)
   {
	   glPushMatrix();
	   glTranslatef(.2,0,.2);
	   rocket();
	   glPopMatrix();
   }
   if(mode==2)
   {
		scenerender();
	}
   //don't want lighting on axes
   glDisable(GL_LIGHTING);
   //  Draw axes in white
   glColor3f(1,1,1);
   if(axes)
   {
	   glBegin(GL_LINES);
	   glVertex3d(-2,0,0);
	   glVertex3d(2,0,0);
	   glVertex3d(0,-2,0);
	   glVertex3d(0,2,0);
	   glVertex3d(0,0,-2);
	   glVertex3d(0,0,2);
	   glEnd();
	   //  Label axes
	   glRasterPos3d(2,0,0);
	   Print("X");
	   glRasterPos3d(0,2,0);
	   Print("Y");
	   glRasterPos3d(0,0,2);
	   Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   if(pmode==0){pmodestr="Orthogonal";}
   else if(pmode==1){pmodestr="Perspective";}
   else{pmodestr="first person";}
   Print("View Angle=%d,%d,  dim=%.1f, fov=%d, type=%s",th,ph,dim,fov,pmodestr);
   glWindowPos2i(5,25);
   Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   //  Flush and swap
   glFlush();
   glutSwapBuffers();
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
      th = ph = 0;
      movex=movey=movez=0;
   }
   //  Increase dim by 0.1
   else if (ch == ':')
   {
	   dim += 0.1;
   }
   //  Decrease w by 0.1
   else if (ch == ';' && dim>0)
   {
	   dim -= 0.1;
   }
   else if (ch == '+' && fov<180)
   {
	   fov += 1;
   }
   //  Decrease w by 0.1
   else if (ch == '-' && fov>1)
   {
	   fov -= 1;
   }
   else if (ch == '1')
   {
	   mode = fmod(mode+1,3) ;
   }
   else if (ch == '2')
   {
	   pmode = fmod(pmode+1,3) ;
   }
   else if (ch == '3')
   {
	   light = fmod(light+1,2) ;
   }
   else if (ch == '4')
   {
	   movelight = fmod(movelight+1,2) ;
   }
   else if (ch == '5')
   {
	   axes = fmod(axes+1,2) ;
   }
   else if (ch == 'w')
   {
	   movex= movex+.1*dirx ;
	   movey= movey+.1*diry;
	   movez= movez+.1*dirz ;
   }
   else if (ch == 's')
   {
	   movex= movex-.1*dirx ;
	   movey= movey-.1*diry;
	   movez= movez-.1*dirz ;
   }
      else if (ch == 'V' && ambient>0)
		ambient-=1;
      else if (ch == 'v' && ambient<100)
		ambient+=1;
	  else if (ch == 'B' && diffuse>0)
		diffuse-=1;
      else if (ch == 'b' && diffuse<100)
		diffuse+=1;
	  else if (ch == 'N' && specular>0)
		specular-=1;
      else if (ch == 'n' && specular<100)
		specular+=1;
	  else if (ch == 'M' && emission>0)
		emission-=1;
      else if (ch == 'm' && emission<100)
		emission+=1;
	  else if(ch=='z')
		zh+=5;
	  else if(ch=='Z')
		zh-=5;
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
	  else if (ch=='g' && shininess>-1)
		shininess -= 1;
      else if (ch=='G' && shininess<7)
		shininess += 1;
	  //  Translate shininess power to value (-1 => 0)
	  shiny = shininess<0 ? 0 : pow(2.0,shininess);
			
   //  Tell GLUT it is necessary to redisplay the scene
   Project();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   Project();
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
   Project();
}
void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/3000.0;
   //  Calculate spin angle 90 degrees/second
   moveth = fmod(90*t,360);
   moveph = fmod(90*t,360);
   if(movelight==1)
		zh = fmod(90*t,360.0);
   //moveph = fmod(90*t,360);
   //  Request display update
   glutPostRedisplay();
}
/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(800,800);
   //  Create the window
   glutCreateWindow("HW5: lighting by Dmitri Tarasov");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glEnable(GL_DEPTH_TEST);
   glEnable (GL_BLEND); 
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
