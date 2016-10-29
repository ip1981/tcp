#ifndef UTILS_H
#define UTILS_H

extern const char *progname;

void info (const char *, ...);
void warning (const char *, ...);
void fatal (const char *, ...);

#define NOTUSED(x) ((void)(x))

#endif /* UTILS_H */
