/* distance.h - the cities, the distances, and the brute-force search. */
#ifndef DISTANCE_H
#define DISTANCE_H

#define MAX_CITIES 13          /* 12! = 479 million routes: already slow */

/* The cities and the distance between every pair of them. */
typedef struct {
    int count;
    double x[MAX_CITIES];
    double y[MAX_CITIES];
    double distance[MAX_CITIES][MAX_CITIES];
} Cities;

/* The best route found so far. */
typedef struct {
    double cost;
    int route[MAX_CITIES];
    long long routesTested;
} Best;

/* Everything the search needs while it is running (one per thread). */
typedef struct {
    const Cities *cities;
    int route[MAX_CITIES];     /* the route being built                */
    int used[MAX_CITIES];      /* used[i] == 1 if city i is in route   */
    Best best;                 /* best route found by THIS search      */
} Search;

/* Fills in random coordinates and the distance matrix. */
void cities_generate(Cities *cities, int count);

/* Length of the closed route (it returns to its first city). */
double route_cost(const Cities *cities, const int route[]);

/* Prepares a search: the route starts at city 0. */
void search_init(Search *search, const Cities *cities);

/* Tries every way to finish the route from 'position' onward. */
void search_continue(Search *search, int position);

#endif
