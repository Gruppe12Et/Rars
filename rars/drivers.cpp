/**
 * DRIVERS.CPP - list of the drivers and their characteristics
 *
 * If you are changing number of drivers here, change also MAXCARS in car.h !!!
 * These are the control or "driver" programs which compete in the race:
 * See drivers[] below, and CAR.H
 *
 * @author    Mitchell E. Timin, State College, PA
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 *
 * Geändert von Ingo Haschler für SE-Projekt ET09
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "car.h"

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------
robot2 getGruppe1Instance;
robot2 getGruppe2Instance;
robot2 getGruppe3Instance;
robot2 getGruppe4Instance;
robot2 getGruppe5Instance;
robot2 getGruppe6Instance;
robot2 getGruppe7Instance;
robot2 getGruppe8Instance;
robot2 getGruppe9Instance;
robot2 getGruppe10Instance;
robot2 getGruppe11Instance;
robot2 getGruppe12Instance;
robot2 getGruppe13Instance;
robot2 getGruppe14Instance;
robot2 getGruppe15Instance;
robot2 getGruppe16Instance;
robot2 getGruppe17Instance;
robot2 getGruppe18Instance;
robot2 getGruppe19Instance;


/**
 * This is the permanent array of available drivers.
 *
 * Each bitmap name is associated with 2 colors.
 */
Driver * drivers[] =
{
    ////////////////////////////////////////////////////////////////////////////
    //  ET09 stage 1 cars
    ////////////////////////////////////////////////////////////////////////////

    getGruppe1Instance(),
    getGruppe2Instance(),
    getGruppe3Instance(),
    getGruppe4Instance(),
    getGruppe5Instance(),
    getGruppe6Instance(),
    getGruppe7Instance(),
    getGruppe8Instance(),
    getGruppe9Instance(),
    getGruppe10Instance(),
    getGruppe11Instance(),
    getGruppe12Instance(),
    getGruppe13Instance(),
    getGruppe14Instance(),
    getGruppe15Instance(),
    getGruppe16Instance(),
    getGruppe17Instance(),
    getGruppe18Instance(),
    getGruppe19Instance(),
};
