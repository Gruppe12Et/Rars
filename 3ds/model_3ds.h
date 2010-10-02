//////////////////////////////////////////////////////////////////////
//
// 3D Studio Model Class
// by: Matthew Fairfax
//
// Model_3DS.h: interface for the Model_3DS class.
// This is a simple class for loading and viewing
// 3D Studio model files (.3ds). It supports models
// with multiple objects. It also supports multiple
// textures per object. It does not support the animation
// for 3D Studio models b/c there are simply too many
// ways for an artist to animate a 3D Studio model and
// I didn't want to impose huge limitations on the artists.
// However, I have imposed a limitation on how the models are
// textured:
// 1) Every faces must be assigned a material
// 2) If you want the face to be textured assign the
//    texture to the Diffuse Color map
// 3) The texture must be supported by the GLTexture class
//    which only supports bitmap and targa right now
// 4) The texture must be located in the same directory as
//    the model
//
// Support for non-textured faces is done by reading the color
// from the material's diffuse color.
//
// Some models have problems loading even if you follow all of
// the restrictions I have stated and I don't know why. If you
// can import the 3D Studio file into Milkshape 3D 
// (http://www.swissquake.ch/chumbalum-soft) and then export it
// to a new 3D Studio file. This seems to fix many of the problems
// but there is a limit on the number of faces and vertices Milkshape 3D
// can read.
//
// Usage:
// Model_3DS m;
//
// m.Load("model.3ds"); // Load the model
// m.Draw();      // Renders the model to the screen
//
// // If you want to show the model's normals
// m.shownormals = true;
//
// // If the model is not going to be lit then set the lit
// // variable to false. It defaults to true.
// m.lit = false;
//
// // You can disable the rendering of the model
// m.visible = false;
// 
// // You can move and rotate the model like this:
// m.rot.x = 90.0f;
// m.rot.y = 30.0f;
// m.rot.z = 0.0f;
//
// m.pos.x = 10.0f;
// m.pos.y = 0.0f;
// m.pos.z = 0.0f;
//
// // If you want to move or rotate individual objects
// m.Objects[0].rot.x = 90.0f;
// m.Objects[0].rot.y = 30.0f;
// m.Objects[0].rot.z = 0.0f;
//
// m.Objects[0].pos.x = 10.0f;
// m.Objects[0].pos.y = 0.0f;
// m.Objects[0].pos.z = 0.0f;
//
//////////////////////////////////////////////////////////////////////

#ifndef MODEL_3DS_H
#define MODEL_3DS_H

//--------------------------------------------------------------------------
//                           D E F I N E S
//--------------------------------------------------------------------------

// This is used to generate a warning from the compiler
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define warn( x )  message( __FILE__LINE__ #x "\n" )
#define MAX_MODELS 1000

// The chunk's id numbers
#define CHUNK_MAIN3DS         0x4D4D
 #define CHUNK_MAIN_VERS      0x0002
 #define CHUNK_EDIT3DS        0x3D3D
  #define CHUNK_MESH_VERS     0x3D3E
  #define CHUNK_OBJECT        0x4000
   #define CHUNK_TRI_MESH     0x4100
    #define CHUNK_VERT_LIST   0x4110
    #define CHUNK_FACE_LIST   0x4120
     #define CHUNK_FACE_MAT   0x4130
    #define CHUNK_TEX_COORDS  0x4140
     #define SMOOTH_GROUP     0x4150
    #define LOCAL_COORDS      0x4160
  #define CHUNK_MATERIAL      0xAFFF
   #define CHUNK_MAT_NAME     0xA000
   #define CHUNK_MAT_AMBIENT  0xA010
   #define CHUNK_MAT_DIFFUSE  0xA020
   #define CHUNK_MAT_SPECULAR 0xA030
   #define CHUNK_SHINY        0xA040
   #define CHUNK_SHINY_STR    0xA041
   #define CHUNK_MAT_TRANSP   0xA050
   #define CHUNK_TRANS_FOFF   0xA052
   #define CHUNK_REF_BLUR     0xA053
   #define RENDER_TYPE        0xA100
   #define CHUNK_SELF_ILLUM   0xA084
   #define CHUNK_WIRE_THICK   0xA087
   #define CHUNK_MAT_TEXMAP   0xA200
    #define CHUNK_MAT_MAPNAME 0xA300
  #define ONE_UNIT            0x0100
 #define CHUNK_KEYF3DS        0xB000
  #define FRAMES              0xB008
  #define MESH_INFO           0xB002
   #define HIER_POS           0xB030
   #define HIER_FATHER        0xB010
   #define PIVOT_PT           0xB013
   #define TRACK00            0xB020
   #define TRACK01            0xB021
   #define TRACK02            0xB022

#define CHUNK_COLOR_FRGB      0x0010
#define CHUNK_COLOR_IRGB      0x0011
#define CHUNK_COLOR_IRGBA     0x0012
#define CHUNK_COLOR_FRGBA     0x0013
#define CHUNK_WORD            0x0030
#define CHUNK_FLOAT           0x0031

#ifndef WIN32
  #define WORD unsigned short
  #define DWORD unsigned int
#endif

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "gl_texture.h"
#include <stdio.h>
#include "../graphics/g_geom.h"

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

// Point
class Vect2D 
{
  public: 
    float x;
    float y;

    void SetXY( double _x, double _y )
    {
      x = (float)_x; 
      y = (float)_y;
    }
};

// A simple vector struct
struct Vect3D
{
  float x;
  float y;
  float z;
};

// A face (3 points)
struct Face
{
  unsigned short a;
  unsigned short b;
  unsigned short c;
};

/**
 * Every chunk in the 3ds file starts with this struct
 */
class Chunk3DS 
{
  public:
    unsigned short m_iId;     // The chunk's id
    unsigned long  m_iLen;    // The length of the chunk

    Chunk3DS * m_pNext;
    Chunk3DS * m_pFirstChild;

    Chunk3DS( Chunk3DS * pParent, int id );
    virtual ~Chunk3DS();
    void AddChild( Chunk3DS * pChild );
    void SaveHeader( FILE * out );
    void SaveChilds( FILE * out );

    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkMain3DS : public Chunk3DS
{
  public:
    ChunkMain3DS();
    virtual ~ChunkMain3DS() {};
};

class ChunkWord3DS : public Chunk3DS
{
  public:
    WORD val;

    ChunkWord3DS( Chunk3DS * pParent, int id, WORD _val  );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkDWord3DS : public Chunk3DS
{
  public:
    DWORD val;

    ChunkDWord3DS( Chunk3DS * pParent, int id, DWORD _val  );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkFloat3DS : public Chunk3DS
{
  public:
    float val;

    ChunkFloat3DS( Chunk3DS * pParent, int id, float _val  );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkString3DS : public Chunk3DS
{
  public:
    char * val;

    ChunkString3DS( Chunk3DS * pParent, int id, char * _val  );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkIRGB3DS : public Chunk3DS
{
  public:
    unsigned char r;
    unsigned char g;
    unsigned char b;

    ChunkIRGB3DS( Chunk3DS * pParent, unsigned char _r, unsigned char _g, unsigned char _b );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkVertexList3DS : public Chunk3DS
{
  public:
    unsigned short iNumVertex;
    Vertex * aVertex;

    ChunkVertexList3DS( Chunk3DS * pParent, int _iNumVertex, Vertex * _aVertex );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkFaceList3DS : public Chunk3DS
{
  public:
    unsigned short iNumFace;  // The number of faces in the object
    Face * aFace;   // The first vertex of the face

    ChunkFaceList3DS( Chunk3DS * pParent, int _iNumFace, Face * _aFace );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkFaceMat3DS : public Chunk3DS
{
  public:
    char * sName;
    unsigned short iNumMatFace;           
    unsigned short * aMatFace; 

    ChunkFaceMat3DS( Chunk3DS * pParent, char * _sName, int _iNumMatFace, unsigned short * _aMatFace );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

class ChunkTexCoord3DS : public Chunk3DS
{
  public:
    unsigned short iNumCoord; 
    Vect2D * aCoord; 

    ChunkTexCoord3DS( Chunk3DS * pParent, int _iNumCoord, Vect2D * _aCoord );
    virtual void Save( FILE * out );
    virtual int CalcLength();
};

/**
 * 3DS model 
 */ 
class Model_3DS  
{
public:
  // Color struct holds the diffuse color of the material
  struct Color4i 
  {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
  };

  // Holds the material info
  // TODO: add color support for non textured polys
  class Material 
  {
    public:
      char name[80];      // The material's name
      GLTexture * tex;    // The texture (this is the only outside reference in this class)
      bool bTextured;     // whether or not it is textured
      Color4i color;
      int iTransparency;  // The transparency (max transparency=100)

      Material()
      {
        tex = NULL;
        iTransparency = 0;
        bTextured = false;
      }
  };

  // Every chunk in the 3ds file starts with this struct
  struct ChunkHeader 
  {
    unsigned short id;  // The chunk's id
    unsigned long  len; // The lenght of the chunk
  };

  // I sort the mesh by material so that I won't have to switch textures a great deal
  class MaterialFaces 
  {
    public:
      int MatIndex;               // An index to our materials

      unsigned short * aFileFace; // A file face is the id of the face
      int numFileFaces;           // The number of faces

      Face * aSubFace;            // A sub face is the list of the 3 vertexes of the face
      int numSubFaces;            // The number of faces

      MaterialFaces()
      {
        aFileFace = NULL;
        aSubFace = NULL;
        numFileFaces = 0;
        numSubFaces = 0;
      }
  };

  // The 3ds file can be made up of several objects
  class Object 
  {
    public:
      char name[80];            // The object name
      Vertex * aFileVertex;     // The array of vertices read from file
      Vertex * aRealVertex;     // The array of vertices without duplicate
      int    * aFile2Real;      // Id of the original vertex, if this one is a duplicate
      Vect3D * aNormal;         // The array of the normals for the vertices
      Vect2D * aTexCoord;       // The array of texture coordinates for the vertices
      Face * aFace;             // The array of face indices
      MaterialFaces * aMatFace; // The faces are divided by materials 

      int numFileVertex;        // The number of vertices read in the file
      int numRealVertex;        // The number of vertices after removing the duplicates
      int numTexCoords;         // The number of vertices
      int numFaces;             // The number of faces
      int numMatFaces;          // The number of differnet material faces
      bool bTextured;           // True: the object has textures
      Vect3D pos;               // The position to move the object to
      Vect3D rot;               // The angles to rotate the object
      bool bFlipNewFace;        // Flip new added faces (AddFace)

      Object();
      void CalcFaceNormal( int a, int b, int c ); // Calculate the normal of a face;
      void FileFace2SubFace( int subfacesindex );

      // Edition
      void SetNumFileVerts( int _numFileVerts );
      void SetNumFaces( int _numFaces );
      void SetNumTexCoords( int _numTexCoords );
      void SetNumMatFaces( int _numMatFaces );
      int AddVertex( Vertex v, Vect2D t, bool bCheckDuplicate=false );
      int AddMatFace( int iMaterial );
      void AddRect( int iMatFace, int p0, int p1, int p2, int p3 );
      int AddFace( int iMatFace, int p0, int p1, int p2 );
  };

  char m_sName[256];      // The name of the model
  char path[256];         // The path of the model
  int numObjects;         // Total number of objects in the model
  int numMaterials;       // Total number of materials in the model
  int totalVerts;         // Total number of vertices in the model
  int totalFaces;         // Total number of faces in the model
  bool m_bShowNormal;     // True: show the normals
  Material * aMaterial;   // The array of materials
  Object * aObject;       // The array of objects in the model
  Vect3D pos;             // The position to move the model to
  Vect3D rot;             // The angles to rotate the model
  float scale;            // The size you want the model scaled to
  bool  m_bLit;           // True: the model is lit
  bool  m_bVisible;       // True: the model gets rendered
  bool  m_bMaterial;      // True: use the material and texture
  bool  m_bTransparent;   // True: some subobjects are transparent

  float m_fCenterX;       // Center and ray of a sphere containing the model.
  float m_fCenterY;
  float m_fCenterZ;
  float m_fMinX, m_fMaxX; // Maximun and minimum coordinates of the object
  float m_fMinY, m_fMaxY;
  float m_fMinZ, m_fMaxZ;
  float m_fRay;
  FILE *bin3ds;           // The binary 3ds file
  int   m_iOpenglId;      // Opengl Id of the main list associated with the model (opaque parts)
  int   m_iOpenglIdTransp;// Opengl Id of the second list associated with the model (transparent parts)

  // Variables for information only
  int   m_iNumFace;       // Number of face for the model

  Model_3DS();            // Constructor
  virtual ~Model_3DS();   // Destructor

  void Load( char * name ); // Loads a model
  void PostCalc();          // Post calculation (after loading)
  void Draw();              // Draws the model
  void DrawList( bool bTransparent ); // Draw one of the list (opaque or transp)
  void DrawObjects( bool bTransparent );
  void SetEnvMap( bool val );

  void CalculateSphere();   // Calculate the Sphere conating the object
  void Flatten(int coord);  // Reset the given coordinate to 0
  void Recenter();          // Recenter the object in (0,0,0)

  // Edition
  void Save( const char * path, const char * name );
  void Create();
  void RemoveAll();
  int AddObject();
  int AddMaterialTexture( const char * sPath, const char * name );
  int AddMaterialColor( unsigned char r, unsigned char g, unsigned char b );

private:
  bool  m_bEnvMap;          // True: use the environment mapping

  // The lists are automatally create in the first call.
  void glCreateList();      // Create a opengl list
  void glDeleteList();      // Delete the opengl list

  void IntColorChunkProcessor(long length, long findex, int matindex);
  void FloatColorChunkProcessor(long length, long findex, int matindex);
  // Processes the Main Chunk that all the other chunks exist is
  void MainChunkProcessor(long length, long findex);
    // Processes the model's info
    void EditChunkProcessor(long length, long findex);
      
      // Processes the model's materials
      void MaterialChunkProcessor(long length, long findex, int matindex);
        // Processes the names of the materials
        void MaterialNameChunkProcessor(long length, long findex, int matindex);
        // Processes the material's diffuse color
        void DiffuseColorChunkProcessor(long length, long findex, int matindex);
        // Processes transparency
        void TransparencyChunkProcessor(long length, long findex, int matindex);
        // Processes the material's texture maps
        void TextureMapChunkProcessor(long length, long findex, int matindex);
          // Processes the names of the textures and load the textures
          void MapNameChunkProcessor(long length, long findex, int matindex);
      
      // Processes the model's geometry
      void ObjectChunkProcessor(long length, long findex, int objindex);
        // Processes the triangles of the model
        void TriangularMeshChunkProcessor(long length, long findex, int objindex);
          // Processes the vertices of the model and loads them
          void VertexListChunkProcessor(long length, long findex, int objindex);
          // Processes the texture cordiantes of the vertices and loads them
          void TexCoordsChunkProcessor(long length, long findex, int objindex);
          // Processes the faces of the model and loads the faces
          void FacesDescriptionChunkProcessor(long length, long findex, int objindex);
            // Processes the materials of the faces and splits them up by material
            void FacesMaterialsListChunkProcessor(long length, long findex, int objindex, int subfacesindex);

  // Reduce the normals to the unit
  void UnitNormals();
};

//---------------------------------------------------------------------------
//  Loading
//---------------------------------------------------------------------------

class ModelLoader
{
  public:
    Model_3DS * aModel[MAX_MODELS];
    int iNumModel;

    ModelLoader();
    ~ModelLoader();
    void Reset();

    Model_3DS * GetModel( const char * sName, float scale, float rotz, bool bMaterial);
    Model_3DS * GetModel( bool &bNew, const char * sName, float scale, float rotz, bool bMaterial);
    Model_3DS * LoadModel( const char * sName, float scale, float rotz, bool bMaterial);
};

extern ModelLoader model_loader;


#endif
