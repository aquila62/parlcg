/* elcg.h - elcg random number generator header file Version 1.0.0 */
/* Copyright (C) 2016 aquila62 at github.com */

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

/* The subroutines below are called billions of times     */
/* in a single run.                                       */
/* Parameters to the subroutines must be validated by     */
/* the calling program, to maximize efficiency            */
/* and minimize redundant editing.                        */

/* elcg stands for extended linear congruential generator */
/* This generator emulates drand48 with a Bays-Durham     */
/* shuffle of 1024 states. */

typedef struct elcgstruct {
   int states;                     /* number of states = 1024 */
   int ofst;                       /* offset into state array */
   /* the following constants are used in the drand48 formula */
   /* x = (a * x) + c mod(m) */
   unsigned long long x;           /* elcg state or seed */
   unsigned long long a;           /* elcg constant a */
   unsigned long long c;           /* elcg constant c */
   unsigned long long m;           /* elcg constant m */
   /* output is the upper 32 bits of x */
   unsigned int out;               /* elcg 32 bit output */
   unsigned long long prev;        /* prev output state */
   unsigned long long pprev;       /* prev prev output state */
   unsigned long long *state;      /* state array of 1024 members */
   } llfmt;

#define LCGMASK (0xffffffffffffULL)

#define LCG (ll->x = (((ll->x * ll->a) + ll->c) & LCGMASK))

llfmt *elcginit(void);                 /* initialization routine */
unsigned int elcg(llfmt *ll);          /* random lluint generator */
int elcgbit(llfmt *ll);                /* random bit generator */
double elcgunif(llfmt *ll);            /* random uniform number 0-1 */
int elcgint(llfmt *ll, int limit);     /* random integer 0 up to limit */
unsigned int elcgpwr(llfmt *ll, int bits); /* random # 1-32 bits */
