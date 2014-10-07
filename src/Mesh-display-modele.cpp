/*****************************************************************************
File: Mesh-display-modele.cpp

Virtual Humans
Master in Computer Science
Christian Jacquemin, University Paris 11

Copyright (C) 2008 University Paris 11 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef __APPLE__
    #include <limits.h>
#elif __linux
    #include <values.h>
#endif
#include <ctype.h>

#include "texpng.h"
#include <png.h>

#define    KEY_ESC  27

#define    WINDOW_WIDTH   700
#define    WINDOW_HEIGHT  700

#define    NBMAXVERTICES  300000
#define    NBMAXFACES     100000
#define    NBMAXMESH      30

#define    STRINGSIZE 80

enum DisplayType{ SURFACE = 0 , MESH , FULL , EmptyDisplayType };

DisplayType TypeOfSurfaceDisplay = MESH;

// light properties of infinite light sources
static GLfloat ambient_light0[4] = { 0.35 , 0.35 , 0.35 , 1.0 };
static GLfloat diffuse_light0[4] = { 0.7 , 0.7 , 0.7 , 1.0 };
static GLfloat specular_light0[4] = { 0.1 , 0.1 , 0.1 , 1.0 };

static GLfloat ambient_light1[4] = { 0.35 , 0.35 , 0.35 , 1.0 };
static GLfloat diffuse_light1[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
static GLfloat specular_light1[4] = { 0.1 , 0.1 , 0.1 , 1.0 };

static GLfloat ambient_light2[4] = { 0.35 , 0.35 , 0.35 , 1.0 };
static GLfloat diffuse_light2[4] = { 0.50 , 0.50 , 0.50 , 1.0 };
static GLfloat specular_light2[4] = { 0.1 , 0.1 , 0.1 , 1.0 };

// infinite light direction i+k
GLfloat light_position0[4] = { 4.0 , 0.0 , 6.0 , 0.0 };
// infinite light direction -i+k
GLfloat light_position1[4] = { -1.0 , 0.0 , 1.0 , 0.0 };
// infinite light direction j-0.5k
GLfloat light_position2[4] = { 0.0 , 1.0 , -0.5 , 0.0 };

// material properties: silver
float Ambient_silver[4] = {.19225, .19225, .19225, 1.0};
float Diffuse_silver[4] = {.50754, .50754, .50754, 1.0};
float Specular_silver[4] = {.508273, .508273, .508273, 1.0};
float Emission_silver[4] = {.0, .0, .0, .0};
float Shininess_silver = 51.2;

float angle_x = -90, angle_y = 0;
int mouse_pos_x = 0, mouse_pos_y = 0;
int MeshID = -1;

// tiff screenshots
int ScreenShot = false;
int ShotRank = 0;
int MaxRank = 300;

int Trace = false;
float Zoom = 0.3;
char MeshFileName[STRINGSIZE];
char MaterialFileName[STRINGSIZE];

int main(int argc, char **argv);

void parse_mesh_obj( char *fileName );

void displayFace( int indFace );
void make_mesh( void );

void init_scene( void );
void initGL( void );

void window_display( void );
void window_reshape(GLsizei width, GLsizei height); 
void window_key(unsigned char key, int x, int y); 
void window_mouseFunc(int button, int state, int x, int y);
void window_motionFunc(int x, int y);
void window_idle( void ); 
void window_special_key(int key, int x, int y);

void render_scene( void );

class Vector {
public:
  float x, y, z;
  Vector( void ) {
    init();
  };
  ~Vector( void ) {
  };
  void init( void ) {
    x = 0;
    y = 0;
    z = 0;
  };
  void normalize( void ) {
    if( x == 0 && y == 0 && z == 0 ) {
      return;
    }
    float norm = 1.0 / sqrt( x*x + y*y + z*z );
    x *= norm;
    y *= norm;
    z *= norm;
  }
  // 1 vector
  float prodScal( Vector &v2 ) {
    return x * v2.x + y * v2.y + z * v2.z;
  }
  // average
  void averageVectors( Vector *vectors , int nbVectors ) {
    x = 0; y = 0; z = 0;
    if( nbVectors <= 0 ) {
      return;
    }
    for( int ind = 0 ; ind < nbVectors ; ind++ ) {
      x += vectors[ ind ].x;
      y += vectors[ ind ].y;
      z += vectors[ ind ].z;
    }
    float inv = 1.0 / (float)nbVectors;
    x *= inv;
    y *= inv;
    z *= inv;
  }
  void operator*=(double d) {
    x *= d;
    y *= d;
    z *= d;
  }
  void operator+=(Vector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
};

class Point {
public:
  float      x, y, z;
  Point( void ) {
    init();
  };
  ~Point( void ) {
  };
  void init( void ) {
    x = 0;
    y = 0;
    z = 0;
  };
  void operator=(Point& v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }
  void operator*=(double d) {
    x *= d;
    y *= d;
    z *= d;
  }
  void operator+=(Vector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
  void operator*(double f) {
    x = f * x;
    y = f * y;
    z = f * z;
  }
  int operator==(Point& v) {
    return((x == v.x) && (y == v.y) && (z == v.z));
  }
  float distance(Point& p) {
    float dx, dy, dz;
    dx = p.x - x;
    dy = p.y - y;
    dz = p.z - z;
    return sqrt(dx*dx + dy*dy + dz*dz);
  }
};

class Vertex {
public:
  // coordinates
  Point location;
  Vertex( void ) {
    location.init();
  };
  ~Vertex( void ) {
  };
};

class Face {
public:
  int indVertex1; 
  int indVertex2; 
  int indVertex3;
  int indNormal1; 
  int indNormal2; 
  int indNormal3;
  Face( void ) {
    indNormal1 = -1;
    indNormal2 = -1;
    indNormal3 = -1;
    indVertex1 = -1;
    indVertex2 = -1;
    indVertex3 = -1;
  }
  ~Face( void ) {
  };
};

class Mesh {
public:
  char *id;
  char *matId;
  int indFaceIni;
  int indFaceEnd;
  Mesh( void ) {
    id = new char[STRINGSIZE];
    id[0] = 0;
    matId = new char[STRINGSIZE];
    matId[0] = 0;
    indFaceIni = 0;
    indFaceEnd = 0;
  }
  ~Mesh( void ) {
    delete [] id;
    delete [] matId;
  }
};

//////////////////////////////////////////////////////////////////
// MESH TABLES
//////////////////////////////////////////////////////////////////

Vertex   *TabVertices = NULL;
Face     *TabFaces = NULL;
Vector   *TabNormals = NULL;
Mesh     *TabMeshes = NULL;
int      NbMeshes = 0;
int      NbVertices = 0;
int      NbFaces = 0;
int      NbNormals = 0;

//////////////////////////////////////////////////////////////////
// MAIN EVENT LOOP
//////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{
  if( argc >=2 ) {
    strcpy( MeshFileName , argv[1] );
  }
  else {    
    printf( "Mesh file (SimpleShape-Facetted.obj):" );
    fflush( stdin );
    fgets( MeshFileName , STRINGSIZE , stdin );
    if( *MeshFileName == '\n' ) {
      strcpy( MeshFileName , "SimpleShape-Facetted.obj" ); ;
    }
    else {
      MeshFileName[ strlen( MeshFileName ) - 1 ] = 0; 
    }
  }
  printf( "Mesh file (%s)\n" , MeshFileName );

  // GLUT initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // window intialization
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Meshes");

  // OpenGL and scene initialization
  initGL();  
  init_scene();

  // GLUT callbacks
  // for drawing
  glutDisplayFunc(&window_display);
  // for window resize
  glutReshapeFunc(&window_reshape);
  // for keyboard events
  glutKeyboardFunc(&window_key);
  // for mouse clicks
  glutMouseFunc(&window_mouseFunc);
  // for mouse drags
  glutMotionFunc(&window_motionFunc);
  // for special keys
  glutSpecialFunc(&window_special_key);

  if( ScreenShot ) {
    glutIdleFunc(&window_idle);
  }

  // main event loop
  glutMainLoop();  

  return 1;
}

//////////////////////////////////////////////////////////////////
// MESH PARSING
//////////////////////////////////////////////////////////////////

// OBJ file parsing (Alias Wavefront ASCII format)
void parse_mesh_obj( FILE *file )
{
  char    tag[256];
  char    line[256];
  char    ch;
  
  // Two comment lines
  // # Blender3D v244 OBJ File: Anime_Girl.blend
  // # www.blender3d.org
  fgets  ( line, 256, file );
  fgets  ( line, 256, file );
  
  // material name
  fgets  ( line, 256, file );
  sscanf ( line, "%s %s", 
	   tag, 
	   MaterialFileName );
  // printf( "MaterialFileName %s %s\n" , tag , MaterialFileName );
  
  // mesh ID
  fgets  ( line, 256, file );
  sscanf ( line, "%s", tag );
     
  while( strcmp( tag , "o" ) == 0 ) {
  // while( strcmp( tag , "v" ) == 0 ) {
   if( NbMeshes > NBMAXMESH ) {
      printf( "Error: Excessive number of Meshes\n" );
      throw 0;
    }

    // mesh ID
    sscanf ( line, "%s %s", 
    	     tag , 
    	     TabMeshes[ NbMeshes ].id );
    printf( "Mesh ID #%d %s\n" , NbMeshes , TabMeshes[ NbMeshes ].id );
    
    // next tag
    fgets  ( line, 256, file );
    sscanf ( line, "%s", tag );
  
    // Scan for Verts in this mesh
    while( strcmp( tag , "v" ) == 0 ) {
      if( NbVertices > NBMAXVERTICES ) {
	printf( "Error: Excessive number of vertices\n" );
	throw 0;
      }
      sscanf ( line, "%s %f %f %f", 
	       tag,
	       &(TabVertices[NbVertices].location.x),
	       &(TabVertices[NbVertices].location.y),
	       &(TabVertices[NbVertices].location.z) );
      // printf( "vertex %f %f %f\n" , TabVertices[NbVertices].location.x,
      // 	      TabVertices[NbVertices].location.y,
      // 	      TabVertices[NbVertices].location.z );
      NbVertices++;
      fgets  ( line, 256, file );
      sscanf ( line, "%s", tag );
    }
    
    // Scan for Norms in this mesh
    while( strcmp( tag , "vn" ) == 0 ) {
      sscanf ( line, "%s %f %f %f", 
	       tag ,
	       &(TabNormals[NbNormals].x),
	       &(TabNormals[NbNormals].y),
	       &(TabNormals[NbNormals].z) );
      // printf( "normal %f %f %f\n" , TabNormals[NbNormals].x,
      // 	      TabNormals[NbNormals].y,
      // 	      TabNormals[NbNormals].z );
      NbNormals++;
      fgets  ( line, 256, file );
      sscanf ( line, "%s", tag );
    }
    
    // Scan for Mat in this mesh
    if( strcmp( tag , "usemtl" ) == 0 ) {
      sscanf ( line, "%s %s", 
	       tag , 
	       TabMeshes[ NbMeshes ].matId );
      // printf( "Mesh %d mat %s\n" , NbMeshes , TabMeshes[ NbMeshes ].matId );
      fgets  ( line, 256, file );
      sscanf ( line, "%s", tag );
    }
    
    TabMeshes[NbMeshes].indFaceIni = NbFaces;
    // printf( "Mesh #%d face ini %d\n" , NbMeshes , TabMeshes[NbMeshes].indFaceIni );

    // Scan for Faces in this mesh
    while( strcmp( tag , "f" ) == 0 
	   || strcmp( tag , "usemtl" ) == 0
	   || strcmp( tag , "s" ) == 0 ) {
      if( NbFaces > NBMAXFACES ) {
	printf( "Error: Excessive number of faces\n" );
	throw 0;
      }

      // Scan for Mat in this mesh
      // currently only one mat per mesh
      if( strcmp( tag , "usemtl" ) == 0 ) {
	sscanf ( line, "%s", 
		 TabMeshes[ NbMeshes ].matId );
	// printf( "mat %s" , line );
      }
      // Scan for Smooth boolean in this mesh
      else if( strcmp( tag , "s" ) == 0 ) {
	sscanf ( line, "%s", tag );
      }
      // Scan for a Face in this mesh
      else {
	sscanf( line, "%s %d//%d %d//%d %d//%d", 
		tag,
		&(TabFaces[NbFaces].indVertex1),
		&(TabFaces[NbFaces].indNormal1),
		&(TabFaces[NbFaces].indVertex2),
		&(TabFaces[NbFaces].indNormal2),
		&(TabFaces[NbFaces].indVertex3),
		&(TabFaces[NbFaces].indNormal3) );
	// indices start from 1 in OBJ format
	// we make start from 0 for C++
	TabFaces[NbFaces].indVertex1--;
	TabFaces[NbFaces].indVertex2--;
	TabFaces[NbFaces].indVertex3--;
	TabFaces[NbFaces].indNormal1--;
	TabFaces[NbFaces].indNormal2--;
	TabFaces[NbFaces].indNormal3--;
	// printf( "face %d %d %d %d %d %d\n" , 
	//       TabFaces[NbFaces].indVertex1,
	//       TabFaces[NbFaces].indNormal1,
	//       TabFaces[NbFaces].indVertex2,
	//       TabFaces[NbFaces].indNormal2,
	//       TabFaces[NbFaces].indVertex3,
	//       TabFaces[NbFaces].indNormal3 );
	if( TabFaces[NbFaces].indVertex1 >= 0 
	    && TabFaces[NbFaces].indVertex2 >= 0 
	    && TabFaces[NbFaces].indVertex3 >= 0 
	    && TabFaces[NbFaces].indNormal1 >= 0 
	    && TabFaces[NbFaces].indNormal2 >= 0 
	    && TabFaces[NbFaces].indNormal3 >= 0 ) {
	  NbFaces++;
	}
      }

      if( !fgets  ( line, 256, file ) ) {
	TabMeshes[NbMeshes].indFaceEnd = NbFaces;
	printf( "Mesh #%d %s Faces %d-%d\n" , NbMeshes , 
		TabMeshes[ NbMeshes ].id , 
		TabMeshes[ NbMeshes ].indFaceIni ,
		TabMeshes[ NbMeshes ].indFaceEnd );
	NbMeshes++;
	return;
      }
      sscanf ( line, "%s", tag );
    }

    TabMeshes[NbMeshes].indFaceEnd = NbFaces;
    printf( "Mesh #%d %s Faces %d-%d\n" , NbMeshes , 
	    TabMeshes[ NbMeshes ].id , 
	    TabMeshes[ NbMeshes ].indFaceIni ,
	    TabMeshes[ NbMeshes ].indFaceEnd );
    NbMeshes++;
  }
}

//////////////////////////////////////////////////////////////////
// MESH DISPLAY
// TODO
//////////////////////////////////////////////////////////////////

// face display

void displayFace( int indFace ) {
  // TODO
}

// transforms the mesh into a Display List

void make_mesh( void ) {
  glNewList(MeshID, GL_COMPILE);  
    for( int ind = 0 ; ind < NbMeshes ; ind++ ) {
      // displays wireframe + surface
      if( TypeOfSurfaceDisplay == FULL ) {
	// TODO
      }
      // displays wireframe
      else if( TypeOfSurfaceDisplay == MESH ) {
	// TODO
      }
      // displays surface
      else {
	// TODO
      }
    }
  glEndList();
}

//////////////////////////////////////////////////////////////////
// INTIALIZATIONS
//////////////////////////////////////////////////////////////////

// OpenGL intialization

GLvoid initGL( void ) 
{
  // 3 light sources
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  glLightfv(GL_LIGHT2, GL_AMBIENT, ambient_light2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse_light2);
  glLightfv(GL_LIGHT2, GL_SPECULAR, specular_light2);
  glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

  // Gouraud shading
  glShadeModel( GL_SMOOTH );

  // two side surface lighting
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 

  // BG color
  glClearColor( 1, 1, 1, 1 );  
      
  // z-buffer
  glEnable(GL_DEPTH_TEST);

  // lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);

  // automatic normal normalization
  glEnable(GL_NORMALIZE);
}

// scene initialization

void init_scene( void )
{
  TabVertices = new Vertex[ NBMAXVERTICES ];
  TabFaces = new Face[ NBMAXFACES ];
  TabNormals = new Vector[ NBMAXVERTICES ];
  TabMeshes = new Mesh[ NBMAXMESH ];
  
  NbMeshes = 0;
  NbVertices = 0;
  NbFaces = 0;
  NbNormals = 0;

  // parses the mesh (obj format)
  FILE * fileMesh = fopen( MeshFileName , "r" ); 
  if( !fileMesh ) {
    printf( "File %s not found\n" , MeshFileName );
    exit(0);
  }
  parse_mesh_obj( fileMesh );
  fclose( fileMesh );

  // Display List compiling
  MeshID =  glGenLists(1);
  make_mesh();

}

//////////////////////////////////////////////////////////////////
// GLUT CALL-BACKS
//////////////////////////////////////////////////////////////////

// glut call-back: window display

GLvoid window_display( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glScalef( Zoom , Zoom , Zoom );

  render_scene();

  glFlush();
}

// glut call-back: window resize

GLvoid window_reshape(GLsizei width, GLsizei height)
{  
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.2, 1.2, -1.2, 1.2, -10.2, 10.2);

  // model view drawing 
  glMatrixMode(GL_MODELVIEW);
}

// glut call-back: keyboard events

GLvoid window_key(unsigned char key, int x, int y) 
{  
  switch (key) {    
  case '<':  
    Zoom /= 1.1;
    glutPostRedisplay();
    break; 
  case '>':  
    Zoom *= 1.1;
    glutPostRedisplay();
    break; 
  case 'm':  
  case 'M':
    if( TypeOfSurfaceDisplay != MESH ) {
      TypeOfSurfaceDisplay = MESH;  
      make_mesh();
      glutPostRedisplay();
    }
    break; 
  case 's':  
  case 'S':
    if( TypeOfSurfaceDisplay != SURFACE ) {
      TypeOfSurfaceDisplay = SURFACE;  
      make_mesh();
      glutPostRedisplay();
    }
    break; 
  case 'f':  
  case 'F':
    if( TypeOfSurfaceDisplay != FULL ) {
      TypeOfSurfaceDisplay = FULL;  
      make_mesh();
      glutPostRedisplay();
    }
    break; 
  case KEY_ESC:  
    exit(1);                    
    break; 
  default:
    printf ("La touche %d n'est pas active.\n", key);
    break;
  }     
}

// glut call-back: mouse click events

GLvoid window_mouseFunc(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
    mouse_pos_x = x;
    mouse_pos_y = y;
  }
}

// glut call-back: mouse drag events

GLvoid window_motionFunc(int x, int y)
{
  angle_x += y - mouse_pos_y;
  angle_y += x - mouse_pos_x;

  mouse_pos_x = x;
  mouse_pos_y = y;

  glutPostRedisplay();
}

// glut call-back: update function called at each frame

GLvoid window_idle( void ) 
{
  angle_x += 1.5;
  angle_y += 1.5;

  glutPostRedisplay();

  // file output
  if( ScreenShot ) {
    char filename[256];
    char description[256];

    glReadBuffer(GL_BACK);
    sprintf( filename , "tmp/tmp%d.png" , ShotRank );
    sprintf( description , "PNG rendering" , ShotRank );
    writepng(filename, description, 0,0,
	     WINDOW_WIDTH,WINDOW_HEIGHT);
    fprintf( stdout , "%s\n" , description );
    ShotRank++;
    if( ShotRank > MaxRank ) {
      ScreenShot = false;
      glutIdleFunc(NULL);
    }
  }
}

// glut call-back: special key processing

void window_special_key(int key, int x, int y)
{
  switch (key) {    
  default:
    printf ("special key %d is not active.\n", key);
    break;
  }     
}

//////////////////////////////////////////////////////////////////
// INTERACTIVE SCENE RENDERING
//////////////////////////////////////////////////////////////////

void render_scene( void )
{
  glPushMatrix();
  glRotatef(angle_x, 1, 0, 0);
  glRotatef(angle_y, 0, 0, 1);

  // axes xx yy & zz
  glEnable( GL_COLOR_MATERIAL );
  glColor3f( 1.0 , 0.0 , 0.0 );
  // glBegin( GL_LINES );
  // glVertex3f( -10.0 , 0.0 , 0.0 );
  // glVertex3f( 10.0 , 0.0 , 0.0 );
  // glVertex3f( 0.0 , -10.0 , 0.0 );
  // glVertex3f( 0.0 , 10.0 , 0.0 );
  // glVertex3f( 0.0 , 0.0 , -10.0 );
  // glVertex3f( 0.0 , 0.0 , 10.0 );
  // glEnd();

  glTranslatef(1.438, 2.702, -2);
  glCallList(MeshID);

  glPopMatrix();

  glutSwapBuffers();
}

