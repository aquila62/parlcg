/* elcginit.c - elcg initialization Version 1.0.0                    */
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

/* This is the initialization routine for elcg               */
/* This initialization routine is based on date/time/ticks   */
/* The caller is welcome to override the following:          */
/* the state array                                           */
/* the two previous outputs                                  */
/* the state (x) for the drand48 algorithm                   */
/*                                                           */
/* The period length of elcg is estimated to be              */
/* 5.41e+2639.                                               */
/* The period length of rand48 is no longer than 2^48.       */
/* The speed of elcg is 1/3 faster than the glibc version    */
/* of drand48.                                               */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include "elcg.h"

/* size of the state array is 2^14 */
#define STATES 16384

/* the ttstruct structure is used for calculating a crc */
typedef struct ttstruct {
   unsigned int clk;           /* #ticks since boot */
   unsigned int now;           /* #seconds since epoch */
   unsigned int byte;          /* current byte in crc stream */
   unsigned int mask;          /* crc mask */
   unsigned int crc;           /* crc */
   unsigned int table[256];    /* crc table */
   } ttfmt;

llfmt *elcginit(void)
   {
   int i;                      /* loop counter */
   int j;                      /* loop counter for crc */
   unsigned int dtcrc;         /* crc of date  */
   unsigned int tkcrc;         /* crc of ticks */
   unsigned long long *stp,*stq;   /* pointer into state array */
   unsigned char *p,*q;        /* pointer for crc stream */
   time_t now;                 /* current date and time */
   clock_t clk;                /* current number of ticks */
   struct tms t;               /* structure used by times() */
   llfmt *ll;                  /* elcg structure */
   ttfmt *tt;                  /* crc structure */

   /***************************************************/
   /* allocate memory for crc structure               */
   /***************************************************/
   tt = (ttfmt *) malloc(sizeof(ttfmt));
   if (tt == NULL)
      {
      fprintf(stderr,"elcginit: out of memory "
      "allocating elcg structure tt\n");
      exit(1);
      } /* out of memory */

   /***************************************************/
   /* build the crc table                             */
   /***************************************************/
   for (tt->byte = 0; tt->byte <= 255; tt->byte++)
      {
      tt->crc = tt->byte;
      for (j = 7; j >= 0; j--)
         {    // Do eight times.
         tt->mask = -(tt->crc & 1);
         tt->crc = (tt->crc >> 1) ^ (0xEDB88320 & tt->mask);
         } /* for each bit in byte */
      tt->table[tt->byte] = tt->crc;
      } /* for each of 256 bytes in table */

   /***************************************************/
   /* allocate memory for elcg structure              */
   /***************************************************/
   ll = (llfmt *) malloc(sizeof(llfmt));
   if (ll == NULL)
      {
      fprintf(stderr,"elcginit: out of memory "
      "allocating elcg structure ll\n");
      exit(1);
      } /* out of memory */
   ll->states = STATES; /* save the number of elcg states */

   /***************************************************/
   /* allocate memory for elcg state array            */
   /***************************************************/
   ll->state = (unsigned long long *)
      malloc(sizeof(unsigned long long) * STATES);
   if (ll->state == NULL)
      {
      fprintf(stderr,"elcginit: out of memory "
      "allocating ll->state state array\n");
      exit(1);
      } /* out of memory */

   /***************************************************/
   /* Randomize the seeds and states                  */
   /***************************************************/
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   tt->clk = clk;
   tt->now = now;
   /********************************************************/
   /* calculate crc of ticks                               */
   /********************************************************/
   p = (unsigned char *) &clk;
   q = (unsigned char *) &clk + 4;
   tt->crc = 0xFFFFFFFF;     /* initialize crc */
   while (p < q)
      {
      int indx;
      tt->byte = *p++;
      indx = (tt->crc ^ tt->byte) & 0xff;
      tt->crc = (tt->crc >> 8) ^ tt->table[indx];
      } /* for each byte in date/time/ticks */
   tt->crc = ~tt->crc;
   tkcrc = tt->crc;
   /********************************************************/
   /* calculate crc of date/time                           */
   /********************************************************/
   p = (unsigned char *) &now;
   q = (unsigned char *) &now + 4;
   tt->crc = 0xFFFFFFFF;     /* initialize crc */
   while (p < q)
      {
      int indx;
      tt->byte = *p++;
      indx = (tt->crc ^ tt->byte) & 0xff;
      tt->crc = (tt->crc >> 8) ^ tt->table[indx];
      } /* for each byte in date/time/ticks */
   tt->crc = ~tt->crc;
   dtcrc = tt->crc;
   /********************************************************/
   /* Initialize the elcg state to crc of date,time,ticks  */
   /* The srand48 algorithm dictates that 0x330e be the    */
   /* lower 16 bits of the initial state                   */
   /********************************************************/
   ll->x = (tkcrc ^ dtcrc);
   ll->x = (ll->x << 16) | 0x330e;
   ll->a = 0x5deece66d;
   ll->c = 11;

   /***************************************************/
   /* Warm up the elcg state.                         */
   /***************************************************/
   i = 128;
   while (i--)
      {
      LCG;
      } /* for each warm up iteration */

   /***************************************************/
   /* initialize out, prev, and prev prev             */
   /* to random values                                */
   /***************************************************/
   LCG;
   ll->pprev = ll->x;
   LCG;
   ll->prev  = ll->x;

   /***************************************************/
   /* initialize the state array to random values     */
   /***************************************************/
   stp = (unsigned long long *) ll->state;
   stq = (unsigned long long *) ll->state + STATES;
   while (stp < stq)
      {
      LCG;
      *stp++ = ll->x;
      } /* for each member in ll->state array */

   /***************************************************/
   /* Warm up the elcg state table.                   */
   /***************************************************/
   i = 128;
   while (i--) elcg(ll);

   free(tt);      /* free the crc structure */

   /***************************************************/
   /* after this subroutine you may initialize the    */
   /* state array to your own values, if you wish     */
   /* to do regression testing.                       */
   /* Use the above 9 instructions as an example of   */
   /* how to initialize the generator                 */
   /***************************************************/
   /* return the elcg structure                       */
   /***************************************************/
   return(ll);
   } /* elcginit subroutine */
