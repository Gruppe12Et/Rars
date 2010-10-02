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
//                           Class Gruppe11
//--------------------------------------------------------------------------

class Gruppe11 : public Driver
{
public:
    // Konstruktor
    Gruppe11()
    {
        // Der Name des Robots
        m_sName = "Gruppe11";
        // Namen der Autoren
        m_sAuthor = "";
        // Hier die vorgegebenen Farben eintragen
        m_iNoseColor = oMAGENTA;
        m_iTailColor = oMAGENTA;
        m_sBitmapName2D = "car_blue_blue";
        // Für alle Gruppen gleich
        m_sModel3D = "futura";
    }

    con_vec drive(situation& s)
    {
        // hier wird die Logik des Robots implementiert
        con_vec result = CON_VEC_EMPTY;




        if (s.cur_rad<-0.001)
        {
            if (s.to_end<0.05)
            {
                result.alpha=0;
                result.vc=500000;
                return result;
            }

            result.alpha=s.cur_rad/(2000);
            printf("%f %f %f\n\r",s.cur_rad, s.nex_rad, result.alpha);

        }

        else if (s.cur_rad==0)
        {
                 if(s.to_lft<s.to_rgt)
        {
            printf("korrektur\n\r");
            result.alpha=-0.001;
            result.vc=100;
            return result;
        }
        else{
          printf("korrektur %f\n\r", s.to_lft);
            result.alpha=0.001;
            result.vc=100;
            return result;


        }
            result.alpha=0;
        }
        result.vc=100;

        return result;
    }
};

/**
 * Diese Methode darf nicht verändert werden.
 * Sie wird vom Framework aufgerufen, um den Robot zu erzeugen.
 * Der Name leitet sich (wie der Klassenname) von der Gruppenbezeichnung ab.
 */
Driver * getGruppe11Instance()
{
    return new Gruppe11();
}
