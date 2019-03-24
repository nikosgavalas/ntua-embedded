/*Parallel Hierarchical One-Dimensional Search for motion estimation*/
/*Initial algorithm - Used for simulation and profiling             */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 144 /*Frame dimension for QCIF format*/
#define M 176 /*Frame dimension for QCIF format*/
#define p 7   /*Search space. Restricted in a [-p,p] region around the \
                                original location of the block.*/

#define LOOP_I(i)                              \
    distx = 0;                                 \
    disty = 0;                                 \
    for (k = 0; k < Bx; k++) {                 \
        for (l = 0; l < By; l++) {             \
            int bxxk = Bx * x + k;             \
            int byyl = By * y + l;             \
            p1 = current[bxxk][byyl];          \
            q1 = current[bxxk][byyl];          \
            bxxk += vectors_x[x][y];           \
            byyl += vectors_x[x][y];           \
            if ((bxxk + i) < 0 ||              \
                (bxxk + i) > (N - 1) ||        \
                (byyl) < 0 ||                  \
                (byyl) > (M - 1)) {            \
                p2 = 0;                        \
            } else {                           \
                p2 = previous[bxxk + i][byyl]; \
            }                                  \
            distx += abs(p1 - p2);             \
            if ((bxxk) < 0 ||                  \
                (bxxk) > (N - 1) ||            \
                (byyl + i) < 0 ||              \
                (byyl + i) > (M - 1)) {        \
                q2 = 0;                        \
            } else {                           \
                q2 = previous[bxxk][byyl + i]; \
            }                                  \
            disty += abs(q1 - q2);             \
        }                                      \
    }                                          \
    if (distx < min1) {                        \
        min1 = distx;                          \
        bestx = i;                             \
    }                                          \
    if (disty < min2) {                        \
        min2 = disty;                          \
        besty = i;                             \
    }

#define LOOP_S(S)             \
    min1 = 255 * Bx * Bx;     \
    min2 = 255 * By * By;     \
    LOOP_I(-S);               \
    LOOP_I(0);                \
    LOOP_I(S);                \
    vectors_x[x][y] += bestx; \
    vectors_y[x][y] += besty;

/*Block size*/
int Bx = 16;
int By = 16;

unsigned long cur_time() {
    struct timeval time;
    gettimeofday(&time, 0);
    return 1000000 * time.tv_sec + time.tv_usec;
}

void read_sequence(int current[N][M], int previous[N][M]) {
    FILE *picture0, *picture1;
    int i, j;

    if ((picture0 = fopen("dato/akiyo0.y", "rb")) == NULL) {
        printf("Previous frame doesn't exist.\n");
        exit(-1);
    }

    if ((picture1 = fopen("dato/akiyo1.y", "rb")) == NULL) {
        printf("Current frame doesn't exist.\n");
        exit(-1);
    }

    /*Input for the previous frame*/
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            previous[i][j] = fgetc(picture0);
        }
    }

    /*Input for the current frame*/
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            current[i][j] = fgetc(picture1);
        }
    }

    fclose(picture0);
    fclose(picture1);
}

void phods_motion_estimation(int current[N][M], int previous[N][M],
                             int vectors_x[N / Bx][M / By], int vectors_y[N / Bx][M / By]) {
    int x, y, i, j, k, l, p1, p2, q1, q2, distx, disty, S, min1, min2, bestx, besty;

    distx = 0;
    disty = 0;

    /*Initialize the vector motion matrices*/
    for (i = 0; i < N / Bx; i++) {
        for (j = 0; j < M / By; j++) {
            vectors_x[i][j] = 0;
            vectors_y[i][j] = 0;
        }
    }

    /*For all blocks in the current frame*/
    for (x = 0; x < N / Bx; x++) {      // N/B = 9
        for (y = 0; y < M / By; y++) {  // M/B = 11
            LOOP_S(4);
            LOOP_S(2);
            LOOP_S(1);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        Bx = By = atoi(argv[1]);
    } else if (argc == 3) {
        Bx = atoi(argv[1]);
        By = atoi(argv[2]);
    }

    int current[N][M], previous[N][M], motion_vectors_x[N / Bx][M / By],
        motion_vectors_y[N / Bx][M / By], i, j;

    read_sequence(current, previous);

    unsigned long start = cur_time();

    phods_motion_estimation(current, previous, motion_vectors_x, motion_vectors_y);

    unsigned long end = cur_time();

    printf("%lu", end - start);

    return 0;
}
