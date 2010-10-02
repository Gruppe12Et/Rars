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
        int gerade;
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
        m_sBitmapName2D = "car_pink_pink";
        // Für alle Gruppen gleich
        m_sModel3D = "futura";
   }

    int right_direc(double left,double right)
    {
        double trackwidth=left+right;

        printf("links : %f        rechts : %f", left,right);


        if(left<0){return 1;}
        else if (right<0){return -1;}


        if (left>55) {return -1;}
        else if (left<45) return 1;



    }



    con_vec drive(situation& s)
    {
        // hier wird die Logik des Robots implementiert
        con_vec result = CON_VEC_EMPTY;
        double width=s.to_lft+s.to_rgt;
        double last;

        if(s.to_lft<0)
        {
            result.alpha=-0.05;
            result.vc=90;
            return result;
        }
        else if (s.to_rgt<0)
        {
            result.alpha=0.05;
            result.vc=90;
            return result;
        }



        if (s.cur_rad<-0.02)
        {
            if (s.cur_len<0.1)
            {
                result.alpha=0;
                result.vc=100;
                return result;
            }


            result.alpha=(s.to_lft-10)/100-(s.vn/s.v+0.93);
           // result.alpha=s.cur_rad/3600*s.cur_len*s.cur_len*s.cur_len*s.cur_len*s.cur_len*s.cur_len;
             printf("s.cur_rad : %f          s.cur_len : %f\n\r ####################### %f ########################## \n\r", s.cur_rad,s.cur_len,result.alpha);
            result.vc=100;
            return result;

        }
        else if (s.cur_rad>0.02)
        {
            if (s.cur_len<0.05)
            {
                result.alpha=0;
                result.vc=100;
                return result;
            }


result.alpha=(s.to_lft-10)/100-(s.vn/s.v+0.93);
            //result.alpha=s.cur_rad/3600*s.cur_len*s.cur_len*s.cur_len*s.cur_len*s.cur_len*s.cur_len;
             printf("s.cur_rad : %f          s.cur_len : %f\n\r ####################### %f ########################## \n\r", s.cur_rad,s.cur_len,result.alpha);
            result.vc=100;
            return result;
        }


//if (last!=NULL)
//{
//
//
//        if (last-s.to_lft<0)
//        {
//            result.alpha=(last-s.to_lft)*-0.0000001;
//            result.vc=100;
//            return result;
//        }
//        else if (last-s.to_lft>0)
//        {
//            result.alpha=(last-s.to_lft)*0.0000001;
//            result.vc=100;
//            return result;
//
//        }
//
//}
//
//        last=s.to_lft;
//
//        if (right_direc(s.to_lft,s.to_rgt)==-1)
//        {
//            result.alpha=(width/2-s.to_lft)*0.0001;
////            result.alpha=(s.to_lft-((s.to_rgt+s.to_lft)/2))/1650;
//            result.vc=100;
//            printf("zu weit rechts\n\r");
//            return result;
//        }
//        else
//        {
//               result.alpha=(width/2-s.to_lft)*-0.0001;
////            result.alpha=(s.to_lft-((s.to_rgt+s.to_lft)/2)*-1)/1650;
//            result.vc=100;
//            printf("zu weit links %f\n\r",result.alpha);
//            return result;
//
//        }
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
