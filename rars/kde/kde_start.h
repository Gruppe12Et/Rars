/****************************************************************************
** Form interface generated from reading ui file './kde_start.ui'
**
** Created: Mo Sep 13 15:02:12 2010
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KDESTART_H
#define KDESTART_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QMultiLineEdit;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QSpinBox;
class QPushButton;
class QComboBox;
class QListBox;
class QListBoxItem;

class KdeStart : public QDialog
{
    Q_OBJECT

public:
    KdeStart( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KdeStart();

    QGroupBox* GroupBox3;
    QMultiLineEdit* LineLastResult;
    QButtonGroup* ButtonGroupRace;
    QRadioButton* RadioQualif;
    QRadioButton* RadioRaceOld;
    QRadioButton* RadioQualifRace;
    QRadioButton* RadioRaceRandom;
    QGroupBox* GroupBox2;
    QLabel* LabelQualif;
    QLabel* LabelsNumberQualif;
    QSpinBox* SpinQualifLaps;
    QSpinBox* SpinQualifNumber;
    QPushButton* PushStart;
    QPushButton* PushExit;
    QGroupBox* GroupBox1;
    QComboBox* ComboTrack;
    QLabel* LabelTrack;
    QLabel* LabelLaps;
    QSpinBox* SpinLaps;
    QLabel* LabelOpenGL;
    QComboBox* ComboOpenGL;
    QLabel* LabelMovie;
    QComboBox* ComboMovie;
    QLabel* LabelRandom;
    QSpinBox* SpinRandom;
    QLabel* TextLabel1_2_2;
    QLabel* TextLabel1_2;
    QComboBox* ComboFollowCar;
    QComboBox* ComboSurface;
    QGroupBox* GroupBox4;
    QPushButton* PushDriverRemoveOne;
    QPushButton* PushDriverSelectOne;
    QPushButton* PushDriverRemoveAll;
    QPushButton* PushDriverSelectAll;
    QListBox* ListAvailable;
    QLabel* TextLabel1;
    QListBox* ListSelected;
    QLabel* TextLabel2;

public slots:
    virtual void exit();
    virtual void slotStartRace();

protected:

protected slots:
    virtual void languageChange();

};

#endif // KDESTART_H
