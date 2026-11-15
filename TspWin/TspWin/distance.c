#include "distance.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void cities_generate(Cities *cities, int count)
{
    int i, j;

    cities->count = count;

    for (i = 0; i < count; i++) {
        cities->x[i] = (rand() % 10000) / 100.0;
        cities->y[i] = (rand() % 10000) / 100.0;
    }

    for (i = 0; i < count; i++) {
        for (j = 0; j < count; j++) {
            double dx = cities->x[i] - cities->x[j];
            double dy = cities->y[i] - cities->y[j];
            cities->distance[i][j] = sqrt(dx * dx + dy * dy);
        }
    }
}

double route_cost(const Cities *cities, const int route[])
{
    double cost = 0.0;
    int i;

    for (i = 0; i + 1 < cities->count; i++) {
        cost += cities->distance[route[i]][route[i + 1]];
    }
    /* come back to the start */
    cost += cities->distance[route[cities->count - 1]][route[0]];
    return cost;
}

void search_init(Search *search, const Cities *cities)
{
    memset(search, 0, sizeof *search);
    search->cities = cities;
    search->best.cost = DBL_MAX;       /* any real route beats this */
    search->route[0] = 0;              /* always start at city 0    */
    search->used[0] = 1;
}

void search_continue(Search *search, int position)
{
    int count = search->cities->count;
    int city;

    /* The route is complete: measure it. */
    if (position == count) {
        double cost = route_cost(search->cities, search->route);

        search->best.routesTested++;
        if (cost < search->best.cost) {
            search->best.cost = cost;
            memcpy(search->best.route, search->route, sizeof search->route);
        }
        return;
    }

    /* Try every unused city in this position, then go deeper. */
    for (city = 1; city < count; city++) {
        if (search->used[city]) {
            continue;
        }
        search->route[position] = city;
        search->used[city] = 1;

        search_continue(search, position + 1);

        search->used[city] = 0;        /* undo, so the next city can try */
    }
}
