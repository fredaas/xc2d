#ifndef DELTATIME_H
#define DELTATIME_H

#include "sys/time.h"
#include "stdio.h"

double dt_walltime(void);
long dt_msec(void);
long dt_sec(void);
void dt_update(void);

extern double dt;

#endif /* DELTATIME_H */
