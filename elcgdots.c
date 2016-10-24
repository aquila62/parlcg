/*
elcgdots.c Version 0.1.0. Flash random dots on an X Windows screen
Copyright (C) 2016   aquila62 at github.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to:

	Free Software Foundation, Inc.
	59 Temple Place - Suite 330
	Boston, MA  02111-1307, USA.
*/

/**********************************************************/
/* This program initializes the elcg random number        */
/* generator state by overriding its state with RANDU.    */
/* RANDU is initialized by the CRC of the input password  */
/* parameter.                                             */
/* RANDU is a weak random number generator, but it is     */
/* satisfactory enough for the encryption program to      */
/* pass the dieharder random number test suite.           */
/* Once initialized with RANDU, elcg is warmed up         */
/* to minimize the weakness behind its initialization     */
/* routine.                                               */
/**********************************************************/

/* to define the escape key */
#define XK_MISCELLANY 1
#define XK_LATIN1 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <assert.h>
#include "elcg.h"

typedef struct xxstruct {
   int whiteColor,blackColor;
   int rootwh;      /* root window height */
   int rootww;      /* root window width  */
   int dpyhght;     /* display window height */
   int dpywdth;     /* display window width  */
   int repeat;
   int runflg;
   unsigned long red;
   Display *dpy;
   Window w;
   GC gc;
   Font fontid;
   Screen *dfltscr;
   Colormap cmap;
   XColor scrdef,exctdef;
   } xxfmt;

#define STATES (16384)

/* RANDU algorithm */
#define RANDU (seed = seed * 65539)

/* print the command syntax */
void putstx(char *pgm)
   {
   fprintf(stderr,"Usage: %s password\n", pgm);
   fprintf(stderr,"Where password is an ASCII string\n");
   fprintf(stderr,"Example: %s abcd\n", pgm);
   fprintf(stderr,"Example: %s \"abcd efgh\"\n", pgm);
   fprintf(stderr,"Example: %s 'abcd efgh'\n", pgm);
   exit(1);
   } /* putstx */

/* X Windows code is based on: */
/* http://tronche.com/gui/x/xlib-tutorial/2nd-program-anatomy.html */
/* or */
/* http://tronche.com/gui/x  */

void getkey(xxfmt *xx)
   {
   /* after each pause wait for command or exposure */

   XSelectInput(xx->dpy, xx->w,
      KeyPressMask|ExposureMask);

   while(1)
      {
      int symbol;
      XEvent e;
      XKeyEvent *k;
      XNextEvent(xx->dpy, &e);
      if (e.type == KeyPress)
         {
         k = (XKeyEvent *) &e;
         symbol = XLookupKeysym(k,0);
         xx->repeat = 0;
         if (symbol == XK_Escape || symbol == 'q')
            {
            xx->repeat = 0;
	    xx->runflg = 0;
            break;
            } /* if quit */
         else if (symbol == 'r' || symbol == 'n')
            {
            xx->repeat = 1;
            break;
            } /* if next */
         } /* if keypress event */
      else if (e.type == Expose)
         {
	 XClearWindow(xx->dpy,xx->w);
         continue;
         } /* if expose event */
      } /* wait for window shutdown */
   } /* getkey */

void ifkey(xxfmt *xx)
   {
   int msk;
   int symbol;
   int XCheckMaskEvent();
   XEvent e;
   XKeyEvent *k;

   msk = KeyPressMask|ExposureMask;

   XSelectInput(xx->dpy, xx->w, msk);

   while (XCheckMaskEvent(xx->dpy, msk, &e))
      {
      if (e.type == KeyPress)
         {
         k = (XKeyEvent *) &e;
         symbol = XLookupKeysym(k,0);
         if (symbol == XK_Escape
	    || symbol == XK_q)
            {
            xx->runflg = 0;
            } /* if quit */
         } /* if keypress event */
      else if (e.type == Expose)
         {
	 XClearWindow(xx->dpy,xx->w);
         } /* if expose event */
      } /* if event received */
   } /* ifkey */

void initx(xxfmt *xx)
   {
   int rslt;
   char title[64];

   xx->dpy = XOpenDisplay(NULL);

   if (xx->dpy == NULL)
      {
      fprintf(stderr,"X Windows failure\n");
      exit(1);
      } /* if X Windows is not active */

   assert(xx->dpy);

   /* get dimensions of root window */
   xx->rootww = XDisplayWidth(xx->dpy,0);
   xx->rootwh = XDisplayHeight(xx->dpy,0);

   /* make display window smaller than root window */
   /* allow for menu bar on top */
   // xx->dpywdth = xx->rootww -  80;
   // xx->dpyhght = xx->rootwh - 100;
   xx->dpywdth = 512;        /* need a power of 2 */
   xx->dpyhght = 512;        /* need a power of 2 */

   xx->whiteColor = WhitePixel(xx->dpy, DefaultScreen(xx->dpy));
   xx->blackColor = BlackPixel(xx->dpy, DefaultScreen(xx->dpy));

   xx->w = XCreateSimpleWindow(xx->dpy,
      DefaultRootWindow(xx->dpy),
      0, 0, 
      xx->dpywdth, xx->dpyhght,
      0, xx->whiteColor,
      xx->whiteColor);

   XSelectInput(xx->dpy, xx->w, StructureNotifyMask);

   XMapWindow(xx->dpy, xx->w);

   xx->gc = XCreateGC(xx->dpy, xx->w, 0, NULL);

   xx->fontid = (Font) XLoadFont(xx->dpy,"12x24");

   XSetFont(xx->dpy,xx->gc,xx->fontid);

   XSetForeground(xx->dpy, xx->gc, xx->blackColor);

   xx->dfltscr = XDefaultScreenOfDisplay(xx->dpy);
   if (xx->dfltscr == NULL)
      {
      fprintf(stderr,"XDefaultScreenOfDisplay failed\n");
      perror("XDefaultScreenOfDisplay failed");
      exit(1);
      } /* if error */

   xx->cmap = XDefaultColormapOfScreen(xx->dfltscr);

   rslt = XAllocNamedColor(xx->dpy,xx->cmap,"red",
      &xx->scrdef,&xx->exctdef);

   if (rslt < 0)
      {
      fprintf(stderr,"XAllocNamedColor failed\n");
      perror("XAllocNamedColor failed");
      exit(1);
      } /* if error */
   xx->red = xx->scrdef.pixel;

   XSetWindowBorderWidth(xx->dpy, xx->w, 40);

   sprintf(title,"Random Screen");
   XStoreName(xx->dpy,xx->w,title);
   XSetIconName(xx->dpy,xx->w,title);

   while(1)
      {
      XEvent e;
      XNextEvent(xx->dpy, &e);
      if (e.type == MapNotify) break;
      } /* wait for window initialization */

   } /* initx */

/* Generate the CRC32C table */
void bldtbl(unsigned int *table)
   {
   int j;
   unsigned int byte;
   unsigned int crc;
   unsigned int mask;
   for (byte = 0; byte <= 255; byte++)
      {
      crc = byte;
      for (j = 7; j >= 0; j--)
         {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
         } /* for each bit in byte */
      table[byte] = crc;
      } /* for each of 256 bytes in table */
   } /* bldtbl */

/* Calculate the 32-bit CRC of a message */
unsigned int crc32c(unsigned char *message,
   int len, unsigned int *table)
   {
   unsigned char *p,*q;
   unsigned int byte, crc;
   p = (unsigned char *) message;
   q = (unsigned char *) message + len;
   crc = 0xFFFFFFFF;
   while (p < q)
      {
      byte = *p++;
      crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
      } /* for each byte in message */
   return ~crc;
   } /* crc32c */

int main(int argc, char **argv)
   {
   int i;
   int pswlen;                 /* length of password parameter */
   unsigned int crc;           /* crc output of crc32c */ 
   unsigned int seed;          /* RANDU 32-bit seed */
   unsigned int tbl[256];      /* CRC table */
   unsigned long long *statep;       /* pointer into ll->state */
   unsigned long long *stateq;       /* pointer for end of ll->state */
   unsigned char psw[128];     /* password text */
   xxfmt *xx;
   llfmt *ll;
   xx = (xxfmt *) malloc(sizeof(xxfmt));
   if (xx == NULL)
      {
      fprintf(stderr,"main: out of memory "
         "allocating xx\n");
      exit(1);
      } /* out of mem */
   if (argc != 2) putstx(*argv);    /* must have password parameter */
   /*******************************************/
   /* validate length of password             */
   /*******************************************/
   pswlen = strlen(*(argv+1));    /* validate length of password */
   if (pswlen > 64)
      {
      fprintf(stderr,"main: password overflow\n");
      fprintf(stderr,"password is 1-64 bytes long\n");
      putstx(*argv);
      } /* password overflow */
   strcpy((char *) psw,*(argv+1));     /* save password */
   /*****************************************************/
   initx(xx);
   ll = (llfmt *) elcginit();
   /*****************************************************/
   bldtbl(tbl);      /* initialize CRC table */
   seed = crc32c(psw,strlen((char *) psw),tbl);
   /*****************************************************/
   /* warm up the RANDU random number generator         */
   /*****************************************************/
   i = 256;
   while (i--) RANDU;
   /*****************************************************/
   /* override the state of the random number generator */
   /* with the password parameter converted into        */
   /* 32-bit unsigned integers.                         */
   /*****************************************************/
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->x = (unsigned long long) crc;
   ll->x     = (ll->x << 16) | 0x330e;
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->prev = (unsigned long long) crc;
   ll->prev = (ll->prev << 16) | 0x330e;
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->pprev = (unsigned long long) crc;
   ll->pprev = (ll->pprev << 16) | 0x330e;
   statep = (unsigned long long *) ll->state;
   stateq = (unsigned long long *) ll->state + STATES;
   while (statep < stateq)
      {
      RANDU;
      crc = crc32c((unsigned char *) &seed,4,tbl);
      *statep = (unsigned long long) crc;
      *statep = (*statep << 16) | 0x330e;
      statep++;
      } /* for each state in ll->state array */
   /*****************************************************/
   /* warm up the elcg random number generator          */
   /*****************************************************/
   i = 2048;
   while (i--) elcg(ll);
   /*****************************************************/
   /* paint the screen with random dots                 */
   /*****************************************************/
   xx->runflg = 1;
   while(xx->runflg)
      {
      int x;
      int y;
      x = (int) elcgpwr(ll,9);   /* 0 to 511 */
      y = (int) elcgpwr(ll,9);   /* 0 to 511 */
      if (elcgpwr(ll,1))
           XSetForeground(xx->dpy, xx->gc, xx->whiteColor);
      else
           XSetForeground(xx->dpy, xx->gc, xx->blackColor);
      XDrawPoint(xx->dpy,xx->w,xx->gc,x,y);
      ifkey(xx);
      } /* while runflg != 0 */
   XFreeGC(xx->dpy,xx->gc);
   XDestroyWindow(xx->dpy,xx->w);
   XCloseDisplay(xx->dpy);
   free(ll->state);
   free(ll);
   free(xx);
   return(0);
   } /* main */
