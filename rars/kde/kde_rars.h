/*
 * KDE_RARS.H - Main Window of RARS in KDE 2
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@synet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <kapp.h>
#include <kmainwindow.h>
#include <qgl.h>

class QLabel;

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

extern KApplication * kde_app;

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------


////
//// Class MyImage
////
class MyImage : public QWidget
{
  Q_OBJECT

  int m_iImageId;

public:
  MyImage( int image_id, QWidget * parent);
protected:
  void paintEvent(QPaintEvent *);
};

////
//// Class KRars
////
class KdeRars: public KMainWindow
{
  Q_OBJECT

  public:
  /**
   * Create the widget
   **/
  KdeRars( const char * name =0 );

  public slots:
    void slotTimeOut();
    void slotCarNext();
    void slotCarPrevious();
    void slotSpeedRewind();
    void slotSpeedStop();
    void slotSpeedSlow();
    void slotSpeedMedium();
    void slotSpeedFast();
    void slotChangeView();
    void slotViewClassic();
    void slotViewFullScreen();
    void slotViewTelemetry();
    void slotViewWindow();
    void slotViewZoom();
    void slotViewTrajectory();
    void slotViewDriverNames();
    void slotFollowCarManual();
    void slotFollowCarNobody();
    void slotFollowCarPosition();
    void slotFollowCarOvertaking();
    void slotZoomIn();
    void slotZoomOut();
    void slotSettingsSave();
    void slotMoveUp();
    void slotMoveDown();
    void slotMoveLeft();
    void slotMoveRight();

  protected:
     virtual void resizeEvent ( QResizeEvent * );

  private:
    void SetOpenGLMode( int mode );
    void InitDialog();
    void SetImageGeometry();

    QTimer * timer;
    QWidget * widget_view;
    QWidget * image_opengl;
    QWidget * image[3];
    int m_offx;
    int m_offy;
};

