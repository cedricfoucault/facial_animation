/*****************************************************************************
File: KP-anim-modele.cpp

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
#include <stdbool.h>

#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//#define socklen_t int
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <wspiapi.h>
#endif

#include <pthread.h>

#define    KEY_ESC  27
#define    PI       3.1415926535898

#define    WINDOW_WIDTH   700
#define    WINDOW_HEIGHT  700

#define    NBMAXVERTICES  300000
#define    NBMAXFACES     100000
#define    NBMAXMESH      30
#define    NBMAXKP        100

#define    MAXKPWEIGHTS   4

#define    STRINGSIZE 80

#define ANIM_DURATION_INCREMENT 0.25
#define NB_ANIM_KPS 30
#define NB_ANIM_KFS 8

char tab_ID[NB_ANIM_KPS][16] = { "4.1" , "4.2" , "4.3" , "4.4" , "4.5" , "4.6" , "3.1" , "3.2" , "3.3" , "3.4" , "3.7" , "3.8" , "3.11" , "3.12" , "5.3" , "5.4" , "9.13" , "9.14" , "9.5" , "9.4" , "8.1" , "8.2" , "8.3" , "8.4" , "8.5" , "8.6" , "8.7" , "8.8" , "2.10" , "2.1" };
char tab_emotion[NB_ANIM_KFS][16] = {"neutral", "happiness", "sadness", "surprise", "anger", "disgust", "fear", "default"};
float tab_KF_x[NB_ANIM_KPS][NB_ANIM_KFS] = { 
    { 0 , 0 , 0 , 0 , -0.04 , 0 , -0.078 }  , 
    { 0 , 0 , 0 , 0 , 0.04 , 0 , 0.078 }  , 
    { 0 , 0 , -0.04 , 0 , -0.1 , 0 , 0 }  , 
    { 0 , 0 , 0.04 , 0 , -0.1 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 }  , 
    { 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , -0.002 , 0 , 0.04 , -0.035 }  , 
    { 0 , 0 , 0 , 0 , -0.04 , 0.035 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0.193 , 0 , 0 , -0.047 , 0 , 0.06 }  , 
    { 0 , -0.193 , 0 , 0 , 0.05 , 0 , -0.06 }  , 
    { 0 , 0.025 , 0 , 0 , 0 , 0 }  , 
    { 0 , -0.025 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 } 
 }; 
float tab_KF_y[NB_ANIM_KPS][NB_ANIM_KFS] = { 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 }  , 
    { 0 , 0 }  , 
    { 0 , 0 }  , 
    { 0 , 0 , 0 }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    { 0 , 0 }  , 
    { 0 , 0 }  , 
    {  }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    {  }  , 
    {  }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0.006 , 0 , 0 }  , 
    { 0 , 0.03 , 0 }  , 
    { 0 , 0.03 }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0.002 , 0 }  , 
    { 0 , 0 , 0.002 }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0 } 
 };
float tab_KF_z[NB_ANIM_KPS][NB_ANIM_KFS] = { 
    { 0 , 0 , 0.07 , 0.12 , -0.108 , -0.07 , 0.13 }  , 
    { 0 , 0 , 0.07 , 0.12 , -0.108 , -0.07 , 0.13 }  , 
    { 0 , 0 , 0 , 0.08 , -0.1 , -0.11 , 0.03 }  , 
    { 0 , 0 , 0 , 0.08 , -0.1 , -0.11 , 0.03 }  , 
    { 0 , 0.032 , 0 , 0.04 , 0.06 , -0.056 }  , 
    { 0 , 0.032 , 0 , 0.04 , 0.06 , -0.056 }  , 
    { 0 , -0.025 , -0.036 , 0.05 , -0.01 , -0.035 , 0.042 }  , 
    { 0 , -0.025 , -0.036 , 0.05 , -0.01 , -0.035 , 0.042 }  , 
    { 0 , 0.025 , 0.035 , 0 , 0.044 , 0.055 , 0.035 }  , 
    { 0 , 0.025 , 0.035 , 0 , 0.044 , 0.055 , 0.035 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    {  }  , 
    {  }  , 
    {  }  , 
    { 0 , 0.06 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0.06 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { 0 , 0 , 0 }  , 
    { 0 , 0 , 0 , 0 , 0 , 0 }  , 
    { -0.05 , -0.03 , -0.03 , 0 , -0.06 , -0.02 }  , 
    { 0.15 , 0 , 0.105 , -0.11 , -0.02 , 0.183 , 0.064 }  , 
    { 0.05 , 0.135 , -0.085 , 0 , 0 , 0.1 , 0 }  , 
    { 0.05 , 0.135 , -0.084 , 0 , 0 , 0.1 , 0 }  , 
    { 0 , 0 , 0 , 0 , -0.01 , 0 }  , 
    { 0 , 0 , 0 , 0 , -0.01 }  , 
    { 0.15 , 0 , 0.06 , -0.102 , 0 , 0.17 , 0.04 }  , 
    { 0.15 , 0 , 0.06 , -0.102 , 0 , 0.17 , 0.04 }  , 
    { 0 , 0 , 0.1 , -0.03 , -0.018 , 0.03 }  , 
    { 0 , 0 , 0.008 , -0.03 , -0.018 , 0.055 } 
 };

enum DisplayType{ SURFACE = 0 , MESH , EmptyDisplayType };
enum WeightingType{ NoWeighting = 0 , Weighting , WeightSubstitution };

DisplayType TypeOfSurfaceDisplay = SURFACE;

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


int Trace = false;
float Zoom = 0.3;
char MeshFileName[STRINGSIZE];
char KPFileName[STRINGSIZE];
char MaterialFileName[STRINGSIZE];

int CurrentActiveKeyPoint = 0; // legacy

// local server socket
int SocketToLocalServer = -1;

// global variables for weighting
float linear_weight( float distance, float radius, int exponent );
float inverse_distance_weight(float distance, float radius, int exponent);
const float RADIUS = 0.3;
float (*weighting_scheme)(float distance , float radius, int exponent) = inverse_distance_weight;

// global variables for animation
typedef enum {LINEAR, EASEINOUTQUAD} curve_id_t;
bool isAnimating = false;
int fromIndKF = NB_ANIM_KFS, toIndKF = NB_ANIM_KFS;
struct timeval tv0;
float timeElapsed, animDuration = 1.0;
curve_id_t curveID = EASEINOUTQUAD;
float (*curve)(float elapsed, float duration, float startVal, float endVal);

int main(int argc, char **argv);

void parse_mesh_obj( char *fileName );
void parse_KP_obj( char *fileName );
void locate_KP_in_mesh( void );
WeightingType weight_one_vertex( int indVertex , int indKP ,
				 float radius , int exponent , 
				 float (*pt2Function)(float,float,int) );
void weight_vertices_on_KP_in_mesh( float radius , int exponent  , 
				    float (*pt2Function)(float,float,int) );

void animate_vertices_in_mesh( void );

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

void init_client();
void main_client();
float curve_ease_inout_quad(float elapsed, float duration, float startVal, float endVal);
float curve_linear(float elapsed, float duration, float startVal, float endVal);
void init_time_counter();
void init_animation(int indKF);
void update_time_counter();
void update_keypoints();
void update_vertices();
void animate_frame();
void idle_func();



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
  // initial coordinates
  Point location;
  // coordinates after KeyPoint transformations
  Point curLocation;
  // 4 weights on keypoints (other vertices in the mesh)
  float      wKP[MAXKPWEIGHTS];
  // 4 indices of the weighted keypoints (other vertices in the mesh)
  int        indKP[MAXKPWEIGHTS];
  // boolean working variable to memorize weighting
  bool       weighted;

  Vertex( void ) {
    location.init();
    curLocation.init();
    for( int ind = 0 ; ind < MAXKPWEIGHTS ; ind++ ) {
      wKP[ind] = 0.0;
      indKP[ind] = -1;
    }
    weighted = false;
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

class KP {
public:
  // keypoint ID (also reported in the vertex)
  char *id;
  // initial coordinates
  Point location;
  //  index of the mesh
  int indMesh;
  //  index of vertex
  int indVertex;
  // current translation
  Vector translation;

  KP( void ) {
    id = new char[STRINGSIZE];
    location.init();
    indMesh = -1;
    indVertex = -1;
    translation.init();
  }
  ~KP( void ) {
    delete [] id;
  }
};

KP *keypoint_with_id(char *id);

class Mesh {
public:
  char *id;
  char *matId;
  int indFaceIni;
  int indFaceEnd;
  int nbKPs;
  Mesh( void ) {
    id = new char[STRINGSIZE];
    id[0] = 0;
    matId = new char[STRINGSIZE];
    matId[0] = 0;
    indFaceIni = 0;
    indFaceEnd = 0;
    nbKPs = 0;
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
KP       *TabKPs = NULL;
int      NbMeshes = 0;
int      NbVertices = 0;
int      NbFaces = 0;
int      NbNormals = 0;
int      NbKPs = 0;

//////////////////////////////////////////////////////////////////
// MAIN EVENT LOOP
//////////////////////////////////////////////////////////////////
int main(int argc, char **argv) 
{
  if( argc >=2 ) {
    strcpy( MeshFileName , argv[1] );
  }
  else {    
    printf( "Mesh file (head_modified.obj):" );
    fflush( stdin );
    fgets( MeshFileName , STRINGSIZE , stdin );
    if( *MeshFileName == '\n' ) {
      strcpy( MeshFileName , "head_modified.obj" ); ;
    }
    else {
      MeshFileName[ strlen( MeshFileName ) - 1 ] = 0; 
    }
  }
  strcpy( KPFileName , MeshFileName ); ;
  KPFileName[ strlen( KPFileName ) - 4 ] = 0; 
  strcat( KPFileName , "_KP.obj" );

  printf( "Mesh file (%s)\n" , MeshFileName );
  printf( "KP file (%s)\n" , KPFileName );

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
  // glutSpecialFunc(&window_special_key);
  
  // add client idle func (checks the socket to translate keypoints)
  // init_client();
  // glutIdleFunc(main_client);
  // use idle func to run facial animation
  glutIdleFunc(idle_func);
  
  // main event loop
  glutMainLoop();

  return 1;
}

//////////////////////////////////////////////////////////////////
// MESH AND KEYPOINT FILE PARSING
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
    if( NbMeshes > NBMAXMESH ) {
      printf( "Error: Excessive number of Meshes\n" );
      throw 0;
    }

    // mesh ID
    sscanf ( line, "%s %s", 
	     tag , 
	     TabMeshes[ NbMeshes ].id );
    // printf( "Mesh ID #%d %s\n" , NbMeshes , TabMeshes[ NbMeshes ].id );
    
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
      TabVertices[NbVertices].curLocation = TabVertices[NbVertices].location;
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

// key-point OBJ file parsing 
// (inhouse format inspired from the Alias Wavefront ASCII format)

void parse_KP_obj( FILE *file )
{
  char    tag[256];
  char    ignore[256];
  int     ignore1;
  int     ignore2;
  int     ignore3;
  char    line[256];
  char    meshID[256];
  int     indMesh;
  char    ch;
  
  // Two comment lines
  // # Anim_Girl Facial animation keypoints
  // # 
  fgets  ( line, 256, file );
  fgets  ( line, 256, file );
  
  // mesh ID
  fgets  ( line, 256, file );
  sscanf ( line, "%s", tag );
    
  NbKPs = 0;
  while( strcmp( tag , "o" ) == 0 ) {
    // mesh ID
    sscanf ( line, "%s %s", 
	     tag , meshID );

    // finds the index of the mesh associated with this KP
    indMesh = -1;
    for( int ind = 0 ; ind < NbMeshes ; ind++ ) {
      if( strcmp( meshID , TabMeshes[ ind ].id ) == 0 ) {
	indMesh = ind;
	// printf( "Mesh #%d ID %s\n" , ind , meshID );
      }
    }
    if( indMesh == -1 ) {
      printf( "Error: KeyPoint Mesh ID [%s] not found\n" , meshID );
    }
    
    // next tag
    fgets  ( line, 256, file );
    sscanf ( line, "%s", tag );
  
    // Scan for KPs in this mesh
    int numberMeshKPs = 0;
    while( strcmp( tag , "kp" ) == 0 ) {
      if( NbKPs > NBMAXKP ) {
	printf( "Error: Excessive number of KeyPoints\n" );
	throw 0;
      }

      sscanf ( line, "%s %s", 
	       tag, TabKPs[NbKPs].id );

      // stores the index of the mesh associated with this keypoint
      TabKPs[NbKPs].indMesh = indMesh;

      fgets  ( line, 256, file );
      sscanf ( line, "%s %f %f %f", 
	       tag,
	       &(TabKPs[NbKPs].location.x),
	       &(TabKPs[NbKPs].location.y),
	       &(TabKPs[NbKPs].location.z) );
      // printf( "vertex %f %f %f\n" , TabVertices[NbVertices].location.x,
      // 	      TabVertices[NbVertices].location.y,
      // 	      TabVertices[NbVertices].location.z );

      if( !fgets  ( line, 256, file ) ) {
	numberMeshKPs++;
	NbKPs++;
	if( indMesh >= 0 ) {
	  TabMeshes[indMesh].nbKPs = numberMeshKPs;
	}
	printf( "Mesh #%d %s KPs %d\n" , indMesh , 
		TabMeshes[ indMesh ].id , 
		TabMeshes[ indMesh ].nbKPs );
	return;
      }

      sscanf ( line, "%s", tag );
      numberMeshKPs++;
      NbKPs++;
    }

    if( indMesh >= 0 ) {
      TabMeshes[indMesh].nbKPs = numberMeshKPs;
    }

    printf( "Mesh #%d %s KPs %d\n" , indMesh , 
	    TabMeshes[ indMesh ].id , 
	    TabMeshes[ indMesh ].nbKPs );
  }
}

//////////////////////////////////////////////////////////////////
// KEYPOINT BINDING AND VERTEX WEIGHTING
//////////////////////////////////////////////////////////////////

// for each keypoint, finds the nearest vertex in mesh
// not really used, just a check

void locate_KP_in_mesh( void ) {
  for( int indMesh = 0 ; indMesh < NbMeshes ; indMesh++ ) {
    for( int indKP = 0 ; indKP < NbKPs ; indKP++ ) {
      if( TabKPs[indKP].indMesh == indMesh ) {
	// accesses the vertices from a mesh and its faces
	float minDist = MAXFLOAT;
	int indVertexKP = -1;
	for (int indFace = TabMeshes[ indMesh ].indFaceIni ; 
	     indFace < TabMeshes[ indMesh ].indFaceEnd ; 
	     indFace++) {
	  float d;
	  if( (d = TabKPs[indKP].location.distance( 
                      TabVertices[ TabFaces[indFace].indVertex1 ].location))
	      < minDist ) {
	    indVertexKP = TabFaces[indFace].indVertex1;
	    minDist = d;
	  }
	  if( (d = TabKPs[indKP].location.distance( 
	              TabVertices[ TabFaces[indFace].indVertex2 ].location))
	      < minDist ) {
	    indVertexKP = TabFaces[indFace].indVertex2;
	    minDist = d;
	  }
	  if( (d = TabKPs[indKP].location.distance( 
	              TabVertices[ TabFaces[indFace].indVertex3 ].location))
	      < minDist ) {
	    indVertexKP = TabFaces[indFace].indVertex3;
	    minDist = d;
	  }
	}
	TabKPs[indKP].indVertex = indVertexKP;
	printf( "KP %s Mesh %s %f %f %f Vertex %d %f %f %f dist %f\n" , 
		TabKPs[indKP].id ,
		TabMeshes[ indMesh ].id ,
		TabKPs[indKP].location.x ,
		TabKPs[indKP].location.y ,
		TabKPs[indKP].location.z ,
		indVertexKP + 1 ,
		TabVertices[indVertexKP].location.x ,
		TabVertices[indVertexKP].location.y ,
		TabVertices[indVertexKP].location.z ,
		minDist );
      }
    }
  }
}

// a keypoint weighting scheme: linear weighting
float linear_weight(float distance , float radius , int exponent) {
  if( distance < radius ) {
    return 1.0 - distance / radius;
  }
  else {
    return 0.f;
  }
}

// a keypoint weighting scheme: the distance's inverse(before normalization)
float inverse_distance_weight(float distance, float radius, int exponent) {
    if (distance < radius) {
        return 1 / distance;
    } else {
        return 0.f;
    }
}

// vertices weighting: each vertex is weighted on at most
// four keypoints
// if there are more than four keypoints with non null
// weight for this vertex, only the 4 KPs with the heighest
// weights are taken into account

int indVertexForComparison = -1;

int compareIndKPDistance(const void *iKP1, const void *iKP2) {
  KP *kp1 = &TabKPs[*(int *)iKP1];
  KP *kp2 = &TabKPs[*(int *)iKP2];
  float fcomp = TabVertices[indVertexForComparison].location.distance(kp1->location) -
    TabVertices[indVertexForComparison].location.distance(kp2->location);
  int comp = fcomp < 0. ? -1 : (fcomp > 0 ? +1 : 0);
  return comp;
}

WeightingType weight_one_vertex( int indVertex , int indKP ,
				 float radius , int exponent , 
				 float (*pt2Function)(float,float,int) ) {
  if( !TabVertices[ indVertex ].weighted ) {
    Vertex *v = &TabVertices[ indVertex ];
    v->weighted = true;
    
    // computes the distance and the weights
    // TODO
    // return Weighting or WeightSubstitution
    // depending on whether the current weight has replaced
    // a previous one or not
    
    // find closest KPs to vertex
    int TabIndKPsSorted[NbKPs];
    for (int iKP = 0; iKP < NbKPs; iKP++) {
      TabIndKPsSorted[iKP] = iKP;
    }
    // sort KPs according to distance
    indVertexForComparison = indVertex;
    qsort(TabIndKPsSorted, NbKPs, sizeof(int), &compareIndKPDistance);
    
    // get first elements of the sorted array
    for(int i = 0; i < MAXKPWEIGHTS; i++) {
      v->indKP[i] = TabIndKPsSorted[i];
    }
    
    // compute weights
    float sum_weights = 0.f;
    for( int i = 0 ; i < MAXKPWEIGHTS ; i++ ) {
      KP *kp = &TabKPs[v->indKP[i]];
      float distance = v->location.distance(kp->location);
      // printf("kp location: %f %f %f\n", kp.location.x, kp.location.y, kp.location.z);
      // printf("distance: %f\n", distance);
      v->wKP[i] = pt2Function(distance, radius, exponent);
      // printf("weight: %f\n", v->wKP[i]);
      sum_weights += v->wKP[i];
    }
    // normalize weights
    if(sum_weights > 0) {
        float norm = 1 / sum_weights;
        for( int i = 0 ; i < MAXKPWEIGHTS ; i++ ) {
          v->wKP[i] *= norm;
        }
    }
    return Weighting;
  }
  return NoWeighting;
}

// vertices weighting: weights all the vertices in a mesh

void weight_vertices_on_KP_in_mesh( float radius , int exponent , 
				    float (*pt2Function)(float,float,int) ) {
    for( int indMesh = 0 ; indMesh < NbMeshes ; indMesh++ ) {
        for( int indKP = 0 ; indKP < NbKPs ; indKP++ ) {
          if( TabKPs[indKP].indMesh == indMesh ) {
            int nbWeightedVertices = 0;

            // marks all the vertices as unprocessed for the current keypoint
            for( int indVertex = 0 ; indVertex < NbVertices ; indVertex++ ) {
                TabVertices[ indVertex ].weighted = false;
            }

            // accesses the vertices from a mesh and its faces
            for (int indFace = TabMeshes[ indMesh ].indFaceIni ; 
                 indFace < TabMeshes[ indMesh ].indFaceEnd ; 
                 indFace++) {
              if( weight_one_vertex( TabFaces[indFace].indVertex1 , indKP , radius , exponent , pt2Function ) == Weighting ) {
                nbWeightedVertices++;
              }
              if( weight_one_vertex( TabFaces[indFace].indVertex2 , indKP ,  radius , exponent , pt2Function ) == Weighting ) {
                nbWeightedVertices++;
              }
              if( weight_one_vertex( TabFaces[indFace].indVertex3 , indKP ,  radius , exponent , pt2Function ) == Weighting ) {
                nbWeightedVertices++;
              }
            }
	
    // printf( "KP %s Mesh %s Nb weighted vertices %d\n" , 
    //     TabKPs[indKP].id ,
    //     TabMeshes[ TabKPs[indKP].indMesh ].id ,
    //     nbWeightedVertices );
      }
    }
  }
}

//////////////////////////////////////////////////////////////////
// MESH ANIMATION
//////////////////////////////////////////////////////////////////

// moves each vertex according to the translation
// of the keypoints to which this vertex is attached

void animate_vertices_in_mesh( void ) {
  int        indKP;
  Vertex    *ptVertex = TabVertices;

  for( int indVertex = 0 ; indVertex < NbVertices ; 
  (ptVertex++ , indVertex++) ) {
      if(ptVertex->weighted == false) {
          continue;
      }
      ptVertex->curLocation = ptVertex->location;
      // printf("original %f %f %f\n", ptVertex->curLocation.x, ptVertex->curLocation.y, ptVertex->curLocation.z);
      for( int i = 0 ; i < MAXKPWEIGHTS ; i++ ) {
          // TODO
          KP *kp = &TabKPs[ptVertex->indKP[i]];
          // printf("indKP: %d\n", ptVertex->indKP[i]);
          // printf("translation: %f %f %f\n", kp->translation.x, kp->translation.y, kp->translation.z);
          // printf("weight: %f\n", ptVertex->wKP[i]);
          ptVertex->curLocation.x += ptVertex->wKP[i] * kp->translation.x;
          ptVertex->curLocation.y += ptVertex->wKP[i] * kp->translation.y;
          ptVertex->curLocation.z += ptVertex->wKP[i] * kp->translation.z;
      }
      // printf("cur %f %f %f\n", ptVertex->curLocation.x, ptVertex->curLocation.y, ptVertex->curLocation.z);
  }
}

//////////////////////////////////////////////////////////////////
// MESH DISPLAY
//////////////////////////////////////////////////////////////////

// face display

// void displayFace( int indFace ) {
//   glNormal3f( TabNormals[ TabFaces[indFace].indNormal1 ].x , 
//           TabNormals[ TabFaces[indFace].indNormal1 ].y , 
//           TabNormals[ TabFaces[indFace].indNormal1 ].z );
//   glVertex3f( TabVertices[ TabFaces[indFace].indVertex1 ].location.x ,
//           TabVertices[ TabFaces[indFace].indVertex1 ].location.y ,
//           TabVertices[ TabFaces[indFace].indVertex1 ].location.z );
//   glNormal3f( TabNormals[ TabFaces[indFace].indNormal2 ].x , 
//           TabNormals[ TabFaces[indFace].indNormal2 ].y , 
//           TabNormals[ TabFaces[indFace].indNormal2 ].z );
//   glVertex3f( TabVertices[ TabFaces[indFace].indVertex2 ].location.x ,
//           TabVertices[ TabFaces[indFace].indVertex2 ].location.y ,
//           TabVertices[ TabFaces[indFace].indVertex2 ].location.z );
//   glNormal3f( TabNormals[ TabFaces[indFace].indNormal3 ].x , 
//           TabNormals[ TabFaces[indFace].indNormal3 ].y , 
//           TabNormals[ TabFaces[indFace].indNormal3 ].z );
//   glVertex3f( TabVertices[ TabFaces[indFace].indVertex3 ].location.x ,
//           TabVertices[ TabFaces[indFace].indVertex3 ].location.y ,
//           TabVertices[ TabFaces[indFace].indVertex3 ].location.z );
// }

void displayFace( int indFace ) {
  glNormal3f( TabNormals[ TabFaces[indFace].indNormal1 ].x , 
	      TabNormals[ TabFaces[indFace].indNormal1 ].y , 
	      TabNormals[ TabFaces[indFace].indNormal1 ].z );
  glVertex3f( TabVertices[ TabFaces[indFace].indVertex1 ].curLocation.x ,
	      TabVertices[ TabFaces[indFace].indVertex1 ].curLocation.y ,
	      TabVertices[ TabFaces[indFace].indVertex1 ].curLocation.z );
  glNormal3f( TabNormals[ TabFaces[indFace].indNormal2 ].x , 
	      TabNormals[ TabFaces[indFace].indNormal2 ].y , 
	      TabNormals[ TabFaces[indFace].indNormal2 ].z );
  glVertex3f( TabVertices[ TabFaces[indFace].indVertex2 ].curLocation.x ,
	      TabVertices[ TabFaces[indFace].indVertex2 ].curLocation.y ,
	      TabVertices[ TabFaces[indFace].indVertex2 ].curLocation.z );
  glNormal3f( TabNormals[ TabFaces[indFace].indNormal3 ].x , 
	      TabNormals[ TabFaces[indFace].indNormal3 ].y , 
	      TabNormals[ TabFaces[indFace].indNormal3 ].z );
  glVertex3f( TabVertices[ TabFaces[indFace].indVertex3 ].curLocation.x ,
	      TabVertices[ TabFaces[indFace].indVertex3 ].curLocation.y ,
	      TabVertices[ TabFaces[indFace].indVertex3 ].curLocation.z );
}

// transforms the mesh into a Display List

void make_mesh( void ) {
  glNewList(MeshID, GL_COMPILE);  
    for( int ind = 0 ; ind < NbMeshes ; ind++ ) {
      // displays wireframe
      if( TypeOfSurfaceDisplay == MESH ) {
	glEnable( GL_COLOR_MATERIAL );
	glColor3f( .0 , .1  , 0.2 );
	for (int i = TabMeshes[ ind ].indFaceIni ; 
	     i < TabMeshes[ ind ].indFaceEnd ; i++) {
	  glBegin(GL_LINE_LOOP);
	  displayFace(i);
	  glEnd();
	}
	glDisable( GL_COLOR_MATERIAL );
      }
      // displays surface
      else {
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Diffuse_silver);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular_silver);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ambient_silver);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission_silver);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Shininess_silver);
	for (int i = TabMeshes[ ind ].indFaceIni ; 
	     i < TabMeshes[ ind ].indFaceEnd ; i++) {
	  glBegin(GL_TRIANGLES);
	  displayFace(i);
	  glEnd();
	}
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
  TabKPs = new KP[ NBMAXKP ];
  
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

  // parses the keypoints
  FILE * fileKP = fopen( KPFileName , "r" ); 
  if( !fileKP ) {
    printf( "File %s not found, no keypoint defined for this mesh\n" , KPFileName );
  }
  else {
    parse_KP_obj( fileKP );
    fclose( fileKP );
  }

  // Display List compiling
  MeshID =  glGenLists(1);
  make_mesh();

  // locates the keypoints in the mesh
  locate_KP_in_mesh();

  // weights the vertices on these keypoints
 // float RADIUS = 1.f;
  // float RADIUS = 0.2;
  // weight_vertices_on_KP_in_mesh( RADIUS , 0 , &linear_weight);
  weight_vertices_on_KP_in_mesh(RADIUS, 0, weighting_scheme);
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

  // toutes les transformations suivantes s´appliquent au modèle de vue 
  glMatrixMode(GL_MODELVIEW);
}

// glut call-back: keyboard events

GLvoid window_key_manualanim(unsigned char key, int x, int y) 
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
  case '1':
    CurrentActiveKeyPoint = 0;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '2':
    CurrentActiveKeyPoint = 1;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '3':
    CurrentActiveKeyPoint = 2;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '4':
    CurrentActiveKeyPoint = 3;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '5':
    CurrentActiveKeyPoint = 4;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '6':
    CurrentActiveKeyPoint = 5;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '7':
    CurrentActiveKeyPoint = 6;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '8':
    CurrentActiveKeyPoint = 7;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case '9':
    CurrentActiveKeyPoint = 8;
    printf( "Current active KP #%d\n", CurrentActiveKeyPoint + 1) ;
    break; 
  case KEY_ESC:  
    exit(1);                    
    break; 
  default:
    printf ("key %d is not active.\n", key);
    break;
  }     
}

GLvoid window_key(unsigned char key, int x, int y) {
    for (int iKF = 0; iKF < NB_ANIM_KFS; iKF++) {
        if (key == ('1' + iKF)) {
            init_animation(iKF);
            printf(tab_emotion[iKF]);
            printf("\n");
            fflush(stdout);
            return;
        }
    }
    switch (key) {
        case '+':
            animDuration += ANIM_DURATION_INCREMENT;
            printf("Animation duration: %f s\n", animDuration);
            fflush(stdout);
            break;
        case '-':
            animDuration -= ANIM_DURATION_INCREMENT;
            printf("Animation duration: %f s\n", animDuration);
            fflush(stdout);
            break;
        case 'q':
            curveID = EASEINOUTQUAD;
            printf("Using easeInOutQuad easing function\n");
            fflush(stdout);
            break;
        case 'w':
            curveID = LINEAR;
            printf("Using linear easing function\n");
            fflush(stdout);
            break;
        case 'a':
            weighting_scheme = inverse_distance_weight;
            weight_vertices_on_KP_in_mesh(RADIUS, 0, weighting_scheme);
            update_vertices();
            glutPostRedisplay();
            printf("Using inverse-distance weighting scheme\n");
            fflush(stdout);
            break;
        case 's':
            weighting_scheme = linear_weight;
            weight_vertices_on_KP_in_mesh(RADIUS, 0, weighting_scheme);
            update_vertices();
            glutPostRedisplay();
            printf("Using linear weighting scheme\n");
            fflush(stdout);
            break;
        case '<':  
            Zoom /= 1.1;
            glutPostRedisplay();
            break; 
        case '>':  
            Zoom *= 1.1;
            glutPostRedisplay();
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

}

// glut call-back: special key processing

// #define ANIMATION_STEP    0.001
#define ANIMATION_STEP    0.01
void window_special_key_manualanim(int key, int x, int y)
{
  switch (key) {    
  case GLUT_KEY_LEFT: // control of horizontal move
    {
      TabKPs[CurrentActiveKeyPoint].translation.x += ANIMATION_STEP;
      // KP *kp = &TabKPs[CurrentActiveKeyPoint];
      // printf("kp: %f %f %f", kp->location.x, kp->location.y, kp->location.z);
      // fflush(stdout);
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1); 
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_RIGHT: // control of horizontal move
    {
      TabKPs[CurrentActiveKeyPoint].translation.x -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_UP: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.y += ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_DOWN: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.y -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_HOME: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.init();
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_END: // control of vertical move
    {
    }
    break;
  case GLUT_KEY_PAGE_UP: // control of view elevation
    {
      TabKPs[CurrentActiveKeyPoint].translation.z += ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_PAGE_DOWN: // control of view elevation
    {
      TabKPs[CurrentActiveKeyPoint].translation.z -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  default:
    printf ("special key %d is not active.\n", key);
    break;
  }
}

void window_special_key(int key, int x, int y)
{
  switch (key) {    
  case GLUT_KEY_LEFT: // control of horizontal move
    {
      TabKPs[CurrentActiveKeyPoint].translation.x += ANIMATION_STEP;
      // KP *kp = &TabKPs[CurrentActiveKeyPoint];
      // printf("kp: %f %f %f", kp->location.x, kp->location.y, kp->location.z);
      // fflush(stdout);
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1); 
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_RIGHT: // control of horizontal move
    {
      TabKPs[CurrentActiveKeyPoint].translation.x -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_UP: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.y += ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_DOWN: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.y -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_HOME: // control of vertical move
    {
      TabKPs[CurrentActiveKeyPoint].translation.init();
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_END: // control of vertical move
    {
    }
    break;
  case GLUT_KEY_PAGE_UP: // control of view elevation
    {
      TabKPs[CurrentActiveKeyPoint].translation.z += ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
  case GLUT_KEY_PAGE_DOWN: // control of view elevation
    {
      TabKPs[CurrentActiveKeyPoint].translation.z -= ANIMATION_STEP;
      animate_vertices_in_mesh();
      glDeleteLists(MeshID, 1);  
      make_mesh();

      glutPostRedisplay();
    }
    break;
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

  // axes xx yy et zz
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
  glDisable( GL_COLOR_MATERIAL );

  glTranslatef(0,0,0);
  glCallList(MeshID);

  glPopMatrix();

  glutSwapBuffers();
}

/// HOST ADDRESS 
class vc_MySocketAddresse {
public:
  bool d_valid;
  sockaddr_in d_sockAddresse;
  int port;
  char host[100];
  char host_IP[100];

  vc_MySocketAddresse() {
    memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
    *host = 0;
    *host_IP = 0;
    d_valid = false;
  }

  ~vc_MySocketAddresse(void) {
  }

  void IPAdd() {
    // gets the string that represents the address "a.b.c.d"
    char* s = inet_ntoa( d_sockAddresse.sin_addr );
    if( s == NULL )
      printf( "Erreur: IPAdd() ne peut convertir l'adresse.\n" );
    else
      strcpy( host_IP, s );
  }

  void InitFromResolver(char *_host, const int _port) {
    struct hostent 		*hostinfo;

    strcpy( host , _host );  
    port=_port;
    d_sockAddresse.sin_family=AF_INET;
    d_sockAddresse.sin_port=htons(port);

    hostinfo=gethostbyname(host);
    if (hostinfo==NULL) {
      fprintf( stdout , "Unknown host %s \n", _host);
      memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
      d_valid = false;
      return;
    }

    d_sockAddresse.sin_addr=*(struct in_addr*)hostinfo->h_addr;
    IPAdd();
    d_valid = true;
  } 

  void TestUDPConnection( void ) {
    vc_MySocketAddresse sock;
    char nam[100];

    gethostname(nam, 100);
    printf( "%s\n" , nam );
    sock.InitFromResolver(nam, 3120);

    char* s = inet_ntoa(sock.d_sockAddresse.sin_addr);

    printf( "%s\n" , s );
  }
};


//////////////////////////////////////////////////////////////////
// MAIN CLIENT
//////////////////////////////////////////////////////////////////

KP *keypoint_with_id(char *id) {
    for(int i = 0; i < NbKPs; ++i) {
        KP *kp = &TabKPs[i];
        if(strcmp(kp->id, id) == 0) {
            return kp;
        }
    }
    return NULL;
}

// local server socket

void init_client() {
    // local server address
    unsigned int Local_server_port = 1979;

    ///////////////////////////////
    // local server creation
    struct sockaddr_in localServAddr;

  #ifndef _WIN32
    int SocketToLocalServerFlags;
    /* socket creation */
    SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, 0);

    if(SocketToLocalServer < 0) {
      printf( "Error: udp server cannot open socket to local server!\n" ); 
      throw(0);
    }
    else {
      // Read the socket's flags
      SocketToLocalServerFlags = fcntl(SocketToLocalServer, F_GETFL, 0);
      // Sets the socket's flags to non-blocking
      SocketToLocalServerFlags |= O_NONBLOCK;
      int ret = fcntl(SocketToLocalServer, F_SETFL, SocketToLocalServerFlags );
      if(ret < 0) {
        printf(  "Error: udp server cannot set flag to non-blocking: %s!" , 
  	       strerror(errno) );
        throw(0);
      }
   }
  #else
    // socket creation
    SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(SocketToLocalServer < 0) {
      printf( "Error: udp server cannot open socket to local server!\n" ); 
      throw(0);
    }
    else {
      // Read the socket's flags
      unsigned long onoff=1;
    
      if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0){
        printf(  "Error: udp server cannot set flag to non-blocking: %s!" , 
  	       strerror(errno) );
        throw(0);
      }
    }
  #endif
    
    // bind local server port
    localServAddr.sin_family = AF_INET;
    localServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localServAddr.sin_port = htons(Local_server_port);
  
    int rc = bind (SocketToLocalServer, 
  		 (struct sockaddr *) &localServAddr,sizeof(localServAddr));
    if(rc < 0) {
      printf( "Error: cannot bind locat port number %d!" , Local_server_port );
      throw(0);
    }
    printf("udp server: waiting for data on port UDP %u\n", Local_server_port);
}

void main_client(){
    // reads incoming UDP messages
    if( SocketToLocalServer >= 0 ) {
      char    message[1024];
      // init message buffer: Null values
      memset(message,0x0,1024);
    
      // receive message
      int n = recv(SocketToLocalServer, message, 1024, 0);
      if( n >= 0 ) {
	// scans the message and extract string & float values
	char KP_ID[256];
	int Keyframe;
	float KP_translation[3];
	// printf( "Message size %d\n" , n );
	// printf( "Rec.: [%s]\n" , message );
	sscanf( message , "%s %d %f %f %f" , KP_ID , &Keyframe ,
		KP_translation , KP_translation + 1 , KP_translation + 2 );
	printf( "ID [%s] KF [%d] tr (%.3f,%.3f,%.3f)\n" , KP_ID , Keyframe ,
		KP_translation[0] , KP_translation[1] , KP_translation[2] );
        // find the keypoint
        KP *kp = keypoint_with_id(KP_ID);
        if(kp != NULL) {
            // update translation
            kp->translation.x = KP_translation[0];
            kp->translation.y = KP_translation[1];
            kp->translation.z = KP_translation[2];
            // update rendering
            animate_vertices_in_mesh();
            glDeleteLists(MeshID, 1);  
            make_mesh();
            glutPostRedisplay();
        } else {
            printf("no keypoint found for id: %s\n", KP_ID);
        }
      }
    }
}

float curve_ease_inout_quad(float elapsed, float duration, float startVal, float endVal) {
    float u = elapsed / (duration / 2);
    if (u < 1) {
        return startVal + (endVal - startVal) / 2 * u * u;
    } else {
        u--;
        return startVal - (endVal - startVal) / 2 * (u * (u - 2) - 1);
    }
};

float curve_linear(float elapsed, float duration, float startVal, float endVal) {
    float percent = elapsed / duration;
    return startVal + (endVal - startVal) * percent;
}

void init_time_counter() {
    gettimeofday(&tv0, NULL);
    timeElapsed = 0.0f;
    // FramesPerFPS = 30;
}

void init_animation(int indKF) {
    // printf("begin animation\n");
    // fflush(stdout);
    init_time_counter();
    fromIndKF = toIndKF;
    toIndKF = indKF;
    switch (curveID) {
        case LINEAR: {
            curve = curve_linear;
            break;
        }
        case EASEINOUTQUAD: {
            curve = curve_ease_inout_quad;
            break;
        }
    }
    isAnimating = true;
}

void update_time_counter() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    timeElapsed = (float) (tv.tv_sec - tv0.tv_sec) + 0.000001 * (float)(tv.tv_usec - tv0.tv_usec);
}

void update_keypoints() {
    if (timeElapsed < animDuration) {
        for (int indKP = 0; indKP < NB_ANIM_KPS; indKP++) {
            char *kpID = tab_ID[indKP];
            float startX = tab_KF_x[indKP][fromIndKF];
            float endX = tab_KF_x[indKP][toIndKF];
            float startY = tab_KF_y[indKP][fromIndKF];
            float endY = tab_KF_y[indKP][toIndKF];
            float startZ = tab_KF_z[indKP][fromIndKF];
            float endZ = tab_KF_z[indKP][toIndKF];
            
            KP& kp = *keypoint_with_id(tab_ID[indKP]);
            kp.translation.x = curve(timeElapsed, animDuration, startX, endX);
            kp.translation.y = curve(timeElapsed, animDuration, startY, endY);
            kp.translation.z = curve(timeElapsed, animDuration, startZ, endZ);
            // if(kp.translation.x != 0) {
            //     printf("x: %f\n", kp.translation.x);
            // }
        }
    } else {
        for (int indKP = 0 ; indKP < NbKPs ; indKP++) {
            float endX = tab_KF_x[indKP][toIndKF];
            float endY = tab_KF_y[indKP][toIndKF];
            float endZ = tab_KF_z[indKP][toIndKF];
            TabKPs[indKP].translation.x = endX;
            TabKPs[indKP].translation.y = endY;
            TabKPs[indKP].translation.z = endZ;
        }
    }
}

void update_vertices() {
    animate_vertices_in_mesh();
    glDeleteLists(MeshID, 1);  
    make_mesh();
}

void animate_frame() {
    update_time_counter();
    update_keypoints();
    update_vertices();
    glutPostRedisplay();
}

void idle_func() {
    // main_client();
    // printf("idle func\n");
    // fflush(stdout);
    if (isAnimating) {
        animate_frame();
        isAnimating = timeElapsed < animDuration;
        // if(!isAnimating) {
        //     printf("end animation\n");
        //     fflush(stdout);
        // }
    }
}

