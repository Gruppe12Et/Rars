/****************************************************************************
** KdeStartImpl meta object code from reading C++ file 'kde_start_impl.h'
**
** Created: Mon Sep 13 15:02:19 2010
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kde_start_impl.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *KdeStartImpl::className() const
{
    return "KdeStartImpl";
}

QMetaObject *KdeStartImpl::metaObj = 0;
static QMetaObjectCleanUp cleanUp_KdeStartImpl( "KdeStartImpl", &KdeStartImpl::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString KdeStartImpl::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KdeStartImpl", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString KdeStartImpl::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KdeStartImpl", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* KdeStartImpl::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KdeStart::staticMetaObject();
    static const QUMethod slot_0 = {"slotStartRace", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ "on", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"slotRadioToggled", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "index", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"slotChangeTrack", 1, param_slot_2 };
    static const QUMethod slot_3 = {"slotDriverSelectOne", 0, 0 };
    static const QUMethod slot_4 = {"slotDriverSelectAll", 0, 0 };
    static const QUMethod slot_5 = {"slotDriverRemoveOne", 0, 0 };
    static const QUMethod slot_6 = {"slotDriverRemoveAll", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "slotStartRace()", &slot_0, QMetaData::Public },
	{ "slotRadioToggled(bool)", &slot_1, QMetaData::Public },
	{ "slotChangeTrack(int)", &slot_2, QMetaData::Public },
	{ "slotDriverSelectOne()", &slot_3, QMetaData::Public },
	{ "slotDriverSelectAll()", &slot_4, QMetaData::Public },
	{ "slotDriverRemoveOne()", &slot_5, QMetaData::Public },
	{ "slotDriverRemoveAll()", &slot_6, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"KdeStartImpl", parentObject,
	slot_tbl, 7,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_KdeStartImpl.setMetaObject( metaObj );
    return metaObj;
}

void* KdeStartImpl::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "KdeStartImpl" ) )
	return this;
    return KdeStart::qt_cast( clname );
}

bool KdeStartImpl::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotStartRace(); break;
    case 1: slotRadioToggled((bool)static_QUType_bool.get(_o+1)); break;
    case 2: slotChangeTrack((int)static_QUType_int.get(_o+1)); break;
    case 3: slotDriverSelectOne(); break;
    case 4: slotDriverSelectAll(); break;
    case 5: slotDriverRemoveOne(); break;
    case 6: slotDriverRemoveAll(); break;
    default:
	return KdeStart::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool KdeStartImpl::qt_emit( int _id, QUObject* _o )
{
    return KdeStart::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool KdeStartImpl::qt_property( int id, int f, QVariant* v)
{
    return KdeStart::qt_property( id, f, v);
}

bool KdeStartImpl::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
