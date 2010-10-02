//--------------------------------------------------------------------------
//
//    FILE: G_TRACKED.CPP (portable)
//
//      - Common classes and code for the Track Editor
//
//    Version       Author          Date
//      0.2      Marc Gueury     08 /09 /01
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "g_tracked.h"
#include "../misc.h"
#include "g_global.h"

//--------------------------------------------------------------------------
//                           D E F I N E 
//--------------------------------------------------------------------------

#define MAX_SUBSEG 100
#define TREE_HEIGHT 50
#define BARRIER_RED_WHITE_SIZE 8
#define RED_WHITE_SIZE 5
#define RELIEF_BORDER_NUM 90
#define RELIEF_BORDER_HEIGHT 60

#define LINEAR( data1, data2, abc ) ( data1->abc + (data2->abc - data1->abc )*j/iNbSubSeg )
#define TEXTURE_UNIT (currentTrack->width)

//--------------------------------------------------------------------------
//                         G L O B A L S
//--------------------------------------------------------------------------

TrackEditor g_TrackEditor;

/*
ProfileTexture g_profileTexture[] =
{ 
  { "standard", "grass.bmp", "sand.bmp", "tree.bmp", "road_nmm.bmp" },
  NULL
}
*/

//--------------------------------------------------------------------------
//                        Class TrackEditor
//--------------------------------------------------------------------------

TrackEditor::TrackEditor()
{
  m_iViewMode = TRACKED_VIEW_MODE_3DS;
  m_iCurSegment = 0;
  m_iCurSection = 0;

  m_sPropValue[0] = 0;
  m_pStructProp = NULL;
  m_pDefaultBase = NULL;

  m_sClipBoardValue[0] = 0;
  m_sClipBoardText[0] = 0;
  m_pClipBoardProp = NULL;
  m_iClipBoardSegmentId = -1;
}

//--------------------------------------------------------------------------
//                        Segment Rendering
//--------------------------------------------------------------------------

/**
 * PreCalc
 */
void TrackEditor::PreCalc()
{
  int i;
  Segment3D * m_aSeg = currentTrack->m_aSeg;
  segment * lftwall  = currentTrack->lftwall;
  segment * rgtwall  = currentTrack->rgtwall;
  m_aCalcData        = new SegmentCalcData[currentTrack->m_iNumSegment];

  for( i=0; i<currentTrack->m_iNumSegment; i++ )
  {
    int i2 = m_aSeg[i].m_iNxtSeg;

    // Avoid that the border (between road and barrier) of diff segment overlap
    m_aCalcData[i].lft.m_fBorderSize = m_aSeg[i].lft.m_fBorderSize;
    m_aCalcData[i].rgt.m_fBorderSize = m_aSeg[i].rgt.m_fBorderSize;
    if( i2>=0 ) 
    {
      if( m_aSeg[i2].m_fRadius>0.0 )
      {
        double radius = m_aSeg[i2].m_fRadius-currentTrack->width-5;
        m_aCalcData[i].lft.m_fBorderSize = min( m_aCalcData[i].lft.m_fBorderSize, radius );
      }
      else if( m_aSeg[i2].m_fRadius<0.0 )
      {
        double radius = fabs(m_aSeg[i2].m_fRadius)-5;
        m_aCalcData[i].rgt.m_fBorderSize = min( m_aCalcData[i].rgt.m_fBorderSize, radius );
      }
      if( m_aSeg[i].m_fRadius>0.0 )
      {
        double radius = m_aSeg[i].m_fRadius-currentTrack->width-5;
        m_aCalcData[i].lft.m_fBorderSize = min( m_aCalcData[i].lft.m_fBorderSize, radius );
      }
      else if( m_aSeg[i].m_fRadius<0.0 )
      {
        double radius = fabs(m_aSeg[i].m_fRadius)-5;
        m_aCalcData[i].rgt.m_fBorderSize = min( m_aCalcData[i].rgt.m_fBorderSize, radius );
      }
    }

    // Calculate a unit vector perpendicular to the road
    double dx = lftwall[i].end_x - rgtwall[i].end_x;
    double dy = lftwall[i].end_y - rgtwall[i].end_y;
    double dist = sqrt( dx*dx+dy*dy ); // should be equal to with;
    dx = dx/dist;
    dy = dy/dist;

    double fTotalSize = m_aCalcData[i].lft.m_fBorderSize + m_aSeg[i].lft.m_fBorderTreeSize;
    m_aCalcData[i].lft.m_vEndBorderTree.SetXYZ 
    ( 
      lftwall[i].end_x+dx*fTotalSize, 
      lftwall[i].end_y+dy*fTotalSize, 
      m_aSeg[i].m_fEndZ+m_aSeg[i].lft.m_fBorderTreeHeight
    );

    fTotalSize = m_aCalcData[i].rgt.m_fBorderSize + m_aSeg[i].rgt.m_fBorderTreeSize;
    m_aCalcData[i].rgt.m_vEndBorderTree.SetXYZ 
    ( 
      rgtwall[i].end_x-dx*fTotalSize, 
      rgtwall[i].end_y-dy*fTotalSize, 
      m_aSeg[i].m_fEndZ+m_aSeg[i].rgt.m_fBorderTreeHeight
    );
  }
}

/**
 * PostCalc
 */
void TrackEditor::PostCalc()
{
  delete m_aCalcData;
}

/**
 * Generate the 3DS model of the segment i
 *
 * @param i the number of the segment in the track
 * @return a new 3DS model
 */
void TrackEditor::GenerateSegmentModel( Model_3DS &model, int i )
{
  int j;

  // Get the general texture profile (NO BMP EXTENSION)
  char * t_grass_in   = "grass_in";
  char * t_road       = "road_nmm";
  char * t_sand       = "sand";
  // char * t_tree       = "tree";
  char * t_red_white  = "red_white_nmm";
  char * t_barrier    = "barrier";
  char * t_barrier_rw = "barrier_red_white_nmm";

  segment * lftwall  = currentTrack->lftwall;
  segment * rgtwall  = currentTrack->rgtwall;
  Segment3D * m_aSeg  = currentTrack->m_aSeg;
  int o = m_aSeg[i].m_iOtherYSeg;

  model.RemoveAll();
  model.Create();

  lft.Reset();
  rgt.Reset();

  int obj = model.AddObject();
  int mat_road       = model.AddMaterialTexture( "", t_road );
  int mat_red_white  = model.AddMaterialTexture( "", t_red_white );

  mf_road       = model.aObject[obj].AddMatFace( mat_road );
  mf_red_white  = model.aObject[obj].AddMatFace( mat_red_white );

  int i0 = m_aSeg[i].m_iPrvSeg;

  // Get the particular texture profile
  // If not use a profile in function of the previous/next segments
  segment * seg = &( currentTrack->rgtwall[i] );

  char lft_texture_border[32];
  char rgt_texture_border[32];
  char lft_texture_barrier[32];
  char rgt_texture_barrier[32];
  strcpy( lft_texture_border, m_aSeg[i].lft.m_sTextureBorder );
  strcpy( rgt_texture_border, m_aSeg[i].rgt.m_sTextureBorder );
  strcpy( lft_texture_barrier, m_aSeg[i].lft.m_sTextureBarrier );
  strcpy( rgt_texture_barrier, m_aSeg[i].rgt.m_sTextureBarrier );

  if( seg->radius>0.0 && seg->radius<1000.0)
  {
    lft.m_fRedWhiteSize = RED_WHITE_SIZE;

    if( strcmp( rgt_texture_border, t_grass_in )==0 ) // if "grass_in"
    {
      strcpy( rgt_texture_border, t_sand );
    }
    if( strcmp( rgt_texture_barrier, t_barrier )==0 )  // if "barrier"
    {
      rgt.m_fBarrierSize = BARRIER_RED_WHITE_SIZE;
      strcpy( rgt_texture_barrier, t_barrier_rw );
    }
  }
  else if( seg->radius<0.0 && seg->radius>-1000.0 )
  {
    rgt.m_fRedWhiteSize = RED_WHITE_SIZE;

    if( strcmp( lft_texture_border, t_grass_in )==0 ) // if "grass_in"
    {
      strcpy( lft_texture_border, t_sand );
    }
    if( strcmp( lft_texture_barrier, t_barrier )==0 ) // if "barrier"
    {
      lft.m_fBarrierSize = BARRIER_RED_WHITE_SIZE;
      strcpy( lft_texture_barrier, t_barrier_rw );
    }
  }

  int lft_mat_border      = model.AddMaterialTexture( "", lft_texture_border );
  int lft_mat_barrier     = model.AddMaterialTexture( "", lft_texture_barrier );
  int lft_mat_border_tree = model.AddMaterialTexture( "", m_aSeg[i].lft.m_sTextureBorderTree );
  int lft_mat_tree        = model.AddMaterialTexture( "", m_aSeg[i].lft.m_sTextureTree );
  lft.mf_border           = model.aObject[obj].AddMatFace( lft_mat_border );
  lft.mf_barrier          = model.aObject[obj].AddMatFace( lft_mat_barrier );
  lft.mf_border_tree      = model.aObject[obj].AddMatFace( lft_mat_border_tree );
  lft.mf_tree             = model.aObject[obj].AddMatFace( lft_mat_tree );

  int rgt_mat_border      = model.AddMaterialTexture( "", rgt_texture_border );
  int rgt_mat_barrier     = model.AddMaterialTexture( "", rgt_texture_barrier );
  int rgt_mat_border_tree = model.AddMaterialTexture( "", m_aSeg[i].rgt.m_sTextureBorderTree );
  int rgt_mat_tree        = model.AddMaterialTexture( "", m_aSeg[i].rgt.m_sTextureTree );
  rgt.mf_border           = model.aObject[obj].AddMatFace( rgt_mat_border );
  rgt.mf_barrier          = model.aObject[obj].AddMatFace( rgt_mat_barrier );
  rgt.mf_border_tree      = model.aObject[obj].AddMatFace( rgt_mat_border_tree );
  rgt.mf_tree             = model.aObject[obj].AddMatFace( rgt_mat_tree );

  // Check if the segment is a part of a Y
  if( o>=0 )
  {
    lft.m_bShow = rgtwall[i].radius>rgtwall[o].radius;
    rgt.m_bShow = !lft.m_bShow;
  } 

  double end_z;
  double end_nb_tex;
  
  // Create a 3DS in memory
  if( seg->radius==0.0 ) 
  {
    // straigth
    float nb_tex = float( rgtwall[i].length/TEXTURE_UNIT );

    ProfileHeight * ph = m_aSeg[i].m_pProfileHeight;
    int iNbSubSeg = ph->GetNbSubSeg();

    lft.m_bShowTree = m_aSeg[i].lft.m_bShowTree;
    rgt.m_bShowTree = m_aSeg[i].rgt.m_bShowTree;
    lft.m_bShowBorderTree = m_aSeg[i].lft.m_bShowBorderTree;
    rgt.m_bShowBorderTree = m_aSeg[i].rgt.m_bShowBorderTree;

    // For all "Sub Segments" of the profile height
    for( j=0; j<=iNbSubSeg; j++ )
    {
      lft.m_fEndX = lftwall[i].beg_x + ( lftwall[i].end_x - lftwall[i].beg_x )*j/iNbSubSeg;
      lft.m_fEndY = lftwall[i].beg_y + ( lftwall[i].end_y - lftwall[i].beg_y )*j/iNbSubSeg;
      rgt.m_fEndX = rgtwall[i].beg_x + ( rgtwall[i].end_x - rgtwall[i].beg_x )*j/iNbSubSeg;
      rgt.m_fEndY = rgtwall[i].beg_y + ( rgtwall[i].end_y - rgtwall[i].beg_y )*j/iNbSubSeg;
      end_z = ((1-ph->aHeight[j])*m_aSeg[i0].m_fEndZ)+(ph->aHeight[j]*m_aSeg[i].m_fEndZ);
      end_z += ph->aHeight[j]*m_aSeg[i].m_fHillHeight; 
      end_nb_tex = 0.0 + ( nb_tex - 0.0 )*j/iNbSubSeg;

      CalcSubSegmentCommon( i, i0, j, iNbSubSeg, end_z, seg->radius, &lft, &lftwall[i], &m_aSeg[i].lft, &m_aSeg[i0].lft, &m_aCalcData[i].lft, &m_aCalcData[i0].lft, -1 );
      CalcSubSegmentCommon( i, i0, j, iNbSubSeg, end_z, seg->radius, &rgt, &rgtwall[i], &m_aSeg[i].rgt, &m_aSeg[i0].rgt, &m_aCalcData[i].rgt, &m_aCalcData[i0].rgt, 1 );

      StraightAutoCorrection( &lft );
      StraightAutoCorrection( &rgt );

      GenerateSubSegment3DS( &(model.aObject[obj]), j, end_nb_tex);
    }
  }
  else
  {
    // Curve
    double max_radius = max( fabs(lftwall[i].radius), fabs(rgtwall[i].radius) );
    double step_size = 25.0/max_radius;
    double fstep = max( min( lftwall[i].length/step_size + 1, 15 ), 2 );
    int iNbSubSeg = (int)fstep;
    step_size = lftwall[i].length/iNbSubSeg;
    double texture_step = (lftwall[i].length*max_radius)/TEXTURE_UNIT/iNbSubSeg;

    double ang;
    double cenx = lftwall[i].cen_x;
    double ceny = lftwall[i].cen_y;

    if(lftwall[i].radius<0.0) 
    {
      step_size = -step_size;
    }

    double pos[MAX_SUBSEG];
    for( j=0; j<=iNbSubSeg; j++) 
    {
      pos[j] = j*step_size;
      if( o>=0 )
      {
        pos[j] = -CorrectSubSegmentPos( i, j, o, &lft, lftwall, -pos[j], -step_size );
        pos[j] = CorrectSubSegmentPos( i, j, o, &rgt, rgtwall, pos[j], step_size );
      }
    }

    for( j=0; j<=iNbSubSeg; j++) 
    {
      ang = lftwall[i].beg_ang + pos[j];
      if(ang > 2.0 * PI) ang -= 2.0 * PI;
      if(ang < 0 ) ang += 2.0 * PI;
      
      lft.m_fEndX = cenx+lftwall[i].radius*sin(ang);
      lft.m_fEndY = ceny-lftwall[i].radius*cos(ang);
      rgt.m_fEndX = cenx+rgtwall[i].radius*sin(ang);
      rgt.m_fEndY = ceny-rgtwall[i].radius*cos(ang);
      end_z = ( ((iNbSubSeg-j)*m_aSeg[i0].m_fEndZ)+(j*m_aSeg[i].m_fEndZ) )/(iNbSubSeg);
      end_nb_tex = j*texture_step;

      CalcSubSegmentCommon( i, i0, j, iNbSubSeg, end_z, seg->radius, &lft, &lftwall[i], &m_aSeg[i].lft, &m_aSeg[i0].lft, &m_aCalcData[i].lft, &m_aCalcData[i0].lft, -1 );
      CalcSubSegmentCommon( i, i0, j, iNbSubSeg, end_z, seg->radius, &rgt, &rgtwall[i], &m_aSeg[i].rgt, &m_aSeg[i0].rgt, &m_aCalcData[i].rgt, &m_aCalcData[i0].rgt, 1 );
      ////
      //// Correction tips
      ////
      // If radius is too small, do not show the tree, the barrier, ....
      CurveAutoCorrection( i, j, o, &lft, lftwall, -pos[j], -1 );
      CurveAutoCorrection( i, j, o, &rgt, rgtwall, pos[j], 1 );

      GenerateSubSegment3DS( &(model.aObject[obj]), j, end_nb_tex );
    }
  }
  model.PostCalc();
}

/**
 * Automatic correction of a segment
 */
void TrackEditor::CalcSubSegmentCommon( int i, int i0, int j, int iNbSubSeg, double end_z, double radius, TrackEditorSideData * side, segment * sidewall, SegmentSide3D  *segside, SegmentSide3D  *segside0, SegmentSideCalcData *calc, SegmentSideCalcData *calc0, double sign )
{
  side->m_fBarrierHeight  = segside->m_fBarrierHeight;
  side->m_bShowTree       = segside->m_bShowTree;
  side->m_bShowBorderTree = segside->m_bShowBorderTree;

  side->m_fBorderSize       = LINEAR( calc0, calc, m_fBorderSize );
  side->m_fBorderTreeSize   = LINEAR( segside0, segside, m_fBorderTreeSize );
  side->m_fBorderTreeHeight = LINEAR( segside0, segside, m_fBorderTreeHeight );
  side->m_fBarrierHeight    = segside->m_fBarrierHeight;

  double banking           = LINEAR( (&currentTrack->m_aSeg[i0]), (&currentTrack->m_aSeg[i]), m_fEndBanking );
  side->m_fEndZ = end_z + sign*tan(banking)*currentTrack->width/2; 

  if( !side->m_bShow )
  {
    side->m_bShowBarrier = false; 
    side->m_bShowBorderTree = false; 
    side->m_bShowTree = false; 
  }

  // If the border tree size = 0
  if( side->m_fBorderTreeSize<=0 ) 
  {
    side->m_bShowTree = false; 
    side->m_bShowBorderTree = false; 
  }

  if( radius==0 )
  {
    double x = LINEAR( calc0, calc, m_vEndBorderTree.x );
    double y = LINEAR( calc0, calc, m_vEndBorderTree.y );
    double z = LINEAR( calc0, calc, m_vEndBorderTree.z );
    side->m_vBorderTree.SetXYZ( x, y, z );
  }
  else
  {
    if( calc0->m_vEndBorderTree==calc->m_vEndBorderTree )
    {
      side->m_vBorderTree = calc0->m_vEndBorderTree;
    }
    else
    {
      double beginAng = atan2( calc0->m_vEndBorderTree.y-sidewall->cen_y, calc0->m_vEndBorderTree.x-sidewall->cen_x );
      double endAng = atan2( calc->m_vEndBorderTree.y-sidewall->cen_y, calc->m_vEndBorderTree.x-sidewall->cen_x );
      double border0 = sqrt( sqr(calc0->m_vEndBorderTree.x-sidewall->cen_x)+sqr(calc0->m_vEndBorderTree.y-sidewall->cen_y) );
      double border = sqrt( sqr(calc->m_vEndBorderTree.x-sidewall->cen_x)+sqr(calc->m_vEndBorderTree.y-sidewall->cen_y) );
      double ang;
      if( sidewall->radius>0.0 )
      {
        if( endAng<beginAng ) endAng += 2*PI;
        ang = beginAng + (endAng - beginAng )*j/iNbSubSeg;
      }
      else
      {
        if( endAng>beginAng ) endAng -= 2*PI;
        ang = beginAng + (endAng - beginAng )*j/iNbSubSeg;
      }
      border = border0 + (border - border0)*j/iNbSubSeg;
      double x = sidewall->cen_x+border*cos(ang);
      double y = sidewall->cen_y+border*sin(ang);
      side->m_vBorderTree.SetXYZ( x, y, end_z+side->m_fBorderTreeHeight );
    }
  }
}

/**
 * Automatic correction of the curve
 */
void TrackEditor::CurveAutoCorrection( int i, int j, int o, TrackEditorSideData * side, segment * sidewall, double a, double sign )
{
  double radius = -1*sign*sidewall[i].radius;
  // If radius is too small, do not show the tree, the barrier, ....
  if( radius>0 )
  {
    if( radius<side->m_fBorderSize+side->m_fBorderTreeSize ) 
    {
      side->m_bShowTree = false; 
      if( side->m_fBorderTreeSize>0.0 )
      {
        side->m_fBorderTreeHeight *= (radius-side->m_fBorderSize)/side->m_fBorderTreeSize;
        side->m_fBorderTreeSize = radius-side->m_fBorderSize;
      }
    }
    if( radius<side->m_fBorderSize ) 
    {
      side->m_bShowBarrier = false; 
      side->m_bShowBorderTree = false; 
      side->m_fBorderSize = radius;
    }
  }

  // Y segment : grass between the 2 segments
  if( o>=0 )
  {
    if( !side->m_bShow )
    {
      // Need a positive radius
      radius = fabs( sidewall[i].radius );
      side->m_fBarrierSize = 0;

      if( strcmp(currentTrack->m_aSeg[i].m_sType, "yinv")==0 || strcmp(currentTrack->m_aSeg[o].m_sType, "yinv")==0 )
      {
        a = sidewall[i].length - a;
      }
      double d1 = (1-cos(a))*radius - currentTrack->width;
      double r = 0;
      if( a<1.55 /*a little less than PI/2*/ )
      {
        if( d1>0 )
        {
          double d2 = tan(a)*(radius-currentTrack->width); // distance in the straight segment
          if( d2<sidewall[o].length+0.01 ) // 0.01 for calculation error
          {
            r = sqrt(tan(a)*tan(a)+1)*d1;
          }
          else
          {
            double a2 = PI/2 - a;
            double d3 = (1-cos(a2))*radius+sidewall[o].length-radius;
            r = sqrt(tan(a2)*tan(a2)+1)*d3;
            r = max( rgt.m_fBorderSize, r ); 
          }
        }
        side->m_fBorderSize = r;
      }
    }
  } 
}

/**
 * Automatic correction of the curve
 */
void TrackEditor::StraightAutoCorrection( TrackEditorSideData * side )
{
  if( !side->m_bShow )
  {
    side->m_fBorderSize = 0;
  }
}


/**
 * Modify the "position/size" of the subsegment such that it fits all angles
 * to get a pretty border between the 2 segments of a Y segment
 */
double TrackEditor::CorrectSubSegmentPos( int i, int j, int o, TrackEditorSideData * side, segment * sidewall, double posj, double step_size )
{
  if( !side->m_bShow )
  {
    double radius = fabs( sidewall[i].radius );
    double cos_a = (radius - currentTrack->width) / radius;
    double a = acos( cos_a );
    double tan_b = sidewall[o].length / (radius - currentTrack->width);
    double b = atan( tan_b );
    if( strcmp(currentTrack->m_aSeg[i].m_sType, "yinv")==0 || strcmp(currentTrack->m_aSeg[o].m_sType, "yinv")==0 )
    {
      a = sidewall[i].length - a;
      b = sidewall[i].length - b;
    }
    if( j*step_size<=a && (j+1)*step_size>a )
    {
      return a;
    }
    else if( j*step_size<=b && (j+1)*step_size>b )
    {
      return b;
    }
  }
  return posj;
}

/**
 * Generate one sub segment of a segment
 */
void TrackEditor::GenerateSubSegment3DS( Model_3DS::Object * pObject, int j, double end_nb_tex )
{
  Vertex v;
  Vect2D t;

  pObject->bFlipNewFace = false; // For CullFace

  // Road
  v.SetXYZ( lft.m_fEndX, lft.m_fEndY, lft.m_fEndZ );
  t.SetXY( end_nb_tex, 0 );
  lft.p_road_end = pObject->AddVertex( v, t );
  v.SetXYZ( rgt.m_fEndX,  rgt.m_fEndY, rgt.m_fEndZ );
  t.SetXY( end_nb_tex, 1 );
  rgt.p_road_end = pObject->AddVertex( v, t );

  // Calculate a unit vector perpendicular to the road
  double dx = lft.m_fEndX - rgt.m_fEndX;
  double dy = lft.m_fEndY - rgt.m_fEndY;
  double dist = sqrt( dx*dx+dy*dy ); // should be equal to with;
  dx = dx/dist;
  dy = dy/dist;

  // When j=0, there are only 2 points available
  if( j>0 )
  {
    // Road
    pObject->AddRect( mf_road, rgt.p_road_beg, rgt.p_road_end, lft.p_road_end, lft.p_road_beg );
  }

  GenerateSubSegmentSide3DS( pObject, j, end_nb_tex, &lft, 1, dx, dy );
  pObject->bFlipNewFace = true; // For CullFace
  GenerateSubSegmentSide3DS( pObject, j, end_nb_tex, &rgt, 0, -dx, -dy );
}

/**
 * Generate a side (left or right) one sub segment of a segment
 */
void TrackEditor::GenerateSubSegmentSide3DS( Model_3DS::Object * pObject, int j, double end_nb_tex, TrackEditorSideData * side, bool bTextureSide, double dx, double dy )
{
  Vertex v;
  Vect2D t;
  double fSubSize = side->m_fBorderSize - side->m_fBarrierSize;
  double fTotalSize = side->m_fBorderSize + side->m_fBorderTreeSize;
  double fBorderLength, fTexCoord;

  // redwhite 
  if( side->m_fRedWhiteSize>0 )
  {
    // red_white is between border and road
    v.SetXYZ( side->m_fEndX+dx*side->m_fRedWhiteSize, side->m_fEndY+dy*side->m_fRedWhiteSize, side->m_fEndZ );
    t.SetXY( end_nb_tex, bTextureSide );
    side->p_redwhite_end = pObject->AddVertex( v, t );
  }

  // border between road and barrier
  fBorderLength = side->m_fBorderSize;
  fTexCoord = fBorderLength/TEXTURE_UNIT;
  t.SetXY( end_nb_tex, fTexCoord );
  v.SetXYZ( side->m_fEndX+dx*side->m_fRedWhiteSize, side->m_fEndY+dy*side->m_fRedWhiteSize, side->m_fEndZ );
  side->p_border_barrier_end = pObject->AddVertex( v, t );

  // barrier
  v.SetXYZ( side->m_fEndX+dx*fSubSize, side->m_fEndY+dy*fSubSize, side->m_fEndZ );
  t.SetXY( end_nb_tex, 0 );
  side->p_barrier_bottom_end = pObject->AddVertex( v, t ); 
  v.SetXYZ( side->m_fEndX+dx*fSubSize, side->m_fEndY+dy*fSubSize, side->m_fEndZ+side->m_fBarrierHeight );
  t.SetXY( end_nb_tex, 1 );
  side->p_barrier_top_end = pObject->AddVertex( v, t );

  // Volumetric barrier (Mostly RedWhite barrier)
  if( side->m_fBarrierSize>0 )
  {
    v.SetXYZ( side->m_fEndX+dx*side->m_fBorderSize, side->m_fEndY+dy*side->m_fBorderSize, side->m_fEndZ );
    t.SetXY( end_nb_tex, 1 );
    side->p_barrier2_bottom_end = pObject->AddVertex( v, t ); 
    v.SetXYZ( side->m_fEndX+dx*side->m_fBorderSize, side->m_fEndY+dy*side->m_fBorderSize, side->m_fEndZ+side->m_fBarrierHeight );
    t.SetXY( end_nb_tex, 0 );
    side->p_barrier2_top_end = pObject->AddVertex( v, t );
  }

  // second border (between tree and barrier)
  if( !side->m_bShowRelief )
  {
    fBorderLength = vec_mag( side->m_fBorderTreeSize, side->m_fBorderTreeHeight ) ;
    fTexCoord = fBorderLength/TEXTURE_UNIT;
    t.SetXY( end_nb_tex, fTexCoord );
    v.SetXYZ( side->m_fEndX+dx*side->m_fBorderSize, side->m_fEndY+dy*side->m_fBorderSize, side->m_fEndZ );
    side->p_border_tree_end = pObject->AddVertex( v, t );

    // tree 
    t.SetXY( end_nb_tex, 0 );
    v.SetXYZ( side->m_fEndX+dx*fTotalSize, side->m_fEndY+dy*fTotalSize, side->m_fEndZ+side->m_fBorderTreeHeight );
    side->p_tree_bottom_end = pObject->AddVertex( v, t );
    t.SetXY( end_nb_tex, 1 );
    v.SetXYZ( side->m_fEndX+dx*fTotalSize, side->m_fEndY+dy*fTotalSize, side->m_fEndZ+side->m_fBorderTreeHeight+TREE_HEIGHT );
    side->p_tree_top_end = pObject->AddVertex( v, t );
  }

  // When j=0, there are only 2 points available
  if( j>0 )
  {
    // redwhite
    if( side->m_fRedWhiteSize>0 )
    {
      pObject->AddRect( mf_red_white, side->p_road_beg, side->p_road_end, side->p_redwhite_end, side->p_redwhite_beg );
    }

    // border between road and barrier
    pObject->AddRect( side->mf_border, side->p_barrier_bottom_beg, side->p_border_barrier_beg, side->p_border_barrier_end, side->p_barrier_bottom_end );

    if( side->m_bShow )
    {
      // barrier 
      if( side->m_bShowBarrier )
      {
        pObject->AddRect( side->mf_barrier, side->p_barrier_bottom_beg, side->p_barrier_bottom_end, side->p_barrier_top_end, side->p_barrier_top_beg );

        if( side->m_fBarrierSize>0 )
        {
          pObject->AddRect( side->mf_barrier, side->p_barrier_top_beg, side->p_barrier_top_end, side->p_barrier2_top_end, side->p_barrier2_top_beg );
          pObject->AddRect( side->mf_barrier, side->p_barrier2_top_beg, side->p_barrier2_top_end, side->p_barrier2_bottom_end, side->p_barrier2_bottom_beg );
        }
      }

      if( !side->m_bShowRelief )
      {
        // border between barrier and tree
        if( side->m_bShowBorderTree )
        {
          pObject->AddRect( side->mf_border_tree, side->p_tree_bottom_beg, side->p_border_tree_beg, side->p_border_tree_end, side->p_tree_bottom_end );
        }

        // tree left
        if( side->m_bShowTree )
        {
          pObject->AddRect( side->mf_tree, side->p_tree_bottom_beg, side->p_tree_bottom_end, side->p_tree_top_end, side->p_tree_top_beg );
        }
      }
    }

    ////
    //// Relief
    //// 
    double x = side->m_fEndX+dx*side->m_fBorderSize;
    double y = side->m_fEndY+dy*side->m_fBorderSize;
    v.SetXYZ( x, y, side->m_fEndZ );

    // get the side model
    GTrackListArea * pArea;
    if( pObject->bFlipNewFace )
    {
      pArea = &rgt.area; 
    }
    else
    {
      pArea = &lft.area; 
    }
    // Add the border point to the relief
    pArea->m_aArea[pArea->m_iNumArea-1]->Add( v ); 
  }
  // copy the end data to the begin of the next sub-segment
  side->p_road_beg            = side->p_road_end;
  side->p_redwhite_beg        = side->p_redwhite_end;

  side->p_border_barrier_beg  = side->p_border_barrier_end;
  side->p_border_tree_beg     = side->p_border_tree_end;

  side->p_barrier_top_beg     = side->p_barrier_top_end;
  side->p_barrier_bottom_beg  = side->p_barrier_bottom_end;
  side->p_barrier2_top_beg    = side->p_barrier2_top_end;
  side->p_barrier2_bottom_beg = side->p_barrier2_bottom_end;
  side->p_tree_top_beg        = side->p_tree_top_end;
  side->p_tree_bottom_beg     = side->p_tree_bottom_end;
}

//--------------------------------------------------------------------------
//                        Relief Rendering
//--------------------------------------------------------------------------

void TrackEditor::GenerateRelief(  )
{
  int i, iArea, iNumVertex, iPosExternal, iIdMaxExternal;
  Vertex v;
  Vect2D t;
  GTrackListArea listArea( lft.area, rgt.area );

  ////
  //// Step 1: Find which size x, y of the track
  ////
  double fCenterX = (currentTrack->m_fXMax+currentTrack->m_fXMin)*0.5;
  double fCenterY = (currentTrack->m_fYMax+currentTrack->m_fYMin)*0.5;
  double fRayX    = (currentTrack->m_fXMax-currentTrack->m_fXMin)*0.75+200;
  double fRayY    = (currentTrack->m_fYMax-currentTrack->m_fYMin)*0.75+200;

  ////
  //// Step 2: group the point in filling area. (an object of the model)
  ////         -> there is one external area
  ////         -> and one or more internal one
  ////
  //// TO IMPROVE
  //// Bad and easy algorithm = copy all the point of the left side, of the right side 
  ////
  listArea.SplitArea( lft.area, rgt.area );

  ////
  //// Step 3: all the areas should be in a clockwise direction
  ////
  for( iArea=0; iArea<listArea.m_iNumArea; iArea++ )
  {
    double sum_ang = listArea.m_aArea[iArea]->SumAngle();
 
    // Anti-clockwize ?
    if( sum_ang>6.2 )
    {
      listArea.m_aArea[iArea]->FlipOrder();
    }
  }

  ////
  //// Step 4: In all the areas, find the
  ////         external area
  ////
  //// Principle: the external area is the biggest one
  ////
  double emaxx = -100000;
  iPosExternal = -1;

  for( iArea=0; iArea<listArea.m_iNumArea; iArea++ )
  {
    int id = listArea.m_aArea[iArea]->FindMaxX();
    double maxx = listArea.m_aArea[iArea]->m_aVertex[id].x;

    if( maxx>emaxx )
    {
      iPosExternal = iArea;
      iIdMaxExternal = id;
      emaxx = maxx;
    }
  }

  /*
  ////
  //// Step 4: For all the filling area, take a point. Find if 
  ////         the point is an internal or an external point.
  ////
  //// Principle: take a point far away from the track
  ////            calculate the number of intersection
  ////            between the segment (point, far point)
  ////            and the track side 
  ////            For a point in the LEFT SIDE of the track
  ////            -> number of intersections/2 pair -> external 
  ////            -> number of intersections/2 impair -> internal 
  ////
  bool aExternal[64]; 

  for( iArea=0; iArea<listArea.m_iNumArea; iArea++ )
  {
    Vertex &v0 = listArea.m_aArea[iArea]->m_aVertex[0];
    Vertex &v1 = listArea.m_aArea[iArea]->m_aVertex[1];

    double dist = vec_mag( v1.x-v0.x, v1.y-v0.y );
    Vertex v2, v3;

    // The far away point is at fRay and perpendicular to the first subsegment
    v2.SetXYZ( v0.x - (v1.y-v0.y)/dist*1.0, v0.y + (v1.x-v0.x)/dist*1.0, 0 );
    v3.SetXYZ( v0.x - (v1.y-v0.y)/dist*fRay, v0.y + (v1.x-v0.x)/dist*fRay, 0 );

    //        Left      Right                 Left      Right
    //         |          |                    |          |
    //         |          |                    |          |
    //    <----|          |          or      <------------|
    //         |          |                    |          |

    int iNumInter = lft.area.NumIntersection(v2,v3) + rgt.area.NumIntersection(v2,v3); 

    // iNumInter%4==0 -> point on the left side and external.
    // iNumInter%4==3 -> point on the right side and external.
    if( (iNumInter%4)==0 || (iNumInter%4)==3  )
    {
      aExternal[iArea] = true;
      iNbExternal ++;
      if( iNbExternal==1 )
      {
        iPosExternal = iArea;
      }
    }
    else
    {
      aExternal[iArea] = false;
    }
  }

  if( iNbExternal==0 )
  {
    exitOnError( "TrackEditor::GenerateRelief: no external relief" );
  }
  if( iNbExternal>1 )
  {
    // TO DO
  }
  */
    
  ////
  //// Step 5: Flip the external area in a anti-clockwise direction
  ////         Rotate the point such that the MaxX is the first point
  ////
  iArea = iPosExternal;
  listArea.m_aArea[iArea]->FlipOrder();
  listArea.m_aArea[iArea]->RotateOrder( listArea.m_aArea[iArea]->m_iNumVertex-1-iIdMaxExternal );
  
  ////
  //// Step 6: For the external filling area, add the points 
  ////         of the external track border - a big oval or rectangle (to see )
  ////         in a clockwise direction
  ////
  Vertex &v0 = listArea.m_aArea[iArea]->m_aVertex[0];
  double fStartAngle = atan2( (v0.y-fCenterY), (v0.x-fCenterX)*fRayY/fRayX ); // !!! atan2(y,x)

  // Add the first point at the end a second time
  listArea.m_aArea[iArea]->Add( v0 ); 

  // Add the oval
  for( i=0; i<=RELIEF_BORDER_NUM; i++ )
  {
    double ang= fStartAngle + i*2.0*PI/RELIEF_BORDER_NUM; 
    v.SetXYZ( fCenterX + fRayX*cos(ang), fCenterY + fRayY*sin(ang), RELIEF_BORDER_HEIGHT + 15*sin(ang*10) );
    listArea.m_aArea[iArea]->Add( v ); 
  }

  ////
  //// Step 8 : Copy all the Area to a Model_3DS
  ////
  Model_3DS &model = track3D.m_ModelRelief;
  listArea.CopyAll( model );

  ////
  //// Step 9: create 3D objects based on this model
  ////
  int mat_relief = model.AddMaterialTexture( "", "grass_out" );
  for( iArea=0; iArea<model.numObjects; iArea++ )
  {
    Model_3DS::Object * obj = &model.aObject[iArea];
    Vertex * v = obj->aRealVertex;
    iNumVertex = obj->numRealVertex;

    int mf_relief  = obj->AddMatFace( mat_relief );

    bool * aUsed = new bool[iNumVertex];

    for( i=0; i<iNumVertex; i++ )
    {
      aUsed[i] = false;
    }

    int iNumUsed=0, last = 0;
    i = 0;
    while( iNumUsed+2<iNumVertex )
    {
      int nxt  = NextUnused( i, aUsed, iNumVertex );
      int nxt2 = NextUnused( nxt, aUsed, iNumVertex );

      // Check if the triangle has the 3 points in a anti-clock direction
      bool bAddPoint = false;
      if( TGeom::LeftSide(v[i], v[nxt], v[nxt2])> 0.05 )
      {
        bAddPoint = true;
        // Check if there is no point inside the new face
        for( int j=0; j<iNumVertex; j++ )
        {
          if( j!=i && j!=nxt && j!=nxt2 )
          {
            double d1 = TGeom::LeftSide(v[i], v[nxt], v[j]);
            if( d1<=0 ) continue;
            double d2 = TGeom::LeftSide(v[nxt], v[nxt2], v[j]);
            if( d2<=0 ) continue;
            double d3 = TGeom::LeftSide(v[nxt2], v[i], v[j]);
            if( d3<=0 ) continue;
            bAddPoint = false;
            break;
          }
        }
      }

      if( bAddPoint )
      {
        obj->AddFace( mf_relief, i, nxt, nxt2 );

        aUsed[nxt] = true;
        i = nxt2;
        iNumUsed ++;
        last = nxt2;
      }
      else
      {
        i = nxt;
        if( nxt==last )
        {
          break;
        }
      }
    }

    delete aUsed;
  } // iArea

  ////
  //// Step 10: Add points for a tree in the oval
  ////
  int iTree =  model.AddObject();
  Model_3DS::Object * obj = &model.aObject[iTree];
  int mat_tree = model.AddMaterialTexture( "", "tree" );
  int mf_tree  = obj->AddMatFace( mat_tree );

  for( i=0; i<=RELIEF_BORDER_NUM; i++ )
  {
    double ang= fStartAngle + i*2.0*PI/RELIEF_BORDER_NUM; 
    v.SetXYZ( fCenterX + fRayX*cos(ang), fCenterY + fRayY*sin(ang), RELIEF_BORDER_HEIGHT + 15*sin(ang*10) );
    t.SetXY( ang * (fRayX+fRayY)/200, 0 ); 
    model.aObject[iArea].AddVertex( v, t, false ); 
    v.z += float( TREE_HEIGHT + 15*sin(ang*30) );
    t.y = 1; 
    model.aObject[iArea].AddVertex( v, t, false ); 
    if( i>0 )
    {
      obj->AddFace( mf_tree, (i-1)*2, (i-1)*2+1, i*2 );
      obj->AddFace( mf_tree, (i-1)*2+1, i*2+1, i*2 );
    }
  }

  model.PostCalc();
}

int TrackEditor::NextUnused( int i, bool aUsed[], int iNumVertex )
{
  int nxt = i;
  do
  {
    nxt = mod( nxt+1, iNumVertex );
  } while( aUsed[nxt] );

  return nxt;
}

//--------------------------------------------------------------------------
// Code for the graphical editor
//--------------------------------------------------------------------------

/**
 * Load a track
 */
void TrackEditor::Load( const char * name ) 
{
  // close the previous track (if any) open the new one
  OnFileClose();
  currentTrack = new Track( (char *)name );

  g_ViewManager->GetOpenGlView()->FullScreenScale();
  Rebuild();
  m_iCurSegment = 0;
  m_iCurSection = 0;

  Refresh();
}

/**
 * Check the current position
 */
void TrackEditor::CheckCurrent()
{
  if( m_iCurSection<0 )
  {
    m_iCurSection = 0; 
  }
  else if( m_iCurSection>=currentTrack->m_iNumSection )
  {
    m_iCurSection = currentTrack->m_iNumSection-1; 
  }
  if( m_iCurSegment<0 )
  {
    m_iCurSegment = 0; 
  }
  else if( m_iCurSegment>=currentTrack->m_iNumSegment )
  {
    m_iCurSegment = currentTrack->m_iNumSegment-1; 
  }

  int iSection = currentTrack->GetSectionId( m_iCurSegment );
  if( iSection!= m_iCurSection )
  {
    m_iCurSection = iSection;
  }
}

/**
 * Common function for set and reset properties
 */
void TrackEditor::SetResetCommon() 
{
  int iSeg = m_iCurSegment;
  m_oActionManager.Do( new ActionSegmentChange(iSeg, m_Segment) );
  Refresh( false );
}

/**
 * Rebuild the track
 */
void TrackEditor::Rebuild( bool bSave, bool bCreateList )
{
  char path[256];
  char name[256];
  sprintf( path, "tracks/%s", currentTrack->m_sShortName );

  // Point of the track (lftwall,rgtwall)
  currentTrack->Rebuild();

  // 3DS model
  if( m_iViewMode!=TRACKED_VIEW_MODE_2D )
  {
    PreCalc();

    lft.area.Reset();
    rgt.area.Reset();

    int iSection=0;
    for( int i=0; i<currentTrack->m_iNumSegment; i++ )
    {
      if( currentTrack->m_aSection[iSection].m_iFirstSeg==i )
      {
        // Add a new area per section for the relief
        lft.area.Add( new GTrackArea(AREA_TYPE_LEFT) );
        rgt.area.Add( new GTrackArea(AREA_TYPE_RIGHT) );
        iSection ++;
      }

      if( strcmp( currentTrack->m_aSeg[i].m_sModel, "" )!=0 )
      {
        sprintf( name, "%s/%s", path, currentTrack->m_aSeg[i].m_sModel );
        track3D.m_aModel[i].m_bLit = false;
        track3D.m_aModel[i].scale = 1.0f; // 3.5f;
        track3D.m_aModel[i].Load(name);
      }
      else
      {
        GenerateSegmentModel( track3D.m_aModel[i], i );
        if( bSave )
        {
          sprintf( name, "track%d.3ds", i );
          track3D.m_aModel[i].Save( path, name );
        }
      }

      if( bCreateList )
      {
        track3D.m_aModel[i].CalculateSphere();
      }
    }

    GenerateRelief();
    if( bSave )
    {
      track3D.m_ModelRelief.Save( path, "relief.3ds" );
    }

    PostCalc();
  }
}

//--------------------------------------------------------------------------
// Portable Actions
//--------------------------------------------------------------------------

/**
 * New track
 */
void TrackEditor::OnFileNew() 
{
  Load( "oval2.trk" );
  strcpy( currentTrack->m_sFileName, "new.trx" );
}

/**
 * Save the file
 */
void TrackEditor::OnFileSave() 
{
  if( currentTrack )
  {
    currentTrack->SaveXml();
  }
}

/**
 * Close the current Track
 */
void TrackEditor::OnFileClose() 
{
  // Remove all do/undo action
  m_oActionManager.Reset();

  if( currentTrack!=NULL )
  {
	  delete currentTrack;
    currentTrack = NULL;
    Refresh();
  }
}

/**
 * Undo
 */
void TrackEditor::OnEditUndo() 
{
  m_oActionManager.Undo();
  Refresh();
}

/**
 * Redo
 */
void TrackEditor::OnEditRedo() 
{
  m_oActionManager.Redo();
  Refresh();
}

/**
 * Called when clicking on the button set
 */
void TrackEditor::OnPropSet() 
{
  if( m_pStructProp!=NULL )
  {
    UpdateData( true );
    m_pStructProp->SetString( m_pStructBase, m_sPropValue );
    SetResetCommon();
  }
}

/**
 * Called when clicking on the button set
 */
void TrackEditor::OnPropReset() 
{
  if( m_pStructProp!=NULL )
  {
    m_pStructProp->CopyValue( m_pStructBase, m_pDefaultBase );
    SetResetCommon();
  }
}

/** 
 * Add a new segment
 */
void TrackEditor::OnSegmentNew() 
{
  if( currentTrack )
  {
    int iSeg = m_iCurSegment;
    int iSection = currentTrack->GetSectionId( iSeg );
    m_oActionManager.Do( new ActionSegmentNew(iSeg+1, currentTrack->m_aSection[iSection].m_sName) );
    m_iCurSegment = iSeg+1;
    Refresh();
  }
}

/** 
 * Delete a segment
 */
void TrackEditor::OnSegmentDelete() 
{
  if( currentTrack )
  {
    int iSeg = m_iCurSegment;
    m_oActionManager.Do( new ActionSegmentDelete(iSeg) );
    Refresh();
  }
}

/**
 * Info on the segment
 */
void TrackEditor::OnSegmentInfo() 
{
  if( currentTrack )
  {
    char sx[128], sy[128], sa[128];
    segment * seg = &(currentTrack->rgtwall[m_iCurSegment]);

    sprintf( sx, "end_x : %s", Double2String(seg->end_x) ); 
    sprintf( sy, "end_y : %s", Double2String(seg->end_y) ); 
    sprintf( sa, "end_ang : %s", Double2String(seg->end_ang) ); 
    warning( "%s\n%s\n%s", sx, sy, sa );
  }
}

/**
 * Go to the next segment
 */
void TrackEditor::OnSegmentNxt() 
{
  if( currentTrack )
  {
    int iSection = currentTrack->GetSectionId( m_iCurSegment );
    if( m_iCurSegment==currentTrack->m_aSection[iSection].m_iLastSeg )
    {
      m_iCurSegment = currentTrack->m_aSection[iSection].m_iFirstSeg ;
    }
    else
    {
      m_iCurSegment ++;
    }
    Refresh( false );
  }
}

/**
 * Go to the previous segment
 */
void TrackEditor::OnSegmentPrv() 
{
  if( currentTrack )
  {
    int iSection = currentTrack->GetSectionId( m_iCurSegment );

    if( m_iCurSegment==currentTrack->m_aSection[iSection].m_iFirstSeg )
    {
      m_iCurSegment = currentTrack->m_aSection[iSection].m_iLastSeg ;
    }
    else
    {
      m_iCurSegment --;
    }
    Refresh( false );
  }
}

/**
 * Copy the current Property in the clipboard
 */
void TrackEditor::OnEditCopy() 
{
  if( currentTrack )
  {
    m_iClipBoardSegmentId = m_iCurSegment;
    if( m_pStructProp!=NULL )
    {
      UpdateData( true );
      strcpy( m_sClipBoardValue, m_sPropValue );
      m_pClipBoardProp = m_pStructProp;
      m_pClipBoardBase = m_pDefaultBase;
      sprintf( m_sClipBoardText, "Segment ID : %d\nProperty : %s\nValue : %s", m_iClipBoardSegmentId, m_pStructProp->sXmlTag, m_sClipBoardValue );
    }	
    else
    {
      m_pClipBoardProp = NULL;
      sprintf( m_sClipBoardText, "Segment ID : %d", m_iClipBoardSegmentId );
    }
    Refresh( false );
  }
}

/**
 * Paste one property 
 */
void TrackEditor::OnEditPaste() 
{
  if( m_pClipBoardProp!=NULL )
  {
    m_pStructProp = m_pClipBoardProp;
    m_pDefaultBase = m_pClipBoardBase;
    EditProperty();
    strcpy( m_sPropValue, m_sClipBoardValue );
    UpdateData( false );
    OnPropSet();
  }	
}

/**
 * Paste all properties the clipboard
 */
void TrackEditor::OnEditPasteAll() 
{
  if( m_iClipBoardSegmentId>=0 )
  {
    int iSeg = m_iCurSegment;
    Segment3D seg3D = currentTrack->m_aSeg[m_iClipBoardSegmentId];
    m_oActionManager.Do( new ActionSegmentChange(iSeg, seg3D) );
    Refresh( false );
  }	
}

/**
 * View Modes
 */
void TrackEditor::OnViewTrack2d() 
{
  if( currentTrack )
  {
    g_TrackEditor.m_oActionManager.Do( new ActionChangeViewMode(TRACKED_VIEW_MODE_2D) );
  }
}

void TrackEditor::OnViewTrack3ds() 
{
  if( currentTrack )
  {
    g_TrackEditor.m_oActionManager.Do( new ActionChangeViewMode(TRACKED_VIEW_MODE_3DS) );
  }
}

void TrackEditor::OnViewTrackOneSeg() 
{
  if( currentTrack )
  {
    g_TrackEditor.m_oActionManager.Do( new ActionChangeViewMode(TRACKED_VIEW_MODE_ONE_SEG) );
  }
}

/**
 * Generate all the 3DS files of the track
 */
void TrackEditor::OnTrackGen3d()
{
  Rebuild( true );
}

/**
 * Helps to aucolose the track
 */
void TrackEditor::OnTrackAutoclosure() 
{
  if( currentTrack!=NULL && currentTrack->NSEG>=4 )
  {
    m_oActionManager.Do( new ActionAutoClosure() );
  }
  else
  {
    warning( "You need at least 4 segments to autoclose the track" );
  }
}

//--------------------------------------------------------------------------
//                      Class ActionManager
//--------------------------------------------------------------------------

/**
 * Constructor
 */
ActionManager::ActionManager()
{
  Init();
}

/**
 * Destructor
 */
ActionManager::~ActionManager()
{
  RemoveNextActions( m_ListAction );
}

/**
 * Init the ActionManager
 */
void ActionManager::Init()
{
  // The first action in the list is a dummy action
  m_pLastAction = new Action();
  m_pLastAction->m_pPrev = NULL;
  m_ListAction = m_pLastAction;
}

/**
 * Reset = Remove all actions (except the dummy one)
 */
void ActionManager::Reset()
{
  RemoveNextActions( m_ListAction );
  Init();
}

/**
 * IsEmpty - return true if there is no action in the list
 */
bool ActionManager::IsEmpty()
{
  return m_pLastAction->m_pPrev==NULL;
}

/**
 * Add a new action
 */
void ActionManager::AddAction( Action * a )
{
  if( m_pLastAction!=NULL )
  {
    if( m_pLastAction->m_pNext!=NULL )
    {
      // Undo has been executed -> remove next actions
      RemoveNextActions( m_pLastAction->m_pNext );
    }
    m_pLastAction->m_pNext = a;
  }
  a->m_pPrev = m_pLastAction; 
  m_pLastAction = a;
}

/**
 * Remove the current action and all the next ones.
 */
void ActionManager::RemoveNextActions( Action * a )
{
  Action * p = a;
  while( p!=NULL )
  {
    Action * d = p;
    p = p->m_pNext;
    delete d;
  }
}

/**
 * Add a new action and execute it
 */
void ActionManager::Do( Action * a )
{
  AddAction( a );
  a->Do();
}

/**
 * Undo an action
 */
void ActionManager::Undo()
{
  if( m_pLastAction->m_pPrev!=NULL )
  {
    m_pLastAction->Undo();
    m_pLastAction = m_pLastAction->m_pPrev;
  }
}

/**
 * Redo an action
 */
void ActionManager::Redo()
{
  if( m_pLastAction->m_pNext!=NULL )
  {
    m_pLastAction = m_pLastAction->m_pNext;
    m_pLastAction->Do();
  }
}

//--------------------------------------------------------------------------
//                      Class ActionSegmentChange
//--------------------------------------------------------------------------

/**
 * Change track segment
 */
ActionSegmentChange::ActionSegmentChange( int iSeg, Segment3D seg3D ):Action()
{
  // Make sure the record is coherent
  seg3D.Rebuild();

  m_iSeg = iSeg;
  m_NewSeg3D = seg3D;
  m_OldSeg3D = currentTrack->m_aSeg[m_iSeg];
}

void ActionSegmentChange::Do()
{
  currentTrack->m_aSeg[m_iSeg] = m_NewSeg3D;
  g_TrackEditor.Rebuild();
}

void ActionSegmentChange::Undo()
{
  currentTrack->m_aSeg[m_iSeg] = m_OldSeg3D;
  g_TrackEditor.Rebuild();
}

//--------------------------------------------------------------------------
//                      Class ActionSegmentNew
//--------------------------------------------------------------------------

/**
 * New track segment
 */
ActionSegmentNew::ActionSegmentNew( int iSeg, const char * section ):Action()
{
  m_iSeg = iSeg;
  strcpy( m_sSection, section );
}

void ActionSegmentNew::Do()
{
  currentTrack->AddSegment( m_iSeg, m_sSection );
  currentTrack->m_aSeg[m_iSeg].m_fLength = 100.0;
  g_TrackEditor.Rebuild();
}

void ActionSegmentNew::Undo()
{
  currentTrack->RemoveSegment( m_iSeg );
  g_TrackEditor.Rebuild();
}

//--------------------------------------------------------------------------
//                      Class ActionSegmentDelete
//--------------------------------------------------------------------------

/**
 * Delete a track segment
 */
ActionSegmentDelete::ActionSegmentDelete( int iSeg ):Action()
{
  m_iSeg = iSeg;
  m_OldSeg = currentTrack->m_aSeg[m_iSeg];
  m_iSection = currentTrack->GetSectionId( iSeg );
  strcpy( m_sSection, currentTrack->m_aSection[m_iSection].m_sName ); 
}

void ActionSegmentDelete::Do()
{
  currentTrack->RemoveSegment( m_iSeg );
  g_TrackEditor.Rebuild();
}

void ActionSegmentDelete::Undo()
{
  currentTrack->AddSegment( m_iSeg, m_sSection );
  currentTrack->m_aSeg[m_iSeg] = m_OldSeg;
  g_TrackEditor.Rebuild();
}

//--------------------------------------------------------------------------
//                      Class ActionChangeViewMode
//--------------------------------------------------------------------------

/**
 * Change the view Mode
 */
ActionChangeViewMode::ActionChangeViewMode( int iViewMode ):Action()
{
  m_iViewMode = iViewMode;
  m_iOldViewMode = g_TrackEditor.m_iViewMode;
}

void ActionChangeViewMode::Do()
{
  g_TrackEditor.m_iViewMode = m_iViewMode;
  g_TrackEditor.Rebuild();
}

void ActionChangeViewMode::Undo()
{
  g_TrackEditor.m_iViewMode = m_iOldViewMode;
  g_TrackEditor.Rebuild();
}

//--------------------------------------------------------------------------
//                      Class ActionAutoClosure
//--------------------------------------------------------------------------

/**
 * Change the view Mode
 */
ActionAutoClosure::ActionAutoClosure():Action()
{
  int i, j;

  for( i=0; i<currentTrack->m_iNumSection; i++ )
  {
    Section * section = &currentTrack->m_aSection[i];
    m_iLastCurve[i] = -1;
    m_iLastStraight[i] = -1;

    for( j=section->m_iLastSeg; j>=section->m_iFirstSeg; j-- ) 
    {
      if( m_iLastStraight[i]<0 && currentTrack->m_aSeg[j].m_fRadius==0.0 ) 
      {
        m_iLastStraight[i] = j;
      }
      else if( m_iLastCurve[i]<0 && currentTrack->m_aSeg[j].m_fRadius!=0.0 ) 
      {
        m_iLastCurve[i] = j;
      }
    }

    if( m_iLastStraight[i]>=0 && m_iLastCurve[i]>=0 )
    {
      m_LastCurve[i] = currentTrack->m_aSeg[m_iLastCurve[i]];
      m_LastStraight[i] = currentTrack->m_aSeg[m_iLastStraight[i]];
    }
  }
}

void ActionAutoClosure::Do()
{
  for( int i=0; i<currentTrack->m_iNumSection; i++ )
  {
    if( m_iLastStraight[i]>=0 && m_iLastCurve[i]>=0 )
    {
      // Angle 
      Section * section = &currentTrack->m_aSection[i];
      segment * rgtwall = currentTrack->rgtwall;
      Segment3D * m_aSeg = currentTrack->m_aSeg;

      int iNextSeg = currentTrack->m_aSeg[section->m_iLastSeg].m_iNxtSeg;

      double d_ang = rgtwall[iNextSeg].beg_ang-rgtwall[section->m_iLastSeg].end_ang;
      if( d_ang>PI )
      {
        d_ang -= 2*PI;
      }
      if( d_ang<-PI )
      {
        d_ang += 2*PI;
      }
      if( m_aSeg[m_iLastCurve[i]].m_fRadius>0.0 )
      {
        m_aSeg[m_iLastCurve[i]].m_fArc +=d_ang; 
      }
      else
      {
        m_aSeg[m_iLastCurve[i]].m_fArc -=d_ang; 
      }

      currentTrack->Rebuild();

      // distance
      // The program tries to add a delta to the radius of the last curve such that the 
      // distance of closure become as small as possible.

      double dist, best_dist, best_radius, best_length, delta;

      best_dist = vec_mag( rgtwall[iNextSeg].beg_x-rgtwall[section->m_iLastSeg].end_x, rgtwall[iNextSeg].beg_y-rgtwall[section->m_iLastSeg].end_y );
      best_radius = m_aSeg[m_iLastCurve[i]].m_fRadius;
      best_length = m_aSeg[m_iLastStraight[i]].m_fLength;

      for( int j=0; j<50; j++ )
      {
        delta = 10;
        while( fabs(delta)>0.001 ) 
        {
          rgtwall[m_iLastCurve[i]].radius = best_radius + delta;
          currentTrack->track_setup( currentTrack->m_fRgtStartX, currentTrack->m_fRgtStartY, currentTrack->m_fStartAng, currentTrack->rgtwall );
          dist = vec_mag( rgtwall[iNextSeg].beg_x-rgtwall[section->m_iLastSeg].end_x, rgtwall[iNextSeg].beg_y-rgtwall[section->m_iLastSeg].end_y );
          if( dist<best_dist )
          {
            best_dist = dist;
            best_radius = rgtwall[m_iLastCurve[i]].radius;
          }
          delta *= -0.95;
        }
        rgtwall[m_iLastCurve[i]].radius = best_radius;

        delta = 10;
        while( fabs(delta)>0.001 ) 
        {
          rgtwall[m_iLastStraight[i]].length = best_length + delta;
          currentTrack->track_setup( currentTrack->m_fRgtStartX, currentTrack->m_fRgtStartY, currentTrack->m_fStartAng, currentTrack->rgtwall );
          dist = vec_mag( rgtwall[iNextSeg].beg_x-rgtwall[section->m_iLastSeg].end_x, rgtwall[iNextSeg].beg_y-rgtwall[section->m_iLastSeg].end_y );
          if( dist<best_dist )
          {
            best_dist = dist;
            best_length = rgtwall[m_iLastStraight[i]].length;
          }
          delta *= -0.95;
        }
        rgtwall[m_iLastStraight[i]].length = best_length;
      }
      m_aSeg[m_iLastCurve[i]].m_fRadius = best_radius;
      m_aSeg[m_iLastStraight[i]].m_fLength = best_length;

      currentTrack->track_setup( currentTrack->m_fRgtStartX, currentTrack->m_fRgtStartY, currentTrack->m_fStartAng, currentTrack->rgtwall );
      dist = vec_mag( rgtwall[iNextSeg].beg_x-rgtwall[section->m_iLastSeg].end_x, rgtwall[iNextSeg].beg_y-rgtwall[section->m_iLastSeg].end_y );
      g_TrackEditor.Rebuild();
      currentTrack->track_setup( currentTrack->m_fRgtStartX, currentTrack->m_fRgtStartY, currentTrack->m_fStartAng, currentTrack->rgtwall );
      dist = vec_mag( rgtwall[iNextSeg].beg_x-rgtwall[section->m_iLastSeg].end_x, rgtwall[iNextSeg].beg_y-rgtwall[section->m_iLastSeg].end_y );
    }
  }

  g_TrackEditor.Rebuild();
}

void ActionAutoClosure::Undo()
{
  for( int i=0; i<currentTrack->m_iNumSection; i++ )
  {
    currentTrack->m_aSeg[m_iLastCurve[i]] = m_LastCurve[i];
    currentTrack->m_aSeg[m_iLastStraight[i]] = m_LastStraight[i];
  }
  g_TrackEditor.Rebuild();
}
