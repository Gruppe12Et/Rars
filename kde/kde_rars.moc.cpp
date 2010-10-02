/****************************************************************************
** MyImage meta object code from reading C++ file 'kde_rars.h'
**
** Created: Mon Sep 13 15:02:18 2010
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kde_rars.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MyImage::className() const
{
    return "MyImage";
}

QMetaObject *MyImage::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MyImage( "MyImage", &MyImage::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MyImage::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyImage", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MyImage::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyImage", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MyImage::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"MyImage", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MyImage.setMetaObject( metaObj );
    return metaObj;
}

void* MyImage::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MyImage" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool MyImage::qt_invoke( int _id, QUObject* _o )
{
    return QWidget::qt_invoke(_id,_o);
}

bool MyImage::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MyImage::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool MyImage::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *KdeRars::className() const
{
    return "KdeRars";
}

QMetaObject *KdeRars::metaObj = 0;
static QMetaObjectCleanUp cleanUp_KdeRars( "KdeRars", &KdeRars::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString KdeRars::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KdeRars", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString KdeRars::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KdeRars", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* KdeRars::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"slotTimeOut", 0, 0 };
    static const QUMethod slot_1 = {"slotCarNext", 0, 0 };
    static const QUMethod slot_2 = {"slotCarPrevious", 0, 0 };
    static const QUMethod slot_3 = {"slotSpeedRewind", 0, 0 };
    static const QUMethod slot_4 = {"slotSpeedStop", 0, 0 };
    static const QUMethod slot_5 = {"slotSpeedSlow", 0, 0 };
    static const QUMethod slot_6 = {"slotSpeedMedium", 0, 0 };
    static const QUMethod slot_7 = {"slotSpeedFast", 0, 0 };
    static const QUMethod slot_8 = {"slotChangeView", 0, 0 };
    static const QUMethod slot_9 = {"slotViewClassic", 0, 0 };
    static const QUMethod slot_10 = {"slotViewFullScreen", 0, 0 };
    static const QUMethod slot_11 = {"slotViewTelemetry", 0, 0 };
    static const QUMethod slot_12 = {"slotViewWindow", 0, 0 };
    static const QUMethod slot_13 = {"slotViewZoom", 0, 0 };
    static const QUMethod slot_14 = {"slotViewTrajectory", 0, 0 };
    static const QUMethod slot_15 = {"slotViewDriverNames", 0, 0 };
    static const QUMethod slot_16 = {"slotFollowCarManual", 0, 0 };
    static const QUMethod slot_17 = {"slotFollowCarNobody", 0, 0 };
    static const QUMethod slot_18 = {"slotFollowCarPosition", 0, 0 };
    static const QUMethod slot_19 = {"slotFollowCarOvertaking", 0, 0 };
    static const QUMethod slot_20 = {"slotZoomIn", 0, 0 };
    static const QUMethod slot_21 = {"slotZoomOut", 0, 0 };
    static const QUMethod slot_22 = {"slotSettingsSave", 0, 0 };
    static const QUMethod slot_23 = {"slotMoveUp", 0, 0 };
    static const QUMethod slot_24 = {"slotMoveDown", 0, 0 };
    static const QUMethod slot_25 = {"slotMoveLeft", 0, 0 };
    static const QUMethod slot_26 = {"slotMoveRight", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "slotTimeOut()", &slot_0, QMetaData::Public },
	{ "slotCarNext()", &slot_1, QMetaData::Public },
	{ "slotCarPrevious()", &slot_2, QMetaData::Public },
	{ "slotSpeedRewind()", &slot_3, QMetaData::Public },
	{ "slotSpeedStop()", &slot_4, QMetaData::Public },
	{ "slotSpeedSlow()", &slot_5, QMetaData::Public },
	{ "slotSpeedMedium()", &slot_6, QMetaData::Public },
	{ "slotSpeedFast()", &slot_7, QMetaData::Public },
	{ "slotChangeView()", &slot_8, QMetaData::Public },
	{ "slotViewClassic()", &slot_9, QMetaData::Public },
	{ "slotViewFullScreen()", &slot_10, QMetaData::Public },
	{ "slotViewTelemetry()", &slot_11, QMetaData::Public },
	{ "slotViewWindow()", &slot_12, QMetaData::Public },
	{ "slotViewZoom()", &slot_13, QMetaData::Public },
	{ "slotViewTrajectory()", &slot_14, QMetaData::Public },
	{ "slotViewDriverNames()", &slot_15, QMetaData::Public },
	{ "slotFollowCarManual()", &slot_16, QMetaData::Public },
	{ "slotFollowCarNobody()", &slot_17, QMetaData::Public },
	{ "slotFollowCarPosition()", &slot_18, QMetaData::Public },
	{ "slotFollowCarOvertaking()", &slot_19, QMetaData::Public },
	{ "slotZoomIn()", &slot_20, QMetaData::Public },
	{ "slotZoomOut()", &slot_21, QMetaData::Public },
	{ "slotSettingsSave()", &slot_22, QMetaData::Public },
	{ "slotMoveUp()", &slot_23, QMetaData::Public },
	{ "slotMoveDown()", &slot_24, QMetaData::Public },
	{ "slotMoveLeft()", &slot_25, QMetaData::Public },
	{ "slotMoveRight()", &slot_26, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"KdeRars", parentObject,
	slot_tbl, 27,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_KdeRars.setMetaObject( metaObj );
    return metaObj;
}

void* KdeRars::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "KdeRars" ) )
	return this;
    return KMainWindow::qt_cast( clname );
}

bool KdeRars::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotTimeOut(); break;
    case 1: slotCarNext(); break;
    case 2: slotCarPrevious(); break;
    case 3: slotSpeedRewind(); break;
    case 4: slotSpeedStop(); break;
    case 5: slotSpeedSlow(); break;
    case 6: slotSpeedMedium(); break;
    case 7: slotSpeedFast(); break;
    case 8: slotChangeView(); break;
    case 9: slotViewClassic(); break;
    case 10: slotViewFullScreen(); break;
    case 11: slotViewTelemetry(); break;
    case 12: slotViewWindow(); break;
    case 13: slotViewZoom(); break;
    case 14: slotViewTrajectory(); break;
    case 15: slotViewDriverNames(); break;
    case 16: slotFollowCarManual(); break;
    case 17: slotFollowCarNobody(); break;
    case 18: slotFollowCarPosition(); break;
    case 19: slotFollowCarOvertaking(); break;
    case 20: slotZoomIn(); break;
    case 21: slotZoomOut(); break;
    case 22: slotSettingsSave(); break;
    case 23: slotMoveUp(); break;
    case 24: slotMoveDown(); break;
    case 25: slotMoveLeft(); break;
    case 26: slotMoveRight(); break;
    default:
	return KMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool KdeRars::qt_emit( int _id, QUObject* _o )
{
    return KMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool KdeRars::qt_property( int id, int f, QVariant* v)
{
    return KMainWindow::qt_property( id, f, v);
}

bool KdeRars::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
