/**
 * 3D Studio Model Class
 *
 * History: 
 *  01-04-01 Matthew Fairfax     Creation
 *  15-06-01 Marc Gueury         Improved calculation of normals
 *
 * Model_3DS.cpp: implementation of the Model_3DS class.
 * This is a simple class for loading and viewing
 * 3D Studio model files (.3ds). It supports models
 * with multiple objects. It also supports multiple
 * textures per object. It does not support the animation
 * for 3D Studio models b/c there are simply too many
 * ways for an artist to animate a 3D Studio model and
 * I didn't want to impose huge limitations on the artists.
 * However, I have imposed a limitation on how the models are
 * textured:
 * 1) Every faces must be assigned a material
 * 2) If you want the face to be textured assign the
 *    texture to the Diffuse Color map
 * 3) The texture must be supported by the GLTexture class
 *    which only supports bitmap and targa right now
 * 4) The texture must be located in the same directory as
 *    the model
 *
 * Support for non-textured faces is done by reading the color
 * from the material's diffuse color.
 *
 * Some models have problems loading even if you follow all of
 * the restrictions I have stated and I don't know why. If you
 * can import the 3D Studio file into Milkshape 3D 
 * (http://www.swissquake.ch/chumbalum-soft) and then export it
 * to a new 3D Studio file. This seems to fix many of the problems
 * but there is a limit on the number of faces and vertices Milkshape 3D
 * can read.
 *
 * Usage:
 * Model_3DS m;
 *
 * m.Load("model.3ds"); // Load the model
 * m.Draw();      // Renders the model to the screen
 *
 * // If you want to show the model's normals
 * m.shownormals = true;
 *
 * // If the model is not going to be lit then set the lit
 * // variable to false. It defaults to true.
 * m.lit = false;
 *
 * // You can disable the rendering of the model
 * m.visible = false;
 * 
 * // You can move and rotate the model like this:
 * m.rot.x = 90.0f;
 * m.rot.y = 30.0f;
 * m.rot.z = 0.0f;
 *
 * m.pos.x = 10.0f;
 * m.pos.y = 0.0f;
 * m.pos.z = 0.0f;
 *
 * // If you want to move or rotate individual objects
 * m.aObject[0].rot.x = 90.0f;
 * m.aObject[0].rot.y = 30.0f;
 * m.aObject[0].rot.z = 0.0f;
 *
 * m.aObject[0].pos.x = 10.0f;
 * m.aObject[0].pos.y = 0.0f;
 * m.aObject[0].pos.z = 0.0f;
 *
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "model_3ds.h"
#include "../misc.h"
#include "../os.h"
#include "../graphics/g_opengl.h"

#include <GL/gl.h>

#ifdef WIN32
  #include "GL/glext.h"
  // #define GLH_EXT_SINGLE_FILE
  #include "GL/glh_extensions.h"
#endif 

//--------------------------------------------------------------------------
//                          Class Object
//--------------------------------------------------------------------------

/**
 * Constructor
 */
Model_3DS::Object::Object()
{
  // Set the textured variable to false until we find a texture
  bTextured = false;
  bFlipNewFace = false;

  // Zero the objects position and rotation
  pos.x = 0.0f;
  pos.y = 0.0f;
  pos.z = 0.0f;

  rot.x = 0.0f;
  rot.y = 0.0f;
  rot.z = 0.0f;

  numFaces = 0;
  numMatFaces = 0;
  numFileVertex = 0;
  numRealVertex = 0;
  numTexCoords = 0;

  aFileVertex = NULL;
  aRealVertex = NULL;
  aFile2Real = NULL;
  aNormal = NULL;
  aTexCoord = NULL;
  aFace = NULL;
  aMatFace = NULL;

  strcpy( name, "object" );
}

void Model_3DS::Object::SetNumFileVerts( int _numFileVertex )
{
  // Assign the number of vertices for future use
  numFileVertex = _numFileVertex;

  // Allocate arrays for the vertices and normals
  aFileVertex  = new Vertex[numFileVertex];
  aRealVertex  = new Vertex[numFileVertex];
  aNormal      = new Vect3D[numFileVertex];
  aFile2Real   = new int[numFileVertex];

  // Zero out the normals array
  for( int i=0; i<numFileVertex; i++)
  {
    aNormal[i].x = 0.0f;
    aNormal[i].y = 0.0f;
    aNormal[i].z = 0.0f;
  }
}

void Model_3DS::Object::SetNumFaces( int _numFaces )
{
  // Store the number of faces
  numFaces = _numFaces;
  // Allocate an array to hold the faces
  aFace = new Face[numFaces];
}

void Model_3DS::Object::SetNumTexCoords( int _numTexCoords )
{
  // Set the number of texture coords
  numTexCoords = _numTexCoords;
  // Allocate an array to hold the texture coordinates
  aTexCoord = new Vect2D[numTexCoords];
}

void Model_3DS::Object::SetNumMatFaces( int _numMatFaces )
{
  // Store the number of material faces
  numMatFaces = _numMatFaces;
  // Allocate an array to hold the lists of faces divided by material
  aMatFace = new MaterialFaces[numMatFaces];
}

//--------------------------------------------------------------------------
//                          Class Model_3DS
//--------------------------------------------------------------------------

/**
 * Constructor
 */
Model_3DS::Model_3DS()
{
  // Initialization

  // Don't show the normals by default
  m_bShowNormal = false;

  // The model is lit by default
  m_bLit = true;
  m_bEnvMap = false;

  // The materials are shown per default
  m_bMaterial = true;

  // The model is visible by default
  m_bVisible = true;
  m_bTransparent = false;

  // Set up the default position
  pos.x = 0.0f;
  pos.y = 0.0f;
  pos.z = 0.0f;
  // Set up the default rotation
  rot.x = 0.0f;
  rot.y = 0.0f;
  rot.z = 0.0f;

  // Set up the path
  path[0] = 0;

  // Zero out our counters
  numObjects = 0;
  numMaterials = 0;
  aObject = NULL;
  aMaterial = NULL;

  // Set the scale to one
  scale = 1.0f;

  // OpenGL list
  m_iOpenglId = -1;
  m_iOpenglIdTransp = -1;
}

/**
 * Destructor
 */
Model_3DS::~Model_3DS()
{
  RemoveAll();
}

/**
 * RemoveAll
 */
void Model_3DS::RemoveAll()
{
  int i, j;
  for( i=0; i<numObjects; i++ )
  {
    Object * o = &( aObject[i] );
    if( o->aMatFace!=NULL )
    {
      for( j=0; j<o->numMatFaces; j++ )
      {
        MaterialFaces * mf = &( o->aMatFace[j] );
        if( mf->aSubFace!=NULL )
        {
          delete mf->aSubFace;
        }
        if( mf->aFileFace!=NULL )
        {
          delete mf->aFileFace;
        }
      }
      delete o->aMatFace;
    }
    if( o->aFace!=NULL )
    {
      delete o->aFace;
    }
    if( o->aFileVertex!=NULL )
    {
      delete o->aFileVertex;
      delete o->aFile2Real;
      opengl.VideoMemoryFree( o->aRealVertex );
      opengl.VideoMemoryFree( o->aNormal );
    }
    if( o->aTexCoord!=NULL )
    {
      opengl.VideoMemoryFree( o->aTexCoord );
    }
  }

  if( aObject!=NULL )
  {
    delete aObject;
    aObject = NULL;
  }
  numObjects = 0;

  if( aMaterial!=NULL )
  {
    delete aMaterial;
    aMaterial = NULL;
  }

  glDeleteList();
}

/**
 * Load a 3DS file
 *
 * @param name : name of the 3DS file
 */
void Model_3DS::Load( char * name )
{
  // holds the main chunk header
  ChunkHeader main;

  // For future reference
  strcpy( m_sName, name );

  // strip "'s
  if (strstr(name, "\""))
    name = strtok(name, "\"");

  // Find the path
  if (strstr(name, "/") || strstr(name, "\\"))
  {
    // Holds the name of the model minus the path
    char * temp;

    // Find the name without the path
    if (strstr(name, "/"))
    {
      temp = strrchr(name, '/');
    }
    else
    {
      temp = strrchr(name, '\\');
    }

    // Get a pointer to the end of the path and name
    char * src = name + strlen(name) - 1;

    // Back up until a \ or the start
    while (src != path && !((*(src-1)) == '\\' || (*(src-1)) == '/'))
    {
      src--;
    }

    // Copy the path into path
    memcpy (path, name, src-name);
    path[src-name] = 0;
  }

  // Load the file
  bin3ds = fopen(name,"rb");

  // File is null
  if( bin3ds==NULL )
  {
    exitOnError("Model_3DS::Load : file not found '%s'", name );
  }

  // Make sure we are at the beginning
  fseek(bin3ds, 0, SEEK_SET);

  // Load the Main Chunk's header
  fread(&main.id,sizeof(main.id),1,bin3ds);
  fread(&main.len,sizeof(main.len),1,bin3ds);

  // Start Processing
  MainChunkProcessor(main.len, ftell(bin3ds));

  // Don't need the file anymore so close it
  fclose(bin3ds);

  PostCalc();
}

/**
 * Post calculation (calculate all missing stuff to draw the model)
 */
void Model_3DS::PostCalc()
{
  int i, j;

  m_iNumFace = 0;

  // Rescale all the points
  for( i = 0; i < numObjects; i++ )
  {
    // Rescale all the points
    // Move all the points
    for( j=0; j<aObject[i].numRealVertex; j++ )
    {
      Vertex * v = &(aObject[i].aRealVertex[j]);
      v->x = v->x*scale+pos.x;
      v->y = v->y*scale+pos.y;
      v->z = v->z*scale+pos.z;
    }

    // Transform the data of the face for OpenGL rendering
    for( j=0; j<aObject[i].numMatFaces; j++ )
    {
      aObject[i].FileFace2SubFace( j );
    }

    m_iNumFace += aObject[i].numFaces;
  }
  scale = 1.0f;
  pos.x = pos.y = pos.z = 0;

  // Calculate the vertex normals
  UnitNormals();

  // Find the total number of faces and vertices
  totalFaces = 0;
  totalVerts = 0;

  for( i=0; i<numObjects; i++ )
  {
    totalFaces += aObject[i].numFaces;
    totalVerts += aObject[i].numRealVertex;
  }

  // If the object doesn't have any texcoords generate some
  for( i=0; i<numObjects; i++ )
  {
    if( aObject[i].numTexCoords==0 )
    {
      aObject[i].SetNumTexCoords( aObject[i].numRealVertex );

      // Make some texture coords
      for( j=0; j<aObject[i].numTexCoords; j++)
      {
        aObject[i].aTexCoord[j].x = aObject[i].aRealVertex[j].x;
        aObject[i].aTexCoord[j].y = aObject[i].aRealVertex[j].y;
      }
    }
  }

  // Find if transparent and which object is textured
  for( i=0; i<numObjects; i++ )
  {
    aObject[i].bTextured = false;
    for( j=0; j<aObject[i].numMatFaces; j++ )
    {
      int matindex = aObject[i].aMatFace[j].MatIndex;
      if( aMaterial[matindex].iTransparency!=0 )
      {
        m_bTransparent = true;
      } 
      if( aMaterial[matindex].bTextured )
      {
        aObject[i].bTextured = true;
      } 
    }
  }

  // Copy the point to the Video memory
  for( i=0; i<numObjects; i++ )
  {
    Vertex * a = aObject[i].aRealVertex;
    Vect3D * b = aObject[i].aNormal;
    aObject[i].aRealVertex = (Vertex *) opengl.VideoMemoryAlloc( aObject[i].numRealVertex*sizeof(Vertex) );
    aObject[i].aNormal = (Vect3D *) opengl.VideoMemoryAlloc( aObject[i].numRealVertex*sizeof(Vect3D) );
    for( j=0; j<aObject[i].numRealVertex; j++ )
    {
      aObject[i].aRealVertex[j] = a[j];
      aObject[i].aNormal[j]     = b[j];
    }
    delete a;
    delete b;

    Vect2D * c = aObject[i].aTexCoord;
    aObject[i].aTexCoord = (Vect2D *) opengl.VideoMemoryAlloc( aObject[i].numTexCoords*sizeof(Vect2D) );
    for( j=0; j<aObject[i].numTexCoords; j++ )
    {
      aObject[i].aTexCoord[j] = c[j];
    }
    delete c;
  }
}

/**
 * Draw the model
 */
void Model_3DS::Draw()
{
  if( m_bVisible )
  {
    glPushMatrix();

    // Move the model
    glTranslatef(pos.x, pos.y, pos.z);

    // Rotate the model
    glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

    DrawObjects( false );
    DrawObjects( true );

    glPopMatrix();
  }
}

/**
 * Draw the model
 */
void Model_3DS::DrawList( bool bTransparent )
{
  if( m_bVisible )
  {
    glPushMatrix();

    if( m_iOpenglId<0 )
    {
      // The lists are automatally created in the first call.
      glCreateList();
    }

    // Move the model
    glTranslatef(pos.x, pos.y, pos.z);

    // Rotate the model
    glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rot.z, 0.0f, 0.0f, 1.0f);

    if( bTransparent )
    {
      glCallList( m_iOpenglIdTransp );
    }
    else
    {
      glCallList( m_iOpenglId );
    }
    glPopMatrix();
  }
  glDisable(GL_TEXTURE_2D);
}

/**
 * Draw the objects (First not transparent -> transparent )
 */
void Model_3DS::DrawObjects( bool bTransparent )
{
  // Loop through the objects
  for( int i=0; i<numObjects; i++ )
  {
    if( m_bEnvMap )
    {
	    glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      //Bind the texture.
      glBindTexture(GL_TEXTURE_2D, g_TextureEnvMap->m_iOpenglId);
      //Use sphere mapping auto coord generation.
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      //Turn on auto coord generation. N.B.! This is a per texture unit operation here!
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      //Multiply this texture by the ships material colors.
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MULT);
  	  glActiveTextureARB(GL_TEXTURE0_ARB);
    }

    // Enable texture coordiantes, normals, and vertices arrays
    glEnableClientState(GL_VERTEX_ARRAY);

    if( aObject[i].bTextured )
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, aObject[i].aTexCoord);
      if( m_bEnvMap )
      {
        // TODO - Does not work
        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
      }
      else
      {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MULT);
      }
    }
    else
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if( m_bLit )
    {
      glEnableClientState(GL_NORMAL_ARRAY);
    }
    else
    {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
    // Point them to the objects arrays

    glVertexPointer(3, GL_FLOAT, 0, aObject[i].aRealVertex);
    if( aObject[i].bTextured )
    {
      glTexCoordPointer(2, GL_FLOAT, 0, aObject[i].aTexCoord);
    }
    if( m_bLit )
    {
      glNormalPointer(GL_FLOAT, 0, aObject[i].aNormal);
    }

    // Loop through the faces as sorted by material and draw them
    for( int j=0; j<aObject[i].numMatFaces; j++ )
    {
      int matindex = aObject[i].aMatFace[j].MatIndex;
      bool mat_transparent = aMaterial[matindex].iTransparency!=0;

      if( mat_transparent==bTransparent )
      {
        if( m_bMaterial )
        {
          // Use the material's texture
          float a = 1.0f;
          if( mat_transparent )
          {
            glEnable(GL_BLEND);
            a = (100.0f-aMaterial[matindex].iTransparency)/100.0f;
          }
      
          if( aMaterial[matindex].bTextured )
          {
            aMaterial[matindex].tex->Use();
            GLfloat mat1[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat1 );
            glEnable(GL_TEXTURE_2D);
          }
          else
          {
            float r = aMaterial[matindex].color.r/255.0f;
            float g = aMaterial[matindex].color.g/255.0f;
            float b = aMaterial[matindex].color.b/255.0f;
            GLfloat mat2[4]  = { r, g, b, a };
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat2 );
            glDisable(GL_TEXTURE_2D);
          }
        }

        glPushMatrix();

        // Move the model (only if needed -> the model is compiled into a OpenGL list)
        if( aObject[i].pos.x!=0.0 || aObject[i].pos.y!=0.0 || aObject[i].pos.z!=0.0 )
        {
          glTranslatef(aObject[i].pos.x, aObject[i].pos.y, aObject[i].pos.z);
        }
        if( aObject[i].rot.z!=0.0 )
        {
          glRotatef(aObject[i].rot.z, 0.0f, 0.0f, 1.0f);
        }
        if( aObject[i].rot.y!=0.0 )
        {
          glRotatef(aObject[i].rot.y, 0.0f, 1.0f, 0.0f);
        }
        if( aObject[i].rot.x!=0.0 )
        {
          glRotatef(aObject[i].rot.x, 1.0f, 0.0f, 0.0f);
        }
        // Draw the faces using an index to the vertex array
        glDrawElements(GL_TRIANGLES, aObject[i].aMatFace[j].numSubFaces*3, GL_UNSIGNED_SHORT, aObject[i].aMatFace[j].aSubFace);

        if( mat_transparent )
        {
          glDisable(GL_BLEND);
        }

        glPopMatrix();
      }
    }
    if( m_bEnvMap )
    {
	    glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
  	  glActiveTextureARB(GL_TEXTURE0_ARB);
    }

    /*
    // Loop through the faces as sorted by material and draw them
    for( int j=0; j<aObject[i].numMatFaces; j++ )
    {
      for( int k=0; k<aObject[i].aMatFace[j].numSubFaces; k++ )
      {
        int v1 = aObject[i].aMatFace[j].aSubFace[k].a;
        int v2 = aObject[i].aMatFace[j].aSubFace[k].b;
        int v3 = aObject[i].aMatFace[j].aSubFace[k].c;
        glBegin (GL_TRIANGLES);
        // Materials[aObject[i].MatFaces[j].MatIndex].tex->Use();
        glNormal3f( aObject[i].aNormal[v1].x, aObject[i].aNormal[v1].y, aObject[i].aNormal[v1].z );
        glVertex3f( aObject[i].aRealVertex[v1].x, aObject[i].aRealVertex[v1].y, aObject[i].aRealVertex[v1].z );
        glNormal3f( aObject[i].aNormal[v2].x, aObject[i].aNormal[v2].y, aObject[i].aNormal[v2].z );
        glVertex3f( aObject[i].aRealVertex[v2].x, aObject[i].aRealVertex[v2].y, aObject[i].aRealVertex[v2].z );
        glNormal3f( aObject[i].aNormal[v3].x, aObject[i].aNormal[v3].y, aObject[i].aNormal[v3].z );
        glVertex3f( aObject[i].aRealVertex[v3].x, aObject[i].aRealVertex[v3].y, aObject[i].aRealVertex[v3].z );
        glEnd ();
      }
    }
    */
      
    // Show the normals?
    if( m_bShowNormal )
    {
      // Loop through the vertices and normals and draw the normal
      for( int k=0; k<aObject[i].numRealVertex; k++ )
      {
        // Disable texturing
        glDisable(GL_TEXTURE_2D);
        // Disbale lighting if the model is lit
        if( m_bLit )
        {
          glDisable(GL_LIGHTING);
        }
        // Draw the normals blue
        glColor3f(0.0f, 0.0f, 1.0f);

        // Draw a line between the vertex and the end of the normal
        glBegin(GL_LINES);
          glVertex3f(aObject[i].aRealVertex[k].x, aObject[i].aRealVertex[k].y, aObject[i].aRealVertex[k].z);
          glVertex3f(aObject[i].aRealVertex[k].x+aObject[i].aNormal[k].x, aObject[i].aRealVertex[k].y+aObject[i].aNormal[k].y, aObject[i].aRealVertex[k].z+aObject[i].aNormal[k].z);
        glEnd();

        // Reset the color to white
        glColor3f(1.0f, 1.0f, 1.0f);
        // If the model is lit then renable lighting
        if( m_bLit )
        {
          glEnable(GL_LIGHTING);
        }
      }
    }
  }
}

void Model_3DS::UnitNormals()
{
   // Reduce each vert's normal to unit
  for (int i = 0; i < numObjects; i++)
  {
    for (int j = 0; j<aObject[i].numRealVertex; j++)
    {
      float length;
      Vect3D unit;

      unit.x = aObject[i].aNormal[j].x;
      unit.y = aObject[i].aNormal[j].y;
      unit.z = aObject[i].aNormal[j].z;

      length = (float)sqrt((unit.x*unit.x) + (unit.y*unit.y) + (unit.z*unit.z));

      if (length == 0.0f)
        length = 1.0f;

      unit.x /= length;
      unit.y /= length;
      unit.z /= length;

      aObject[i].aNormal[j].x = unit.x;
      aObject[i].aNormal[j].y = unit.y;
      aObject[i].aNormal[j].z = unit.z;
    }
  }
}

void Model_3DS::MainChunkProcessor(long length, long findex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      // This is the mesh information like vertices, faces, and materials
      case CHUNK_EDIT3DS  :
        EditChunkProcessor(h.len, ftell(bin3ds));
        break;
      // I left this in case anyone gets very ambitious
      case CHUNK_KEYF3DS  :
        //KeyFrameChunkProcessor(h.len, ftell(bin3ds));
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::EditChunkProcessor(long length, long findex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // First count the number of Objects and Materials
  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_OBJECT :
        numObjects++;
        break;
      case CHUNK_MATERIAL :
        numMaterials++;
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // Now load the materials
  if (numMaterials > 0)
  {
    aMaterial = new Material[numMaterials];
    fseek(bin3ds, findex, SEEK_SET);

    int i = 0;

    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
        case CHUNK_MATERIAL :
          MaterialChunkProcessor(h.len, ftell(bin3ds), i);
          i++;
          break;
        default     :
          break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }
  }

  // Load the Objects (individual meshes in the whole model)
  if (numObjects > 0)
  {
    aObject = new Object[numObjects];

    fseek(bin3ds, findex, SEEK_SET);

    int j = 0;

    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
        case CHUNK_OBJECT :
          ObjectChunkProcessor(h.len, ftell(bin3ds), j);
          j++;
          break;
        default     :
          break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }

  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::MaterialChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_MAT_NAME :
        // Loads the material's names
        MaterialNameChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_MAT_AMBIENT  :
        //ColorChunkProcessor(h.len, ftell(bin3ds));
        break;
      case CHUNK_MAT_DIFFUSE  :
        DiffuseColorChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_MAT_SPECULAR :
        //ColorChunkProcessor(h.len, ftell(bin3ds));
      case CHUNK_MAT_TEXMAP :
        // Finds the names of the textures of the material and loads them
        TextureMapChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_MAT_TRANSP :
        // Finds the names of the textures of the material and loads them
        TransparencyChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::MaterialNameChunkProcessor(long /*length*/, long findex, int matindex)
{
  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the material's name
  for (int i = 0; i < 80; i++)
  {
    aMaterial[matindex].name[i] = fgetc(bin3ds);
    if( aMaterial[matindex].name[i]==0 )
    {
      aMaterial[matindex].name[i] = 0;
      break;
    }
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::DiffuseColorChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    // Determine the format of the color and load it
    switch (h.id)
    {
      case CHUNK_COLOR_FRGB  :
        // A rgb float color chunk
        FloatColorChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_COLOR_IRGB  :
        // A rgb int color chunk
        IntColorChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_COLOR_FRGBA :
        // A rgb gamma corrected float color chunk
        FloatColorChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      case CHUNK_COLOR_IRGBA :
        // A rgb gamma corrected int color chunk
        IntColorChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::TransparencyChunkProcessor(long length, long findex, int matindex)
{
  unsigned short t;
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  fread(&h.id,sizeof(h.id),1,bin3ds);
  fread(&h.len,sizeof(h.len),1,bin3ds);
  fread(&t,sizeof(t),1,bin3ds);

  aMaterial[matindex].iTransparency = t;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}


void Model_3DS::FloatColorChunkProcessor(long /*length*/, long findex, int matindex)
{
  float r;
  float g;
  float b;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  fread(&r,sizeof(r),1,bin3ds);
  fread(&g,sizeof(g),1,bin3ds);
  fread(&b,sizeof(b),1,bin3ds);

  aMaterial[matindex].color.r = (unsigned char)(r*255.0f);
  aMaterial[matindex].color.g = (unsigned char)(r*255.0f);
  aMaterial[matindex].color.b = (unsigned char)(r*255.0f);
  aMaterial[matindex].color.a = 255;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::IntColorChunkProcessor(long /*length*/, long findex, int matindex)
{
  unsigned char r;
  unsigned char g;
  unsigned char b;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  fread(&r,sizeof(r),1,bin3ds);
  fread(&g,sizeof(g),1,bin3ds);
  fread(&b,sizeof(b),1,bin3ds);

  aMaterial[matindex].color.r = r;
  aMaterial[matindex].color.g = g;
  aMaterial[matindex].color.b = b;
  aMaterial[matindex].color.a = 255;

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::TextureMapChunkProcessor(long length, long findex, int matindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_MAT_MAPNAME:
        // Read the name of texture in the Diffuse Color map
        MapNameChunkProcessor(h.len, ftell(bin3ds), matindex);
        break;
      default:
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::MapNameChunkProcessor(long /*length*/, long findex, int matindex)
{
  char name[80];

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the name of the texture
  for (int i = 0; i < 80; i++)
  {
    name[i] = fgetc(bin3ds);
    if (name[i] == 0)
    {
      name[i] = 0;
      break;
    }
  }

  // Load the name and indicate that the material has a texture
  aMaterial[matindex].tex = GLTexture::GetLoadedTexture( path, name, true ); 
  aMaterial[matindex].bTextured = true;
  if( aMaterial[matindex].tex->type==GL_RGBA )
  {
     aMaterial[matindex].iTransparency = 1; // Dummy value 
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::ObjectChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Load the object's name
  for( int i=0; i<80; i++ )
  {
    aObject[objindex].name[i] = fgetc(bin3ds);
    if( aObject[objindex].name[i]==0 )
    {
      aObject[objindex].name[i] = 0;
      break;
    }
  }

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_TRI_MESH  :
        // Process the triangles of the object
        TriangularMeshChunkProcessor(h.len, ftell(bin3ds), objindex);
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::TriangularMeshChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_VERT_LIST  :
        // Load the vertices of the onject
        VertexListChunkProcessor(h.len, ftell(bin3ds), objindex);
        break;
      case LOCAL_COORDS :
        //LocalCoordinatesChunkProcessor(h.len, ftell(bin3ds));
        break;
      case CHUNK_TEX_COORDS  :
        // Load the texture coordinates for the vertices
        TexCoordsChunkProcessor(h.len, ftell(bin3ds), objindex);
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // After we have loaded the vertices we can load the faces
  fseek(bin3ds, findex, SEEK_SET);

  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_FACE_LIST :
        // Load the faces of the object
        FacesDescriptionChunkProcessor(h.len, ftell(bin3ds), objindex);
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::VertexListChunkProcessor(long /*length*/, long findex, int objindex)
{
  int i, bFound;
  unsigned short numFileVertex, numRealVertex;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of vertices of the object
  fread(&numFileVertex,sizeof(numFileVertex),1,bin3ds);
  aObject[objindex].SetNumFileVerts(numFileVertex);

  // Read the vertices
  for( i=0; i<numFileVertex; i++ )
  {
    fread(&aObject[objindex].aFileVertex[i].x,sizeof(float),1,bin3ds);
    fread(&aObject[objindex].aFileVertex[i].y,sizeof(float),1,bin3ds);
    fread(&aObject[objindex].aFileVertex[i].z,sizeof(float),1,bin3ds);
  }

  // Check for duplicate vertexes 
  numRealVertex = 0; 
  for( i=0; i<numFileVertex; i++ )
  {
    bFound = false;
    /*
    for( int j=0; j<numRealVertex; j++ )
    {
      if( (aObject[objindex].aFileVertex[i].x==aObject[objindex].aRealVertex[j].x)
       && (aObject[objindex].aFileVertex[i].y==aObject[objindex].aRealVertex[j].y)
       && (aObject[objindex].aFileVertex[i].z==aObject[objindex].aRealVertex[j].z) ) 
      {
        aObject[objindex].aFile2Real[i] = j; 
        bFound = true;
        // TODO
        break;
      }
    }
    */
    if( !bFound )
    {
      aObject[objindex].aRealVertex[numRealVertex].x = aObject[objindex].aFileVertex[i].x;
      aObject[objindex].aRealVertex[numRealVertex].y = aObject[objindex].aFileVertex[i].y;
      aObject[objindex].aRealVertex[numRealVertex].z = aObject[objindex].aFileVertex[i].z;
      aObject[objindex].aFile2Real[i] = numRealVertex;
      numRealVertex ++;
    }
  }
  aObject[objindex].numRealVertex = numRealVertex; 

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::TexCoordsChunkProcessor(long /*length*/, long findex, int objindex)
{
  // The number of texture coordinates
  unsigned short numCoords;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of coordinates
  fread(&numCoords,sizeof(numCoords),1,bin3ds);
  if( numCoords!=0 )
  {
    aObject[objindex].SetNumTexCoords( numCoords );

    // Read teh texture coordiantes into the array
    for( int i=0; i<numCoords; i++ )
    {
      fread(&aObject[objindex].aTexCoord[i].x,sizeof(GLfloat),1,bin3ds);
      fread(&aObject[objindex].aTexCoord[i].y,sizeof(GLfloat),1,bin3ds);
    }
  }
  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::FacesDescriptionChunkProcessor(long length, long findex, int objindex)
{
  ChunkHeader h;
  unsigned short numFaces;  // The number of faces in the object
  unsigned short vertA;   // The first vertex of the face
  unsigned short vertB;   // The second vertex of the face
  unsigned short vertC;   // The third vertex of the face
  unsigned short flags;   // The winding order flags
  long subs;          // Holds our place in the file
  int numMatFaces = 0;    // The number of different materials

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the number of faces
  fread(&numFaces,sizeof(numFaces),1,bin3ds);

  aObject[objindex].SetNumFaces(numFaces);

  // Read the faces into the array
  for( int i=0; i<numFaces; i++ )
  {
    // Read the vertices of the face
    fread(&vertA,sizeof(vertA),1,bin3ds);
    fread(&vertB,sizeof(vertB),1,bin3ds);
    fread(&vertC,sizeof(vertC),1,bin3ds);
    fread(&flags,sizeof(flags),1,bin3ds);

    // Get the id of the real point (not the duplicate ones)
    vertA = aObject[objindex].aFile2Real[vertA];
    vertB = aObject[objindex].aFile2Real[vertB];
    vertC = aObject[objindex].aFile2Real[vertC];

    // Place them in the array
    aObject[objindex].aFace[i].a = vertA;
    aObject[objindex].aFace[i].b = vertB;
    aObject[objindex].aFace[i].c = vertC;

    aObject[objindex].CalcFaceNormal( vertA, vertB, vertC ); 
  }

  // Store our current file position
  subs = ftell(bin3ds);

  // Check to see how many materials the faces are split into
  while (ftell(bin3ds) < (findex + length - 6))
  {
    fread(&h.id,sizeof(h.id),1,bin3ds);
    fread(&h.len,sizeof(h.len),1,bin3ds);

    switch (h.id)
    {
      case CHUNK_FACE_MAT :
        //FacesMaterialsListChunkProcessor(h.len, ftell(bin3ds), objindex);
        numMatFaces++;
        break;
      default     :
        break;
    }

    fseek(bin3ds, (h.len - 6), SEEK_CUR);
  }

  // Split the faces up according to their materials
  if (numMatFaces > 0)
  {
    aObject[objindex].SetNumMatFaces( numMatFaces );
    fseek(bin3ds, subs, SEEK_SET);

    int j = 0;
    // Split the faces up
    while (ftell(bin3ds) < (findex + length - 6))
    {
      fread(&h.id,sizeof(h.id),1,bin3ds);
      fread(&h.len,sizeof(h.len),1,bin3ds);

      switch (h.id)
      {
        case CHUNK_FACE_MAT :
          // Process the faces and split them up
          FacesMaterialsListChunkProcessor(h.len, ftell(bin3ds), objindex, j);
          j++;
          break;
        default     :
          break;
      }

      fseek(bin3ds, (h.len - 6), SEEK_CUR);
    }
  }

  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

/**
 * Calculate the face's normal
 */
void Model_3DS::Object::CalcFaceNormal( int a, int b, int c )
{
  Vect3D n;
  Vertex v1;
  Vertex v2;
  Vertex v3;
  float n_length;

  v1.x = aRealVertex[a].x;
  v1.y = aRealVertex[a].y;
  v1.z = aRealVertex[a].z;
  v2.x = aRealVertex[b].x;
  v2.y = aRealVertex[b].y;
  v2.z = aRealVertex[b].z;
  v3.x = aRealVertex[c].x;
  v3.y = aRealVertex[c].y;
  v3.z = aRealVertex[c].z;

  // calculate the normal
  float u[3], v[3];

  // V2 - V3;
  u[0] = v2.x - v3.x;
  u[1] = v2.y - v3.y;
  u[2] = v2.z - v3.z;

  // V2 - V1;
  v[0] = v2.x - v1.x;
  v[1] = v2.y - v1.y;
  v[2] = v2.z - v1.z;

  n.x = (u[1]*v[2] - u[2]*v[1]);
  n.y = (u[2]*v[0] - u[0]*v[2]);
  n.z = (u[0]*v[1] - u[1]*v[0]);

  n_length = (float)sqrt((n.x*n.x) + (n.y*n.y) + (n.z*n.z));

  if (n_length == 0.0f)
  {
    n_length = 1.0f;
  }

  n.x /= n_length;
  n.y /= n_length;
  n.z /= n_length;

  // Add this normal to its verts' normals
  aNormal[a].x += n.x;
  aNormal[a].y += n.y;
  aNormal[a].z += n.z;
  aNormal[b].x += n.x;
  aNormal[b].y += n.y;
  aNormal[b].z += n.z;
  aNormal[c].x += n.x;
  aNormal[c].y += n.y;
  aNormal[c].z += n.z;
}

void Model_3DS::FacesMaterialsListChunkProcessor(long /*length*/, long findex, int objindex, int subfacesindex)
{
  char name[80];        // The material's name
  unsigned short numEntries;  // The number of faces associated with this material
  unsigned short iFace; // Holds the faces as they are read
  int material;         // An index to the Materials array for this material
  int i;

  // move the file pointer to the beginning of the main
  // chunk's data findex + the size of the header
  fseek(bin3ds, findex, SEEK_SET);

  // Read the material's name
  for( i=0; i<80; i++ )
  {
    name[i] = fgetc(bin3ds);
    if (name[i] == 0)
    {
      name[i] = 0;
      break;
    }
  }

  // Find the material's index in the Materials array
  for( material=0; material<numMaterials; material++ )
  {
    if( strcmp(name, aMaterial[material].name)==0 )
      break;
  }

  // Store this value for later so that we can find the material
  aObject[objindex].aMatFace[subfacesindex].MatIndex = material;

  // Read the number of faces associated with this material
  fread(&numEntries,sizeof(numEntries),1,bin3ds);

  // Allocate an array to hold the list of faces associated with this material
  // Store this number for later use
  aObject[objindex].aMatFace[subfacesindex].aFileFace = new GLushort[numEntries];
  aObject[objindex].aMatFace[subfacesindex].numFileFaces = numEntries;

  // Read the faces into the array
  for( i=0; i<numEntries; i++ )
  {
    // read the face
    fread(&iFace,sizeof(iFace),1,bin3ds);

    aObject[objindex].aMatFace[subfacesindex].aFileFace[i] = iFace;
  }
  
  // move the file pointer back to where we got it so
  // that the ProcessChunk() which we interrupted will read
  // from the right place
  fseek(bin3ds, findex, SEEK_SET);
}

void Model_3DS::Object::FileFace2SubFace( int subfacesindex )
{
  MaterialFaces * mf = &( aMatFace[subfacesindex] );

  mf->numSubFaces = mf->numFileFaces;
  mf->aSubFace = new Face[mf->numSubFaces];

  for( int i=0; i<mf->numFileFaces; i++ )
  {
    // Add the face's vertices to the list
    mf->aSubFace[i].a = aFace[ mf->aFileFace[i] ].a;
    mf->aSubFace[i].b = aFace[ mf->aFileFace[i] ].b;
    mf->aSubFace[i].c = aFace[ mf->aFileFace[i] ].c;
  }
}

/**
 * Create an Opengl List
 */
void Model_3DS::glCreateList()
{
  // Generate the OpenGL texture id
  m_iOpenglId = glGenLists(1);
  glNewList( m_iOpenglId, GL_COMPILE);
  DrawObjects( false );
  glEndList();
  if( m_bTransparent )
  {
    m_iOpenglIdTransp = glGenLists(1);
    glNewList( m_iOpenglIdTransp, GL_COMPILE);
    DrawObjects( true );
    glEndList();
  }
}

/**
 * Delete an Opengl List
 */
void Model_3DS::glDeleteList()
{
  if( m_iOpenglId>0 )
  {
    glDeleteLists( m_iOpenglId, 1 );
    m_iOpenglId = -1;
  }
  if( m_iOpenglIdTransp>0 )
  {
    glDeleteLists( m_iOpenglIdTransp, 1 );
    m_iOpenglIdTransp = -1;
  }
}

/**
 * Calculate the center and the ray of the sphere containing the object
 */
void Model_3DS::CalculateSphere()
{
  int i, j;
  bool bInit = true;

  for( i=0; i<numObjects; i++ )
  {
    for( j=0; j<aObject[i].numRealVertex; j++ )
    {
      float x = aObject[i].aRealVertex[j].x;
      float y = aObject[i].aRealVertex[j].y;
      float z = aObject[i].aRealVertex[j].z;

      if( bInit )
      {
        m_fMaxX = m_fMinX = x;
        m_fMaxY = m_fMinY = y;
        m_fMaxZ = m_fMinZ = z;
        bInit = false;
      }

      if( x>m_fMaxX ) 
        m_fMaxX = x;
      else if ( x<m_fMinX) 
        m_fMinX = x;
      if( y>m_fMaxY )
        m_fMaxY = y;
      else if ( y<m_fMinY)
        m_fMinY = y;
      if( z>m_fMaxZ ) 
        m_fMaxZ = z;
      else if ( z<m_fMinZ) 
        m_fMinZ = z;
    }
  }
  m_fCenterX = (m_fMinX+m_fMaxX)/2;
  m_fCenterY = (m_fMinY+m_fMaxY)/2;
  m_fCenterZ = (m_fMinZ+m_fMaxZ)/2;

  m_fRay = 0;
  for( i=0; i<numObjects; i++ )
  {
    for( j=0; j<aObject[i].numRealVertex; j++ )
    {
      float dx = aObject[i].aRealVertex[j].x-m_fCenterX;
      float dy = aObject[i].aRealVertex[j].y-m_fCenterY;
      float dz = aObject[i].aRealVertex[j].z-m_fCenterZ;

      float ray = (float) sqrt( dx*dx + dy*dy + dz*dz );
      if( ray>m_fRay )
      {
        m_fRay = ray;
      }
    }
  }
}

/**
 * Flatten
 */
void Model_3DS::Flatten( int coord )
{
  // Flattent the given coordinate
  for( int i=0; i<numObjects; i++ )
  {
    for( int j=0; j<aObject[i].numRealVertex; j++ )
    {
      if( coord==0 )
      {
        aObject[i].aRealVertex[j].x = 0.0;
      }
      else if( coord==1 )
      {
        aObject[i].aRealVertex[j].y = 0.0;
      }
      else
      {
        aObject[i].aRealVertex[j].z = 0.0;
      }
    }
  }
}

/**
 * Recenter the object in (0,0,0)
 */
void Model_3DS::Recenter()
{
  // Flattent the given coordinate
  for( int i = 0; i < numObjects; i++ )
  {
    for( int j = 0; j<aObject[i].numRealVertex; j++ )
    {
      aObject[i].aRealVertex[j].x -= m_fCenterX;
      aObject[i].aRealVertex[j].y -= m_fCenterY;
      aObject[i].aRealVertex[j].z -= m_fCenterZ;
    }
  }
}

/**
 * Save
 */
void Model_3DS::Save( const char * path, const char * name )
{
  int i, j;

  ///
  /// Construct the chunks
  /// Special indentation to see CHUNKS structure
  ///
  ChunkMain3DS * c_main    = new ChunkMain3DS();
  new ChunkDWord3DS( c_main, 2, 0 );
  Chunk3DS * c_edit3ds = new Chunk3DS( c_main, CHUNK_EDIT3DS );
  new ChunkDWord3DS( c_edit3ds, 0x3D3E, 0 );

  for( i=0; i<numMaterials; i++ )
  {
    Material * mat = &( aMaterial[i] );

    Chunk3DS * c_mat = new Chunk3DS( c_edit3ds, CHUNK_MATERIAL );
      new ChunkString3DS( c_mat, CHUNK_MAT_NAME, mat->name );
      Chunk3DS * c_ambient = new Chunk3DS( c_mat, CHUNK_MAT_AMBIENT );
        new ChunkIRGB3DS( c_ambient, 192, 192, 192 );
      Chunk3DS * c_diffuse = new Chunk3DS( c_mat, CHUNK_MAT_DIFFUSE );
        new ChunkIRGB3DS( c_diffuse, mat->color.r, mat->color.g, mat->color.b );
      Chunk3DS * c_specular = new Chunk3DS( c_mat, CHUNK_MAT_SPECULAR );
        new ChunkIRGB3DS( c_specular, 255, 255, 255 );
      Chunk3DS * c_shininess = new Chunk3DS( c_mat, CHUNK_SHINY );
        new ChunkWord3DS( c_shininess, CHUNK_WORD, 20 );
      Chunk3DS * c_shiny_str = new Chunk3DS( c_mat, CHUNK_SHINY_STR );
        new ChunkWord3DS( c_shiny_str, CHUNK_WORD, 100 );
      Chunk3DS * c_transparency = new Chunk3DS( c_mat, CHUNK_MAT_TRANSP );
        new ChunkWord3DS( c_transparency, CHUNK_WORD, mat->iTransparency );
      Chunk3DS * c_trans_foff = new Chunk3DS( c_mat, CHUNK_TRANS_FOFF );
        new ChunkWord3DS( c_trans_foff, CHUNK_WORD, 0 );
      Chunk3DS * c_ref_blur = new Chunk3DS( c_mat, CHUNK_REF_BLUR );
        new ChunkWord3DS( c_ref_blur, CHUNK_WORD, 0 );
      Chunk3DS * c_self_illum = new Chunk3DS( c_mat, CHUNK_SELF_ILLUM );
        new ChunkWord3DS( c_self_illum, CHUNK_WORD, 0 );
      new ChunkFloat3DS( c_mat, CHUNK_WIRE_THICK, 1.0f );

      if( mat->bTextured )
      {
        Chunk3DS * c_texmap = new Chunk3DS( c_mat, CHUNK_MAT_TEXMAP );
          new ChunkString3DS( c_texmap, CHUNK_MAT_MAPNAME, mat->tex->m_sTextureName );
      }
  }

  new ChunkDWord3DS( c_edit3ds, 0x0100, 0 );


  for( i=0; i<numObjects; i++ )
  {
    Object * obj = &( aObject[i] );

    Chunk3DS * c_object = new ChunkString3DS( c_edit3ds, CHUNK_OBJECT, obj->name );

      Chunk3DS * c_tri_mesh = new Chunk3DS( c_object, CHUNK_TRI_MESH );
        new ChunkVertexList3DS( c_tri_mesh, obj->numFileVertex, obj->aFileVertex );
        new ChunkTexCoord3DS( c_tri_mesh, obj->numTexCoords, obj->aTexCoord );
        Chunk3DS * c_flist = new ChunkFaceList3DS( c_tri_mesh, obj->numFaces, obj->aFace );

        for( j=0; j<obj->numMatFaces; j++ )
        {
          int matindex = obj->aMatFace[j].MatIndex;
          new ChunkFaceMat3DS( c_flist, aMaterial[matindex].name, obj->aMatFace[j].numFileFaces, obj->aMatFace[j].aFileFace );
        }
  }

  ///
  /// Calculate the lengths
  ///
  c_main->CalcLength();

  ///
  /// Save the 3ds file
  ///
  char filename[256];
  sprintf( filename, "%s/%s", path, name );
  FILE * out= fopen(filename,"wb");

  if( out==NULL )
  {
    // Try to create the directory
    os.CreateDirectory( path );
    out = fopen(filename,"wb");
  }

  if( out==NULL )
  {
    warning("Model_3DS::Save : can not create file '%s'", filename );
  } 
  else
  {
    c_main->Save( out );
    fclose( out );
  }

  ///
  /// Cleanup
  ///
  delete c_main;
}

//---------------------------------------------------------------------------
//                         Creation of 3DS objects
//---------------------------------------------------------------------------

#define CR_MAX_OBJECTS   15 
#define CR_MAX_MATERIALS 10 
#define CR_MAX_FILEVERTS 600 
#define CR_MAX_FACES     600 
#define CR_MAX_MATFACES  10

/**
 * Add a new object
 */
void Model_3DS::Create()
{
  aObject = new Object[CR_MAX_OBJECTS];
  numObjects = 0;
  aMaterial = new Material[CR_MAX_MATERIALS];
  numMaterials = 0;
}

/**
 * Add a new object
 */
int Model_3DS::AddObject()
{
  int id = numObjects;

  aObject[id].SetNumFileVerts( CR_MAX_FILEVERTS );
  aObject[id].numFileVertex = 0;
  aObject[id].SetNumFaces( CR_MAX_FACES );
  aObject[id].numFaces = 0;
  aObject[id].SetNumTexCoords( CR_MAX_FILEVERTS );
  aObject[id].numTexCoords = 0;
  aObject[id].SetNumMatFaces( CR_MAX_MATFACES );
  aObject[id].numMatFaces = 0;

  numObjects ++;
  if( numObjects>CR_MAX_OBJECTS )
  {
    exitOnError( "3DS: Increase CR_MAX_OBJECTS" );
  }

  return id;
}

/**
 * Add a texture material to the model
 *
 * @param sPath : path (must finish with /)
 * @param sName : name of the texture
 */
int Model_3DS::AddMaterialTexture( const char * sPath, const char * sName )
{
  // First, look if the texture is not already there
  for( int i=0; i<numMaterials; i++ )
  {
    if( strcmp(aMaterial[i].name, sName)==0 )
    {
      return i;
    }
  }
  int id = numMaterials;
  aMaterial[id].bTextured = true;
  sprintf( aMaterial[id].name, "%s.bmp", sName );

  aMaterial[id].tex = GLTexture::GetLoadedTexture( sPath, aMaterial[id].name, true ); 
  if( aMaterial[id].tex->type==GL_RGBA )
  {
     aMaterial[id].iTransparency = 1; // Dummy value 
  }

  numMaterials ++;
  if( numMaterials>CR_MAX_MATERIALS )
  {
    exitOnError( "3DS: Increase CR_MAX_MATERIALS" );
  }
  return id;
}

/**
 * Add a color material 
 *
 * @param r, g, b : color( 0-255)
 */
int Model_3DS::AddMaterialColor( unsigned char r, unsigned char g, unsigned char b )
{
  int id = numMaterials;
  aMaterial[id].bTextured = false;
  aMaterial[id].color.r = r;
  aMaterial[id].color.g = g;
  aMaterial[id].color.b = b;

  numMaterials ++;
  if( numMaterials>CR_MAX_MATERIALS )
  {
    exitOnError( "3DS: Increase CR_MAX_MATERIALS" );
  }
  return id;
}

/**
 * Add a vertex to an object
 * v : coordinates
 * t : texture coord.
 */
int Model_3DS::Object::AddVertex( Vertex v, Vect2D t, bool bCheckDuplicate )
{
  if( bCheckDuplicate )
  {
    for( int i=0; i<numFileVertex; i++ )
    {
      // Check only the point and not the texture coordinate
      if( aFileVertex[i].x==v.x && aFileVertex[i].y==v.y && aFileVertex[i].z==v.z )
      {
        return i;
      }
    }
  }

  int id = numFileVertex;
  aFileVertex[id] = v;
  aRealVertex[id] = v;
  aTexCoord[id] = t;
  aFile2Real[id] = id;

  numFileVertex ++;
  if( numFileVertex>CR_MAX_FILEVERTS )
  {
    exitOnError( "3DS: Increase CR_MAX_FILEVERTS" );
  }
  numTexCoords = numFileVertex;
  numRealVertex = numFileVertex;

  return id;
}

/**
 * Add a material to an object
 */
int Model_3DS::Object::AddMatFace( int iMaterial )
{
  // First, look if the material is not already there
  for( int i=0; i<numMatFaces; i++ )
  {
    if( aMatFace[i].MatIndex==iMaterial )
    {
      return i;
    }
  }

  int id = numMatFaces;
  aMatFace[id].MatIndex = iMaterial;
  aMatFace[id].aFileFace = new GLushort[CR_MAX_FACES];

  numMatFaces ++;
  if( numMatFaces>CR_MAX_MATFACES )
  {
    exitOnError( "3DS: Increase CR_MAX_MATFACES" );
  }

  return id;
}

/**
 * Add a rectangle to the model (2 faces)
 */
void Model_3DS::Object::AddRect( int iMatFace, int p0, int p1, int p2, int p3 )
{
  AddFace( iMatFace, p0, p1, p2 );
  AddFace( iMatFace, p0, p2, p3 );
}

/**
 * Add a rectangle to the model (2 triangles)
 */
int Model_3DS::Object::AddFace( int iMatFace, int p0, int p1, int p2 )
{
  if( bFlipNewFace )
  {
    int tmp = p1;
    p1 = p2;
    p2 = tmp;
  }
  int id = numFaces;
  aFace[id].a = p0;
  aFace[id].b = p1;
  aFace[id].c = p2;
  CalcFaceNormal( p0, p1, p2 ); 

  aMatFace[iMatFace].aFileFace[aMatFace[iMatFace].numFileFaces] = id;

  numFaces ++;
  if( numFaces>CR_MAX_FACES )
  {
    exitOnError( "3DS: Increase CR_MAX_FACES" );
  }

  aMatFace[iMatFace].numFileFaces ++;
  if( aMatFace[iMatFace].numFileFaces>CR_MAX_FACES )
  {
    exitOnError( "3DS: Increase CR_MAX_FACES" );
  }

  return id;
}

/**
 * Set EnvMapping
 */
void Model_3DS::SetEnvMap( bool val )
{
  if( val )
  {
    if( glActiveTextureARB==0 )
    {
      return;
    }
  }
  m_bEnvMap = val;
}

//---------------------------------------------------------------------------
//  Class ModelLoader
//---------------------------------------------------------------------------

ModelLoader model_loader;

ModelLoader::ModelLoader()
{
  iNumModel = 0;
}

ModelLoader::~ModelLoader()
{
  Reset();
}

void ModelLoader::Reset()
{
  for( int i=0; i<iNumModel; i++ )
  {
    delete aModel[i];
    aModel[i] = NULL;
  }
  iNumModel = 0;
}

/**
 * Find a model in the list of the already loaded models
 */
Model_3DS * ModelLoader::GetModel( const char * sName, float scale, float rotz, bool bMaterial)
{
  bool bNew;
  return GetModel( bNew, sName, scale, rotz, bMaterial );
}

Model_3DS * ModelLoader::GetModel( bool &bNew, const char * sName, float scale, float rotz, bool bMaterial )
{
  for( int i=0; i<model_loader.iNumModel; i++ )
  {
    if( strcmp( sName, model_loader.aModel[i]->m_sName )==0 )
    {
      bNew = false;
      return model_loader.aModel[i];
    }
  }

  bNew = true;
  return LoadModel( sName, scale, rotz, bMaterial );
}

Model_3DS * ModelLoader::LoadModel( const char * sName, float scale, float rotz, bool bMaterial )
{
  Model_3DS * model = new Model_3DS();
  model->rot.z = rotz;
  model->scale = scale;
  model->m_bMaterial = bMaterial;
  model->Load( (char *)sName );

  model_loader.aModel[model_loader.iNumModel++] = model;
  return model;
}
