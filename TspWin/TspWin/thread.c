#include "thread.h"

#include <string.h>
#include <windows.h>

/* One worker = one thread + the cities it is responsible for. */
typedef struct {
    const Cities* cities;
    int firstCity;             /* this worker tries second cities           */
    int endCity;               /* firstCity ... endCity-1                   */
    Best best;                 /* its answer, written once when it finishes  */
} Worker;

/* Windows requires exactly this shape: DWORD WINAPI f(LPVOID).
 * The answer is left inside the Worker, so we simply return 0. */
static DWORD WINAPI worker_main(LPVOID parameter)
{
    Worker* worker = (Worker*)parameter;
    Search search;                 /* lives on THIS thread's own stack */
    int city;

    search_init(&search, worker->cities);

    for (city = worker->firstCity; city < worker->endCity; city++) {
        search.route[1] = city;
        search.used[city] = 1;

        search_continue(&search, 2);

        search.used[city] = 0;
    }

    worker->best = search.best;    /* the only write into shared memory */
    return 0;
}

Best run_sequential(const Cities* cities)
{
    Search search;

    search_init(&search, cities);
    search_continue(&search, 1);
    return search.best;
}

Best run_parallel(const Cities* cities, int threadCount)
{
    Worker workers[MAX_THREADS];
    HANDLE handles[MAX_THREADS];
    Best best;
    int others = cities->count - 1;       /* cities that can come second */
    int i;

    if (threadCount > MAX_THREADS) threadCount = MAX_THREADS;
    if (threadCount > others)      threadCount = others;
    if (threadCount < 1)           threadCount = 1;

    /* Give each worker a slice of the second cities: 1..others. */
    for (i = 0; i < threadCount; i++) {
        workers[i].cities = cities;
        workers[i].firstCity = 1 + (i * others) / threadCount;
        workers[i].endCity = 1 + ((i + 1) * others) / threadCount;

        handles[i] = CreateThread(NULL, 0, worker_main, &workers[i], 0, NULL);
        if (handles[i] == NULL) {
            worker_main(&workers[i]);     /* could not start: do it here */
        }
    }

    /* First wait for every thread to finish... */
    for (i = 0; i < threadCount; i++) {
        if (handles[i] != NULL) {
            WaitForSingleObject(handles[i], INFINITE);
            CloseHandle(handles[i]);
        }
    }

    /* ...only then it is safe to read their answers and keep the best. */
    best = workers[0].best;
    for (i = 1; i < threadCount; i++) {
        best.routesTested += workers[i].best.routesTested;
        if (workers[i].best.cost < best.cost) {
            best.cost = workers[i].best.cost;
            memcpy(best.route, workers[i].best.route, sizeof best.route);
        }
    }
    return best;
}