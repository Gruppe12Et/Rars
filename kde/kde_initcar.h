/****************************************************************************
** Form interface generated from reading ui file './kde_initcar.ui'
**
** Created: Mo Sep 13 15:01:09 2010
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KDEINITCAR_H
#define KDEINITCAR_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QGroupBox;
class QListBox;
class QListBoxItem;

class KdeInitCar : public QDialog
{
    Q_OBJECT

public:
    KdeInitCar( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KdeInitCar();

    QLabel* TextLabel1;
    QGroupBox* GroupBox1;
    QListBox* ListMessage;

protected:

protected slots:
    virtual void languageChange();

};

#endif // KDEINITCAR_H
