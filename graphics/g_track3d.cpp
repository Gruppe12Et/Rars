//--------------------------------------------------------------------------
//
//    FILE: G_TRACK3D.CPP (portable)
//
//    TView3D
//      - This represent the track.
//
//    Version       Author          Date
//      0.1      Marc Gueury     08 /09 /01
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
  #include <windows.h>
  #include <direct.h>
#endif

#include "g_global.h"
#include "misc.h"
#include "g_tracked.h"

#ifdef TRACK_EDITOR
  #include "compatibility.h"
#endif 

//--------------------------------------------------------------------------
//                         D E F I N E S
//--------------------------------------------------------------------------

#define FX_SCALE(a) float( a )
#define FY_SCALE(a) float( a )

//--------------------------------------------------------------------------
//                           T Y P E S
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                         G L O B A L S
//--------------------------------------------------------------------------

const int kSMALL_ENOUGH = 15;
const int kBIG = 50000; // Relying on int = long

extern GLTexture * t_road;
extern GLTexture * t_pitstop;
extern GLTexture * t_red_brick;

//--------------------------------------------------------------------------
//                        Class TTrack3D
//--------------------------------------------------------------------------

TTrack3D::TTrack3D()
{
}

TTrack3D::~TTrack3D()
{
}

void TTrack3D::Load()
{
  char s[256];

  // Sky
  sprintf( s, "tracks/%s", currentTrack->m_oSky.m_sModel );
  m_ModelSky = model_loader.LoadModel( s, 1.0f, 0, true );
  m_ModelSky->m_bLit = false;

  // Track and Relief
  g_TrackEditor.Rebuild( false, true );
  m_iNbModel = currentTrack->m_iNumSegment;

  // Objects
  for( int i=0; i<currentTrack->m_iNumObject3D; i++ )
  {
    Object3D * obj = &currentTrack->m_aObject3D[i];
    sprintf( s, "tracks/%s/%s.3ds", currentTrack->m_sShortName, obj->m_sModel );
    Model_3DS * pModel = model_loader.LoadModel( s, (float)currentTrack->m_fUnitDistanceScale, 0, true );
    pModel->m_bLit = false;
    pModel->pos.x = float(obj->x);
    pModel->pos.y = float(obj->y);
    pModel->pos.z = float(obj->z);
    pModel->rot.x = float(obj->rot_x*180/PI);
    pModel->rot.y = float(obj->rot_y*180/PI);
    pModel->rot.z = float(obj->rot_z*180/PI);
    pModel->CalculateSphere();
    m_aOject3DModel[i] = pModel;
  }
}

void TTrack3D::Destroy()
{
  int i;

  for( i=0; i<m_iNbModel; i++ )
  {
    m_aModel[i].RemoveAll();
  }
  m_iNbModel = 0;
}

/**
 * Draw the sky
 */
void TTrack3D::DrawSky( double cam_x, double cam_y )
{
  glShadeModel(GL_FLAT);

  glPushMatrix();
  glTranslatef( (float)cam_x, (float)cam_y, 0.0f );
  m_ModelSky->DrawObjects( false );
  glPopMatrix();
  
  glShadeModel(GL_SMOOTH);
}

/**
 * Draw the road for the Rars runtime
 */
void TTrack3D::Draw()
{
  int i;
  int iNbSeen = 0;
  float dist;
  Seen aSeen[MAX_MODEL];

  // 1. Draw the objects, the relief
  m_ModelRelief.Draw();
  DrawObject3D();

  // 2. Draw the opaque objects.
  for( i=0; i<m_iNbModel; i++ )
  {
    dist=SphereInFrustum( m_aModel[i].m_fCenterX, m_aModel[i].m_fCenterY, m_aModel[i].m_fCenterZ, m_aModel[i].m_fRay );
    if( dist>0.0 )
    {
      m_aModel[i].DrawObjects( false );
      // 2. Get the distance of the transparent objects.
      if( m_aModel[i].m_bTransparent )
      {
        aSeen[iNbSeen].pModel = &m_aModel[i];
        aSeen[iNbSeen].dist = dist;
        iNbSeen++;
      }
    }
  }

  // 3. Quicksort the transparent objects seen
  Quicksort( aSeen, 0, iNbSeen-1 );

  // 4. Draw the transparent objects
  for( i=iNbSeen-1; i>=0; i-- )
  {
    aSeen[i].pModel->DrawObjects( true );
  }
}

/**
 * Draw the road for the track editor
 */
void TTrack3D::DrawRoad3D()
{
  static GLfloat mat_light[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color of a lighted face
  static GLfloat mat_shadow[]  = { 0.6f, 0.6f, 0.6f, 1.0f }; // Color of a shadowed face
  static GLfloat mat_red[]     = { 1.0f, 0.0f, 0.0f, 1.0f }; 

  m_ModelRelief.Draw();

  for( int i=0; i<currentTrack->m_iNumSegment; i++ )
  {
    if( i==g_TrackEditor.m_iCurSegment )
    {
      glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_red );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_red );
    }
    m_aModel[i].Draw();
  
    if( i==g_TrackEditor.m_iCurSegment )
    {
      glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_light );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_shadow );
    }
  }
  DrawObject3D();
}

/**
 * Draw all Object3D of the track
 */
void TTrack3D::DrawObject3D()
{
  ExtractFrustum();

  for( int i=0; i<currentTrack->m_iNumObject3D; i++ )
  {
    Model_3DS * pModel = m_aOject3DModel[i];
    if( SphereInFrustum( pModel->m_fCenterX+pModel->pos.x, pModel->m_fCenterY+pModel->pos.y, pModel->m_fCenterZ+pModel->pos.z, pModel->m_fRay ) )
    {
      pModel->Draw();
    }
  }
}

/**
 * Extract the fustrum from the OpenGL view.
 */
void TTrack3D::ExtractFrustum()
{
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   m_aFrustum[0][0] = clip[ 3] - clip[ 0];
   m_aFrustum[0][1] = clip[ 7] - clip[ 4];
   m_aFrustum[0][2] = clip[11] - clip[ 8];
   m_aFrustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[0][0] * m_aFrustum[0][0] + m_aFrustum[0][1] * m_aFrustum[0][1] + m_aFrustum[0][2] * m_aFrustum[0][2] ) );
   m_aFrustum[0][0] /= t;
   m_aFrustum[0][1] /= t;
   m_aFrustum[0][2] /= t;
   m_aFrustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   m_aFrustum[1][0] = clip[ 3] + clip[ 0];
   m_aFrustum[1][1] = clip[ 7] + clip[ 4];
   m_aFrustum[1][2] = clip[11] + clip[ 8];
   m_aFrustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[1][0] * m_aFrustum[1][0] + m_aFrustum[1][1] * m_aFrustum[1][1] + m_aFrustum[1][2] * m_aFrustum[1][2] ) );
   m_aFrustum[1][0] /= t;
   m_aFrustum[1][1] /= t;
   m_aFrustum[1][2] /= t;
   m_aFrustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   m_aFrustum[2][0] = clip[ 3] + clip[ 1];
   m_aFrustum[2][1] = clip[ 7] + clip[ 5];
   m_aFrustum[2][2] = clip[11] + clip[ 9];
   m_aFrustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[2][0] * m_aFrustum[2][0] + m_aFrustum[2][1] * m_aFrustum[2][1] + m_aFrustum[2][2] * m_aFrustum[2][2] ) );
   m_aFrustum[2][0] /= t;
   m_aFrustum[2][1] /= t;
   m_aFrustum[2][2] /= t;
   m_aFrustum[2][3] /= t;

   /* Extract the TOP plane */
   m_aFrustum[3][0] = clip[ 3] - clip[ 1];
   m_aFrustum[3][1] = clip[ 7] - clip[ 5];
   m_aFrustum[3][2] = clip[11] - clip[ 9];
   m_aFrustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[3][0] * m_aFrustum[3][0] + m_aFrustum[3][1] * m_aFrustum[3][1] + m_aFrustum[3][2] * m_aFrustum[3][2] ) );
   m_aFrustum[3][0] /= t;
   m_aFrustum[3][1] /= t;
   m_aFrustum[3][2] /= t;
   m_aFrustum[3][3] /= t;

   /* Extract the FAR plane */
   m_aFrustum[4][0] = clip[ 3] - clip[ 2];
   m_aFrustum[4][1] = clip[ 7] - clip[ 6];
   m_aFrustum[4][2] = clip[11] - clip[10];
   m_aFrustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[4][0] * m_aFrustum[4][0] + m_aFrustum[4][1] * m_aFrustum[4][1] + m_aFrustum[4][2] * m_aFrustum[4][2] ) );
   m_aFrustum[4][0] /= t;
   m_aFrustum[4][1] /= t;
   m_aFrustum[4][2] /= t;
   m_aFrustum[4][3] /= t;

   /* Extract the NEAR plane */
   m_aFrustum[5][0] = clip[ 3] + clip[ 2];
   m_aFrustum[5][1] = clip[ 7] + clip[ 6];
   m_aFrustum[5][2] = clip[11] + clip[10];
   m_aFrustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[5][0] * m_aFrustum[5][0] + m_aFrustum[5][1] * m_aFrustum[5][1] + m_aFrustum[5][2] * m_aFrustum[5][2] ) );
   m_aFrustum[5][0] /= t;
   m_aFrustum[5][1] /= t;
   m_aFrustum[5][2] /= t;
   m_aFrustum[5][3] /= t;
}

/**
 * SphereInFrustum
 *
 * Return 0 is the sphere is not in the cone view
 * else return the distance from the camera.
 */
float TTrack3D::SphereInFrustum( float x, float y, float z, float radius )
{
   int p;
   float d;

   for( p = 0; p < 6; p++ )
   {
      d = m_aFrustum[p][0] * x + m_aFrustum[p][1] * y + m_aFrustum[p][2] * z + m_aFrustum[p][3];
      if( d <= -radius )
         return 0;
   }
   return d + radius;
}

void TTrack3D::SelectionSort(Seen data[], int left, int right)
{
  for(int i = left; i < right; i++) 
  {
    int min = i;
    for(int j=i+1; j <= right; j++)
    {
      if(data[j].dist < data[min].dist)
      {
        min = j;
      }
    }
    Seen temp = data[min];
    data[min] = data[i];
    data[i] = temp;
  }
}

int TTrack3D::Partition( Seen d[], int left, int right)
{
  float val =d[left].dist;
  int lm = left-1;
  int rm = right+1;
  for(;;) 
  {
    do
    {
      rm--;
    }
    while (d[rm].dist > val);
  
    do
    {
      lm++;
    }
    while( d[lm].dist < val);

    if(lm < rm)
    {
      Seen tempr = d[rm];
      d[rm] = d[lm];
      d[lm] = tempr;
    }
    else 
    {
      return rm;
    }
  }
}

void TTrack3D::Quicksort( Seen d[], int left, int right)
{
  if(left < (right-kSMALL_ENOUGH)) 
  {
    int split_pt = Partition(d,left, right);
    Quicksort(d, left, split_pt);
    Quicksort(d, split_pt+1, right);
  }
  else
  {
    SelectionSort(d, left, right);
  }
}

/**
 * Draw the road
 */
void TTrack3D::DrawRoad( bool bTexture )
{
  Float2D v[4], v2[4];
  int i, i2, j;
  const float TEXTURE_ZOOM_X = 1.0f;
  const float TEXTURE_ZOOM_Y = 1.0f;

  if( currentTrack==NULL )
  {
    return;
  }

  int iNumSegment = currentTrack->m_iNumSegment;
  float track_width = float( currentTrack->get_track_description().width );
  segment * lftwall  = currentTrack->get_track_description().lftwall;
  segment * rgtwall = currentTrack->get_track_description().rgtwall;

  if( bTexture )
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, t_road->m_iOpenglId );
  }

  for(i=iNumSegment-1; i>=0; i--) 
  {                 // for each segment:
    i2 = mod( i+1, iNumSegment );
    if( lftwall[i].radius==0.0 ) 
    {
      // straigth
      v[0].x= FX_SCALE( lftwall[i2].beg_x );
      v[0].y= FY_SCALE( lftwall[i2].beg_y );
      v[1].x= FX_SCALE( rgtwall[i2].beg_x );
      v[1].y= FY_SCALE( rgtwall[i2].beg_y );
      v[2].x= FX_SCALE( rgtwall[i].beg_x );
      v[2].y= FY_SCALE( rgtwall[i].beg_y );
      v[3].x= FX_SCALE( lftwall[i].beg_x );
      v[3].y= FY_SCALE( lftwall[i].beg_y );

      if( !bTexture )
      { 
        if( i==g_TrackEditor.m_iCurSegment )
        {
          glColor3f( 1.0f, 0.0f, 0.0f );
        }
        else
        {
          glColor3f( 1.0f, 1.0f, 1.0f );
        }
      } 
      float nb_tex = float( lftwall[i].length/track_width ) * TEXTURE_ZOOM_Y;
      glBegin (GL_QUADS);
      glTexCoord2f(0.0,nb_tex);
      glVertex3f( GLfloat(v[3].x), GLfloat(v[3].y), 0.0f);
      glTexCoord2f(TEXTURE_ZOOM_X,nb_tex);
      glVertex3f( GLfloat(v[2].x), GLfloat(v[2].y), 0.0f);
      glTexCoord2f(TEXTURE_ZOOM_X,0.0);
      glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 0.0f);
      glTexCoord2f(0.0,0.0);
      glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 0.0f);
      glEnd ();
    }
    else
    {
      // curve
      double max_radius = max( fabs(lftwall[i].radius), fabs(rgtwall[i].radius) );
      double step_size = max( 10.0/max_radius, 0.1 );
      double fstep = lftwall[i].length/step_size;
      int nb_step = (int)fstep;
      double texture_step = (lftwall[i].length*max_radius)/track_width/nb_step;
      texture_step *= TEXTURE_ZOOM_Y;

      v[2].x=FX_SCALE( rgtwall[i].beg_x );
      v[2].y=FY_SCALE( rgtwall[i].beg_y );
      v[3].x=FX_SCALE( lftwall[i].beg_x );
      v[3].y=FY_SCALE( lftwall[i].beg_y );

      double ang = lftwall[i].beg_ang;
      double cenx = lftwall[i].cen_x;
      double ceny = lftwall[i].cen_y;

      glBegin (GL_QUAD_STRIP);
      if( bTexture )
      { 
        glColor3f( 1.0f, 1.0f, 1.0f );
      }
      else
      {
        if( i==g_TrackEditor.m_iCurSegment )
        {
          glColor3f( 1.0f, 0.0f, 0.0f );
        }
        else if( lftwall[i].radius>0.0 )
        {
          glColor3f( 0.5f, 1.0f, 0.5f );
        }
        else // if( lftwall[i].radius>0.0 )
        {
          glColor3f( 0.5f, 0.5f, 1.0f );
        }
      }
      glTexCoord2f(0.0,0.0);
      glVertex3f( GLfloat(v[3].x), GLfloat(v[3].y), 0.0f);
      glTexCoord2f(TEXTURE_ZOOM_X,0.0);
      glVertex3f( GLfloat(v[2].x), GLfloat(v[2].y), 0.0f);

      if(lftwall[i].radius>0.0) 
      {
        for( j=0; j<nb_step; j++) 
        {
          ang+=step_size;
          if(ang > 2.0 * PI) ang -= 2.0 * PI;
          v[0].x= FX_SCALE( cenx+lftwall[i].radius*sin(ang) );
          v[0].y= FY_SCALE( ceny-lftwall[i].radius*cos(ang) );
          v[1].x= FX_SCALE( cenx+rgtwall[i].radius*sin(ang) );
          v[1].y= FY_SCALE( ceny-rgtwall[i].radius*cos(ang) );
          glTexCoord2f(0.0, float((j+1)*texture_step) );
          glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 0.0f);
          glTexCoord2f(TEXTURE_ZOOM_X, float((j+1)*texture_step) );
          glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 0.0f);
        }
      }
      else 
      {
        for( j=0; j<nb_step; j++) 
        {
          ang-=step_size;
          if(ang < 0.0) ang += 2.0 * PI;
          v[0].x= FX_SCALE( cenx+lftwall[i].radius*sin(ang) );
          v[0].y= FY_SCALE( ceny-lftwall[i].radius*cos(ang) );
          v[1].x= FX_SCALE( cenx+rgtwall[i].radius*sin(ang) );
          v[1].y= FY_SCALE( ceny-rgtwall[i].radius*cos(ang) );
          glTexCoord2f(0.0f, float((j+1)*texture_step) );
          glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 0.0f);
          glTexCoord2f(TEXTURE_ZOOM_X, float((j+1)*texture_step) );
          glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 0.0f);
        }
      }
      v[0].x=FX_SCALE( lftwall[i2].beg_x );
      v[0].y=FY_SCALE( lftwall[i2].beg_y );
      v[1].x=FX_SCALE( rgtwall[i2].beg_x );
      v[1].y=FY_SCALE( rgtwall[i2].beg_y );
      glTexCoord2f(0.0,float(fstep*texture_step));
      glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 0.0f);
      glTexCoord2f(TEXTURE_ZOOM_X,float(fstep*texture_step));
      glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 0.0f);
      glEnd ();
    }
    if( i==0 ) 
    {
      v2[0] = v[0];
      v2[1] = v[1];
      v2[2] = v[2];
      v2[3] = v[3];
    }
  }
  glDisable(GL_TEXTURE_2D);

  // start
  v[0].x = FX_SCALE( currentTrack->finish_rx ); v[0].y = FY_SCALE( currentTrack->finish_ry );
  v[1].x = FX_SCALE( currentTrack->finish_lx ); v[1].y = FY_SCALE( currentTrack->finish_ly );
  double dir_x = -(currentTrack->finish_ry - currentTrack->finish_ly)/4;
  double dir_y = (currentTrack->finish_rx - currentTrack->finish_lx)/4;
  v[2].x = FX_SCALE( currentTrack->finish_lx + dir_x);v[2].y = FY_SCALE( currentTrack->finish_ly + dir_y);
  v[3].x = FX_SCALE( currentTrack->finish_rx + dir_x);v[3].y = FY_SCALE( currentTrack->finish_ry+dir_y);

  glBegin (GL_QUADS);
  glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
  glVertex3f( GLfloat(v[3].x), GLfloat(v[3].y), 1.0f);
  glVertex3f( GLfloat(v[2].x), GLfloat(v[2].y), 1.0f);
  glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 1.0f);
  glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 1.0f);
  glEnd ();

  if( bTexture )
  {
    // pitstop
    float dx = (v[1].x-v[0].x)/2.0f;
    float dy = (v[1].y-v[0].y)/2.0f;
    double ang = atan2( dy, dx );
    dx = (float)( 40.0*cos(ang) );
    dy = (float)( 40.0*sin(ang) );
    v[0]   = v2[1];
    v[1]   = v2[2];
    v[2].x = v[1].x-dx; v[2].y = v[1].y-dy;
    v[3].x = v[0].x-dx; v[3].y = v[0].y-dy;

    glEnable(GL_TEXTURE_2D);
    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f);

    glBindTexture( GL_TEXTURE_2D, t_pitstop->m_iOpenglId );
    float nstep;
    if( fabs(v2[1].x-v[1].x)>fabs(v2[1].y-v[1].y) )
    {
      nstep = (v2[1].x-v[1].x)/40.0f;
    }
    else
    {
      nstep = (v2[1].y-v[1].y)/40.0f;
    }

    glBegin (GL_QUADS);
    glTexCoord2f(1.0,0.0);
    glVertex3f( GLfloat(v[0].x), GLfloat(v[0].y), 0.0f);
    glTexCoord2f(0.0,0.0);
    glVertex3f( GLfloat(v[3].x), GLfloat(v[3].y), 0.0f);
    glTexCoord2f(0.0,nstep);
    glVertex3f( GLfloat(v[2].x), GLfloat(v[2].y), 0.0f);
    glTexCoord2f(1.0,nstep);
    glVertex3f( GLfloat(v[1].x), GLfloat(v[1].y), 0.0f);
    glEnd ();

    ////
    //// House Pitstop
    ////
    float dx2 = 2.0f*dx;
    float dy2 = 2.0f*dy;

    glBindTexture( GL_TEXTURE_2D, t_red_brick->m_iOpenglId );

    #define H_BUILD    15.0f
    #define H_TEXTURE  2.0f
    #define L_BUILD    30.0f

    glBegin (GL_QUAD_STRIP);
    nstep = float( floor( fabs(v[1].x-v[0].x)*H_TEXTURE/H_BUILD ));
    float pos = 0.0f;
    glTexCoord2f(pos,H_TEXTURE);
    glVertex3f( GLfloat(v[0].x-dx),  GLfloat(v[0].y)-dy, 0.0f);
    glTexCoord2f(pos,0.0);
    glVertex3f( GLfloat(v[0].x-dx),  GLfloat(v[0].y)-dy, H_BUILD);
    pos += nstep;
    glTexCoord2f(pos,H_TEXTURE);
    glVertex3f( GLfloat(v[1].x-dx),  GLfloat(v[1].y)-dy, 0.0f);
    glTexCoord2f(pos,0.0);
    glVertex3f( GLfloat(v[1].x-dx),  GLfloat(v[1].y)-dy, H_BUILD);
    pos += 3.0f;
    glTexCoord2f(pos,H_TEXTURE);
    glVertex3f( GLfloat(v[1].x-dx2), GLfloat(v[1].y)-dy2, 0.0f);
    glTexCoord2f(pos,0.0);
    glVertex3f( GLfloat(v[1].x-dx2), GLfloat(v[1].y)-dy2, H_BUILD);
    pos += nstep;
    glTexCoord2f(pos,H_TEXTURE);
    glVertex3f( GLfloat(v[0].x-dx2), GLfloat(v[0].y)-dy2, 0.0f);
    glTexCoord2f(pos,0.0);
    glVertex3f( GLfloat(v[0].x-dx2), GLfloat(v[0].y)-dy2, H_BUILD);
    pos += 3.0f;
    glTexCoord2f(pos,H_TEXTURE);
    glVertex3f( GLfloat(v[0].x-dx),  GLfloat(v[0].y)-dy, 0.0f);
    glTexCoord2f(pos,0.0);
    glVertex3f( GLfloat(v[0].x-dx),  GLfloat(v[0].y)-dy, H_BUILD);
    glEnd ();

    glDisable(GL_TEXTURE_2D);

    glBegin (GL_QUADS);
    glColor4f ( 0.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f( GLfloat(v[0].x)-dx,  GLfloat(v[0].y)-dy,  H_BUILD );
    glVertex3f( GLfloat(v[0].x)-dx2, GLfloat(v[0].y)-dy2, H_BUILD );
    glVertex3f( GLfloat(v[1].x)-dx2, GLfloat(v[1].y)-dy2, H_BUILD );
    glVertex3f( GLfloat(v[1].x)-dx,  GLfloat(v[1].y)-dy,  H_BUILD );
    glEnd ();

    // Reset the color
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
  }
}

/**
 * Car Height
 *
 * @param i: id of the car
 * @return height of the car
 */
double TTrack3D::CarHeight( int iCar, double to_end_offset, double &banking )
{
/*
  TO DO :
    Store the points of the subsegments in the track structure.
    And calculate the height with them.
*/
  /*
   The right way should be to look for the triangle in the 3ds
   file containing the point of the car. And calculate the height of this point
  */
  double h = 0;
  int seg_id  = race_data.cars[iCar]->seg_id;
  segment * seg  = &(currentTrack->rgtwall[seg_id]);

  // Add the 'to_end_offset' to 'to_end' and change the segment if needed
  double to_end = race_data.cars[iCar]->to_end;
  double dist_to_end;
  if( seg->radius==0.0 )
  {
    dist_to_end = to_end;
  }
  else
  {
    dist_to_end = fabs(seg->radius) * to_end;
  }
  dist_to_end += to_end_offset;
    
  if( dist_to_end<0 )
  {
    // Add the size of the next segment 
    seg_id = incseg( seg_id );
    seg  = &(currentTrack->rgtwall[seg_id]);
    if( seg->radius==0.0 )
    {
      dist_to_end += seg->length;
    }
    else
    {
      dist_to_end += fabs(seg->radius) * seg->length;
    }
  }
  else if( to_end>seg->length )
  {
    // Remove the size of the current segment 
    if( seg->radius==0.0 )
    {
      dist_to_end -= seg->length;
    }
    else
    {
      dist_to_end -= fabs(seg->radius) * seg->length;
    }
    seg_id = decseg( seg_id );
    seg  = &(currentTrack->rgtwall[seg_id]);
  }
  if( seg->radius==0.0 )
  {
    to_end = dist_to_end;
  }
  else
  {
    to_end = dist_to_end / fabs(seg->radius);
  }
  Segment3D * seg3D  = &(currentTrack->m_aSeg[seg_id]);

  int prv_seg_id = decseg( seg_id ); 
  Segment3D * prv_seg3D = &(currentTrack->m_aSeg[prv_seg_id]);

  ProfileHeight * ph = seg3D->m_pProfileHeight;
  int iNbSubSeg = ph->GetNbSubSeg();

  double f = ( (seg->length-to_end)/seg->length ) * iNbSubSeg;
  int i = (int)floor( f );
  double g = f-i;

  // Special case due to an error in track (between begin and end )
  if( i<0 ) 
  {
    h = prv_seg3D->m_fEndZ;
  }
  else if( i>=iNbSubSeg ) 
  {
    h = seg3D->m_fEndZ;
  }
  else
  {
    double h1 = prv_seg3D->m_fEndZ+seg3D->m_pProfileHeight->aHeight[i]*(seg3D->m_fEndZ-prv_seg3D->m_fEndZ);
    double h2 = prv_seg3D->m_fEndZ+seg3D->m_pProfileHeight->aHeight[i+1]*(seg3D->m_fEndZ-prv_seg3D->m_fEndZ);
    h = (1-g)*h1 + g*h2;
    double hill1 = seg3D->m_pProfileHeight->aHeight[i]*seg3D->m_fHillHeight;
    double hill2 = seg3D->m_pProfileHeight->aHeight[i+1]*seg3D->m_fHillHeight;
    h += (1-g)*hill1 + g*hill2;
  }

  // Banking 
  double half_width = currentTrack->width/2;
  double b = race_data.cars[iCar]->to_rgt-half_width;
  double c = (seg->length-to_end)/seg->length;
  if( b<-half_width) b = -half_width;
  if( b>half_width ) b = half_width;

  banking = prv_seg3D->m_fEndBanking+c*(seg3D->m_fEndBanking-prv_seg3D->m_fEndBanking);
  banking = -banking;
  h += tan( banking ) * b;
  banking *= 180/PI;

  // The calculation is not right : the curve are cuuted in subsegments
  // it is not taken into account -> this is the reason of the 0.5
  return h + 0.5;
}

/**
 * Car Angle with road
 *
 * @param i: id of the car
 * @return the angle of the car with the road
 */
double TTrack3D::CarAngWithRoad( int iCar )
{
  Car * car = race_data.cars[iCar];
  int seg_id  = race_data.cars[iCar]->seg_id;
  segment * seg  = &(currentTrack->rgtwall[seg_id]);

  if( seg->radius==0.0 )
  {
    return TGeom::AngleMinPItoPI(seg->end_ang-car->ang);
  }
  else
  {
    return TGeom::AngleMinPItoPI(seg->end_ang-car->to_end-car->ang);
  }
}
