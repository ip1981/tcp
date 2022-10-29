#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

static void
info_va (const char *msg, va_list va)
{
  fprintf (stderr, "%s: ", progname);
  vfprintf (stderr, msg, va);
  fputs ("\n", stderr);
}

void
info (const char *msg, ...)
{
  va_list ap;

  va_start (ap, msg);
  info_va (msg, ap);
  va_end (ap);
}

static void
warning_va (const char *msg, va_list va)
{
  fprintf (stderr, "%s: WARNING: ", progname);
  vfprintf (stderr, msg, va);
  if (errno)
    {
      fprintf (stderr, ": %s", strerror (errno));
    }
  fputs ("\n", stderr);
}

void
warning (const char *msg, ...)
{
  va_list ap;

  va_start (ap, msg);
  warning_va (msg, ap);
  va_end (ap);
}

static void
fatal_va (const char *msg, va_list va)
{
  fprintf (stderr, "%s: FATAL: ", progname);
  vfprintf (stderr, msg, va);
  if (errno)
    {
      fprintf (stderr, ": %s", strerror (errno));
    }
  fputs ("\n", stderr);
}

void
fatal (const char *msg, ...)
{
  va_list ap;

  va_start (ap, msg);
  fatal_va (msg, ap);
  va_end (ap);
  exit (EXIT_FAILURE);
}
