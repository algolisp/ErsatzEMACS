#include <string.h>

void copy_string (char *to, char *from, int n)
{
  strncpy (to, from, n - 1);
  to[n - 1] = 0;
}

int utf8_length (char c)
{
  if ((c & 0x80) == 0)
    return 1;
  if ((c & 0x20) == 0)
    return 2;
  if ((c & 0x10) == 0)
    return 3;
  if ((c & 0x08) == 0)
    return 4;
  if ((c & 0x04) == 0)
    return 5;
  if ((c & 0x02) == 0)
    return 6;
  return 7;
}

int string_width (char *s, int n)
{
  int u;
  int i, w;

  w = 0;
  for (i = 0; i < n; i++)
    {
      u = utf8_length (s[i]) - 1;
      if (u == 0)
	{
	  w++;
	}
      else if (u < n - i)
	{
	  i += u;
	  w += 2;
	}
      else
	{
	  return w;
	}
    }
  return w;
}
