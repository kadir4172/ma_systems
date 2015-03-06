#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int x_coor;
	int y_coor;
	int alive;
	float energy;
} agent_feats;

static int EMPTY = 0;
static int HUNTER = 1;
static int PREY = 2;
static int OBSTACLE = 3;

static int UP = 5;
static int LEFT = 6;
static int DOWN = 7;
static int RIGHT = 8;
static int STAY_STILL = 9;

static int CONT = 10;
static int DISCRETE = 11;

int time = 1;
int mode = 0;

// by input
int n,d;
float e,i,T;
int ** grid_map;
agent_feats * hunters;
agent_feats * preys;

int numbOfHunters = 0;
int numbOfPreys = 0;

void printEnv() {
	int j,k;
	for(j = 0; j < n; j++) {
		for(k = 0; k < n; k++) {
			if(grid_map[j][k] == EMPTY)
				printf("- ",grid_map[j][k]);
			else if(grid_map[j][k] == OBSTACLE)
				printf("X ",grid_map[j][k]);
			else if(grid_map[j][k] == PREY)
				printf("P ",grid_map[j][k]);
			else if(grid_map[j][k] == HUNTER)
				printf("H ",grid_map[j][k]);
		}
		printf("\n");
	}
}

void WaitForEnter()
{
	int c;
	while((c = getchar()) != '\n' && c != EOF)
		;
}

int manDist(agent_feats t1, agent_feats t2) {
	return abs(t1.x_coor - t2.x_coor) + abs(t1.y_coor - t2.y_coor);
}

agent_feats * runPreyPlan() {
	int j,k;
	agent_feats * decisions;
	decisions = malloc(sizeof(agent_feats)*numbOfPreys);

	// main decision loop for each prey agent
	for(j = 0; j < numbOfPreys; j++) {
		printf("------- main decision loop for prey %d,%d --------------------------------------\n",preys[j].x_coor,preys[j].y_coor);
		// find nearest two hunters
		// find first nearest
		int minDist = 2*n+1;
		int minDistHuntInd1 = 0;
		for(k = 0; k < numbOfHunters; k++) {
			int dist = manDist(preys[j],hunters[k]);
			if(dist < minDist) {
				minDist = dist;
				minDistHuntInd1 = k;
			}
		}
		printf("1. nearest hunter: %d %d\n",hunters[minDistHuntInd1].x_coor,hunters[minDistHuntInd1].y_coor);

		// find second nearest
		int minDistHuntInd2 = 0;
		if(numbOfHunters > 1) {
			minDist = 2*n+1;
			for(k = 0; k < numbOfHunters; k++) {
				int dist = manDist(preys[j],hunters[k]);
				if(k != minDistHuntInd1 && dist < minDist) {
					minDist = dist;
					minDistHuntInd2 = k;
				}
			}
		}
		printf("2. nearest hunter: %d %d\n",hunters[minDistHuntInd2].x_coor,hunters[minDistHuntInd2].y_coor);

		// for 4 possible moves, check the maximazing the displacement
		agent_feats possibleMove;
		int distChoice1,distChoice2,distChoice3,distChoice4,distChoice5;

		// up
		if(preys[j].x_coor-1 >= 0 && grid_map[preys[j].x_coor-1][preys[j].y_coor] != OBSTACLE && grid_map[preys[j].x_coor-1][preys[j].y_coor] != HUNTER
													&& grid_map[preys[j].x_coor-1][preys[j].y_coor] != PREY) {
			possibleMove.x_coor = preys[j].x_coor-1;
			possibleMove.y_coor = preys[j].y_coor;
			if(numbOfHunters > 1)
				distChoice1 = manDist(possibleMove,hunters[minDistHuntInd1]) +
								manDist(possibleMove,hunters[minDistHuntInd2]);
			else
				distChoice1 = manDist(possibleMove,hunters[minDistHuntInd1]);
		}
		else // we eliminate this choice
			distChoice1 = -1;
		// left
		if(preys[j].y_coor-1 >= 0 && grid_map[preys[j].x_coor][preys[j].y_coor-1] != OBSTACLE && grid_map[preys[j].x_coor][preys[j].y_coor-1] != HUNTER
													&& grid_map[preys[j].x_coor][preys[j].y_coor-1] != PREY) {
			possibleMove.x_coor = preys[j].x_coor;
			possibleMove.y_coor = preys[j].y_coor-1;
			if(numbOfHunters > 1)
				distChoice2 = manDist(possibleMove,hunters[minDistHuntInd1]) +
								manDist(possibleMove,hunters[minDistHuntInd2]);
			else
				distChoice2 = manDist(possibleMove,hunters[minDistHuntInd1]);
		}
		else  // we eliminate this choice
			distChoice2 = -1;

		// down
		if(preys[j].x_coor+1 < n && grid_map[preys[j].x_coor+1][preys[j].y_coor] != OBSTACLE && grid_map[preys[j].x_coor+1][preys[j].y_coor] != HUNTER
													&& grid_map[preys[j].x_coor+1][preys[j].y_coor] != PREY) {
			possibleMove.x_coor = preys[j].x_coor+1;
			possibleMove.y_coor = preys[j].y_coor;
			if(numbOfHunters > 1)
				distChoice3 = manDist(possibleMove,hunters[minDistHuntInd1]) +
								manDist(possibleMove,hunters[minDistHuntInd2]);
			else
				distChoice3 = manDist(possibleMove,hunters[minDistHuntInd1]);
		}
		else // we eliminate this choice
			distChoice3 = -1;

		// right
		if(preys[j].y_coor+1 < n && grid_map[preys[j].x_coor][preys[j].y_coor+1] != OBSTACLE && grid_map[preys[j].x_coor][preys[j].y_coor+1] != HUNTER
													&& grid_map[preys[j].x_coor][preys[j].y_coor+1] != PREY) {
			possibleMove.x_coor = preys[j].x_coor;
			possibleMove.y_coor = preys[j].y_coor+1;
			if(numbOfHunters > 1)
				distChoice4 = manDist(possibleMove,hunters[minDistHuntInd1]) +
								manDist(possibleMove,hunters[minDistHuntInd2]);
			else
				distChoice4 = manDist(possibleMove,hunters[minDistHuntInd1]);
		}
		else // we eliminate this choice
			distChoice4 = -1;

		// stay still
		possibleMove.x_coor = preys[j].x_coor;
		possibleMove.y_coor = preys[j].y_coor;
		if(numbOfHunters > 1)
			distChoice5 = manDist(possibleMove,hunters[minDistHuntInd1]) +
							manDist(possibleMove,hunters[minDistHuntInd2]);
		else
			distChoice5 = manDist(possibleMove,hunters[minDistHuntInd1]);

		if(distChoice1 >= distChoice2 && distChoice1 >= distChoice3 && distChoice1 >= distChoice4 && distChoice1 >= distChoice5) { // up
			decisions[j].x_coor = preys[j].x_coor-1;
			decisions[j].y_coor = preys[j].y_coor;
		}
		else if(distChoice2 >= distChoice1 && distChoice2 >= distChoice3 && distChoice2 >= distChoice4 && distChoice2 >= distChoice5) { // left
			decisions[j].x_coor = preys[j].x_coor;
			decisions[j].y_coor = preys[j].y_coor-1;
		}
		else if(distChoice3 >= distChoice1 && distChoice3 >= distChoice2 && distChoice3 >= distChoice4 && distChoice3 >= distChoice5) { // down
			decisions[j].x_coor = preys[j].x_coor+1;
			decisions[j].y_coor = preys[j].y_coor;
		}
		else if(distChoice4 >= distChoice1 && distChoice4 >= distChoice2 && distChoice4 >= distChoice3 && distChoice4 >= distChoice5) { // right
			decisions[j].x_coor = preys[j].x_coor;
			decisions[j].y_coor = preys[j].y_coor+1;
		}
		else if(distChoice5 >= distChoice1 && distChoice5 >= distChoice2 && distChoice5 >= distChoice3 && distChoice5 >= distChoice4) { // stay still
			decisions[j].x_coor = preys[j].x_coor;
			decisions[j].y_coor = preys[j].y_coor;
		}
		else
				printf("ERROR in possible move selection of PREYs !!");
	}

	return decisions;
}

// for a hunter and a prey finds farest move for hunter to move away from the prey and applies the decision
void findFarestMove(agent_feats hunter, agent_feats prey, agent_feats * decisions, int ind) {

	// for 4 possible moves, check the move that make the hunter nearest to the prey
	agent_feats possibleMove;
	int distChoice1,distChoice2,distChoice3,distChoice4,distChoice5;

	// up
	if(hunter.x_coor-1 >= 0 && grid_map[hunter.x_coor-1][hunter.y_coor] != OBSTACLE && grid_map[hunter.x_coor-1][hunter.y_coor] != HUNTER
													&& grid_map[hunter.x_coor-1][hunter.y_coor] != PREY) {
		possibleMove.x_coor = hunter.x_coor-1;
		possibleMove.y_coor = hunter.y_coor;
		distChoice1 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice1 = -1;

	// left
	if(hunter.y_coor-1 >= 0 && grid_map[hunter.x_coor][hunter.y_coor-1] != OBSTACLE && grid_map[hunter.x_coor][hunter.y_coor-1] != HUNTER
													&& grid_map[hunter.x_coor][hunter.y_coor-1] != PREY) {
		possibleMove.x_coor = hunter.x_coor;
		possibleMove.y_coor = hunter.y_coor-1;
		distChoice2 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice2 = -1;


	// down
	if(hunter.x_coor+1 < n && grid_map[hunter.x_coor+1][hunter.y_coor] != OBSTACLE && grid_map[hunter.x_coor+1][hunter.y_coor] != HUNTER
													&& grid_map[hunter.x_coor+1][hunter.y_coor] != PREY) {
		possibleMove.x_coor = hunter.x_coor+1;
		possibleMove.y_coor = hunter.y_coor;
		distChoice3 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice3 = -1;

	// right
	if(hunter.y_coor+1 < n && grid_map[hunter.x_coor][hunter.y_coor+1] != OBSTACLE && grid_map[hunter.x_coor][hunter.y_coor+1] != HUNTER
													&& grid_map[hunter.x_coor][hunter.y_coor+1] != PREY) {
		possibleMove.x_coor = hunter.x_coor;
		possibleMove.y_coor = hunter.y_coor+1;
		distChoice4 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice4 = -1;

	// stay still
	possibleMove.x_coor = hunter.x_coor;
	possibleMove.y_coor = hunter.y_coor;
	distChoice5 = manDist(possibleMove,prey);

	if(distChoice1 >= distChoice2 && distChoice1 >= distChoice3 && distChoice1 >= distChoice4 && distChoice1 >= distChoice5) { // up
		//printf("---- decision is up\n");
		decisions[ind].x_coor = hunter.x_coor-1;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else if(distChoice2 >= distChoice1 && distChoice2 >= distChoice3 && distChoice2 >= distChoice4 && distChoice2 >= distChoice5) { // left
		//printf("---- decision is left\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor-1;
	}
	else if(distChoice3 >= distChoice1 && distChoice3 >= distChoice2 && distChoice3 >= distChoice4 && distChoice3 >= distChoice5) { // down
		//printf("---- decision is down\n");
		decisions[ind].x_coor = hunter.x_coor+1;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else if(distChoice4 >= distChoice1 && distChoice4 >= distChoice2 && distChoice4 >= distChoice3 && distChoice4 >= distChoice5) { // right
		//printf("---- decision is right\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor+1;
	}
	else if(distChoice5 >= distChoice1 && distChoice5 >= distChoice2 && distChoice5 >= distChoice3 && distChoice5 >= distChoice4) { // stay still
		//printf("---- decision is right\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else
			printf("ERROR1 in possible move selection of HUNTERs !!");

	return;
}

// for a hunter and a prey finds nearest move for hunter to get closer to the prey and applies the decision
void findNearestMove(agent_feats hunter, agent_feats prey, agent_feats * decisions, int ind) {
	int j,k;
	// for 4 possible moves, check the move that make the hunter nearest to the prey
	agent_feats possibleMove;
	int distChoice1,distChoice2,distChoice3,distChoice4,distChoice5;

	// up
	if(hunter.x_coor-1 >= 0 && grid_map[hunter.x_coor-1][hunter.y_coor] != OBSTACLE && grid_map[hunter.x_coor-1][hunter.y_coor] != HUNTER
													&& grid_map[hunter.x_coor-1][hunter.y_coor] != PREY) {
		possibleMove.x_coor = hunter.x_coor-1;
		possibleMove.y_coor = hunter.y_coor;
		distChoice1 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice1 = 2*n+1;

	// left
	if(hunter.y_coor-1 >= 0 && grid_map[hunter.x_coor][hunter.y_coor-1] != OBSTACLE && grid_map[hunter.x_coor][hunter.y_coor-1] != HUNTER
													&& grid_map[hunter.x_coor][hunter.y_coor-1] != PREY) {
		possibleMove.x_coor = hunter.x_coor;
		possibleMove.y_coor = hunter.y_coor-1;
		distChoice2 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice2 = 2*n+1;


	// down
	if(hunter.x_coor+1 < n && grid_map[hunter.x_coor+1][hunter.y_coor] != OBSTACLE && grid_map[hunter.x_coor+1][hunter.y_coor] != HUNTER
													&& grid_map[hunter.x_coor+1][hunter.y_coor] != PREY) {
		possibleMove.x_coor = hunter.x_coor+1;
		possibleMove.y_coor = hunter.y_coor;
		distChoice3 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice3 = 2*n+1;

	// right
	if(hunter.y_coor+1 < n && grid_map[hunter.x_coor][hunter.y_coor+1] != OBSTACLE && grid_map[hunter.x_coor][hunter.y_coor+1] != HUNTER
													&& grid_map[hunter.x_coor][hunter.y_coor+1] != PREY) {
		possibleMove.x_coor = hunter.x_coor;
		possibleMove.y_coor = hunter.y_coor+1;
		distChoice4 = manDist(possibleMove,prey);
	}
	else // we eliminate this choice
		distChoice4 = 2*n+1;

	// stay still
	possibleMove.x_coor = hunter.x_coor;
	possibleMove.y_coor = hunter.y_coor;
	distChoice5 = manDist(possibleMove,prey);

	if(distChoice1 <= distChoice2 && distChoice1 <= distChoice3 && distChoice1 <= distChoice4 && distChoice1 <= distChoice5) { // up
		//printf("---- decision is up\n");
		decisions[ind].x_coor = hunter.x_coor-1;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else if(distChoice2 <= distChoice1 && distChoice2 <= distChoice3 && distChoice2 <= distChoice4 && distChoice2 <= distChoice5) { // left
		//printf("---- decision is left\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor-1;
	}
	else if(distChoice3 <= distChoice1 && distChoice3 <= distChoice2 && distChoice3 <= distChoice4 && distChoice3 <= distChoice5) { // down
		//printf("---- decision is down\n");
		decisions[ind].x_coor = hunter.x_coor+1;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else if(distChoice4 <= distChoice1 && distChoice4 <= distChoice2 && distChoice4 <= distChoice3 && distChoice4 <= distChoice5) { // right
		//printf("---- decision is right\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor+1;
	}
	else if(distChoice5 <= distChoice1 && distChoice5 <= distChoice2 && distChoice5 <= distChoice5 && distChoice5 <= distChoice4) { // stay still
		//printf("---- decision is right\n");
		decisions[ind].x_coor = hunter.x_coor;
		decisions[ind].y_coor = hunter.y_coor;
	}
	else
			printf("ERROR1 in possible move selection of HUNTERs !!");

	return;
}

agent_feats * removePreysOrHunters(int type) {
	int j,k;
	int deads = 0;
	if(type == PREY) {
		for(j = 0; j < numbOfPreys; j++) {
			if(!preys[j].alive)
				deads++;
		}
		agent_feats temp[numbOfPreys-deads];
		for(j = 0, k = 0; j < numbOfPreys; j++) {
			if(preys[j].alive) {
				temp[k].x_coor = preys[j].x_coor;
				temp[k].y_coor = preys[j].y_coor;
				temp[k].alive = preys[j].alive;
				temp[k].energy = preys[j].energy;
				k++;
			}
			else
				grid_map[preys[j].x_coor][preys[j].y_coor] = EMPTY;
		}
		free(preys);
		numbOfPreys -= deads;

		preys = malloc(sizeof(agent_feats)*numbOfPreys);

		for(j = 0; j < numbOfPreys; j++) {
			preys[j].x_coor = temp[j].x_coor;
			preys[j].y_coor = temp[j].y_coor;
			preys[j].alive = temp[j].alive;
			preys[j].energy = temp[j].energy;
		}
		return preys;

	}
	else {
		for(j = 0; j < numbOfHunters; j++)
			if(!hunters[j].alive)
				deads++;

		agent_feats temp[numbOfHunters-deads];
		for(j = 0, k = 0; j < numbOfHunters; j++) {
			if(hunters[j].alive) {
				temp[k].x_coor = hunters[j].x_coor;
				temp[k].y_coor = hunters[j].y_coor;
				temp[k].alive = hunters[j].alive;
				temp[k].energy = hunters[j].energy;
				k++;
			}
			else
				grid_map[hunters[j].x_coor][hunters[j].y_coor] = EMPTY;
		}

		free(hunters);
		numbOfHunters -= deads;

		hunters = malloc(sizeof(agent_feats)*numbOfHunters);

		for(j = 0; j < numbOfHunters; j++) {
			 hunters[j].x_coor = temp[j].x_coor;
			 hunters[j].y_coor = temp[j].y_coor;
			 hunters[j].alive = temp[j].alive;
			 hunters[j].energy = temp[j].energy;
		}
		return hunters;
	}

}

agent_feats * runHunterPlan() {
	int j,k,l;
	agent_feats * decisions = malloc(sizeof(agent_feats)*numbOfHunters);

	/* the second best decisions are removed due to memoryless constraint in reactive planning
	// second best decision is the 2nd best action decision that will be used in collision detection
	// agent_feats * secondBestDecisions = malloc(sizeof(agent_feats)*numbOfHunters);
	for(j = 0; j < numbOfHunters; j++)
		secondBestDecisions[j].x_coor = -1;	// -1 means this agent has decided its action certainly and does not have second decision
	*/

	// first, if there are adj/same cell preys eat them
	// keep the eater hunters and eaten preys, -1 means hunter j eat nothing
	printf("check for eaten preys\n");
	int eaters[numbOfHunters];
	for(j = 0; j < numbOfHunters; j++)
		eaters[j] = -1;

	for(j = 0; j < numbOfHunters; j++) {
		for(k = 0; k < numbOfPreys; k++) {
			int dist = manDist(hunters[j],preys[k]);
			if(dist <= 1) {
				eaters[j] = k;
				break;
			}
		}
	}

	// for each eaten kth prey, increment the eaters energy accordingly
	int anyEaten = 0;
	for(k = 0; k < numbOfPreys; k++) {
		int numbOfEaters = 0;
		for(j = 0; j < numbOfHunters; j++) {
			if(eaters[j] == k) {
				numbOfEaters++;
			}
		}

		// set the eaten prey dead
		if(numbOfEaters > 0) {
			printf("the prey at %d,%d has been eaten by %d number of hunters\n",preys[k].x_coor,preys[k].y_coor,numbOfEaters);
			preys[k].alive = 0;
			anyEaten = 1;
		}

		// increment the energy
		for(j = 0; j < numbOfHunters; j++) {
			if(eaters[j] == k) {
				hunters[j].energy += (i/numbOfEaters);
				printf("the hunter at %d,%d has eaten the prey at %d,%d and its energy is incremented by %f, becomes %f\n"
										,hunters[j].x_coor,hunters[j].y_coor,preys[k].x_coor,preys[k].y_coor,i/numbOfEaters,hunters[j].energy);
			}
		}
	}
	// remove the eaten preys from the space
	if(anyEaten)
		preys = removePreysOrHunters(PREY);
	else
		printf("there is no eaten prey in this time step\n");
	printf("the number of remaining preys is %d\n",numbOfPreys);

	// check for no energy hunters
	printf("check for dead hunters\n");
	int anyDead = 0;
	for(j = 0; j < numbOfHunters; j++) {
		if(hunters[j].energy < T) {
			printf("the hunter at %d,%d has energy below the threshold, %f, so it will be removed from the environment\n"
																				,hunters[j].x_coor,hunters[j].y_coor,hunters[j].energy);
			hunters[j].alive = 0;
			anyDead = 1;
		}
	}
	if(anyDead)
		hunters = removePreysOrHunters(HUNTER);
	else
		printf("there is no dead hunter in this time step\n");
	printf("the number of remaining hunters is %d\n",numbOfHunters);

	// main decision loop, for each hunter
	for(j = 0; j < numbOfHunters; j++) {
		printf("------- main decision loop for hunter %d,%d  -----------------------------------\n",hunters[j].x_coor,hunters[j].y_coor);
		// if no energy to move, just stay still
		if(hunters[j].energy <= T+1) {
			decisions[j].x_coor = hunters[j].x_coor;
			decisions[j].y_coor = hunters[j].y_coor;
			printf("the hunter at %d,%d does not have enough energy to move, %f, so stays still\n"
														,hunters[j].x_coor,hunters[j].y_coor,hunters[j].energy);
			continue;
		}

		// check the preys in the observable area
		int preyDists[numbOfPreys];
		for(k = 0; k < numbOfPreys; k++)
			preyDists[k] = 2*n+1;

		int numbOfFoundPreys = 0;
		for(k = 0; k < numbOfPreys; k++) {
			if(preys[k].x_coor <= hunters[j].x_coor+d && preys[k].x_coor >= hunters[j].x_coor-d &&
							preys[k].y_coor <= hunters[j].y_coor+d && preys[k].y_coor >= hunters[j].y_coor-d) {
				preyDists[k] = manDist(preys[k],hunters[j]);
				numbOfFoundPreys++;
			}
		}

		if(numbOfFoundPreys > 0) {
			printf("the hunter at %d,%d sees %d many preys in its observable area, and will try to get close to the nearest one\n"
																				,hunters[j].x_coor,hunters[j].y_coor,numbOfFoundPreys);
			// find the nearest prey in the observable area
			int minDist = preyDists[0];
			int ind = 0;
			for(k = 0; k < numbOfPreys; k++) {
				if(preyDists[k] < minDist) {
					ind = k;
					minDist = preyDists[k];
				}
			}
			printf("the closest prey is at %d,%d, the hunter at %d,%d will try to get close to it\n"
																	,preys[ind].x_coor,preys[ind].y_coor,hunters[j].x_coor,hunters[j].y_coor);
			// find the action that make the hunter closer to the prey,
			// same as the prey displacement maximization strategy, but minimization.
			findNearestMove(hunters[j],preys[ind],decisions,j);
		}
		else {
			// if there is no prey in the observable area, and energy is above T+2, move to nearest hunter in the observable area
			// if energy is less than or equal to T+2, stay still
			if(hunters[j].energy <= T+2) {
				decisions[j].x_coor = hunters[j].x_coor;
				decisions[j].y_coor = hunters[j].y_coor;
				printf("the hunter at %d,%d does not see any preys in its observable area and does not have enough energy to move, %f, so stays still\n",hunters[j].x_coor,hunters[j].y_coor,hunters[j].energy);
			}
			else {
				printf("the hunter at %d,%d does not see any preys in its observable area, it will check the number of hunters in its observable area\n",hunters[j].x_coor,hunters[j].y_coor);
				// find the nearest hunter in the observable area.
				int hunterDists[numbOfHunters];
				for(k = 0; k < numbOfHunters; k++)
					hunterDists[k] = 2*n+1;

				int numbOfFoundHunters = 0;
				for(k = 0; k < numbOfHunters; k++) {
					if(k != j)
						if(hunters[k].x_coor <= hunters[j].x_coor+d && hunters[k].x_coor >= hunters[j].x_coor-d &&
										hunters[k].y_coor <= hunters[j].y_coor+d && hunters[k].y_coor >= hunters[j].y_coor-d) {
							hunterDists[k] = manDist(hunters[k],hunters[j]);
							numbOfFoundHunters++;
						}
				}

				int flipcoin = rand() % 3;
				if(numbOfFoundHunters > 0 && numbOfFoundHunters < d && flipcoin == 0) {
					// if there are less than d-many hunters in its observable area,
  					// get close to the nearest one --> attractive mode.
					printf("the hunter at %d,%d sees %d many hunters in its observable area, since %d is below the threshold %d, it will try to get close to the nearest one\n",hunters[j].x_coor,hunters[j].y_coor,numbOfFoundHunters,numbOfFoundHunters,d);
					int minDist = hunterDists[0];
					int ind = 0;
					for(k = 0; k < numbOfHunters; k++) {
						if(hunterDists[k] < minDist) {
							ind = k;
							minDist = hunterDists[k];
						}
					}
					findNearestMove(hunters[j],hunters[ind],decisions,j);
				}
				else if(numbOfFoundHunters > 0) {
					// if there are greater than or equal to d-many hunters in its observable area,
					// go further from the nearest one --> repulsive mode. Repulsive mode is further encouraged.
					printf("the hunter at %d,%d sees %d many hunters in its observable area, since it is above the threshold %d, it will try to move way from the nearest one\n",hunters[j].x_coor,hunters[j].y_coor,numbOfFoundHunters,d);
					int minDist = hunterDists[0];
					int ind = 0;
					for(k = 0; k < numbOfHunters; k++) {
						if(hunterDists[k] < minDist) {
							ind = k;
							minDist = hunterDists[k];
						}
					}
					findFarestMove(hunters[j],hunters[ind],decisions,j);
				}
				else {
					// if there isn't any hunter in the observable area for hunters[j] (and energy is already above T+2),
					// random move.
					printf("the hunter at %d,%d does not see any hunters also in it observable area, so it will move randomly\n"
																								,hunters[j].x_coor,hunters[j].y_coor);

					int act = rand() % 5;
					if(act == 0) {	// up
						if(hunters[j].x_coor-1 >= 0)
							decisions[j].x_coor = hunters[j].x_coor-1;
						else
							decisions[j].x_coor = hunters[j].x_coor+1;
						decisions[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 1) { // left
						decisions[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decisions[j].y_coor = hunters[j].y_coor-1;
						else
							decisions[j].y_coor = hunters[j].y_coor+1;
					}
					else if(act == 2) {	// down
						if(hunters[j].x_coor+1 < n)
							decisions[j].x_coor = hunters[j].x_coor+1;
						else
							decisions[j].x_coor = hunters[j].x_coor-1;
						decisions[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 3) {	// right
						decisions[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decisions[j].y_coor = hunters[j].y_coor+1;
						else
							decisions[j].y_coor = hunters[j].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisions[j].x_coor = hunters[j].x_coor;
						decisions[j].y_coor = hunters[j].y_coor;
					}
				}
				// NOTE: for the above if-else statement
				// d is chosen as a threshold, it seems a reasonable number not to get catch up
				// complete accumulation or complete dispersion
			}
		}
	}

	return decisions;
}

void runReactiveMultiAgentPlan() {
	int j,k;
	while(numbOfHunters > 0 && numbOfPreys > 0) {
		// each decides what to do first, then it is applied together.
		// as a special condition eating a prey will be done instantenously.
		agent_feats * decisionHunters = runHunterPlan(); // hunters decisions changes
																	   			// the environment by removing preys.
		agent_feats * decisionPreys = runPreyPlan();

		// apply decisions for hunters

		printf("HUNTER DECISIONS\n");
		if(numbOfHunters <= 0)
			printf("There is no remaining hunter so the plan will end at the next time step\n");

		// collision cases
		for(j = 0; j < numbOfHunters; j++) {
			for(k = 0; k < numbOfHunters; k++) {
				if(j != k && decisionHunters[j].x_coor == decisionHunters[k].x_coor && decisionHunters[j].y_coor == decisionHunters[k].y_coor) {
					printf("COLLISION: the hunter at %d,%d has collided with the hunter at %d,%d while moving to %d,%d. their energy is decremented by 1, becomes %f and they will spill out randomly\n",hunters[j].x_coor,hunters[j].y_coor,hunters[k].x_coor,hunters[k].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor,hunters[j].energy);
					// after a collision it moves randomly, note that it looses energy both for collision and random move
					int act = rand() % 5;
					if(act == 0) {	// up
						if(hunters[j].x_coor-1 >= 0)
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 1) { // left
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
					}
					else if(act == 2) {	// down
						if(hunters[j].x_coor+1 < n)
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 3) {	// right
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionHunters[j].x_coor = hunters[j].x_coor;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					// not to collide again
					int prevact = act;
					act = rand() % 5;
					if(act == prevact && act < 4)
						act += 1;
					else if(act == prevact && act == 4)
						act -= 1;

					if(act == 0) {	// up
						if(hunters[k].x_coor-1 >= 0)
							decisionHunters[k].x_coor = hunters[k].x_coor-1;
						else
							decisionHunters[k].x_coor = hunters[k].x_coor+1;
						decisionHunters[k].y_coor = hunters[k].y_coor;
					}
					else if(act == 1) { // left
						decisionHunters[k].x_coor = hunters[k].x_coor;
						if(hunters[k].y_coor-1 >= 0)
							decisionHunters[k].y_coor = hunters[k].y_coor-1;
						else
							decisionHunters[k].y_coor = hunters[k].y_coor+1;
					}
					else if(act == 2) {	// down
						if(hunters[k].x_coor+1 < n)
							decisionHunters[k].x_coor = hunters[k].x_coor+1;
						else
							decisionHunters[k].x_coor = hunters[k].x_coor-1;
						decisionHunters[k].y_coor = hunters[k].y_coor;
					}
					else if(act == 3) {	// right
						decisionHunters[k].x_coor = hunters[k].x_coor;
						if(hunters[k].y_coor+1 < n)
							decisionHunters[k].y_coor = hunters[k].y_coor+1;
						else
							decisionHunters[k].y_coor = hunters[k].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionHunters[k].x_coor = hunters[k].x_coor;
						decisionHunters[k].y_coor = hunters[k].y_coor;
					}

					hunters[j].energy -= 1;
					hunters[k].energy -= 1;
					break;
				}
			}
			if(grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] == OBSTACLE) {
				printf("COLLISION: the hunter at %d,%d has collided with the obstacle at %d,%d. their energy is decremented by 1, becomes %f, and it will spill out randomly\n",hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor,hunters[j].energy);
					int act = rand() % 5;
					if(act == 0) {	// up
						if(hunters[j].x_coor-1 >= 0)
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 1) { // left
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
					}
					else if(act == 2) {	// down
						if(hunters[j].x_coor+1 < n)
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(act == 3) {	// right
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionHunters[j].x_coor = hunters[j].x_coor;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}

					if(grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] == OBSTACLE) { // if still an obstacle just stay still
							decisionHunters[j].x_coor = hunters[j].x_coor;
							decisionHunters[j].y_coor = hunters[j].y_coor;
					}

					hunters[j].energy -= 1;	// in collision case, they do not move to anywhere, just looses energy
			}
			// decrement the energy levels accordingly
			if(hunters[j].x_coor == decisionHunters[j].x_coor && hunters[j].y_coor == decisionHunters[j].y_coor) {
				printf("STAY_STILL: the hunter at %d,%d moves to the position %d,%d, STAY_STILL, and "
											,hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor);
				hunters[j].energy -= 0.2;
				printf("its energy is decremented as 0.2, becomes %f\n",hunters[j].energy);
			}
			else if(hunters[j].x_coor-1 == decisionHunters[j].x_coor && hunters[j].y_coor == decisionHunters[j].y_coor) {
				printf("UP: the hunter at %d,%d moves to the position %d,%d, and "
											,hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor);
				hunters[j].energy -= 1.0;
				printf("its energy is decremented as 1.0, becomes %f\n",hunters[j].energy);
			}
			else if(hunters[j].x_coor+1 == decisionHunters[j].x_coor && hunters[j].y_coor == decisionHunters[j].y_coor) {
				printf("DOWN: the hunter at %d,%d moves to the position %d,%d, and "
											,hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor);
				hunters[j].energy -= 1.0;
				printf("its energy is decremented as 1.0, becomes %f\n",hunters[j].energy);
			}
			else if(hunters[j].x_coor == decisionHunters[j].x_coor && hunters[j].y_coor-1 == decisionHunters[j].y_coor) {
				printf("LEFT: the hunter at %d,%d moves to the position %d,%d, and "
											,hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor);
				hunters[j].energy -= 1.0;
				printf("its energy is decremented as 1.0, becomes %f\n",hunters[j].energy);
			}
			else if(hunters[j].x_coor == decisionHunters[j].x_coor && hunters[j].y_coor+1 == decisionHunters[j].y_coor) {
				printf("RIGHT: the hunter at %d,%d moves to the position %d,%d, and "
											,hunters[j].x_coor,hunters[j].y_coor,decisionHunters[j].x_coor,decisionHunters[j].y_coor);
				hunters[j].energy -= 1.0;
				printf("its energy is decremented as 1.0, becomes %f\n",hunters[j].energy);
			}
			grid_map[hunters[j].x_coor][hunters[j].y_coor] = EMPTY;
			grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] = HUNTER;
			hunters[j].x_coor = decisionHunters[j].x_coor;
			hunters[j].y_coor = decisionHunters[j].y_coor;
		}

		// apply decisions for preys

		printf("PREY DECISIONS\n");
		// collision cases
		if(numbOfPreys <= 0)
			printf("There is no remaining prey so the plan will end at the next time step\n");
		for(j = 0; j < numbOfPreys; j++) {
			for(k = 0; k < numbOfPreys; k++) {
				if(j != k && decisionPreys[j].x_coor == decisionPreys[k].x_coor && decisionPreys[j].y_coor == decisionPreys[k].y_coor) {
					printf("COLLISION: the prey at %d,%d has collided with the prey at %d,%d while moving to %d,%d and they will spill out randomly\n"
									,preys[j].x_coor,preys[j].y_coor,preys[k].x_coor,preys[k].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
					// after a collision it moves randomly, note that it looses energy both for collision and random move
					int act = rand() % 5;
					if(act == 0) {	// up
						if(preys[j].x_coor-1 >= 0)
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(act == 1) { // left
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor-1 >= 0)
							decisionPreys[j].y_coor = preys[j].y_coor-1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor+1;
					}
					else if(act == 2) {	// down
						if(preys[j].x_coor+1 < n)
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(act == 3) {	// right
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor+1 < n)
							decisionPreys[j].y_coor = preys[j].y_coor+1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionPreys[j].x_coor = preys[j].x_coor;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}

					// not to collide again
					int prevact = act;
					act = rand() % 5;
					if(act == prevact && act < 4)
						act += 1;
					else if(act == prevact && act == 4)
						act -= 1;

					if(act == 0) {	// up
						if(preys[k].x_coor-1 >= 0)
							decisionPreys[k].x_coor = preys[k].x_coor-1;
						else
							decisionPreys[k].x_coor = preys[k].x_coor+1;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					else if(act == 1) { // left
						decisionPreys[k].x_coor = preys[k].x_coor;
						if(preys[k].y_coor-1 >= 0)
							decisionPreys[k].y_coor = preys[k].y_coor-1;
						else
							decisionPreys[k].y_coor = preys[k].y_coor+1;
					}
					else if(act == 2) {	// down
						if(preys[k].x_coor+1 < n)
							decisionPreys[k].x_coor = preys[k].x_coor+1;
						else
							decisionPreys[k].x_coor = preys[k].x_coor-1;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					else if(act == 3) {	// right
						decisionPreys[k].x_coor = preys[k].x_coor;
						if(preys[k].y_coor+1 < n)
							decisionPreys[k].y_coor = preys[k].y_coor+1;
						else
							decisionPreys[k].y_coor = preys[k].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionPreys[k].x_coor = preys[k].x_coor;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					break;
				}
			}
			if(grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] == OBSTACLE) {
				printf("COLLISION: the prey at %d,%d has collided with the obstacle at %d,%d and it will spill out randomly\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
					int act = rand() % 5;
					if(act == 0) {	// up
						if(preys[j].x_coor-1 >= 0)
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(act == 1) { // left
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor-1 >= 0)
							decisionPreys[j].y_coor = preys[j].y_coor-1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor+1;
					}
					else if(act == 2) {	// down
						if(preys[j].x_coor+1 < n)
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(act == 3) {	// right
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor+1 < n)
							decisionPreys[j].y_coor = preys[j].y_coor+1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor-1;
					}
					else if(act == 4) {	// stay still
						decisionPreys[j].x_coor = preys[j].x_coor;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}

					if(grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] == OBSTACLE) { // if still an obstacle just stay still
							decisionPreys[j].x_coor = preys[j].x_coor;
							decisionPreys[j].y_coor = preys[j].y_coor;
					}
			}

			if(preys[j].x_coor == decisionPreys[j].x_coor && preys[j].y_coor == decisionPreys[j].y_coor) {
				printf("STAY_STILL: the prey at %d,%d moves to the position %d,%d\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
			}
			else if(preys[j].x_coor-1 == decisionPreys[j].x_coor && preys[j].y_coor == decisionPreys[j].y_coor) {
				printf("UP: the prey at %d,%d moves to the position %d,%d\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
			}
			else if(preys[j].x_coor+1 == decisionPreys[j].x_coor && preys[j].y_coor == decisionPreys[j].y_coor) {
				printf("DOWN: the prey at %d,%d moves to the position %d,%d\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
			}
			else if(preys[j].x_coor == decisionPreys[j].x_coor && preys[j].y_coor-1 == decisionPreys[j].y_coor) {
				printf("LEFT: the prey at %d,%d moves to the position %d,%d\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
			}
			else if(preys[j].x_coor == decisionPreys[j].x_coor && preys[j].y_coor+1 == decisionPreys[j].y_coor) {
				printf("RIGHT: the prey at %d,%d moves to the position %d,%d\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
			}
			grid_map[preys[j].x_coor][preys[j].y_coor] = EMPTY;
			grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] = PREY;
			preys[j].x_coor = decisionPreys[j].x_coor;
			preys[j].y_coor = decisionPreys[j].y_coor;
		}

		printEnv();
		printf("%d. time step is finished, press enter to continue...\n",time);
		time++;
		if(mode == DISCRETE)
			WaitForEnter();
	}

	return;
}

int main(void) {
	FILE * input_file = fopen("environment.inp","r");
	int j,k;


	fscanf(input_file,"%d %d %f %f %f",&n,&d,&e,&i,&T);  //girdileri okuyalim

	fgetc(input_file);                  //yeni satiri okuyalim


	grid_map = malloc(sizeof(int *)*n);
	for(j = 0; j < n; j++)
		grid_map[j] = malloc(sizeof(int)*n);

	for(j = 0; j < n; j++)
		for(k = 0; k < n; k++)
			grid_map[j][k] = EMPTY;

	// place hunter, prey and obstacles from input variables
	char object;
	int x_coor, y_coor;
	while((fscanf(input_file,"%c %d %d", &object, &x_coor, &y_coor)) != EOF) {
		if(object == '\n') {
			fgetc(input_file);
			continue;
		}
		fgetc(input_file);
		if(object == 'h') {
			grid_map[x_coor-1][y_coor-1] = HUNTER;
			numbOfHunters++;
		}
		else if(object == 'p') {
			grid_map[x_coor-1][y_coor-1] = PREY;
			numbOfPreys++;
		}
		else if(object == 'o')
			grid_map[x_coor-1][y_coor-1] = OBSTACLE;
		else
			printf("ERROR while reading objects from input, obstacle: ----%c----\n",object);
	}
	// save hunters and preys in a separate array
	hunters = malloc(sizeof(agent_feats)*numbOfHunters);
	preys = malloc(sizeof(agent_feats)*numbOfPreys);
	int huntInd = 0;
	int preyInd = 0;
	for(j = 0; j < n; j++) {
		for(k = 0; k < n; k++) {
			if(grid_map[j][k] == HUNTER) {
				hunters[huntInd].x_coor = j;
				hunters[huntInd].y_coor = k;
				hunters[huntInd].alive = 1;
				hunters[huntInd].energy = e;
				huntInd++;
			}
			else if(grid_map[j][k] == PREY) {
				preys[preyInd].x_coor = j;
				preys[preyInd].y_coor = k;
				preys[preyInd].alive = 1;
				preyInd++;
			}
		}
	}
	printf("The system is loaded.\n");
	printf("\n******************************************* Explanation *****************************************************\n");
	printf("Firstly the hunters and preys run their planning algorithms and decides on some moves.\n");
	printf("These intuitions and decisions are logged to the shell screen.\n");
	printf("Then they apply their decisions to the environment together, so that any collision can be handled correctly.\n");
	printf("At the end of each time step, the simulated form of environment matrix is drawn also. This matrix shows the\n");
	printf("environment after applying all changes at that time step.\n\n");
	printf("The name of the input file is always 'environment.inp'. To change this, just go to line #1007.\n");
	printf("*************************************************************************************************************\n\n");
	printf("If you want to use discrete time mode please press 1(suggested)\n");
	printf("If you want to use continuous time mode please press 2\n");

	char c;
	scanf("%c",&c);
	if(c == '1')
		mode = DISCRETE;
	else
		mode = CONT;

	printf("The first form of the environment is as below,\n");
	printf("'-' means empty cell,\n'X' means an obstacle,\n'H' means a hunter,\n'P' means a prey:\n");
	printEnv();
	printf("Press enter to start the plan\n");
	WaitForEnter();	 // first enter corresponds to the scanf
	WaitForEnter();  // this for starting the plan

	printf("****************************************************************************************\n");
	printf("************************** Reactive planning started.  *********************************\n");
	printf("****************************************************************************************\n\n");
	runReactiveMultiAgentPlan();
	printf("****************************************************************************************\n");
	printf("************************** Reactive planning finished. *********************************\n");
	printf("****************************************************************************************\n");

	return 0;
}
