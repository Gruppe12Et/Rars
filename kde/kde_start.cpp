#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kde_start.ui'
**
** Created: Mo Sep 13 15:02:15 2010
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kde_start.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KdeStart as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KdeStart::KdeStart( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KdeStart" );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setGeometry( QRect( 280, 10, 510, 230 ) );

    LineLastResult = new QMultiLineEdit( GroupBox3, "LineLastResult" );
    LineLastResult->setGeometry( QRect( 10, 20, 490, 200 ) );
    QFont LineLastResult_font(  LineLastResult->font() );
    LineLastResult_font.setFamily( "clean" );
    LineLastResult->setFont( LineLastResult_font ); 

    ButtonGroupRace = new QButtonGroup( this, "ButtonGroupRace" );
    ButtonGroupRace->setGeometry( QRect( 10, 250, 260, 130 ) );

    RadioQualif = new QRadioButton( ButtonGroupRace, "RadioQualif" );
    RadioQualif->setEnabled( FALSE );
    RadioQualif->setGeometry( QRect( 20, 70, 220, 21 ) );
    ButtonGroupRace->insert( RadioQualif, 0 );

    RadioRaceOld = new QRadioButton( ButtonGroupRace, "RadioRaceOld" );
    RadioRaceOld->setEnabled( FALSE );
    RadioRaceOld->setGeometry( QRect( 20, 90, 211, 21 ) );
    ButtonGroupRace->insert( RadioRaceOld, 0 );

    RadioQualifRace = new QRadioButton( ButtonGroupRace, "RadioQualifRace" );
    RadioQualifRace->setGeometry( QRect( 20, 50, 210, 21 ) );
    ButtonGroupRace->insert( RadioQualifRace, 0 );

    RadioRaceRandom = new QRadioButton( ButtonGroupRace, "RadioRaceRandom" );
    RadioRaceRandom->setGeometry( QRect( 20, 30, 210, 21 ) );
    RadioRaceRandom->setChecked( TRUE );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setGeometry( QRect( 10, 390, 260, 90 ) );

    LabelQualif = new QLabel( GroupBox2, "LabelQualif" );
    LabelQualif->setGeometry( QRect( 10, 20, 110, 21 ) );

    LabelsNumberQualif = new QLabel( GroupBox2, "LabelsNumberQualif" );
    LabelsNumberQualif->setGeometry( QRect( 10, 50, 150, 21 ) );

    SpinQualifLaps = new QSpinBox( GroupBox2, "SpinQualifLaps" );
    SpinQualifLaps->setEnabled( FALSE );
    SpinQualifLaps->setGeometry( QRect( 170, 20, 80, 21 ) );
    SpinQualifLaps->setMaxValue( 20 );
    SpinQualifLaps->setMinValue( 1 );
    SpinQualifLaps->setValue( 1 );

    SpinQualifNumber = new QSpinBox( GroupBox2, "SpinQualifNumber" );
    SpinQualifNumber->setEnabled( FALSE );
    SpinQualifNumber->setGeometry( QRect( 170, 50, 80, 21 ) );
    SpinQualifNumber->setMinValue( 1 );
    SpinQualifNumber->setValue( 1 );

    PushStart = new QPushButton( this, "PushStart" );
    PushStart->setGeometry( QRect( 280, 490, 101, 31 ) );

    PushExit = new QPushButton( this, "PushExit" );
    PushExit->setGeometry( QRect( 410, 490, 101, 31 ) );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 10, 10, 260, 230 ) );

    ComboTrack = new QComboBox( FALSE, GroupBox1, "ComboTrack" );
    ComboTrack->setGeometry( QRect( 110, 20, 130, 21 ) );

    LabelTrack = new QLabel( GroupBox1, "LabelTrack" );
    LabelTrack->setGeometry( QRect( 10, 21, 61, 20 ) );

    LabelLaps = new QLabel( GroupBox1, "LabelLaps" );
    LabelLaps->setGeometry( QRect( 10, 50, 61, 21 ) );

    SpinLaps = new QSpinBox( GroupBox1, "SpinLaps" );
    SpinLaps->setGeometry( QRect( 110, 50, 130, 21 ) );
    SpinLaps->setMinValue( 1 );
    SpinLaps->setValue( 80 );

    LabelOpenGL = new QLabel( GroupBox1, "LabelOpenGL" );
    LabelOpenGL->setGeometry( QRect( 10, 80, 81, 21 ) );

    ComboOpenGL = new QComboBox( FALSE, GroupBox1, "ComboOpenGL" );
    ComboOpenGL->setGeometry( QRect( 111, 80, 130, 21 ) );

    LabelMovie = new QLabel( GroupBox1, "LabelMovie" );
    LabelMovie->setGeometry( QRect( 10, 110, 71, 21 ) );

    ComboMovie = new QComboBox( FALSE, GroupBox1, "ComboMovie" );
    ComboMovie->setGeometry( QRect( 110, 110, 130, 21 ) );

    LabelRandom = new QLabel( GroupBox1, "LabelRandom" );
    LabelRandom->setGeometry( QRect( 10, 140, 80, 21 ) );

    SpinRandom = new QSpinBox( GroupBox1, "SpinRandom" );
    SpinRandom->setGeometry( QRect( 110, 140, 131, 21 ) );
    SpinRandom->setMaxValue( 65535 );

    TextLabel1_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 10, 200, 81, 21 ) );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 10, 170, 81, 21 ) );

    ComboFollowCar = new QComboBox( FALSE, GroupBox1, "ComboFollowCar" );
    ComboFollowCar->setGeometry( QRect( 110, 170, 130, 21 ) );

    ComboSurface = new QComboBox( FALSE, GroupBox1, "ComboSurface" );
    ComboSurface->setGeometry( QRect( 110, 200, 130, 21 ) );

    GroupBox4 = new QGroupBox( this, "GroupBox4" );
    GroupBox4->setGeometry( QRect( 280, 250, 510, 231 ) );

    PushDriverRemoveOne = new QPushButton( GroupBox4, "PushDriverRemoveOne" );
    PushDriverRemoveOne->setGeometry( QRect( 230, 170, 50, 30 ) );

    PushDriverSelectOne = new QPushButton( GroupBox4, "PushDriverSelectOne" );
    PushDriverSelectOne->setGeometry( QRect( 230, 130, 50, 30 ) );

    PushDriverRemoveAll = new QPushButton( GroupBox4, "PushDriverRemoveAll" );
    PushDriverRemoveAll->setGeometry( QRect( 230, 90, 50, 30 ) );

    PushDriverSelectAll = new QPushButton( GroupBox4, "PushDriverSelectAll" );
    PushDriverSelectAll->setGeometry( QRect( 230, 50, 50, 30 ) );

    ListAvailable = new QListBox( GroupBox4, "ListAvailable" );
    ListAvailable->setGeometry( QRect( 10, 40, 200, 180 ) );

    TextLabel1 = new QLabel( GroupBox4, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 20, 130, 16 ) );

    ListSelected = new QListBox( GroupBox4, "ListSelected" );
    ListSelected->setGeometry( QRect( 300, 40, 200, 180 ) );

    TextLabel2 = new QLabel( GroupBox4, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 300, 20, 140, 16 ) );
    languageChange();
    resize( QSize(793, 531).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KdeStart::~KdeStart()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KdeStart::languageChange()
{
    setCaption( tr2i18n( "Rars" ) );
    GroupBox3->setTitle( tr2i18n( "Last result" ) );
    ButtonGroupRace->setTitle( tr2i18n( "Starting Grid" ) );
    RadioQualif->setText( tr2i18n( "Qualification only" ) );
    RadioRaceOld->setText( tr2i18n( "Race with previous qualification" ) );
    RadioQualifRace->setText( tr2i18n( "Qualification and Race" ) );
    RadioRaceRandom->setText( tr2i18n( "Race with random starting grid" ) );
    GroupBox2->setTitle( tr2i18n( "Qualification" ) );
    LabelQualif->setText( tr2i18n( "Qualification laps" ) );
    LabelsNumberQualif->setText( tr2i18n( "Number of qualif. sessions" ) );
    PushStart->setText( tr2i18n( "Start Race" ) );
    PushExit->setText( tr2i18n( "Exit" ) );
    GroupBox1->setTitle( tr2i18n( "Race Parameters" ) );
    LabelTrack->setText( tr2i18n( "Track" ) );
    LabelLaps->setText( tr2i18n( "Laps" ) );
    LabelOpenGL->setText( tr2i18n( "OpenGL" ) );
    LabelMovie->setText( tr2i18n( "Movie" ) );
    LabelRandom->setText( tr2i18n( "Random seed" ) );
    TextLabel1_2_2->setText( tr2i18n( "Surface" ) );
    TextLabel1_2->setText( tr2i18n( "Follow car" ) );
    GroupBox4->setTitle( tr2i18n( "Drivers" ) );
    PushDriverRemoveOne->setText( tr2i18n( "<" ) );
    PushDriverSelectOne->setText( tr2i18n( ">" ) );
    PushDriverRemoveAll->setText( tr2i18n( "<<" ) );
    PushDriverSelectAll->setText( tr2i18n( ">>" ) );
    ListAvailable->clear();
    ListAvailable->insertItem( tr2i18n( "New Item" ) );
    TextLabel1->setText( tr2i18n( "Available drivers" ) );
    ListSelected->clear();
    ListSelected->insertItem( tr2i18n( "New Item" ) );
    TextLabel2->setText( tr2i18n( "Selected drivers" ) );
}

void KdeStart::exit()
{
    qWarning( "KdeStart::exit(): Not implemented yet" );
}

void KdeStart::slotStartRace()
{
    qWarning( "KdeStart::slotStartRace(): Not implemented yet" );
}

#include "kde_start.moc"
