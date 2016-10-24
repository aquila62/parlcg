/* elcgunif.c - elcg uniform random number 0-1 Version 1.0.0       */
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

/* this subroutine produces a uniform random number       */
/* from zero to one                                       */
/* For a slower method with higher precision, use         */
/* elcgbit() instead.                                    */

/* maximum integer is 2^32 */
#define MAXINT 4294967296.0

/* the modulo for ll->x is 2^48 */
#define MODULO 281474976710656.0

#include "elcg.h"

double elcgunif(llfmt *ll)
   {
   double num;             /* uniform random number from 0-1  */
   elcg(ll);
   num = (double) ll->out / MAXINT;
   return(num);            /* return uniform random number    */
   } /* elcgunif */
