/* termios video driver */

#define	termdef	1		/* don't define "term" external */

#include <stdio.h>		/* puts(3), snprintf(3) */
#include <stdlib.h>
#include "estruct.h"
#include "edef.h"
#undef CTRL
#include <sys/ioctl.h>

char *tgoto(char *s, int c, int r)
{
  static char buf[32];

  sprintf(buf,s,r+1,c+1);
  return buf;
}

void tputs(char *s, int x, void (*f)())
{
  while (*s)
    (*f)(*s++);
}

extern char *getenv();
extern void ttopen();
extern int ttgetc();
extern void ttputc();
extern void ttflush();
extern void ttclose();

void getwinsize();
void tcapopen();
void tcapmove(int row, int col);
void tcapeeol();
void tcapeeop();
void tcaprev();
void tcapbeep();
void tcapboldon();
void tcapboldoff();

#define	MARGIN 8
#define	SCRSIZ 64
#define BEL 0x07
#define TCAPSLEN 64

char *CM, *CE, *CL, *SO, *SE, *MD, *ME;

TERM term = {
  0, 0, 0, MARGIN, SCRSIZ, tcapopen, ttclose, ttgetc, ttputc,
  ttflush, tcapmove, tcapeeol, tcapeeop, tcapbeep, tcaprev,
  tcapboldon, tcapboldoff
};

void getwinsize ()
{
  int cols, rows;
#ifndef FORCE_COLS
  struct winsize ws;

  ioctl (0, TIOCGWINSZ, &ws);
  cols = ws.ws_col;
  rows = ws.ws_row;
#else
  cols = FORCE_COLS;
  rows = FORCE_ROWS;
#endif
  if ((cols < 10) || (rows < 3))
    {
      puts ("Unbelievable screen size\n");
      exit (1);
    }
  term.t_ncol = cols;
  term.t_nlinebuf = cols * 8;
  term.t_nrow = rows-1;
}

void tcapopen ()
{
  char *tv_stype;

  if ((tv_stype = getenv ("TERM")) == NULL)
    {
      puts ("Environment variable TERM not defined\n");
      exit (1);
    }
  CL="\033[H\033[2J";
  CM="\033[%d;%dH";
  CE="\033[K";
  SE="\033[27m";
  SO="\033[7m";
  MD="\033[1m";
  ME="\033[0m";
  revexist = TRUE;
  ttopen ();
}

void tcaprev (int state)
{
  if (revexist)
    tputs ((state ? SO : SE), 1, ttputc);
}

void tcapmove (int row, int col)
{
  tputs (tgoto (CM, col, row), 1, ttputc);
}

void tcapeeol ()
{
  tputs (CE, 1, ttputc);
}

void tcapeeop ()
{
  tputs (CL, 1, ttputc);
}

void tcapbeep ()
{
  ttputc (BEL);
}

void tcapboldon ()
{
  tputs (MD, 1, ttputc);
}

void tcapboldoff ()
{
  tputs (ME, 1, ttputc);
}

