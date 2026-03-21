/*
 * solver.c
 *
 *  Created on: Mar 20, 2026
 *      Author: james
 */
#include "solver.h"
#include "irs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 16
#define INF 9999

int flood[MAX][MAX];
int posX = 0;
int posY = 0;
Heading dir = NORTH;
int run = 1;

int wallsN[MAX][MAX];
int wallsE[MAX][MAX];
int wallsS[MAX][MAX];
int wallsW[MAX][MAX];

Action floodFill(); // forward declaration

Action solver() {
    return floodFill();
}

Action leftWallFollower() {
    if (readFrontIR()) {
        if (readLeftIR()) return RIGHT;
        return LEFT;
    }
    return FORWARD;
}

// --------- BFS flood from a goal ---------
void recomputeFlood(int goalX[], int goalY[], int count) {
    for (int y = 0; y < MAX; y++)
        for (int x = 0; x < MAX; x++)
            flood[y][x] = -1;

    typedef struct { int x, y; } Cell;
    Cell q[MAX * MAX];
    int head = 0, tail = 0;

    for (int i = 0; i < count; i++) {
        flood[goalY[i]][goalX[i]] = 0;
        q[tail++] = (Cell){goalX[i], goalY[i]};
    }

    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};

    while (head < tail) {
        Cell c = q[head++];
        int walled[4] = {
            wallsN[c.y][c.x],
            wallsE[c.y][c.x],
            wallsS[c.y][c.x],
            wallsW[c.y][c.x]
        };
        for (int d = 0; d < 4; d++) {
            int nx = c.x + dx[d], ny = c.y + dy[d];
            if (nx < 0 || nx >= MAX || ny < 0 || ny >= MAX) continue;
            if (walled[d] || flood[ny][nx] != -1) continue;
            flood[ny][nx] = flood[c.y][c.x] + 1;
            q[tail++] = (Cell){nx, ny};
        }
    }
}

void displayFlood() {
    char buf[8];
    for (int y = 0; y < MAX; y++)
        for (int x = 0; x < MAX; x++) {
            //sprintf(buf, "%d", flood[y][x]);
            //API_setText(x, y, buf);
        }
}

void floodToCenter() {
    int gx[] = {7, 8, 7, 8};
    int gy[] = {7, 7, 8, 8};
    recomputeFlood(gx, gy, 4);
    displayFlood();
}

void floodToStart() {
    int gx[] = {0};
    int gy[] = {0};
    recomputeFlood(gx, gy, 1);
    displayFlood();
}

// --------- Position update (called by main after a FORWARD) ---------
void updatePosition() {
    if      (dir == NORTH) posY++;
    else if (dir == EAST)  posX++;
    else if (dir == SOUTH) posY--;
    else if (dir == WEST)  posX--;
}

// --------- Direction update (called by main after a turn) ---------
void updateDirLeft()  { dir = (Heading)((dir + 3) % 4); }
void updateDirRight() { dir = (Heading)((dir + 1) % 4); }

// --------- Bounds-safe neighbor lookup ---------
int neighborValue(Heading h) {
    int nx = posX, ny = posY;
    if      (h == NORTH) ny++;
    else if (h == EAST)  nx++;
    else if (h == SOUTH) ny--;
    else if (h == WEST)  nx--;
    if (nx < 0 || nx >= MAX || ny < 0 || ny >= MAX) return INF;
    if (flood[ny][nx] == -1) return INF;
    return flood[ny][nx];
}

// --------- Sense and store walls ---------
int senseWalls() {
    int wf = readFrontIR();
    int wl = readLeftIR();
    int wr = readRightIR();

    Heading hs[3] = {dir, (Heading)((dir+3)%4), (Heading)((dir+1)%4)};
    int     ws[3] = {wf, wl, wr};
    int dx[]      = {0, 1, 0, -1};
    int dy[]      = {1, 0, -1, 0};
    int newWall   = 0;

    for (int i = 0; i < 3; i++) {
        if (!ws[i]) continue;
        int h = hs[i];

        int known = 0;
        if (h == NORTH && wallsN[posY][posX]) known = 1;
        if (h == EAST  && wallsE[posY][posX]) known = 1;
        if (h == SOUTH && wallsS[posY][posX]) known = 1;
        if (h == WEST  && wallsW[posY][posX]) known = 1;
        if (known) continue;

        newWall = 1;
        if (h == NORTH) wallsN[posY][posX] = 1;
        if (h == EAST)  wallsE[posY][posX] = 1;
        if (h == SOUTH) wallsS[posY][posX] = 1;
        if (h == WEST)  wallsW[posY][posX] = 1;
        //API_setWall(posX, posY, "nesw"[h]);

        int nx = posX + dx[h], ny = posY + dy[h];
        if (nx >= 0 && nx < MAX && ny >= 0 && ny < MAX) {
            int opp = (h + 2) % 4;
            if (opp == NORTH) wallsN[ny][nx] = 1;
            if (opp == EAST)  wallsE[ny][nx] = 1;
            if (opp == SOUTH) wallsS[ny][nx] = 1;
            if (opp == WEST)  wallsW[ny][nx] = 1;
        }
    }
    return newWall;
}

// --------- Flood Fill — returns Action, no direct API turn/move calls ---------
Action floodFill() {
    static int initialized = 0;
    if (!initialized) {
        for (int i = 0; i < MAX; i++) {
            wallsN[MAX-1][i] = wallsS[0][i] = 1;
            wallsE[i][MAX-1] = wallsW[i][0] = 1;
        }
        floodToCenter();
        initialized = 1;
    }

    int atCenter = (posX == 7 || posX == 8) && (posY == 7 || posY == 8);
    int atStart  = (posX == 0 && posY == 0);

    if (run % 2 == 1 && atCenter) {
        run++;
        floodToStart();
        return IDLE;
    }
    if (run % 2 == 0 && atStart) {
        run++;
        floodToCenter();
        return IDLE;
    }

    // Sense walls, recompute if new
    if (senseWalls()) {
        if (run % 2 == 1) floodToCenter();
        else              floodToStart();
    }

    Heading front = dir;
    Heading left  = (Heading)((dir + 3) % 4);
    Heading right = (Heading)((dir + 1) % 4);
    Heading back  = (Heading)((dir + 2) % 4);

    int wallFront = readFrontIR();
    int wallLeft  = readLeftIR();
    int wallRight = readRightIR();

    int bestVal         = INF;
    Heading bestHeading = back;

    if (!wallFront && neighborValue(front) < bestVal) { bestVal = neighborValue(front); bestHeading = front; }
    if (!wallLeft  && neighborValue(left)  < bestVal) { bestVal = neighborValue(left);  bestHeading = left;  }
    if (!wallRight && neighborValue(right) < bestVal) { bestVal = neighborValue(right); bestHeading = right; }

    // Trap detection — update cell value and backtrack
    if (bestVal >= flood[posY][posX]) {
        flood[posY][posX] = bestVal + 1;
        char buf[8];
        //sprintf(buf, "%d", flood[posY][posX]);
        //API_setText(posX, posY, buf);
        bestHeading = back; // backtrack
    }

    // Return the action needed to face bestHeading
    // main.c will execute it; we update dir/pos on next call via the static state
    int turn = ((int)bestHeading - (int)dir + 4) % 4;

    if (turn == 1) {
        updateDirRight();   // update our tracked direction now
        return RIGHT;
    }
    if (turn == 3) {
        updateDirLeft();
        return LEFT;
    }
    if (turn == 2) {
        // Need to turn 180 — do one turn this step, next call will finish it
        updateDirRight();
        return RIGHT;
    }

    // turn == 0: already facing best direction, move forward
    updatePosition();
    return FORWARD;
}

//Action solver() {
//    return leftWallFollower();
//}

// This is an example of a simple left wall following algorithm.
//Action leftWallFollower() {
//    if(readFrontIR()) {
//        if(readLeftIR()){
//            return RIGHT;
//        }
//        return LEFT;
//    }
//    return FORWARD;
//}

// Put your implementation of floodfill here!
//Action floodFill() {
//    return FORWARD;
//}


