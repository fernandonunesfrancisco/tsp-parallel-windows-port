/*
 * TSP by brute force: one thread vs. several Windows threads.
 *
 * Usage:  TspWin [cities] [threads] [seed]
 *         TspWin 10 4 1
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "distance.h"
#include "thread.h"

/* Seconds from the high-resolution Windows clock. */
static double now_seconds(void)
{
    LARGE_INTEGER frequency, counter;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
}

static int number_argument(int argc, char *argv[], int index, int fallback)
{
    return (index < argc) ? atoi(argv[index]) : fallback;
}

static void print_route(const char *title, const Best *best, int count, double seconds)
{
    int i;

    printf("\n%s\n", title);
    printf("  cost:          %.2f\n", best->cost);
    printf("  route:         ");
    for (i = 0; i < count; i++) {
        printf("%d -> ", best->route[i] + 1);
    }
    printf("%d\n", best->route[0] + 1);
    printf("  routes tested: %lld\n", best->routesTested);
    printf("  time:          %.4f s\n", seconds);
}

int main(int argc, char *argv[])
{
    SYSTEM_INFO system;
    Cities cities;
    Best sequential, parallel;
    double start, sequentialTime, parallelTime;
    int count, threads, seed, i;

    GetSystemInfo(&system);
    count   = number_argument(argc, argv, 1, 10);
    threads = number_argument(argc, argv, 2, (int)system.dwNumberOfProcessors);
    seed    = number_argument(argc, argv, 3, 1);

    if (count < 3 || count > MAX_CITIES || threads < 1) {
        printf("Usage: TspWin [cities 3..%d] [threads] [seed]\n", MAX_CITIES);
        return 1;
    }

    srand((unsigned)seed);
    cities_generate(&cities, count);

    printf("%d cities, %d threads requested, seed %d\n", count, threads, seed);
    for (i = 0; i < count; i++) {
        printf("  city %2d: (%.2f, %.2f)\n", i + 1, cities.x[i], cities.y[i]);
    }

    start = now_seconds();
    sequential = run_sequential(&cities);
    sequentialTime = now_seconds() - start;

    start = now_seconds();
    parallel = run_parallel(&cities, threads);
    parallelTime = now_seconds() - start;

    print_route("SEQUENTIAL", &sequential, count, sequentialTime);
    print_route("PARALLEL", &parallel, count, parallelTime);

    printf("\nSpeedup: %.2fx\n", sequentialTime / parallelTime);

    if (fabs(sequential.cost - parallel.cost) < 1e-9 &&
        sequential.routesTested == parallel.routesTested) {
        printf("OK: both found the same best cost and tested the same routes.\n");
        return 0;
    }
    printf("ERROR: the two results differ!\n");
    return 1;
}
