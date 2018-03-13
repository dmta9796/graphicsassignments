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
double dim=2.0;   //  Size of world
double moveth=0;// angle for the planar movement of rocket
double moveph=0;// angle for the spherical cords of rocket
int mode=1;     // model archtiype/ model scene
char* pmodestr="Othogonal";
double dirx=0;
double diry=0;
double dirz=0;
double movex=0;
double movey=0;
double movez=0;
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
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}
static void sphere1(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

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
void fin1()
{
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.1,0.0,-0.2);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.2,0.0,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.2,0.0,-0.6);
	glVertex3d(0.2,0.0,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.2,0.0,-0.4);
	glEnd();
}
void fin2()
{
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0,-0.1,-0.2);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(0.0,-0.2,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0,-0.2,-0.6);
	glVertex3d(0.0,-0.2,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.0,-0.2,-0.4);
	glEnd();
}
void fin3()
{
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(-0.1,0.0,-0.2);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(-0.2,0.0,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(-0.2,0.0,-0.6);
	glVertex3d(-0.2,0.0,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(-0.2,0.0,-0.4);
	glEnd();
}
void fin4()
{
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0,0.1,-0.2);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.0,0.2,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0,0.2,-0.6);
	glVertex3d(0.0,0.2,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(0.0,0.2,-0.4);
	glEnd();
}
void rocketbody()
{
	glColor3f(1,0,1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0,0.0,0.75);
	glVertex3d(0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,0.4);
	glVertex3d(-0.1,-0.1,0.4);
	glVertex3d(0.1,-0.1,0.4);
	glVertex3d(0.1,0.1,0.4);
	glEnd();
	glColor3f(0,1,1);
	glBegin(GL_POLYGON);
	glVertex3d(0.1,0.1,0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,0.4);
	glEnd();
	glColor3f(0,1,1);
	glBegin(GL_POLYGON);
	glVertex3d(-0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,-0.1,0.4);
	glEnd();
	glColor3f(0,1,1);
	glBegin(GL_POLYGON);
	glVertex3d(-0.1,-0.1,0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(0.1,-0.1,0.4);
	glEnd();
	glColor3f(0,1,1);
	glBegin(GL_POLYGON);
	glVertex3d(-0.1,0.1,0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,0.1,0.4);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3d(0.1,0.1,-0.4);
	glVertex3d(0.1,-0.1,-0.4);
	glVertex3d(-0.1,-0.1,-0.4);
	glVertex3d(-0.1,0.1,-0.4);
	glEnd();
}
void rocket()
{
	rocketbody();
	fin1();
	fin2();
	fin3();
	fin4();
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
	  //double Ex = +dim*Sin(th);
      //double Ey = -dim*Cos(th);
      //double Ez = +dim*Cos(ph);
      dirx = +dim*Sin(th)*Cos(ph);
      diry = fmod(+dim*Sin(ph),89);
      dirz = -dim*Cos(th)*Cos(ph);
      gluLookAt(movex,movey,movez , dirx+movex,diry+movey,dirz+movez , 0,Cos(ph),0);
   }
   //  Draw 10 pixel yellow points
   //glPointSize(10);
   if(mode==0)
   {
	   glPushMatrix();
	   glTranslatef(.2,0,.2);
	   rocket();
	   glPopMatrix();
   }
   if(mode==1)
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
	   glPopMatrix();
	}
   //  Draw axes in white
   glColor3f(1,1,1);
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
   //  Display parameters
   glWindowPos2i(5,5);
   if(pmode==0){pmodestr="Orthogonal";}
   else if(pmode==1){pmodestr="Perspective";}
   else{pmodestr="first person";}
   Print("View Angle=%d,%d,  dim=%.1f, fov=%d, type=%s",th,ph,dim,fov,pmodestr);
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
   else if (ch == '+')
   {
	   dim += 0.1;
   }
   //  Decrease w by 0.1
   else if (ch == '-' && dim>0)
   {
	   dim -= 0.1;
   }
   else if (ch == 'x' && fov<180)
   {
	   fov += 1;
   }
   //  Decrease w by 0.1
   else if (ch == 'z' && fov>1)
   {
	   fov -= 1;
   }
   else if (ch == 'm')
   {
	   mode = fmod(mode+1,2) ;
   }
   else if (ch == 'p')
   {
	   pmode = fmod(pmode+1,3) ;
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
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("HW4: Perspective and First Person by Dmitri Tarasov");
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
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
