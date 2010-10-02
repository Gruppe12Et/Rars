/**
 *  Chunk3DS : Use to save a 3DS file
 */ 

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define CHUNK_HEADER_SIZE 6

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "../memory.h"
#include "model_3ds.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

//--------------------------------------------------------------------------
//                            Class Chunk3DS
//--------------------------------------------------------------------------


Chunk3DS::Chunk3DS( Chunk3DS * pParent, int id )
{
  m_iId   = id;
  m_iLen  = CHUNK_HEADER_SIZE;
  m_pNext = NULL;
  m_pFirstChild = NULL;
  if( pParent!=NULL )
  {
    pParent->AddChild( this );
  }
}

Chunk3DS::~Chunk3DS()
{
  if( m_pNext!=NULL )
  {
    delete m_pNext;
  }
  if( m_pFirstChild!=NULL )
  {
    delete m_pFirstChild;
  }
}

void Chunk3DS::AddChild( Chunk3DS * pChild )
{
  if( m_pFirstChild==NULL ) 
  {
    m_pFirstChild = pChild;
  }
  else
  {
    Chunk3DS * pLast = m_pFirstChild;
     
    while( pLast->m_pNext!=NULL )
    {
      pLast = pLast->m_pNext;
    }
    pLast->m_pNext = pChild;
  }
}

void Chunk3DS::SaveHeader( FILE * out )
{
  // Write the header
  fwrite(&m_iId,sizeof(m_iId),1,out);
  fwrite(&m_iLen,sizeof(m_iLen),1,out);
}

void Chunk3DS::SaveChilds( FILE * out )
{
  // Write the childs
  Chunk3DS * pLast = m_pFirstChild;

  if( pLast!=NULL )
  {
    while( pLast!=NULL )
    {
      pLast->Save( out );
      pLast = pLast->m_pNext;
    }
  }
}

void Chunk3DS::Save( FILE * out )
{
  SaveHeader( out );
  SaveChilds( out );
}

int Chunk3DS::CalcLength()
{
  Chunk3DS * pLast = m_pFirstChild;

  while( pLast!=NULL )
  {
    m_iLen += pLast->CalcLength();
    pLast = pLast->m_pNext;
  }
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkMain3DS::ChunkMain3DS()
  :Chunk3DS( NULL, CHUNK_MAIN3DS ) 
{
}

//--------------------------------------------------------------------------

ChunkWord3DS::ChunkWord3DS( Chunk3DS * pParent, int id, WORD _val )
  :Chunk3DS( pParent, id ) 
{
  val = _val;
}

void ChunkWord3DS::Save( FILE * out )
{
  // Write the header
  SaveHeader( out );
  fwrite(&val,sizeof(val),1,out);
  SaveChilds( out );
}

int ChunkWord3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += sizeof(val);
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkDWord3DS::ChunkDWord3DS( Chunk3DS * pParent, int id, DWORD _val )
  :Chunk3DS( pParent, id ) 
{
  val = _val;
}

void ChunkDWord3DS::Save( FILE * out )
{
  // Write the header
  SaveHeader( out );
  fwrite(&val,sizeof(val),1,out);
  SaveChilds( out );
}

int ChunkDWord3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += sizeof(val);
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkFloat3DS::ChunkFloat3DS( Chunk3DS * pParent, int id, float _val )
  :Chunk3DS( pParent, id ) 
{
  val = _val;
}

void ChunkFloat3DS::Save( FILE * out )
{
  // Write the header
  SaveHeader( out );
  fwrite(&val,sizeof(val),1,out);
  SaveChilds( out );
}

int ChunkFloat3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += sizeof(val);
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkString3DS::ChunkString3DS( Chunk3DS * pParent, int id, char * _val )
  :Chunk3DS( pParent, id ) 
{
  val = _val;
}

void ChunkString3DS::Save( FILE * out )
{
  // Write the header
  SaveHeader( out );
  fprintf(out, val);
  fputc(0, out);
  SaveChilds( out );
}

int ChunkString3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += strlen(val) + 1;
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkIRGB3DS::ChunkIRGB3DS( Chunk3DS * pParent, unsigned char _r, unsigned char _g, unsigned char _b )
  :Chunk3DS( pParent, CHUNK_COLOR_IRGB ) 
{
  r = _r;
  g = _g;
  b = _b;
}

void ChunkIRGB3DS::Save( FILE * out )
{
  // Write the header
  SaveHeader( out );
  fwrite(&r,sizeof(r),1,out);
  fwrite(&g,sizeof(g),1,out);
  fwrite(&b,sizeof(b),1,out);
}

int ChunkIRGB3DS::CalcLength()
{
  m_iLen += 3;
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkVertexList3DS::ChunkVertexList3DS( Chunk3DS * pParent, int _iNumVertex, Vertex * _aVertex )
  :Chunk3DS( pParent, CHUNK_VERT_LIST ) 
{
  iNumVertex = _iNumVertex;
  aVertex = _aVertex;
}

void ChunkVertexList3DS::Save( FILE * out )
{
  SaveHeader( out );
  fwrite(&iNumVertex,sizeof(iNumVertex),1,out);

  // Read the vertices, switching the y and z coordinates and changing the sign of the z coordinate
  for( int i=0; i<iNumVertex; i++ )
  {
    fwrite(&aVertex[i].x, sizeof(float),1,out);
    fwrite(&aVertex[i].y, sizeof(float),1,out);
    fwrite(&aVertex[i].z, sizeof(float),1,out);
  }
  SaveChilds( out );
}

int ChunkVertexList3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += sizeof( iNumVertex ) + iNumVertex*3*sizeof(float); 
  return m_iLen;
}


//--------------------------------------------------------------------------

ChunkFaceList3DS::ChunkFaceList3DS( Chunk3DS * pParent, int _iNumFace, Face * _aFace )
  :Chunk3DS( pParent, CHUNK_FACE_LIST ) 
{
  iNumFace = _iNumFace;
  aFace = _aFace;
}

void ChunkFaceList3DS::Save( FILE * out )
{
  SaveHeader( out );
  fwrite(&iNumFace,sizeof(iNumFace),1,out);

  unsigned short flags = 0x407; 
  for( int i=0; i<iNumFace; i++ )
  {
    fwrite(&aFace[i].a, sizeof(short),1,out);
    fwrite(&aFace[i].b, sizeof(short),1,out);
    fwrite(&aFace[i].c, sizeof(short),1,out);
    // flags
    fwrite(&flags,sizeof(short),1,out);
  }
  SaveChilds( out );
}

int ChunkFaceList3DS::CalcLength()
{
  Chunk3DS::CalcLength();
  m_iLen += sizeof( iNumFace ) + iNumFace*4*sizeof(short); 
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkFaceMat3DS::ChunkFaceMat3DS( Chunk3DS * pParent, char * _sName, int _iNumMatFace, unsigned short * _aMatFace )
  :Chunk3DS( pParent, CHUNK_FACE_MAT ) 
{
  sName = _sName;
  iNumMatFace = _iNumMatFace;
  aMatFace = _aMatFace;
}

void ChunkFaceMat3DS::Save( FILE * out )
{
  SaveHeader( out );
  fprintf(out, sName);
  fputc(0, out);
  fwrite(&iNumMatFace,sizeof(iNumMatFace),1,out);

  for( int i=0; i<iNumMatFace; i++ )
  {
    fwrite(&aMatFace[i],  sizeof(short),1,out);
  }
  SaveChilds( out );
}

int ChunkFaceMat3DS::CalcLength()
{
  m_iLen += strlen(sName) + 1 + sizeof(iNumMatFace) + iNumMatFace*sizeof(short); 
  return m_iLen;
}

//--------------------------------------------------------------------------

ChunkTexCoord3DS::ChunkTexCoord3DS( Chunk3DS * pParent, int _iNumCoord, Vect2D * _aCoord )
  :Chunk3DS( pParent, CHUNK_TEX_COORDS ) 
{
  iNumCoord = _iNumCoord;
  aCoord = _aCoord;
}

void ChunkTexCoord3DS::Save( FILE * out )
{
  SaveHeader( out );
  fwrite(&iNumCoord,sizeof(iNumCoord),1,out);

  for( int i=0; i<iNumCoord; i++ )
  {
    fwrite(&aCoord[i].x, sizeof(float),1,out);
    fwrite(&aCoord[i].y, sizeof(float),1,out);
  }
}

int ChunkTexCoord3DS::CalcLength()
{
  m_iLen += sizeof(iNumCoord) + iNumCoord*2*sizeof(float); 
  return m_iLen;
}


