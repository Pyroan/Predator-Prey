// gcc filename -lm -lGL -lGLU -lglut
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define DEAD 0
#define PREY 1
#define PREDATOR 2

// Edit these to determine number of cells
#define width 400
#define height 400

// Edit this to determine size of cells (and thus the window)
float pointSize = 1.0;

long Time, timeToNextFrame;
// Edit this to change the step rate
float frameRate = 1.0/60;

// Edit this to change how quickly new prey spawns and old predators die.
int health_threshold = 5;

struct cell {
	int health;
	int type; // Either DEAD, PREY, or PREDATOR
};

struct cell grid[width][height];
struct cell newGrid[width][height]; // Next stage 


void init(void) {
	//Seed RNG
	time_t t;
	srand((unsigned) time(&t));

	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_FLAT);
	
	glPointSize(pointSize);
}

/***********************
 * Initialize the grid *
 *    - OPTIONAL -     *
 ***********************/
void initGrid(void) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			struct cell newCell;
			int rando = rand()%10000;
			if (rando >= 0 && rando < 6000) {
				newCell.type = DEAD;
			} else if (rando >= 7000 && rando < 9999) {
				newCell.type = PREY;
			} else {
				newCell.type = PREDATOR;
			}
			if (newCell.type == PREY) {
				newCell.health = 1;
			} else if (newCell.type == PREDATOR) {
				newCell.health = health_threshold;
			}
			grid[i][j] = newCell;
		}
	}	
}

typedef struct {
	int x;
	int y;
} point;

int countCellsOfType(int type) {
	int count = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (grid[i][j].type == type)
				count++;
		}
	}
	return count;
}

/*********************************************
 * Spawn a new cell into the grid,           *
 * if there aren't enough for the sim to run *
 *********************************************/
void populateGrid() {
	int newGuyType;
	if (1.0*countCellsOfType(PREY) / countCellsOfType(DEAD) < .0001)
	{
		newGuyType = PREY;
	}
	else if (1.0*countCellsOfType(PREDATOR)/countCellsOfType(DEAD)<.0001)
	{
		newGuyType = PREDATOR;
	}
	int x = rand() % width;
	int y = rand() % height;
	if (grid[x][y].type == DEAD) {
		struct cell newCell;
		newCell.type = newGuyType;
		if (newCell.type == PREY) {
			newCell.health = 1;
		} else if (newCell.type == PREDATOR) {
			newCell.health = health_threshold;
		}
		grid[x][y] = newCell;
	}
}

// Return a random cell of the given type adjacent to
// the given cell.
// Return null if can't find cell?
point findCellOfType(int type, int x, int y) {
	point candidates[8];
	int count = 0;
	for (int i = x-1; i <= x+1; i++) {
		for (int j = y-1; j <= y+1; j++) {
		if (!(i == x && j == y)) {
				if (i>=0 && j>=0 && i < width && j< height) {
					if (grid[i][j].type == type)
					{
						candidates[count].x = i;
						candidates[count].y = j;
						count++;
					}
				}
			}
		}
	}
	point candidate;
	if (count>0) {
		candidate = candidates[rand()%count];	
		return candidate;
	} else {
		candidate.x = -1;
		candidate.y = -1;
		return candidate;
	}
}

/*************************
 * Update the grid state *
 *************************/
void updateGrid(void) {
	populateGrid();
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
//			printf("Finding neighbors for (%d, %d)\n",i,j);
			// Handle predators
			if (grid[i][j].type == PREDATOR) {
				grid[i][j].health--;
				if (grid[i][j].health == 0) {
					newGrid[i][j].type = DEAD;
				} else {
					// Try to eat prey
					// Change this to if() if you want to weaken predators.
					while (findCellOfType(PREY,i,j).x != -1) {
						point prey = findCellOfType(PREY,i,j);
						grid[prey.x][prey.y].type = DEAD;
						newGrid[i][j].health += grid[prey.x][prey.y].health; 
						newGrid[prey.x][prey.y].type = PREDATOR;
						newGrid[prey.x][prey.y].health = health_threshold;
					}
					// Try to move.
					if (findCellOfType(DEAD, i, j).x != -1) {
						point newPoint = findCellOfType(DEAD,i,j);
						newGrid[newPoint.x][newPoint.y].type = PREDATOR;
						newGrid[newPoint.x][newPoint.y].health = grid[i][j].health;
						newGrid[i][j].type = DEAD;
					} else {
						newGrid[i][j].type = grid[i][j].type;
						newGrid[i][j].health = grid[i][j].health;
					}
				}
	
			} else if (grid[i][j].type == PREY) {
				grid[i][j].health++;
				// Try to reproduce
				if (findCellOfType(DEAD, i, j).x != -1){
					if (grid[i][j].health > health_threshold) {
						newGrid[i][j].health = 1;
						point offspring = findCellOfType(DEAD, i, j);
						newGrid[offspring.x][offspring.y].type = PREY;
						newGrid[offspring.x][offspring.y].health = 1;
					} else {
						// Damn this is very very bad.
						// the order you update cells matters
						// and that's lame.
						point newPoint = findCellOfType(DEAD,i,j);
						newGrid[i][j].type = DEAD;
						newGrid[newPoint.x][newPoint.y].type = PREY;
						newGrid[newPoint.x][newPoint.y].health = grid[i][j].health;
					}
				}
			}
		}
	}
	
	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			grid[i][j].type = newGrid[i][j].type;
			grid[i][j].health = newGrid[i][j].health;
		}
	}
}

/*****************
 * Draw the grid *
 *****************/
void drawGrid(void) {
	glBegin(GL_POINTS);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			switch(grid[i][j].type) {
				case DEAD:
					glColor3f(0,0,0);
					break;
				case PREY:
					glColor3f(0,1,0);
					break;
				case PREDATOR:
					glColor3f((1.0f*grid[i][j].health/health_threshold),0,0);
					break;
				default:
					glColor3f(1,1,1);
					break;
			}
			glVertex2i(i*pointSize,j*pointSize);
		}
	}
	glEnd();
}
/********************
 * DISPLAY FUNCTION *
 ********************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawGrid();
	
	glutSwapBuffers();
}

/*************************
 * Update between frames *
 *************************/
void idle(void) {
	Time = clock();
	if (Time > timeToNextFrame) {
		timeToNextFrame = timeToNextFrame + frameRate * CLOCKS_PER_SEC;
		updateGrid();
		glutPostRedisplay();
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(width * pointSize, height * pointSize);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Predator-Prey");
	
	init();
	//initGrid();
	
	glutDisplayFunc (display);
	glutIdleFunc (idle);
	
	glViewport(0,0,(GLsizei) width * pointSize, (GLsizei) height * pointSize);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluOrtho2D(0.0, (GLdouble) width * pointSize, 0.0, (GLdouble) height * pointSize);
	glutMainLoop();
	return 0;
}
