#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

const char *progname = NULL;
int port = 1234;
int listen_queue = 100;
const char *basedir = "/var/tmp";

static void
recvfile (int fd)
{
  ssize_t rc;
  int dfd = -1;
  char *filename = NULL;
  size_t filename_len;
  off_t filelength = 0;
  char *buf = malloc (PATH_MAX);
  if (!buf)
    {
      warning ("failed to allocate space");
      goto clean;
    }

  rc = read (fd, buf, PATH_MAX);        /* filename\0payload  */
  if (rc < 0)
    {
      warning ("failed to read filename");
      goto clean;
    }
  else if (0 == rc)
    {
      info ("no data received, connection closed by client");
      goto clean;
    }

  /* XXX no subdirs.  */
  filename = strndup (buf, PATH_MAX);
  if (!filename)
    {
      warning ("failed to create filename");
      goto clean;
    }
  filename_len = strlen (filename);

  dfd =
    openat (AT_FDCWD, filename, O_CREAT + O_WRONLY,
            S_IRUSR | S_IWUSR | S_IRGRP);
  if (dfd < 0)
    {
      warning ("failed to open file `%s'", filename);
      goto clean;
    }

  char *payload_start = buf + filename_len + 1;
  ssize_t rest_len = rc - filename_len - 1;
  rc = write (dfd, payload_start, rest_len);    /* write the rest of buf.  */
  if (rc != rest_len)
    {
      warning ("Failed to write `%s'", filename);
      goto clean;
    }
  filelength += rc;

  while ((rc = read (fd, buf, PATH_MAX)) > 0)
    {
      if (write (dfd, buf, rc) < 0)
        {
          warning ("failed to write `%s'", filename);
          goto clean;
        }
      filelength += rc;
    }
  info ("received %lu of `%s'", filelength, filename);

clean:
  if (buf)
    free (buf);
  if (filename)
    free (filename);
  if (dfd > 0)
    {
      if (close (dfd))
        {
          warning ("failed to close %d", dfd);
        }
    }
}

int
main (int argc, char **argv)
{
  NOTUSED (argc);

  int listen_fd;
  int conn_fd;
  struct sockaddr_in servaddr;
  int rc;

  progname = argv[0];

  if (chdir (basedir))
    {
      fatal ("failed to change directory to `%s'", basedir);
    }

  listen_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
    {
      fatal ("failed to create listen socket");
    }

  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (port);

  rc = bind (listen_fd, (const struct sockaddr *) &servaddr,
             sizeof (servaddr));
  if (0 != rc)
    {
      fatal ("failed to bind listen socket");
    }

  rc = listen (listen_fd, listen_queue);
  if (0 != rc)
    {
      fatal ("failed to plug listen socket");
    }

  info ("accepting connections on port %d", port);
  info ("will save files under `%s'", basedir);
  while (1)
    {
      conn_fd = accept (listen_fd, NULL, NULL);
      if (conn_fd < 0)
        {
          warning ("accept() failed");
          continue;
        }
      recvfile (conn_fd);
    }

  return EXIT_SUCCESS;
}
