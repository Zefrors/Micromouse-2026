/*
 * solver.h
 *
 *  Created on: Mar 20, 2026
 *      Author: james
 */

#ifndef SRC_SOLVER_H_
#define SRC_SOLVER_H_

#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

Action solver();
Action leftWallFollower();
Action floodFill();

#endif


#endif /* SRC_SOLVER_H_ */
