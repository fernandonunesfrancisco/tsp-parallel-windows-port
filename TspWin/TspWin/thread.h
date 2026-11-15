/* thread.h - run the search on one thread, or split it over several. */
#ifndef THREAD_H
#define THREAD_H

#include "distance.h"

#define MAX_THREADS 16

Best run_sequential(const Cities *cities);
Best run_parallel(const Cities *cities, int threadCount);

#endif
