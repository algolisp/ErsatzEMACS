/*
 * The routines in this file implement commands that work word at a time.
 * There are all sorts of word mode commands. If I do any sentence and/or
 * paragraph mode commands, they are likely to be put in this file
 */

#include <string.h>
#include <ctype.h>
#include "estruct.h"
#include "edef.h"

extern int backchar (int f, int n);
extern int forwchar (int f, int n);
extern void lchange (int flag);
extern int ldelete (int n, int kflag);
extern void mlwrite ();
extern int gotobop (int f, int n);
extern int gotoeop (int f, int n);
extern int linsert (int n, int c);
extern int lnewline ();

int backword (int f, int n);
int forwword (int f, int n);
int upperword (int f, int n);
int lowerword (int f, int n);
int capword (int f, int n);
int delfword (int f, int n);
int delbword (int f, int n);
int inword ();
int fillpara (int f, int n);

char wordbuf[NLINE];

/*
 * Move the cursor backward by "n" words. All of the details of motion are
 * performed by the "backchar" and "forwchar" routines. Error if you try to
 * move beyond the buffers
 */
int backword (int f, int n)
{
  if (n < 0)
    return (forwword (f, -n));
  if (backchar (FALSE, 1) == FALSE)
    return (FALSE);
  while (n--)
    {
      while (inword () == FALSE)
	{
	  if (backchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
      while (inword () != FALSE)
	{
	  if (backchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
    }
  return (forwchar (FALSE, 1));
}

/*
 * Move the cursor forward by the specified number of words. All of the motion
 * is done by "forwchar". Error if you try and move beyond the buffer's end
 */
int forwword (int f, int n)
{
  if (n < 0)
    return (backword (f, -n));
  while (n--)
    {
      while (inword () != FALSE)
	{
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
      while (inword () == FALSE)
	{
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
    }
  return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move,
 * convert any characters to upper case. Error if you try and move beyond the
 * end of the buffer. Bound to "M-U"
 */
int upperword (int f, int n)
{
  int c;

  if (n < 0)
    return (FALSE);
  while (n--)
    {
      while (inword () == FALSE)
	{
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
      while (inword () != FALSE)
	{
	  c = lgetc (curwp->w_dotp, curwp->w_doto);
	  if (c >= 'a' && c <= 'z')
	    {
	      c -= 'a' - 'A';
	      lputc (curwp->w_dotp, curwp->w_doto, c);
	      lchange (WFHARD);
	    }
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
    }
  return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move
 * convert characters to lower case. Error if you try and move over the end of
 * the buffer. Bound to "M-L"
 */
int lowerword (int f, int n)
{
  int c;

  if (n < 0)
    return (FALSE);
  while (n--)
    {
      while (inword () == FALSE)
	{
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
      while (inword () != FALSE)
	{
	  c = lgetc (curwp->w_dotp, curwp->w_doto);
	  if (c >= 'A' && c <= 'Z')
	    {
	      c += 'a' - 'A';
	      lputc (curwp->w_dotp, curwp->w_doto, c);
	      lchange (WFHARD);
	    }
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
    }
  return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move
 * convert the first character of the word to upper case, and subsequent
 * characters to lower case. Error if you try and move past the end of the
 * buffer. Bound to "M-C"
 */
int capword (int f, int n)
{
  int c;

  if (n < 0)
    return (FALSE);
  while (n--)
    {
      while (inword () == FALSE)
	{
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	}
      if (inword () != FALSE)
	{
	  c = lgetc (curwp->w_dotp, curwp->w_doto);
	  if (c >= 'a' && c <= 'z')
	    {
	      c -= 'a' - 'A';
	      lputc (curwp->w_dotp, curwp->w_doto, c);
	      lchange (WFHARD);
	    }
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	  while (inword () != FALSE)
	    {
	      c = lgetc (curwp->w_dotp, curwp->w_doto);
	      if (c >= 'A' && c <= 'Z')
		{
		  c += 'a' - 'A';
		  lputc (curwp->w_dotp, curwp->w_doto, c);
		  lchange (WFHARD);
		}
	      if (forwchar (FALSE, 1) == FALSE)
		return (FALSE);
	    }
	}
    }
  return (TRUE);
}

/*
 * Kill forward by "n" words. Remember the location of dot. Move forward by
 * the right number of words. Put dot back where it was and issue the kill
 * command for the right number of characters. Bound to "M-D"
 */
int delfword (int f, int n)
{
  LINE *l;
  int x, size, d;

  if (n < 0)
    return (FALSE);
  l = curwp->w_dotp;
  d = curwp->w_doto;
  size = 0;
  while (n--)
    {
      if (inword () != FALSE)
      while (inword () != FALSE)
	{
	  x = curwp->w_doto;
	  if (x == l->l_used)
	    break;
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	  if (curwp->w_doto <= x)
	    size += 1;
	  else
	    size += curwp->w_doto - x;
	}
      else
      while (inword () == FALSE)
	{
	  x = curwp->w_doto;
	  if (x == l->l_used)
	    break;
	  if (forwchar (FALSE, 1) == FALSE)
	    return (FALSE);
	  if (curwp->w_doto <= x)
	    size += 1;
	  else
	    size += curwp->w_doto - x;
	}
    }
  curwp->w_dotp = l;
  curwp->w_doto = d;
  return (ldelete (size, TRUE));
}

/*
 * Kill backwards by "n" words. Move backwards by the desired number of words,
 * counting the characters. When dot is finally moved to its resting place,
 * fire off the kill command. Bound to "M-Rubout" and to "M-Backspace"
 */
int delbword (int f, int n)
{
  int size, x;

  if (n < 0)
    return (FALSE);
  x = curwp->w_doto;
  if (backchar (FALSE, 1) == FALSE)
    return (FALSE);
  size = x - curwp->w_doto;
  while (n--)
    {
      if (inword () != FALSE)
      while (inword () == FALSE)
	{
	  x = curwp->w_doto;
	  if (backchar (FALSE, 1) == FALSE)
	    return (FALSE);
	  if (curwp->w_doto <= x)
	    size += 1;
	  else
	    size += x - curwp->w_doto;
	}
      else
      while (inword () != FALSE)
	{
	  x = curwp->w_doto;
	  if (backchar (FALSE, 1) == FALSE)
	    return (FALSE);
	  if (curwp->w_doto <= x)
	    size += 1;
	  else
	    size += x - curwp->w_doto;
	}
    }
  x = curwp->w_doto;
  if (forwchar (FALSE, 1) == FALSE)
    return (FALSE);
  if (curwp->w_doto <= x)
    size -= 1;
  else
    size -= curwp->w_doto - x;
  return (ldelete (size, TRUE));
}

/*
 * Return TRUE if the character at dot is a character that is considered to be
 * part of a word. The word character list is hard coded. Should be setable
 */
int inword ()
{
  int c;

  if (curwp->w_doto == llength (curwp->w_dotp))
    return (FALSE);
  c = lgetc (curwp->w_dotp, curwp->w_doto);
  if (c >= 'a' && c <= 'z')
    return (TRUE);
  if (c >= 'A' && c <= 'Z')
    return (TRUE);
  if (c >= '0' && c <= '9')
    return (TRUE);
  if (c == '$' || c == '_')	/* For identifiers */
    return (TRUE);
  if ((c & 0x80) != 0)
    return (TRUE);
  return (FALSE);
}

/* Fill the current paragraph according to the current fill column
 */
int fillpara (int f, int n)
{
  LINE *eopline;		/* pointer to line just past EOP */
  char wbuf[NLINE];		/* buffer for current word */
  int c;			/* current char during scan */
  int wordlen;			/* length of current word */
  int clength;			/* position on line during fill */
  int i;			/* index during word copy */
  int newlength;		/* tentative new line length */
  int eopflag;			/* Are we at the End-Of-Paragraph? */
  int firstflag;		/* first word? (needs no space) */

  if (fillcol == 0)
    {				/* no fill column set */
      mlwrite ("No fill column set");
      return (FALSE);
    }
  /* record the pointer to the line just past the EOP */
  gotoeop (FALSE, 1);
  eopline = lforw (curwp->w_dotp);

  /* and back top the begining of the paragraph */
  gotobop (FALSE, 1);

  /* initialize various info */
  clength = curwp->w_doto;
  if (clength && curwp->w_dotp->l_text[0] == TAB)
    clength = 8;
  wordlen = 0;

  /* scan through lines, filling words */
  firstflag = TRUE;
  eopflag = FALSE;
  while (!eopflag)
    {
      /* get the next character in the paragraph */
      if (curwp->w_doto == llength (curwp->w_dotp))
	{
	  c = ' ';
	  if (lforw (curwp->w_dotp) == eopline)
	    eopflag = TRUE;
	}
      else
	c = lgetc (curwp->w_dotp, curwp->w_doto);

      ldelete (1, FALSE);	/* and then delete it */

      /* if not a separator, just add it in */
      if (c != ' ' && c != '	')
	{
	  if (wordlen < (NLINE - 1))
	    wbuf[wordlen++] = c;
	}
      else if (wordlen)
	{
	  /* at a word break with a word waiting */
	  /* calculate tantitive new length with word added */
	  newlength = clength + 1 + wordlen;
	  if (newlength <= fillcol)
	    {
	      /* add word to current line */
	      if (!firstflag)
		{
		  linsert (1, ' '); /* the space */
		  ++clength;
		}
	      firstflag = FALSE;
	    }
	  else
	    {
	      lnewline ();	/* start a new line */
	      clength = 0;
	    }

	  /* and add the word in in either case */
	  for (i = 0; i < wordlen; i++)
	    {
	      linsert (1, wbuf[i]);
	      ++clength;
	    }
	  wordlen = 0;
	}
    }
  /* and add a last newline for the end of our new paragraph */
  return (lnewline ());
}

int na (int c)
{
  return !(isalpha(c) || ((c & 0x80) != 0));
}

int completeword (int f, int n)
{
  static LINE *l, *cl;
  static int b, d, cd, x;
  int xd, xe, xx;

  if (curwp->w_dotp != l || curwp->w_doto != b + x)
    {
      cl = l = curwp->w_dotp;
      b = d = curwp->w_doto;
      while (na (l->l_text[b]) && b != 0)
	b--;
      while (!na (l->l_text[b]) && b != 0)
	b--;
      if (b != 0 || na (l->l_text[b]))
	b++;
      cd = b;
      xx = 0;
    }
  else
    {
      curwp->w_dotp = l;
      curwp->w_doto = d;
      xx = x - (d - b);
    }

  if (b == d)
    return FALSE;

 again:
  x = 0;
  xd = cd;
  if (xd != 0)
    xd--;
  do
    {
      if (xd == 0)
	goto l;
      while (na (cl->l_text[xd]) && xd != 0)
	xd--;
      xe = xd + 1;
      while (!na (cl->l_text[xd]) && xd != 0)
	xd--;
      if (na (cl->l_text[xd]))
	xd++;
      if (strncmp (l->l_text + b, cl->l_text + xd, d - b) == 0
	  && xe - xd != d - b)
	{
	  if (!(cl == l && xd == b))
	    x = xe - xd;
	  break;
	}
      if (xd > 0 && !(cl == l && xd == b))
	xd--;
      continue;
    l:
      cl = lback (cl);
      xd = cl->l_used;
      if (xd > 0 && !(cl == l && xd == b))
	xd--;
    }
  while (!(cl == l && xd == b));
  
  curwp->w_dotp = l;
  curwp->w_doto = d;

  if (x != 0)
    {
      int k, i, w;
      
      k = d - b;
      w = x - k;
      if (w > NLINE)
	w = NLINE;
      strncpy (wordbuf, cl->l_text + xd + k, w);
      if (d + w <= curwp->w_dotp->l_used
	  && strncmp (wordbuf, curwp->w_dotp->l_text + d, w) == 0)
	{
	  cd = xd;
	  goto again;
	}
      ldelete (xx, 0);
      for (i = 0; i < w; i++)
	linsert (1, wordbuf[i]);
    }
  else
    {
      ldelete (xx, 0);
    }
  if (l != curwp->w_dotp)
    cl = l = curwp->w_dotp;
  cd = xd;
  return TRUE;
}
