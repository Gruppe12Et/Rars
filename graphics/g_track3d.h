/**
 * g_track3d.h - track in 3 dimensions
 *
 * History
 *  ver. 0.90 Created 10.09.2001
 *
 * @author    Marc Gueury
 * @version   0.90
 */

#ifndef __G_TRACK3D_H
#define __G_TRACK3D_H

//--------------------------------------------------------------------------
//                           D E F I N E S
//--------------------------------------------------------------------------

#define MAX_MODEL 1000

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * To sort the transparent objects
 */
struct Seen
{
  Model_3DS * pModel;
  float dist;
};

/**
 * TTrack3D
 */
class TTrack3D
{
  private:
    Model_3DS m_aModel[MAX_MODEL];
    int       m_iNbModel;
    Model_3DS * m_ModelSky;
    Model_3DS m_ModelRelief;

    Model_3DS * m_aOject3DModel[MAX_MODEL];

    float  m_aFrustum[6][4];

    float SphereInFrustum( float x, float y, float z, float radius );
    void ExtractFrustum();

    void SelectionSort( Seen data[], int left, int right);
    int Partition( Seen d[], int left, int right);
    void Quicksort( Seen d[], int left, int right);

  public:
    TTrack3D();
    ~TTrack3D();

    void Load();
    void DrawSky( double cam_x, double cam_y );
    void Draw();
    void DrawRoad3D();
    void DrawObject3D();
    void DrawRoad( bool bTexture );
    void Destroy();

    double CarHeight( int iCar, double to_end_offset, double &banking );
    double CarAngWithRoad( int iCar );

    friend class TrackEditor;
};

#endif


