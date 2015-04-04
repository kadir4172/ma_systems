#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


int ** grid_map;
int size_of_map;
#define ENGEL    3

typedef struct state {
	int cstate[4][2];
	int posx; // position of empty cell
	int posy;
	int index;
	int hvalue;
} state;

state * currentStates;

state finalState;

state * visitedStates1;
state * visitedStates2;
state * visitedStates3;
state * visitedStates4;

state * visitedStatesG; // globally visited states and their heuristic values

int numberOfVisitedStates1;
int numberOfVisitedStates2;
int numberOfVisitedStates3;
int numberOfVisitedStates4;

int numberOfVisitedStatesG;

int numberOfAgents;

int episodMode;
int episodNumber = 1;

FILE * inp;
FILE * out;

clock_t start, end;
double cpu_time_used1;
double cpu_time_used2;
int totalRun1;
int totalRun2;

void WaitForEnter() {
	int c;
	while((c = getchar()) != '\n' && c != EOF)
		;
}

void fprintState(state s) {
	/*
	int i,j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 2; j++)
			fprintf(out,"%d ",s.cstate[i][j]);
		fprintf(out,"\n");
	}
	*/
	int i,j;
	for(i = 0; i < size_of_map; i++) {
		for(j = 0; j < size_of_map; j++){
			if(grid_map[i][j] == ENGEL)
			  fprintf(out,"o ");
			else if(s.posx == i && s.posy == j)
			  fprintf(out,"A ");
		    else
		      fprintf(out,"x ");
		}
		fprintf(out,"\n");
	}
}

void printState(state s) {
	int i,j;
	for(i = 0; i < size_of_map; i++) {
		for(j = 0; j < size_of_map; j++){
			if(grid_map[i][j] == ENGEL)
				printf("o ");
			else if(s.posx == i && s.posy == j)
			  printf("A ");
		    else
		      printf("x ");
		}
		printf("\n");
	}
}

state * expandState(state currentState) {
	//int i;
	state * children = malloc(sizeof(state)*4);

	//printf("expand state, current state is:\n");
	//printState(currentState);
	//printf("%d %d\n",currentState.posx,currentState.posy);
	//printf("------\n");

	// left

	printf("Buradayiz cok sukur\n");
	//ASAGI
	if((currentState.posy-1 >= 0)){
	  if(grid_map[currentState.posx][currentState.posy-1] != ENGEL) {
		printf("ASAGI\n");
			children[0] = currentState;
			children[0].posx = currentState.posx;
		    children[0].posy = currentState.posy-1;


		    //UST
			children[0].cstate[0][0] = currentState.posx;
			children[0].cstate[0][1] = currentState.posy;
			//ASAGI
			if(children[0].posy-1 >= 0){
			if(grid_map[children[0].posx][children[0].posy-1] != ENGEL  ){
			  children[0].cstate[1][0] = children[0].posx;
			  children[0].cstate[1][1] = children[0].posy - 1;
			}
			else{
			  children[0].cstate[1][0] = -100;
			  children[0].cstate[1][1] = -100;
			}
			}
			//SAG
			if(children[0].posx+1 < size_of_map){
			if(grid_map[children[0].posx+1][children[0].posy] != ENGEL ){
			  children[0].cstate[2][0] = children[0].posx + 1;
			  children[0].cstate[2][1] = children[0].posy ;
			}
			else{
			  children[0].cstate[2][0] = -100;
		      children[0].cstate[2][1] = -100;
			}
			}
			//SOL
			if(children[0].posx-1 >= 0){
			if(grid_map[children[0].posx-1][children[0].posy] != ENGEL ){
			  children[0].cstate[3][0] = children[0].posx - 1;
			  children[0].cstate[3][1] = children[0].posy ;
			}
			else{
			  children[0].cstate[3][0] = -100;
			  children[0].cstate[3][1] = -100;
			}
			}


	}
		else {
			children[0] = currentState; // means no child !
			children[0].posx = -100;
			children[0].posy = -100;
		}
	}
	else {
		children[0] = currentState; // means no child !
		children[0].posx = -100;
		children[0].posy = -100;
	}
	// YUKARI
	if(currentState.posy+1 < size_of_map) {
	  if(grid_map[currentState.posx][currentState.posy+1] != ENGEL ) {
		printf("YUKARI\n");
		    children[1] = currentState;
			children[1].posx = currentState.posx;
		    children[1].posy = currentState.posy+1;

		    //ASAGI
			children[1].cstate[1][0] = currentState.posx;
			children[1].cstate[1][1] = currentState.posy;
			//YUKARI
			if(children[1].posy+1 < size_of_map){
			if(grid_map[children[1].posx][children[1].posy+1] != ENGEL ){
			  children[1].cstate[0][0] = children[1].posx;
			  children[1].cstate[0][1] = children[1].posy + 1;
			}
			else{
			  children[1].cstate[0][0] = -100;
			  children[1].cstate[0][1] = -100;
			}
			}
			//SAG
			if(children[1].posx+1 < size_of_map){
			  if(grid_map[children[1].posx+1][children[1].posy] != ENGEL  ){
			  children[1].cstate[2][0] = children[1].posx + 1;
			  children[1].cstate[2][1] = children[1].posy;
			  }
			  else{
			   children[1].cstate[2][0] = -100;
		       children[1].cstate[2][1] = -100;
			  }
			}
			//SOL
			if (children[1].posx-1 >= 0) {
			if(grid_map[children[1].posx-1][children[1].posy] != ENGEL ){
			  children[1].cstate[3][0] = children[1].posx - 1;
			  children[1].cstate[3][1] = children[1].posy;
			}
			else{
			  children[1].cstate[3][0] = -100;
			  children[1].cstate[3][1] = -100;
			}
			}
	}
	else {
		children[1] = currentState; // means no child !
		children[1].posx = -100;
		children[1].posy = -100;
	}
	}
	else {
		children[1] = currentState; // means no child !
		children[1].posx = -100;
		children[1].posy = -100;
	}

	//SOL
	//if(currentState.posx - 1 >= 0) {
	if((currentState.posx-1 >= 0)){
	  if(grid_map[currentState.posx-1][currentState.posy] != ENGEL ) {
		printf("SOL\n");
		    children[2] = currentState;
			children[2].posx = currentState.posx -1;
		    children[2].posy = currentState.posy;

		    //SAG
			children[2].cstate[2][0] = currentState.posx;
			children[2].cstate[2][1] = currentState.posy;
			//YUKARI
			if(children[2].posy+1 < size_of_map){
			if(grid_map[children[2].posx][children[2].posy+1] != ENGEL ){
			  children[2].cstate[0][0] = children[2].posx;
			  children[2].cstate[0][1] = children[2].posy + 1;
			}
			else{
			  children[2].cstate[0][0] = -100;
			  children[2].cstate[0][1] = -100;
			}
			}
			//ASAGI
			if(children[2].posy-1 >= 0){
			if(grid_map[children[2].posx][children[2].posy-1] != ENGEL  ){
			  children[2].cstate[1][0] = children[2].posx;
			  children[2].cstate[1][1] = children[2].posy - 1;
			}
			else{
			  children[2].cstate[1][0] = -100;
		      children[2].cstate[1][1] = -100;
			}
			}
			//SOL
			 if(children[2].posx-1 >= 0){
			if(grid_map[children[2].posx-1][children[2].posy] != ENGEL ){
			  children[2].cstate[3][0] = children[2].posx - 1;
			  children[2].cstate[3][1] = children[2].posy ;
			}
			else{
			  children[2].cstate[3][0] = -100;
			  children[2].cstate[3][1] = -100;
			}
			 }
	}
	else {
		children[2] = currentState; // means no child !
		children[2].posx = -100;
		children[2].posy = -100;
	}
	}
	else {
		children[2] = currentState; // means no child !
		children[2].posx = -100;
		children[2].posy = -100;
	}

	//SAG
	//if(currentState.posx + 1 <= 2) {
	if(currentState.posx+1 <size_of_map){
	  if(grid_map[currentState.posx+1][currentState.posy] != ENGEL ) {
		printf("SAG\n");
	    children[3] = currentState;
		children[3].posx = currentState.posx +1;
	    children[3].posy = currentState.posy;

	    //SOL
		children[3].cstate[3][0] = currentState.posx;
		children[3].cstate[3][1] = currentState.posy;
		//YUKARI
		if (children[3].posy+1 < size_of_map){
		if(grid_map[children[3].posx][children[3].posy+1] != ENGEL  ){
		  children[3].cstate[0][0] = children[3].posx;
		  children[3].cstate[0][1] = children[3].posy + 1;
		}
		else{
		  children[3].cstate[0][0] = -100;
		  children[3].cstate[0][1] = -100;
		}
		}
		//ASAGI
		if(children[3].posy-1 >= 0){
		if(grid_map[children[3].posx][children[3].posy-1] != ENGEL  ){
		  children[3].cstate[1][0] = children[3].posx;
		  children[3].cstate[1][1] = children[3].posy - 1;
		}
		else{
		  children[3].cstate[1][0] = -100;
	      children[3].cstate[1][1] = -100;
		}
		}
		//SAG
		if (children[3].posx+1 < size_of_map){
		if(grid_map[children[3].posx+1][children[3].posy] != ENGEL  ){
		  children[3].cstate[2][0] = children[3].posx + 1;
		  children[3].cstate[2][1] = children[3].posy ;
		}
		else{
		  children[3].cstate[2][0] = -100;
		  children[3].cstate[2][1] = -100;
		}
		}
	}
	else {
		children[3] = currentState; // means no child !
		children[3].posx = -100;
		children[3].posy = -100;
	}
	}
	else {
		children[3] = currentState; // means no child !
		children[3].posx = -100;
		children[3].posy = -100;
	}
	//printf("children are:\n");
	//for(i = 0; i < 4; i++) {
	//	printState(children[i]);
	//	printf("%d %d\n",children[i].posx,children[i].posy);
	//	printf("-------\n");
	//}

	return children;
}

int isThereGoal(state * children) {
	int i,j,k;
	int isGoal = 0;
	int id = -1;
	for(i = 0; i < 4; i++) {
		if(children[i].posx == finalState.posx && children[i].posy == finalState.posy) {
			isGoal = 1;
		}
		if(isGoal) {
			id = i;
			break;
		}
	}

	return id;
}

int numberOfChildren(state * children) {
	int i;
	int number = 0;
	for(i = 0; i < 4; i++) {
		if(children[i].posx != -100 && children[i].posy != -100) {
			number++;
		}
	}

	return number;
}

int isEqual(state s1, state s2) {
	//printf("coming states:\n");
	//printState(s1);
	//printf("-------\n");
	//printState(s2);
	//printf("-------\n");
	//int i,j;
	int ret = 0;
	if(s1.posx == s2.posx && s1.posy == s2.posy) {
		ret = 1;
	}
	//printf("and result: %d s1: %d %d s2: %d %d\n",ret,s1.posx,s1.posy,s2.posx,s2.posy);
	return ret;
}

int visitedStateId = -1;

int isVisited(int agent, state child) {
	int i;
	if(agent == 0) {
		for(i = 0; i < numberOfVisitedStates1; i++) {
			if(isEqual(visitedStates1[i],child)) {
				visitedStateId = i;
				return 1;
			}
		}
	}
	else if(agent == 1) {
		for(i = 0; i < numberOfVisitedStates2; i++) {
			if(isEqual(visitedStates2[i],child)) {
				visitedStateId = i;
				return 1;
			}
		}

	}
	else if(agent == 2) {
		for(i = 0; i < numberOfVisitedStates3; i++) {
			if(isEqual(visitedStates3[i],child)) {
				visitedStateId = i;
				return 1;
			}
		}
	}
	else if(agent == 3) {
		for(i = 0; i < numberOfVisitedStates4; i++) {
			if(isEqual(visitedStates4[i],child)) {
				visitedStateId = i;
				return 1;
			}
		}
	}
	else
		printf("ERROR in isVisited() function !\n");

	return 0;
}

int getLocalHeuristicValue(int agentId, state children) {
	int i,j,k;

	if(agentId == 0) {
		return visitedStates1[visitedStateId].hvalue;
	}
	else if(agentId == 1) {
		return visitedStates2[visitedStateId].hvalue;
	}
	else if(agentId == 2) {
		return visitedStates3[visitedStateId].hvalue;
	}
	else if(agentId == 3) {
		return visitedStates4[visitedStateId].hvalue;
	}
	else
		printf("ERROR in getLocalHeuristicValue() function\n");
}

int calculateHeuristicValue(state s1, state s2) {
	int hval = abs(s1.posx-s2.posx) + abs(s1.posy-s2.posy);
	return hval;

	/*
	int i,j,k,l;
	int hval = 0;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			int found = 0;
			for(k = 0; k < 3; k++) {
				for(l = 0; l < 3; l++) {
					if(s1.cstate[i][j] == s2.cstate[k][l]) {
						found = 1;
						break;
					}
				}
				if(found)
					break;
			}

			// manhattan distance
			hval += abs(i-k) + abs(j-l);
		}
	}

	return hval;
	*/
}

int getGlobalHeuristicValue(state s) {
	int i,j,k;
	int visited = 0;
	//printf("getGlobal Heur Value, numb Of visited G: %d\n",numberOfVisitedStatesG);
	//printf("the search state is:\n");
	//printState(s);
	//printf("--------\n");
	for(i = 0; i < numberOfVisitedStatesG; i++) {
		if(isEqual(visitedStatesG[i],s)) {
			//printf("yes found !\n");
			//printf("real hval is: %d\n",calculateHeuristicValue(s,finalState));
			//printf("now it is: %d\n",visitedStatesG[i].hvalue);
			//WaitForEnter();
			return visitedStatesG[i].hvalue;
		}
	}

	//return calculateHeuristicValue(s,finalState);
	return 0;
}

void setCurrentState(state * currentState, state s) {
	int i,j;
	(*currentState).posx = s.posx;
	(*currentState).posy = s.posy;
	(*currentState).hvalue = s.hvalue;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 2; j++)
			(*currentState).cstate[i][j] = s.cstate[i][j];

}

int calculateIsolation(int agent, state candidate) {
	int i;
	int min = 10000;
	for(i = 0; i < numberOfAgents; i++) {
		if(i != agent) {
			int temp = calculateHeuristicValue(candidate, currentStates[i]);
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
		//printf("%d. candidate is: \n",candidates[i]);
		//printState(children[candidates[i]]);
		//printf("its isolation is %d, min: %d\n",temp,min);
		if(temp >= max) {
			maxId = candidates[i];
			max = temp;
		}
	}

	return maxId;
}

void updateGlobalHeuristicValue(state s, int hval) {
	int i,j,k;
	int visited = 0;
	//printf("getGlobal Heur Value, numb Of visited G: %d\n",numberOfVisitedStatesG);
	//printf("the search state is:\n");
	//printState(s);
	//printf("--------\n");

	for(i = 0; i < numberOfVisitedStatesG; i++) {
		if(isEqual(visitedStatesG[i],s)) {
			visitedStatesG[i].hvalue = hval;
			return;
		}
	}

	//printf("not visited before will be added to the visited ones\n");

	state * temp = malloc(sizeof(state)*(numberOfVisitedStatesG+1));
	for(i = 0; i < numberOfVisitedStatesG; i++) {
		temp[i].posx = visitedStatesG[i].posx;
		temp[i].posy = visitedStatesG[i].posy;
		temp[i].hvalue = visitedStatesG[i].hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				temp[i].cstate[j][k] = visitedStatesG[i].cstate[j][k];
	}

	s.hvalue = hval;

	temp[i].posx = s.posx;
	temp[i].posy = s.posy;
	temp[i].hvalue = s.hvalue;
	for(j = 0; j < 4; j++)
		for(k = 0; k < 2; k++)
			temp[i].cstate[j][k] = s.cstate[j][k];

	free(visitedStatesG);
	numberOfVisitedStatesG++;
	visitedStatesG = malloc(sizeof(state)*(numberOfVisitedStatesG));
	for(i = 0; i < numberOfVisitedStatesG; i++) {
		visitedStatesG[i].posx = temp[i].posx;
		visitedStatesG[i].posy = temp[i].posy;
		visitedStatesG[i].hvalue = temp[i].hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				visitedStatesG[i].cstate[j][k] = temp[i].cstate[j][k];
		//printf("added state %d:\n",i);
		//printState(visitedStatesG[i]);
		//printf("%d %d\n",visitedStatesG[i].posx,visitedStatesG[i].posy);
		//printf("-------\n");
	}

	return;
}

void updateLocalHeuristicValue(int agent, state s, int hval) {
	int i,j,k;

	if(agent == 0) {
		for(i = 0; i < numberOfVisitedStates1; i++) {
			if(isEqual(visitedStates1[i],s)) {
				visitedStates1[i].hvalue = hval;
				return;
			}
		}

		//printf("not visited before will be added to the visited ones\n");

		state * temp = malloc(sizeof(state)*(numberOfVisitedStates1+1));
		for(i = 0; i < numberOfVisitedStates1; i++) {
			temp[i].posx = visitedStates1[i].posx;
			temp[i].posy = visitedStates1[i].posy;
			temp[i].hvalue = visitedStates1[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					temp[i].cstate[j][k] = visitedStates1[i].cstate[j][k];
		}

		s.hvalue = hval;

		temp[i].posx = s.posx;
		temp[i].posy = s.posy;
		temp[i].hvalue = s.hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				temp[i].cstate[j][k] = s.cstate[j][k];

		free(visitedStates1);
		numberOfVisitedStates1++;
		visitedStates1 = malloc(sizeof(state)*(numberOfVisitedStates1));
		for(i = 0; i < numberOfVisitedStates1; i++) {
			visitedStates1[i].posx = temp[i].posx;
			visitedStates1[i].posy = temp[i].posy;
			visitedStates1[i].hvalue = temp[i].hvalue;
			for(j = 0; j < 4 ;j++)
				for(k = 0; k < 2; k++)
					visitedStates1[i].cstate[j][k] = temp[i].cstate[j][k];
			//printf("added state %d:\n",i);
			//printState(visitedStatesG[i]);
			//printf("%d %d\n",visitedStatesG[i].posx,visitedStatesG[i].posy);
			//printf("-------\n");
		}

	}
	else if(agent == 1) {
		for(i = 0; i < numberOfVisitedStates2; i++) {
			if(isEqual(visitedStates2[i],s)) {
				visitedStates2[i].hvalue = hval;
				return;
			}
		}

		//printf("not visited before will be added to the visited ones\n");

		state * temp = malloc(sizeof(state)*(numberOfVisitedStates2+1));
		for(i = 0; i < numberOfVisitedStates2; i++) {
			temp[i].posx = visitedStates2[i].posx;
			temp[i].posy = visitedStates2[i].posy;
			temp[i].hvalue = visitedStates2[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					temp[i].cstate[j][k] = visitedStates2[i].cstate[j][k];
		}

		s.hvalue = hval;

		temp[i].posx = s.posx;
		temp[i].posy = s.posy;
		temp[i].hvalue = s.hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				temp[i].cstate[j][k] = s.cstate[j][k];

		free(visitedStates2);
		numberOfVisitedStates2++;
		visitedStates2 = malloc(sizeof(state)*(numberOfVisitedStates2));
		for(i = 0; i < numberOfVisitedStates2; i++) {
			visitedStates2[i].posx = temp[i].posx;
			visitedStates2[i].posy = temp[i].posy;
			visitedStates2[i].hvalue = temp[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					visitedStates2[i].cstate[j][k] = temp[i].cstate[j][k];
			//printf("added state %d:\n",i);
			//printState(visitedStatesG[i]);
			//printf("%d %d\n",visitedStatesG[i].posx,visitedStatesG[i].posy);
			//printf("-------\n");
		}
	}
	else if(agent == 2) {
		for(i = 0; i < numberOfVisitedStates3; i++) {
			if(isEqual(visitedStates3[i],s)) {
				visitedStates3[i].hvalue = hval;
				return;
			}
		}

		//printf("not visited before will be added to the visited ones\n");

		state * temp = malloc(sizeof(state)*(numberOfVisitedStates3+1));
		for(i = 0; i < numberOfVisitedStates3; i++) {
			temp[i].posx = visitedStates3[i].posx;
			temp[i].posy = visitedStates3[i].posy;
			temp[i].hvalue = visitedStates3[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					temp[i].cstate[j][k] = visitedStates3[i].cstate[j][k];
		}

		s.hvalue = hval;

		temp[i].posx = s.posx;
		temp[i].posy = s.posy;
		temp[i].hvalue = s.hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				temp[i].cstate[j][k] = s.cstate[j][k];

		free(visitedStates3);
		numberOfVisitedStates3++;
		visitedStates3 = malloc(sizeof(state)*(numberOfVisitedStates3));
		for(i = 0; i < numberOfVisitedStates3; i++) {
			visitedStates3[i].posx = temp[i].posx;
			visitedStates3[i].posy = temp[i].posy;
			visitedStates3[i].hvalue = temp[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					visitedStates3[i].cstate[j][k] = temp[i].cstate[j][k];
			//printf("added state %d:\n",i);
			//printState(visitedStatesG[i]);
			//printf("%d %d\n",visitedStatesG[i].posx,visitedStatesG[i].posy);
			//printf("-------\n");
		}
	}
	else if(agent == 3) {
		for(i = 0; i < numberOfVisitedStates4; i++) {
			if(isEqual(visitedStates4[i],s)) {
				visitedStates4[i].hvalue = hval;
				return;
			}
		}

		//printf("not visited before will be added to the visited ones\n");

		state * temp = malloc(sizeof(state)*(numberOfVisitedStates4+1));
		for(i = 0; i < numberOfVisitedStates4; i++) {
			temp[i].posx = visitedStates4[i].posx;
			temp[i].posy = visitedStates4[i].posy;
			temp[i].hvalue = visitedStates4[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					temp[i].cstate[j][k] = visitedStates4[i].cstate[j][k];
		}

		s.hvalue = hval;

		temp[i].posx = s.posx;
		temp[i].posy = s.posy;
		temp[i].hvalue = s.hvalue;
		for(j = 0; j < 4; j++)
			for(k = 0; k < 2; k++)
				temp[i].cstate[j][k] = s.cstate[j][k];

		free(visitedStates4);
		numberOfVisitedStates4++;
		visitedStates4 = malloc(sizeof(state)*(numberOfVisitedStates4));
		for(i = 0; i < numberOfVisitedStates4; i++) {
			visitedStates4[i].posx = temp[i].posx;
			visitedStates4[i].posy = temp[i].posy;
			visitedStates4[i].hvalue = temp[i].hvalue;
			for(j = 0; j < 4; j++)
				for(k = 0; k < 2; k++)
					visitedStates4[i].cstate[j][k] = temp[i].cstate[j][k];
			//printf("added state %d:\n",i);
			//printState(visitedStatesG[i]);
			//printf("%d %d\n",visitedStatesG[i].posx,visitedStatesG[i].posy);
			//printf("-------\n");
		}
	}
	else
		printf("ERROR in updateLocalHeuristicValue() function 1!\n");

	return;
}


void runMARTA() {
	int i,j;
	state * children;

	int agentId;
	while(1) {
		// each loop is one episod for all agents
		agentId = 0;
		state decidedStates[4];
		int decidedActions[4];
		int globalUpdateValues[4];
		int localUpdateValues[4];

		while(agentId < numberOfAgents) {

			// expand the current state
			children = expandState(currentStates[agentId]);

			int len = numberOfChildren(children);
			printf("%d. agent has %d children, ",agentId+1,len);
			// check if it is the goal state, if yes terminate
			int goalId = isThereGoal(children);

			if(goalId != -1) {
				setCurrentState(currentStates + agentId,children[goalId]);
				printf("and has the final state in one of its children\nThe search will be ended.\n",agentId+1);
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
				printState(currentStates[agentId]);
				fprintf(out,"State(Final):\n");
				fprintState(currentStates[agentId]);
				printf("Total number of moves at this run(including this): %d\n",episodNumber);
				fprintf(out,"Total number of moves at this run(including this): %d\n",episodNumber);
				printf("***********************************************************\n");
				printf("Episod #%d is over for all agents\n",episodNumber);
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
					if(children[i].posx >= 0 && children[i].posy >= 0) {
						int res = isVisited(agentId,children[i]); // if the state is visited by the ith agent before
						if(res)
							f = getLocalHeuristicValue(agentId,children[i]) + 1; // as single move cost is 1
						else
							f = getGlobalHeuristicValue(children[i]) + 1; // as single move cost is 1

						fValues[i] = f;
						//printf("%d. children and its f-value is:\n",++j);
						//printState(children[i]);
						//printf("f value: %d\n",f);
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

				decidedActions[agentId] = decidedStateId;
				decidedStates[agentId] = children[decidedStateId];
				globalUpdateValues[agentId] = min;

				int secondMin = 10000;			// means infinite
				for(i = 0; i < 4; i++) {
						if(fValues[i] < secondMin && i != minChildId)
							secondMin = fValues[i];
				}
				localUpdateValues[agentId] = secondMin;
			}
			updateGlobalHeuristicValue(currentStates[agentId],globalUpdateValues[agentId]);
            updateLocalHeuristicValue(agentId,currentStates[agentId],localUpdateValues[agentId]);

			setCurrentState(currentStates + agentId,decidedStates[agentId]);

			printf("current state of Agent %d :\n", agentId+1);
			printf("Agent Pos: %d % d \n", currentStates[agentId].posx, currentStates[agentId].posy);
			printState(currentStates[agentId]);
			printf("-----\n");


			free(children);
			agentId++;
		}

		// for each agent apply decisions concurrently
		//printf("\nThe decided actions and states for this episod is as below:\n");
		/*
		for(i = 0; i < numberOfAgents; i++) {
			printf("Agent%d:",i+1);
			fprintf(out,"Agent%d:",i+1);
			if(decidedActions[i] == 0) {
				printf("Left\n");
				fprintf(out,"Left\n");
			}
			else if(decidedActions[i] == 1) {
				printf("Right\n");
				fprintf(out,"Right\n");
			}
			else if(decidedActions[i] == 2) {
				printf("Up\n");
				fprintf(out,"Up\n");
			}
			else if(decidedActions[i] == 3) {
				printf("Down\n");
				fprintf(out,"Down\n");
			}
			else
				printf("ERROR in decided action selection !\n");

			fprintf(out,"State:\n");
			fprintState(decidedStates[i]);
			fprintf(out,"--------\n");

			// estimation update
			updateGlobalHeuristicValue(currentStates[i],globalUpdateValues[i]);
			updateLocalHeuristicValue(i,currentStates[i],localUpdateValues[i]);

			setCurrentState(currentStates + i,decidedStates[i]);
			printf("current state is:\n");
			printState(currentStates[i]);
		}
		*/

		printf("***********************************************************\n");
		printf("Episod #%d is over for all agents\n",episodNumber);
		printf("***********************************************************\n\n");

		if(episodMode)
				WaitForEnter();

		episodNumber++;
	}
}

void initialization() {

	int n, d, o;
	int i,j;

	printf("Girdi dosyasi okunuyor\n");
	FILE * input_file = fopen("environment.inp", "r");
	if (!input_file)
			printf("Dosya Okunamadi.");

	printf("Girdi dosyasi okundu\n");



	fscanf(input_file, "%d %d %d", &n, &d, &o); //girdileri okuyalim
	//fscanf(input_file, "%d", &n); //girdileri okuyalim

	//fgetc(input_file);   				                 //yeni satiri okuyalim

	printf("n=%d\n",n);
	printf("d=%d\n",d);
	printf("o=%d\n",o);

    size_of_map = n;
	grid_map = malloc(sizeof(int *) * n);
	for (j = 0; j < n; j++)
		grid_map[j] = malloc(sizeof(int) * n);

	int x_coor, y_coor;

	for(i = 0 ; i < o ; i++) {
		fscanf(input_file, "%d %d", &x_coor, &y_coor);
		printf("Girdi dosyasi okundu\n");
		grid_map[x_coor - 1][y_coor - 1] = ENGEL;
	}
   fclose(input_file);

   d = 4;
   numberOfAgents = d;
   currentStates = malloc(sizeof(state) * d);

   for (i = 0 ; i < d; i++){
	   currentStates[i].posx = 0;
	   currentStates[i].posy = 0;
   }
   finalState.posx = n-1;
   finalState.posy = n-1;
/*

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++) {
			int temp;
			fscanf(inp,"%d",&temp);
			currentStates[0].cstate[i][j] = temp;
			if(temp == 0) {
				currentStates[0].posx = i;
				currentStates[0].posy = j;
			}
		}

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++) {
			int temp;
			fscanf(inp,"%d",&temp);
			currentStates[1].cstate[i][j] = temp;
			if(temp == 0) {
				currentStates[1].posx = i;
				currentStates[1].posy = j;
			}
		}

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++) {
			int temp;
			fscanf(inp,"%d",&temp);
			currentStates[2].cstate[i][j] = temp;
			if(temp == 0) {
				currentStates[2].posx = i;
				currentStates[2].posy = j;
			}
		}

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++) {
			int temp;
			fscanf(inp,"%d",&temp);
			currentStates[3].cstate[i][j] = temp;
			if(temp == 0) {
				currentStates[3].posx = i;
				currentStates[3].posy = j;
			}
		}


	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++) {
			int temp;
			fscanf(inp,"%d",&temp);
			finalState.cstate[i][j] = temp;

			if(temp == 0) {
				finalState.posx = i;
				finalState.posy = j;

			}
		}

	fclose(inp);
	*/
}

void destroy() {
	free(currentStates);
	currentStates = NULL;

	free(visitedStates1);
	free(visitedStates2);
	free(visitedStates3);
	free(visitedStates4);

	visitedStates1 = NULL;
	visitedStates2 = NULL;
	visitedStates3 = NULL;
	visitedStates4 = NULL;

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
		episodMode = 1;
	else
		episodMode = 0;
	WaitForEnter();

	numberOfVisitedStates1 = 0;
	numberOfVisitedStates2 = 0;
	numberOfVisitedStatesG = 0;
	episodNumber = 1;
	initialization();

	printf("System is loaded, initial states are as below:\n");
	for(i = 0; i < 4; i++) {
		printf("Agent %d:\n",i+1);
		printState(currentStates[i]);
		printf("-----\n");
	}

	printf("The final state is as below:\n");
	printState(finalState);

	printf("Press enter to start the MARTA* search\n");
	WaitForEnter();

	start = clock();
	runMARTA();
	end = clock();
    cpu_time_used1 = ((double) (end - start)) / CLOCKS_PER_SEC;
	totalRun1 = episodNumber;
	fprintf(out,"CPU Time is: %f\n",cpu_time_used1);

	printf("*****************************************************************************************\n");
	printf("The session for 2 agents is over, please press enter to start the session for 4 agents\n");
	printf("*****************************************************************************************\n");

	fprintf(out,"\n******************************************************************\n");
	fprintf(out,"The session for 2 agents is over, system is restarted for 4 agents\n");
	fprintf(out,"******************************************************************\n\n");

	if(episodMode)
		WaitForEnter();

	destroy();

	numberOfVisitedStates1 = 0;
	numberOfVisitedStates2 = 0;
	numberOfVisitedStates3 = 0;
	numberOfVisitedStates4 = 0;
	numberOfVisitedStatesG = 0;
	episodNumber = 1;
	numberOfAgents = 4;
	initialization();

	start = clock();
	runMARTA();
	end = clock();
	cpu_time_used2 = ((double) (end - start)) / CLOCKS_PER_SEC;
	totalRun2 = episodNumber;
	fprintf(out,"CPU Time is: %f\n",cpu_time_used2);

	printf("\n******************************************\n");
	printf("TOTAL # of RUNS for 2 AGENTs RUN: %d\n",totalRun1);
	printf("CPU TIME of RUNS for 2 AGENTs RUN: %f\n",cpu_time_used1);

	printf("TOTAL # of RUNS for 4 AGENTs RUN: %d\n",totalRun2);
	printf("CPU TIME of RUNS for 4 AGENTs RUN: %f\n",cpu_time_used2);
	printf("******************************************\n\n");

	fprintf(out,"\n******************************************\n");
	fprintf(out,"TOTAL # of RUNS for 2 AGENTs RUN: %d\n",totalRun1);
	fprintf(out,"CPU TIME of RUNS for 2 AGENTs RUN: %f\n",cpu_time_used1);

	fprintf(out,"TOTAL # of RUNS for 4 AGENTs RUN: %d\n",totalRun2);
	fprintf(out,"CPU TIME of RUNS for 4 AGENTs RUN: %f\n",cpu_time_used2);
	fprintf(out,"******************************************\n\n");

	fclose(out);

}
