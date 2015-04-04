#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


int ** grid_map;
int size_of_map;
#define ENGEL    3

typedef struct state {
	int pos_x;
	int pos_y;
	int hvalue;
} state;

state * agentStates;
state goalState;

state ** visitedStates;
int* numberOfLocalStatesVisited;

state * visitedStatesG;


int numberOfGlobalStatesVisited;
int numberOfAgents;

int iterationMode;
int iterationNumber = 1;


FILE * out;

clock_t start, end;
double cpu_time_used;
int totalRun;


void print_map_to_file(state s) {

	int i,j;
	for(i = 0; i < size_of_map; i++) {
		for(j = 0; j < size_of_map; j++){
			if(grid_map[i][j] == ENGEL)
			  fprintf(out,"o ");
			else if(s.pos_x == i && s.pos_y == j)
			  fprintf(out,"A ");
		    else
		      fprintf(out,"x ");
		}
		fprintf(out,"\n");
	}
}

void print_map_to_console(state s) {
	int i,j;
	for(i = 0; i < size_of_map; i++) {
		for(j = 0; j < size_of_map; j++){
			if(grid_map[i][j] == ENGEL)
				printf("o ");
			else if(s.pos_x == i && s.pos_y == j)
			  printf("A ");
		    else
		      printf("x ");
		}
		printf("\n");
	}
}

state * expand_current_state(state agentState) {

	state * children = malloc(sizeof(state)*4);

	//ASAGI
	if((agentState.pos_y-1 >= 0)){
	  if(grid_map[agentState.pos_x][agentState.pos_y-1] != ENGEL) {
			children[0] = agentState;
			children[0].pos_x = agentState.pos_x;
		    children[0].pos_y = agentState.pos_y-1;
			}
		else {
			children[0] = agentState;
			children[0].pos_x = -100;
			children[0].pos_y = -100;
		}
	}
	else {
			children[0] = agentState;
			children[0].pos_x = -100;
			children[0].pos_y = -100;
	}

	// YUKARI
	if(agentState.pos_y+1 < size_of_map) {
	  if(grid_map[agentState.pos_x][agentState.pos_y+1] != ENGEL ) {
		    children[1] = agentState;
			children[1].pos_x = agentState.pos_x;
		    children[1].pos_y = agentState.pos_y+1;
		}
	  	  else {
	  		children[1] = agentState;
	  		children[1].pos_x = -100;
	  		children[1].pos_y = -100;
	   }
	 }
	else {
		children[1] = agentState;
		children[1].pos_x = -100;
		children[1].pos_y = -100;
	}

	//SOL
	if((agentState.pos_x-1 >= 0)){
	  if(grid_map[agentState.pos_x-1][agentState.pos_y] != ENGEL ) {

		children[2] = agentState;
		children[2].pos_x = agentState.pos_x -1;
		children[2].pos_y = agentState.pos_y;
	 }
	  else {
		children[2] = agentState;
		children[2].pos_x = -100;
		children[2].pos_y = -100;
         	}
	  }
	   else {
		children[2] = agentState;
		children[2].pos_x = -100;
		children[2].pos_y = -100;
	   }

	//SAG
	if(agentState.pos_x+1 <size_of_map){
	  if(grid_map[agentState.pos_x+1][agentState.pos_y] != ENGEL ) {

	        children[3] = agentState;
		children[3].pos_x = agentState.pos_x +1;
	        children[3].pos_y = agentState.pos_y;


		}

	else {
		children[3] = agentState;
		children[3].pos_x = -100;
		children[3].pos_y = -100;
	}
	}
	else {
		children[3] = agentState;
		children[3].pos_x = -100;
		children[3].pos_y = -100;
	}

	return children;
}

void WaitForEnter() {
	int c;
	while((c = getchar()) != '\n' && c != EOF)
		;
}

int next_to_final_state(state * children) {
	int i;
	int isGoal = 0;
	int id = -1;
	for(i = 0; i < 4; i++) {
		if(children[i].pos_x == goalState.pos_x && children[i].pos_y == goalState.pos_y) {
			isGoal = 1;
		}
		if(isGoal) {
			id = i;
			break;
		}
	}

	return id;
}

int number_of_children(state * children) {
	int i;
	int number = 0;
	for(i = 0; i < 4; i++) {
		if(children[i].pos_x != -100 && children[i].pos_y != -100) {
			number++;
		}
	}

	return number;
}

int isEqual(state s1, state s2) {

	int ret = 0;
	if(s1.pos_x == s2.pos_x && s1.pos_y == s2.pos_y) {
		ret = 1;
	}

	return ret;
}

int visitedStateId = -1;

int isVisited(int agent, state child) {
	int i;

		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			if(isEqual(visitedStates[agent][i],child)) {
				visitedStateId = i;
				return 1;
			}
		}

	return 0;
}

int getLocalHeuristicValue(int agentId, state children) {



		return visitedStates[agentId][visitedStateId].hvalue;

}

int calculateHeuristicValue(state s1, state s2) {
	int hval = abs(s1.pos_x-s2.pos_x) + abs(s1.pos_y-s2.pos_y);
	return hval;
}

int getGlobalHeuristicValue(state s) {
	int i;


	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		if(isEqual(visitedStatesG[i],s)) {
			return visitedStatesG[i].hvalue;
		}
	}

	return calculateHeuristicValue(s,goalState);
	//return 0;
}

void setCurrentState(state * agentState, state s) {

	(*agentState).pos_x = s.pos_x;
	(*agentState).pos_y = s.pos_y;
	(*agentState).hvalue = s.hvalue;


}

int calculateIsolation(int agent, state candidate) {
	int i;
	int min = 10000;
	for(i = 0; i < numberOfAgents; i++) {
		if(i != agent) {
			int temp = calculateHeuristicValue(candidate, agentStates[i]);
			if(temp < min)
				min = temp;
		}
	}

	return min;
}

int getFarthestStateFromCandidates(int agent, state * children, int * candidates, int numbOfCand) {
	int i;
	int max = 0;
	int maxId = 0;
	for(i = 0; i < numbOfCand; i++) {
		int temp = calculateIsolation(agent,children[candidates[i]]);

		if(temp >= max) {
			maxId = candidates[i];
			max = temp;
		}
	}

	return maxId;
}

void updateGlobalHeuristicValue(state s, int hval) {
	int i;


	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		if(isEqual(visitedStatesG[i],s)) {
			visitedStatesG[i].hvalue = hval;
			return;
		}
	}



	state * temp = malloc(sizeof(state)*(numberOfGlobalStatesVisited+1));
	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		temp[i].pos_x = visitedStatesG[i].pos_x;
		temp[i].pos_y = visitedStatesG[i].pos_y;
		temp[i].hvalue = visitedStatesG[i].hvalue;

	}

	s.hvalue = hval;

	temp[i].pos_x = s.pos_x;
	temp[i].pos_y = s.pos_y;
	temp[i].hvalue = s.hvalue;


	free(visitedStatesG);
	numberOfGlobalStatesVisited++;
	visitedStatesG = malloc(sizeof(state)*(numberOfGlobalStatesVisited));
	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		visitedStatesG[i].pos_x = temp[i].pos_x;
		visitedStatesG[i].pos_y = temp[i].pos_y;
		visitedStatesG[i].hvalue = temp[i].hvalue;

	}

	return;
}

void updateLocalHeuristicValue(int agent, state s, int hval) {
	int i;

		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			if(isEqual(visitedStates[agent][i],s)) {
				visitedStates[agent][i].hvalue = hval;
				return;
			}
		}

		//printf("not visited before will be added to the visited ones\n");

		state * temp = malloc(sizeof(state)*(numberOfLocalStatesVisited[agent]+1));
		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			temp[i].pos_x = visitedStates[agent][i].pos_x;
			temp[i].pos_y = visitedStates[agent][i].pos_y;
			temp[i].hvalue = visitedStates[agent][i].hvalue;

		}

		s.hvalue = hval;

		temp[i].pos_x = s.pos_x;
		temp[i].pos_y = s.pos_y;
		temp[i].hvalue = s.hvalue;


		free(visitedStates[agent]);
		numberOfLocalStatesVisited[agent]++;
		visitedStates[agent] = malloc(sizeof(state)*(numberOfLocalStatesVisited[agent]));
		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			visitedStates[agent][i].pos_x = temp[i].pos_x;
			visitedStates[agent][i].pos_y = temp[i].pos_y;
			visitedStates[agent][i].hvalue = temp[i].hvalue;


		}



	return;
}


void runMARTA() {
	int i,j;
	state * children;

	int agentId;
	while(1) {
		// each loop is one episod for all agents
		agentId = 0;
		state decidedStates[numberOfAgents];

		int globalUpdateValues[numberOfAgents];
		int localUpdateValues[numberOfAgents];

		while(agentId < numberOfAgents) {

			// expand the current state
			children = expand_current_state(agentStates[agentId]);

			int len = number_of_children(children);
			printf("%d. agent has %d children, ",agentId+1,len);
			// check if it is the goal state, if yes terminate
			int goalId = next_to_final_state(children);

			if(goalId != -1) {
				setCurrentState(agentStates + agentId,children[goalId]);
				printf("and has the final state in one of its children\nThe search will be ended.\n");
				printf("Agent%d:",agentId+1);
				fprintf(out,"Agent%d:",agentId+1);
				if(goalId == 0) {
					printf("Left\n");
					fprintf(out,"Left\n");
				}
				else if(goalId == 1) {
					printf("Right\n");
					fprintf(out,"Right\n");
				}
				else if(goalId == 2) {
					printf("Up\n");
					fprintf(out,"Up\n");
				}
				else if(goalId == 3) {
					printf("Down\n");
					fprintf(out,"Down\n");
				}
				else
					printf("ERROR in decided action selection !\n");
				printf("State(Final):\n");
				print_map_to_console(agentStates[agentId]);
				fprintf(out,"State(Final):\n");
				print_map_to_file(agentStates[agentId]);
				printf("Total number of moves at this run(including this): %d\n",iterationNumber);
				fprintf(out,"Total number of moves at this run(including this): %d\n",iterationNumber);
				printf("***********************************************************\n");
				printf("Episod #%d is over for all agents\n",iterationNumber);
				printf("***********************************************************\n\n");
				return;
			}
			else {
				printf("and none of them is goal\n");
				// lookahead search, depth is 1
				int f;
				int fValues[4];
				int minChildId = -1;
				int min = 10000;			// means inifite

				for(i = 0,j = 0; i < 4; i++) {
					if(children[i].pos_x >= 0 && children[i].pos_y >= 0) {
						int res = isVisited(agentId,children[i]); // if the state is visited by the ith agent before
						if(res)
							f = getLocalHeuristicValue(agentId,children[i]) + 1; // as single move cost is 1
						else
							f = getGlobalHeuristicValue(children[i]) + 1; // as single move cost is 1

						fValues[i] = f;

						if(f < min) {
							min = f;
							minChildId = i;
						}
					}
					else
						fValues[i] = 10000; 	// means infinite
				}

				// choice
				int decidedStateId;
				int numberOfCandidates = 0;
				for(i = 0; i < 4; i++) {
					if(fValues[i] == min)
						numberOfCandidates++;
				}
				printf("number of candidates is: %d, minimum f-value is: %d\n",numberOfCandidates,min);
				if(numberOfCandidates == 1)
					decidedStateId = minChildId;
				else {
					int * candidates = malloc(sizeof(int)*numberOfCandidates);
					for(i = 0,j = 0; i < 4; i++) {
						if(fValues[i] == min) {
							candidates[j] = i;
							j++;
						}
					}

					decidedStateId = getFarthestStateFromCandidates(agentId,children,candidates,numberOfCandidates);
				}


				decidedStates[agentId] = children[decidedStateId];
				globalUpdateValues[agentId] = min;

				int secondMin = 10000;			// means infinite
				for(i = 0; i < 4; i++) {
						if(fValues[i] < secondMin && i != minChildId)
							secondMin = fValues[i];
				}
				localUpdateValues[agentId] = secondMin;
			}
			updateGlobalHeuristicValue(agentStates[agentId],globalUpdateValues[agentId]);
            updateLocalHeuristicValue(agentId,agentStates[agentId],localUpdateValues[agentId]);

			setCurrentState(agentStates + agentId,decidedStates[agentId]);

			printf("current state of Agent %d :\n", agentId+1);
			printf("Agent Pos: %d % d \n", agentStates[agentId].pos_x, agentStates[agentId].pos_y);
			print_map_to_console(agentStates[agentId]);
			printf("-----\n");


			free(children);
			agentId++;
		}



		printf("***********************************************************\n");
		printf("Episod #%d is over for all agents\n",iterationNumber);
		printf("***********************************************************\n\n");

		if(iterationMode)
				WaitForEnter();

		iterationNumber++;
	}
}

int initialization() {

	int n, d, o;
	int i,j;

	printf("Girdi dosyasi okunuyor\n");
	FILE * input_file = fopen("environment.inp", "r");
	if (!input_file){
			printf("Dosya Okunamadi.");
	return 0;
	}

	printf("Girdi dosyasi okundu\n");



	fscanf(input_file, "%d %d %d", &n, &d, &o); //girdileri okuyalim


	printf("n=%d\n",n);
	printf("d=%d\n",d);
	printf("o=%d\n",o);

    size_of_map = n;
	grid_map = malloc(sizeof(int *) * n);
	visitedStates = malloc(sizeof(state *) *d);
	numberOfLocalStatesVisited = malloc(sizeof(int) * d);

	for (j = 0; j < n; j++)
		grid_map[j] = malloc(sizeof(int) * n);

	for (j = 0; j < d; j++){
			visitedStates[j] = malloc(sizeof(state) * (n*n));
	        numberOfLocalStatesVisited[j] = 0;
	}

	int x_coor, y_coor;

	for(i = 0 ; i < o ; i++) {
		fscanf(input_file, "%d %d", &x_coor, &y_coor);
		printf("Girdi dosyasi okundu\n");
		grid_map[x_coor - 1][y_coor - 1] = ENGEL;
	}
   fclose(input_file);


   numberOfAgents = d;
   agentStates = malloc(sizeof(state) * d);



   for (i = 0 ; i < d; i++){
	   agentStates[i].pos_x = 0;
	   agentStates[i].pos_y = 0;
   }
   goalState.pos_x = n-1;
   goalState.pos_y = n-1;
 return 1;
}

void destroy() {
	free(agentStates);
	agentStates = NULL;

	free(visitedStates);

	visitedStates = NULL;
	free(visitedStatesG);
	visitedStatesG = NULL;
}

/*

ASSUMPTIONS

- manhattan distance heuristic is used
- assume attraction range is zero, i.e., agents always see each other and moves to the closer
- single move is assumed as single cost as in the kitamura's paper
- lookahead search depth is assumed to be 1, as in the kitamura's paper
- all agents starts from different initial state is assumed.
- input.txt has two lines, all lines except last one are the initial states for the agents and last line is the final state.
  They are ordered from right to left and top to bottow, i.e., first number is 0,0 th value in the 8-tile
  and last number is 3,3 th value in 8-tile
  Assume 4 agent's initial states are given in the file, but first two is used for first run of 2 agents, and
  all is used for the second run of 4 agents.
- check moves.txt for the actions and states taken at each step.

*/


int main() {
	int i;

	out = fopen("moves.txt","w");

	printf("**********************************************  ASSUMPTIONS  ******************************************************************\n\n");

	printf("- Manhattan distance heuristic is used\n");
	printf("- Assume attraction range is zero, i.e., agents always see each other and moves to the closer\n");
	printf("- Single move is assumed as single cost as in the kitamura's paper\n");
	printf("- Lookahead search depth is assumed to be 1, as in the kitamura's paper\n");
	printf("- All agents starts from different initial state is assumed.\n");
	printf("- input.txt has five lines, all lines except last one are the initial states for the agents and last line is the final state.\n");
	printf("  They are ordered from right to left and top to bottom, i.e., first number is 0,0 th value in the 8-tile\n");
	printf("  and the last number is 3,3 th value in 8-tile.\n");
	printf("  Further assume that first two agents are used for the first run of 2 agents, and\n");
	printf("  all is used for the second run of 4 agents.\n");
	printf("- Check moves.txt for the actions and states taken at each step.\n");

	printf("\n******************************************************************************************************************************\n\n");

	printf("If you want to use discrete time mode, please press 1 and press enter (stops at each episod by logging info)\n");
	printf("If you want to use continuous time mode, please press 2 and press enter (logs all info, and prints total # of runs and CPU time at the end)\n");

	char c;
	scanf("%c",&c);
	if(c == '1')
		iterationMode = 1;
	else
		iterationMode = 0;
	WaitForEnter();

	numberOfGlobalStatesVisited = 0;
	iterationNumber = 1;
	if(!initialization()){
		printf("Program Sonlandiriliyor");
		return 0;
	}

	printf("System is loaded, initial states are as below:\n");
	for(i = 0; i < numberOfAgents; i++) {
		printf("Agent %d:\n",i+1);
		print_map_to_console(agentStates[i]);
		printf("-----\n");
	}

	printf("The final state is as below:\n");
	print_map_to_console(goalState);

	printf("Press enter to start the MARTA* search\n");
	WaitForEnter();

	start = clock();
	runMARTA();
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	totalRun = iterationNumber;
	fprintf(out,"CPU Time is: %f\n",cpu_time_used);

	printf("*****************************************************************************************\n");
	printf("The session for 2 agents is over, please press enter to start the session for 4 agents\n");
	printf("*****************************************************************************************\n");

	/*
	fprintf(out,"\n******************************************************************\n");
	fprintf(out,"The session for 2 agents is over, system is restarted for 4 agents\n");
	fprintf(out,"******************************************************************\n\n");

	if(iterationMode)
		WaitForEnter();

	destroy();

	numberOfLocalStatesVisited1 = 0;
	numberOfLocalStatesVisited2 = 0;
	numberOfLocalStatesVisited3 = 0;
	numberOfLocalStatesVisited4 = 0;
	numberOfGlobalStatesVisited = 0;
	iterationNumber = 1;
	numberOfAgents = 4;
	initialization();

	start = clock();
	runMARTA();
	end = clock();
	cpu_time_used2 = ((double) (end - start)) / CLOCKS_PER_SEC;
	totalRun2 = iterationNumber;
	*/
	//fprintf(out,"CPU Time is: %f\n",cpu_time_used2);

	printf("\n******************************************\n");
	printf("TOTAL # of RUNS for 2 AGENTs RUN: %d\n",totalRun);
	printf("CPU TIME of RUNS for 2 AGENTs RUN: %f\n",cpu_time_used);

	//printf("TOTAL # of RUNS for 4 AGENTs RUN: %d\n",totalRun2);
	//printf("CPU TIME of RUNS for 4 AGENTs RUN: %f\n",cpu_time_used2);
	//printf("******************************************\n\n");

	fprintf(out,"\n******************************************\n");
	fprintf(out,"TOTAL # of RUNS for 2 AGENTs RUN: %d\n",totalRun);
	fprintf(out,"CPU TIME of RUNS for 2 AGENTs RUN: %f\n",cpu_time_used);

	//fprintf(out,"TOTAL # of RUNS for 4 AGENTs RUN: %d\n",totalRun2);
	//fprintf(out,"CPU TIME of RUNS for 4 AGENTs RUN: %f\n",cpu_time_used2);
	//fprintf(out,"******************************************\n\n");

	fclose(out);
return 0;
}
