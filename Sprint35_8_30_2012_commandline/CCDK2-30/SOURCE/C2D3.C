/* -2 -3 */
/*********************************** C2D3.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989-1994        Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <stdio.h>
#include <math.h>

#include <bbsdefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>

C2_CURVE c2d_arc_pt_tan_curve ( PT2 p0, PT2 tan0, C2_CURVE c1 )
{
    PT2 p1, p1p, vp0c, vp0p1, vp1c, vp1p0, tan1;
    REAL a0, a1, t0, t1, t, dt, tol, dist;
    C2_CURVE arc;
    PARM_S parm_s;
    PARM parm = &parm_s;
    INT i = 0;

    c2v_rotate_vec ( tan0, PI / 2.0, vp0c );

    t0 = C2_CURVE_T0(c1);
    t1 = C2_CURVE_T1(c1);

    t = t0 + ( t1 - t0 ) / 2.0;
    dt = ( t1 - t0 ) / 4.0;
    tol = 0.00001;
    PARM_T(parm) = t0;
    c2c_eval_pt ( c1, parm, p1p );
    dist = 1.0;

    while ( ( dist > tol )
        && ( i < 32 ) )
    {
        PARM_T(parm) = t;
        c2c_eval_pt_tan ( c1, parm, p1, tan1 );
        dist = c2v_dist ( p1, p1p );
        c2v_copy ( p1, p1p );
        c2v_rotate_vec ( tan1, PI / 2.0, vp1c );
        c2v_sub ( p0, p1, vp1p0 );
        c2v_negate ( vp1p0, vp0p1 );
        a0 = c2v_vecs_angle ( vp0c, vp0p1 );
        a1 = c2v_vecs_angle ( vp1p0, vp1c );

//        printf ( "a0: %lf a1: %lf\n", a0, a1 );

        if ( a0 > PI )
            a0 = a0 - 2.0 * PI;

        if ( a1 > PI )
            a1 = a1 - 2.0 * PI;

        if ( a1 > a0 )
        {
            t += dt;
        }
        else
        {
            t -= dt;
        }

        dt /= 2.0;
        i++;
    }

    arc = c2d_arc_2pts_tan1 ( p1, p0, tan0 );

    return ( arc );
}

