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

state ** visited_states;
int* numberOfLocalStatesVisited;

state * globally_visited_states;


int numberOfGlobalStatesVisited;
int numberOfAgents;

int iterationMode;
int iterationNumber = 1;

int StateId = -1;

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

int is_same(state s1, state s2) {

	int ret = 0;
	if(s1.pos_x == s2.pos_x && s1.pos_y == s2.pos_y) {
		ret = 1;
	}

	return ret;
}



int calculate_manhattan_distance(state s1, state s2) {
	int hval = abs(s1.pos_x-s2.pos_x) + abs(s1.pos_y-s2.pos_y);
	return hval;
}


int get_hl(int agentId, state children) {

		return visited_states[agentId][StateId].hvalue;

}



int get_hg(state s) {
	int i;


	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		if(is_same(globally_visited_states[i],s)) {
			return globally_visited_states[i].hvalue;
		}
	}

	//return calculate_manhattan_distance(s,goalState);
	return 0;
}

void set_state(state * agentState, state s) {

	(*agentState).pos_x = s.pos_x;
	(*agentState).pos_y = s.pos_y;
	(*agentState).hvalue = s.hvalue;


}

int is_visited(int agent, state child) {
	int i;

		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			if(is_same(visited_states[agent][i],child)) {
				StateId = i;
				return 1;
			}
		}

	return 0;
}

int get_closest_agent_to_candidates(int agent, state candidate) {
	int i;
	int min = 10000;
	for(i = 0; i < numberOfAgents; i++) {
		if(i != agent) {
			int temp = calculate_manhattan_distance(candidate, agentStates[i]);
			if(temp < min)
				min = temp;
		}
	}

	return min;
}

int choose_candidate_with_max_dist_to_agents(int agent, state * children, int * candidates, int numbOfCand) {
	int i;
	int max = 0;
	int maxId = 0;
	for(i = 0; i < numbOfCand; i++) {
		int temp = get_closest_agent_to_candidates(agent,children[candidates[i]]);

		if(temp >= max) {
			maxId = candidates[i];
			max = temp;
		}
	}

	return maxId;
}

void update_hg(state s, int hval) {
	int i;


	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		if(is_same(globally_visited_states[i],s)) {
			globally_visited_states[i].hvalue = hval;
			return;
		}
	}



	state * temp = malloc(sizeof(state)*(numberOfGlobalStatesVisited+1));
	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		temp[i].pos_x = globally_visited_states[i].pos_x;
		temp[i].pos_y = globally_visited_states[i].pos_y;
		temp[i].hvalue = globally_visited_states[i].hvalue;

	}

	s.hvalue = hval;

	temp[i].pos_x = s.pos_x;
	temp[i].pos_y = s.pos_y;
	temp[i].hvalue = s.hvalue;


	free(globally_visited_states);
	numberOfGlobalStatesVisited++;
	globally_visited_states = malloc(sizeof(state)*(numberOfGlobalStatesVisited));
	for(i = 0; i < numberOfGlobalStatesVisited; i++) {
		globally_visited_states[i].pos_x = temp[i].pos_x;
		globally_visited_states[i].pos_y = temp[i].pos_y;
		globally_visited_states[i].hvalue = temp[i].hvalue;

	}

	return;
}

void update_hl(int agent, state s, int hval) {
	int i;

		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			if(is_same(visited_states[agent][i],s)) {
				visited_states[agent][i].hvalue = hval;
				return;
			}
		}



		state * temp = malloc(sizeof(state)*(numberOfLocalStatesVisited[agent]+1));
		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			temp[i].pos_x = visited_states[agent][i].pos_x;
			temp[i].pos_y = visited_states[agent][i].pos_y;
			temp[i].hvalue = visited_states[agent][i].hvalue;

		}

		s.hvalue = hval;

		temp[i].pos_x = s.pos_x;
		temp[i].pos_y = s.pos_y;
		temp[i].hvalue = s.hvalue;


		free(visited_states[agent]);
		numberOfLocalStatesVisited[agent]++;
		visited_states[agent] = malloc(sizeof(state)*(numberOfLocalStatesVisited[agent]));
		for(i = 0; i < numberOfLocalStatesVisited[agent]; i++) {
			visited_states[agent][i].pos_x = temp[i].pos_x;
			visited_states[agent][i].pos_y = temp[i].pos_y;
			visited_states[agent][i].hvalue = temp[i].hvalue;


		}



	return;
}


void multi_agent_search() {
	int i,j;
	state * children;

	int agentId;
	while(1) {

		agentId = 0;
		state decidedStates[numberOfAgents];

		int globalUpdateValues[numberOfAgents];
		int localUpdateValues[numberOfAgents];

		while(agentId < numberOfAgents) {


			children = expand_current_state(agentStates[agentId]);


			int goalId = next_to_final_state(children);

			if(goalId != -1) {
				set_state(agentStates + agentId,children[goalId]);
				printf("Agent %d chilren state lerinden birisi final state dir \n Iterasyon durduruluyor,gorev tamamlandi.\n", agentId+1);

				printf("Son Durum:\n");
				print_map_to_console(agentStates[agentId]);
				fprintf(out,"Son Durum:\n");
				print_map_to_file(agentStates[agentId]);
				printf("Toplam Iterasyon Sayisi: %d\n",iterationNumber);
				fprintf(out,"Toplam Iterasyon Sayisi: %d\n",iterationNumber);
				printf("***********************************************************\n");
				return;
			}
			else {
				int calculated_Cost;
				int child_with_min_cost = -1;
				int min = 10000;
				int Costs[4];

				for(i = 0,j = 0; i < 4; i++) {
					if(children[i].pos_x >= 0 && children[i].pos_y >= 0) {
						int visited_before = is_visited(agentId,children[i]);
						if(visited_before)
						  calculated_Cost = get_hl(agentId,children[i]) + 1;
						else
						  calculated_Cost = get_hg(children[i]) + 1;

						Costs[i] = calculated_Cost;

						if(calculated_Cost < min) {
							min = calculated_Cost;
							child_with_min_cost = i;
						}
					}
					else
						Costs[i] = 10000; 	// means infinite
				}

				// choice
				int decided_index;
				int numberOfCandidates = 0;
				for(i = 0; i < 4; i++) {
					if(Costs[i] == min)
						numberOfCandidates++;
				}

				if(numberOfCandidates == 0 | min == 10000){
				  printf("No Movement Option for the Agent, Simulation will be stopped");

				  return;
				}
				if(numberOfCandidates == 1)
					decided_index = child_with_min_cost;
				else {
					int * candidates = malloc(sizeof(int)*numberOfCandidates);
					for(i = 0,j = 0; i < 4; i++) {
						if(Costs[i] == min) {
							candidates[j] = i;
							j++;
						}
					}

					decided_index = choose_candidate_with_max_dist_to_agents(agentId,children,candidates,numberOfCandidates);
				}


				decidedStates[agentId] = children[decided_index];
				globalUpdateValues[agentId] = min;

				int secondMin = 10000;			// means infinite
				for(i = 0; i < 4; i++) {
						if(Costs[i] < secondMin && i != child_with_min_cost)
							secondMin = Costs[i];
				}
				localUpdateValues[agentId] = secondMin;
			}
			update_hg(agentStates[agentId],globalUpdateValues[agentId]);
			update_hl(agentId,agentStates[agentId],localUpdateValues[agentId]);

			set_state(agentStates + agentId,decidedStates[agentId]);

			printf("Current state of Agent %d :\n", agentId+1);
			print_map_to_console(agentStates[agentId]);
			printf("-----\n");


			free(children);
			agentId++;
		}



		printf("***********************************************************\n");
		printf("Tum agentlar icin %d nolu iterasyon tamamlandi \n",iterationNumber);
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
	visited_states = malloc(sizeof(state *) *d);
	numberOfLocalStatesVisited = malloc(sizeof(int) * d);

	for (j = 0; j < n; j++)
		grid_map[j] = malloc(sizeof(int) * n);

	for (j = 0; j < d; j++){
			visited_states[j] = malloc(sizeof(state) * (n*n));
	        numberOfLocalStatesVisited[j] = 0;
	}

	int x_coor, y_coor;

	for(i = 0 ; i < o ; i++) {
		fscanf(input_file, "%d %d", &x_coor, &y_coor);
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




int main() {
	int i;

	out = fopen("output.txt","w");


	printf("Tum iterasyonlari gormek icin 1'e ve Enter a basin ve her iterasyon sonunda Enter a basin\n");
	printf("Tum iterasyonlari gormek istemiyorsaniz 2'ye ve Enter a basin\n");


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

	printf("Baslangic kosullari girdi dosyasindan asagidaki gibi okunmustur:\n");
	for(i = 0; i < numberOfAgents; i++) {
		printf("Agent %d:\n",i+1);
		print_map_to_console(agentStates[i]);
		printf("-----\n");
	}


	printf("Real Time Search algoritmasini baslatmak icin Enter a basiniz\n");
	WaitForEnter();

	start = clock();
	multi_agent_search();
	end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	totalRun = iterationNumber;
	fprintf(out,"CPU Time is: %f\n",cpu_time_used);


	printf("\n******************************************\n");
	printf("%d agent ile Toplam Iterasyon Sayisi : %d\n",numberOfAgents,totalRun);
	printf("%d agent ile Toplam CPU Zamani : %f\n",numberOfAgents,cpu_time_used);

	fclose(out);
return 0;
}
