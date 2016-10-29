#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

const char *progname = NULL;
char *port = "1234";

static void
usage ()
{
  printf ("%s: send file to remote host\n", progname);
  printf ("Usage: %s host[:port] file\n", progname);
  printf ("Default port is %s\n", port);
}

static void
send_file (int fd, const char *filename)
{
  off_t dummy = 0;              /* specially for Solaris (it crashes if sendfile's 3rd arg is NULL).  */
  int sfd = open (filename, O_RDONLY);
  if (sfd < 0)
    {
      fatal ("cannot read `%s'", filename);
    }

  struct stat st;
  if (!fstat (sfd, &st))
    {
      ssize_t rc;
      info ("sending `%s'", filename);
      const char *name = basename ((char *) filename);
      size_t filename_len = strlen (name) + 1;
      rc = write (fd, name, filename_len);      /* filename\0payload  */
      if (rc != filename_len)
        {
          fatal ("failed to write filename");
        }
      rc = sendfile (fd, sfd, &dummy, st.st_size);
      if (rc < 0)
        {
          fatal ("failed to send file `%s'", filename);
        }
      else if (rc != st.st_size)
        {
          warning ("sent only %d of %d bytes of file `%s'", rc,
                   st.st_size, filename);
        }
      else
        {
          info ("sent %d bytes of `%s'", rc, filename);
        }
    }
  else
    {
      fatal ("failed to stat `%s'", filename);
    }

  (void) close (sfd);
}

int
main (int argc, char **argv)
{
  const char *host = NULL;
  char *colon = NULL;
  ssize_t rc;
  int fd;
  struct addrinfo hints;
  struct addrinfo *result;
  struct addrinfo *rp = NULL;

  progname = argv[0];
  if (argc != 3)
    {
      usage ();
      exit (EXIT_FAILURE);
    }

  host = argv[1];
  colon = strchr (argv[1], ':');
  if (colon)
    {
      *colon = '\0';
      port = colon + 1;
    }

  info ("connecting to host %s, port %s", host, port);

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  rc = getaddrinfo (host, port, &hints, &result);
  if (0 != rc)
    {
      fatal ("getaddrinfo() failed");
    }
  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      fd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (fd == -1)
        continue;

      if (!connect (fd, rp->ai_addr, rp->ai_addrlen))
        break;

      (void) close (fd);
    }
  freeaddrinfo (result);

  if (rp == NULL)
    {
      fatal ("failed to connect");
    }

  send_file (fd, argv[2]);

  return EXIT_SUCCESS;
}
