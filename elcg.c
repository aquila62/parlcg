/* elcg.c - random number generator Version 1.0.0                    */
/* Copyright (C) 2016 aquila62 at github.com                         */

/* This program is free software; you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License as    */
/* published by the Free Software Foundation; either version 2 of    */
/* the License, or (at your option) any later version.               */

/* This program is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of    */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/* GNU General Public License for more details.                      */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to:                        */

   /* Free Software Foundation, Inc.                                 */
   /* 59 Temple Place - Suite 330                                    */
   /* Boston, MA 02111-1307, USA.                                    */

/**********************************************************/
/* elcg: "extended drand48"                               */
/* elcg stands for extended linear congruential generator */
/* This subroutine produces a random unsigned             */
/* 48-bit integer using the drand48 algorithm plus a      */
/* Bays-Durham shuffle.  The previous two cycles are      */
/* xor'd together with the current cycle.                 */
/* The speed of elcg is faster than the glibc             */
/* subroutine drand48().                                  */
/**********************************************************/

#include "elcg.h"

unsigned int elcg(llfmt *ll)
   {
   unsigned long long *p;       /* state array pointer */
   unsigned long long tmp;      /* used for Bays-Durham shuffle */
   /**********************************************************/
   /* The offset into the state array is not related         */
   /* to the current state.                                  */
   /**********************************************************/
   /* calculate a 14-bit offset into the state array         */
   /* See the Bays-Durham shuffle below.                     */
   /* offset into state array from prev prev                 */
   ll->ofst  = (ll->pprev >> 34);    /* upper 10 bits */         
   ll->pprev = ll->prev;   /* prev prev <== prev             */
   ll->prev  = ll->x;      /* prev <== current               */
   /* calculate new current state                            */
   /* using the drand48 algorithm                            */
   /* The  macro is in elcg.h                                */
   LCG;

   /********************************************************/
   /* Bays-Durham shuffle of state array                   */
   /* 16384! = 61937 digits                                */
   /* The period length of the state array is theoretical  */
   /* and cannot be proven with empirical testing.         */
   /********************************************************/
   /* point to a state array element                       */
   p     = (unsigned long long *) ll->state + ll->ofst;
   /* swap the current output with the member of the state array */
   tmp   = *p;
   *p    = ll->x;
   ll->x = tmp;
   /*********************************************************/
   /* XOR the two previous state with the current state     */
   /*********************************************************/
   ll->out = (unsigned int) ((ll->x ^ ll->prev ^ ll->pprev) >> 16);
   return(ll->out);
   } /* elcg subroutine */
