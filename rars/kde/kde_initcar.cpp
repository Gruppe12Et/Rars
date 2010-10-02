#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kde_initcar.ui'
**
** Created: Mo Sep 13 15:01:09 2010
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kde_initcar.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KdeInitCar as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KdeInitCar::KdeInitCar( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KdeInitCar" );
    setEnabled( TRUE );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 30, 240, 20 ) );
    TextLabel1->setAlignment( int( QLabel::AlignCenter ) );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 10, 61, 241, 270 ) );

    ListMessage = new QListBox( GroupBox1, "ListMessage" );
    ListMessage->setGeometry( QRect( 10, 20, 220, 240 ) );
    languageChange();
    resize( QSize(256, 352).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KdeInitCar::~KdeInitCar()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KdeInitCar::languageChange()
{
    setCaption( tr2i18n( "Initialisation" ) );
    TextLabel1->setText( tr2i18n( "Please Wait" ) );
    GroupBox1->setTitle( tr2i18n( "Messages" ) );
    ListMessage->clear();
    ListMessage->insertItem( tr2i18n( "New Item" ) );
}

#include "kde_initcar.moc"
