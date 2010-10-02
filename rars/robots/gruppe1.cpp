/**
 * Vorlage für den Robot im Software-Projekt
 *
 * Der Klassenname ergibt sich aus der Gruppennummer (z.B. Gruppe12)
 *
  *
 * @author    Ingo Haschler <ih@ingohaschler.de>
 * @version   0.10
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "car.h"

//--------------------------------------------------------------------------
//                           Class Gruppe1
//--------------------------------------------------------------------------

class Gruppe1 : public Driver
{
public:
    // Konstruktor
    Gruppe1()
    {
        // Der Name des Robots
        m_sName = "Gruppe1";
        // Namen der Autoren
        m_sAuthor = "";
        // Hier die vorgegebenen Farben eintragen
        m_iNoseColor = oBLUE;
        m_iTailColor = oBLUE;
        m_sBitmapName2D = "car_blue_blue";
        // Für alle Gruppen gleich
        m_sModel3D = "futura";
    }

    con_vec drive(situation& s)
    {
        // hier wird die Logik des Robots implementiert
        con_vec result = CON_VEC_EMPTY;
        return result;
    }
};

/**
 * Diese Methode darf nicht verändert werden.
 * Sie wird vom Framework aufgerufen, um den Robot zu erzeugen.
 * Der Name leitet sich (wie der Klassenname) von der Gruppenbezeichnung ab.
 */
Driver * getGruppe1Instance()
{
    return new Gruppe1();
}
