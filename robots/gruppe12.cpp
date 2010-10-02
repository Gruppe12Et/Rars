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

const double tempmulti = 30;

//--------------------------------------------------------------------------
//                           Class Gruppe12
//--------------------------------------------------------------------------

class Gruppe12 : public Driver
{
public:
    // Konstruktor
    Gruppe12()
    {
        // Der Name des Robots
        m_sName = "Gruppe12";
        // Namen der Autoren
        m_sAuthor = "";
        // Hier die vorgegebenen Farben eintragen
        m_iNoseColor = oBLUE;
        m_iTailColor = oBLUE;
        m_sBitmapName2D = "car_blue_blue";
        // Für alle Gruppen gleich
        m_sModel3D = "futura";
    }

    double checktemp(situation& s)
    {
        double slow=0.0;
        int i=0;



        if(s.cur_rad==0)
        {

            if((s.to_end<s.cur_len*0.30))//&&(s.nex_rad!=0))
            {
                //return 50;
                if (s.nex_rad>0)
                return sqrt(s.nex_rad*tempmulti);
                else if (s.nex_rad<0)
                return sqrt(-s.nex_rad*tempmulti);
            }
            else
            {
                return 1000;
            }



        }
        else
        {
            if((s.to_end<s.cur_len*0.30)&&s.nex_rad==0)
            return 1000;
            else
            {
                if (s.cur_rad>0)
                if (s.to_end>s.cur_len*0.30)
                return sqrt(s.cur_rad*tempmulti);
                else
                {
                    return sqrt(s.nex_rad*tempmulti);
                }
                else if (s.cur_rad<0)
                if (s.to_end>s.cur_len*0.3)
                return sqrt(-s.cur_rad*tempmulti);
                else
                {
                    return sqrt(-s.nex_rad*tempmulti);
                }
            }
        }




//        if ((s.to_end>s.cur_len*0.15)&&(s.cur_rad==0))
//        {
//
//
//
//            if (s.cur_rad==0)
//            {
////                printf("TOPSPEED\n\r");
//                return 10000;
//            }
//            else
//            return 100;
//        }
//        else
//        if (s.cur_rad==0)
//        {
//
//
//        if(s.nex_rad<0){
//        slow=sqrt(-s.nex_rad*30);}
//        else if (s.nex_rad>0)
//        {
//            slow=sqrt(s.nex_rad*30);
//        }
//        printf("Langsam tun !!! : %f\n\r",slow);
//        return slow;
//        }
   }



    con_vec drive(situation& s)
    {
        // hier wird die Logik des Robots implementiert
        con_vec result = CON_VEC_EMPTY;
        double lane = 0.0;
        if (s.starting)
        {
            lane=s.to_lft;
        }
//        printf("%f\n\r",lane);
        result.alpha=1 * (s.to_lft - 50)/(s.to_lft+s.to_rgt) - 1.2* s.vn/s.v ;
        result.vc=checktemp(s);
        printf("Speed: %f  Einschlag: %f Radius aktuelle : %f radius next : %f berechneter speed : %f\n\r",result.vc,result.alpha,s.cur_rad,s.nex_rad, checktemp(s));
        return result;
    }
};

/**
 * Diese Methode darf nicht verändert werden.
 * Sie wird vom Framework aufgerufen, um den Robot zu erzeugen.
 * Der Name leitet sich (wie der Klassenname) von der Gruppenbezeichnung ab.
 */
Driver * getGruppe12Instance()
{
    return new Gruppe12();
}
