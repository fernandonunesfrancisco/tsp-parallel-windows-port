# TSP brute force: one thread vs. Windows threads

Open `TspWin.sln`, choose **Release | x64**, press **Ctrl+F5**.
Arguments (Project > Properties > Debugging > Command Arguments): `cities threads seed`, e.g. `10 4 1`.

| File | What it does |
| --- | --- |
| `distance.h/.c` | cities, distance matrix, the brute-force search (recursion) |
| `thread.h/.c`   | `run_sequential` and `run_parallel` (Windows threads) |
| `main.c`        | reads arguments, times both runs, prints the comparison |

Up to 13 cities (12! = 479 million routes). Beyond ~11 cities use Release, not Debug.
