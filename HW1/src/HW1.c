#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
	int x_coor;
	int y_coor;
	bool live;
	float energy;
	bool close_to_prey;
	int agent_index;
} agent_feats;

#define BOS      0
#define AVCI     1
#define AV       2
#define ENGEL    3



int time = 1;


// Simulasyon Girdileri
int n,d;
float e,R,T;
// Simulasyon Girdileri

//Harita
int ** grid_map;

//Avcilar, avlar ve sayilari
agent_feats * hunters;
agent_feats * preys;

int hunter_number = 0;
int prey_number = 0;


//Grid map uzerindeki tum agentlari ve engelleri basan fonksiyon
void printEnv() {
	int j,k,l;

	for(j = 0; j < n; j++) {
		for(k = 0; k < n; k++) {
			if(grid_map[j][k] == BOS)
				printf("- ");
			else if(grid_map[j][k] == ENGEL)
				printf("O ");
			else if(grid_map[j][k] == AV){
			    for (l = 0; l<prey_number; l++){
			    	if(preys[l].x_coor == j && preys[l].y_coor == k)
			    		printf("P%d",l);
			    }
			}
		    else{
		    	for (l = 0; l<hunter_number; l++){
		    	   	if(hunters[l].x_coor == j && hunters[l].y_coor == k)
		    	  		printf("H%d",l);
		    	}
			}
		}
		printf("\n");
	}
	for(j = 0; j < (prey_number); j++){
		printf("P%d ",preys[j].agent_index);

	}

	for(j = 0; j < (hunter_number); j++){
		printf("H%d=%.2f ",hunters[j].agent_index,hunters[j].energy);
	}
	printf("\n");
}

void Bekle()
{
	int c;
	while(c != EOF && (c = getchar()) != '\n')
		;
}


//iki agent ya da nokta arasi manhattan uzakligi hesaplayan fonksiyon
int Manhattan(agent_feats t1, agent_feats t2) {
	return abs(t1.x_coor - t2.x_coor) + abs(t1.y_coor - t2.y_coor);
}


agent_feats * runPreyPlan() {
	int j,k;
	agent_feats * secilen_hamle;
	secilen_hamle = malloc(sizeof(agent_feats)*prey_number);

	agent_feats hamle;
	int left_move,down_move,right_move,up_move,hold_nomove;

	//en yakin hunter i bulalim
	for(j = 0; j < prey_number; j++) {
		//olabilecek maksimum uzaklik
		int min_dist = 2*n+1;
		int nearest_hunt1 = 0;
		for(k = 0; k < hunter_number; k++) {
			int dist = Manhattan(preys[j],hunters[k]);
			if(dist < min_dist) {
				min_dist = dist;
				nearest_hunt1 = k;
			}
		}


		// varsa en yakin ikinci hunter i bulalim
		int nearest_hunt2 = 0;
		if(hunter_number > 1) {
			min_dist = 2*n+1;
			for(k = 0; k < hunter_number; k++) {
				int dist = Manhattan(preys[j],hunters[k]);
				if(k != nearest_hunt1 && dist < min_dist) {
				  min_dist = dist;
				  nearest_hunt2 = k;
				}
			}
		}


		// Hamle yapmazsak en yakin iki hunter dan uzakligimiz ne olur
				hamle.x_coor = preys[j].x_coor;
				hamle.y_coor = preys[j].y_coor;
				if(hunter_number > 1)
					hold_nomove = Manhattan(hamle,hunters[nearest_hunt1]) +
									Manhattan(hamle,hunters[nearest_hunt2]);
				else
					hold_nomove = Manhattan(hamle,hunters[nearest_hunt1]);

		// asagi hamle yaparsak en yakin iki hunter dan uzakligimiz ne olur
		if(preys[j].y_coor-1 >= 0 && grid_map[preys[j].x_coor][preys[j].y_coor-1] != ENGEL && grid_map[preys[j].x_coor][preys[j].y_coor-1] != AVCI
													&& grid_map[preys[j].x_coor][preys[j].y_coor-1] != AV) {
			hamle.x_coor = preys[j].x_coor;
			hamle.y_coor = preys[j].y_coor-1;
			if(hunter_number > 1)
				down_move = Manhattan(hamle,hunters[nearest_hunt1]) +
								Manhattan(hamle,hunters[nearest_hunt2]);
			else
				down_move = Manhattan(hamle,hunters[nearest_hunt1]);
		}
		else  //Sinir, engel, av ya da avci var
			down_move = -1;


		// sola hamle yaparsak en yakin iki hunter dan uzakligimiz ne olur
		if(preys[j].x_coor-1 >= 0 && grid_map[preys[j].x_coor-1][preys[j].y_coor] != ENGEL && grid_map[preys[j].x_coor-1][preys[j].y_coor] != AVCI
													&& grid_map[preys[j].x_coor-1][preys[j].y_coor] != AV) {
			hamle.x_coor = preys[j].x_coor-1;
			hamle.y_coor = preys[j].y_coor;
			if(hunter_number > 1)
				left_move = Manhattan(hamle,hunters[nearest_hunt1]) +
								Manhattan(hamle,hunters[nearest_hunt2]);
			else
				left_move = Manhattan(hamle,hunters[nearest_hunt1]);
		}
		else //Sinir, engel, av ya da avci var
			left_move = -1;



		// yukari hamle yaparsak en yakin iki hunter dan uzakligimiz ne olur
		if(preys[j].y_coor+1 < n && grid_map[preys[j].x_coor][preys[j].y_coor+1] != ENGEL && grid_map[preys[j].x_coor][preys[j].y_coor+1] != AVCI
													&& grid_map[preys[j].x_coor][preys[j].y_coor+1] != AV) {
			hamle.x_coor = preys[j].x_coor;
			hamle.y_coor = preys[j].y_coor+1;
			if(hunter_number > 1)
				up_move = Manhattan(hamle,hunters[nearest_hunt1]) +
								Manhattan(hamle,hunters[nearest_hunt2]);
			else
				up_move = Manhattan(hamle,hunters[nearest_hunt1]);
		}
		else //Sinir, engel, av ya da avci var
			up_move = -1;

		// saga hamle yaparsak en yakin iki hunter dan uzakligimiz ne olur
				if(preys[j].x_coor+1 < n && grid_map[preys[j].x_coor+1][preys[j].y_coor] != ENGEL && grid_map[preys[j].x_coor+1][preys[j].y_coor] != AVCI
															&& grid_map[preys[j].x_coor+1][preys[j].y_coor] != AV) {
					hamle.x_coor = preys[j].x_coor+1;
					hamle.y_coor = preys[j].y_coor;
					if(hunter_number > 1)
						right_move = Manhattan(hamle,hunters[nearest_hunt1]) +
										Manhattan(hamle,hunters[nearest_hunt2]);
					else
						right_move = Manhattan(hamle,hunters[nearest_hunt1]);
				}
				else //Sinir, engel, av ya da avci var
					right_move = -1;


		if(left_move >= down_move && left_move >= right_move && left_move >= up_move && left_move >= hold_nomove) { // sola hamle en karlisiysa
			secilen_hamle[j].x_coor = preys[j].x_coor-1;
			secilen_hamle[j].y_coor = preys[j].y_coor;
		}
		else if(down_move >= left_move && down_move >= right_move && down_move >= up_move && down_move >= hold_nomove) {// asagi hamle en karlisiysa
			secilen_hamle[j].x_coor = preys[j].x_coor;
			secilen_hamle[j].y_coor = preys[j].y_coor-1;
		}
		else if(right_move >= left_move && right_move >= down_move && right_move >= up_move && right_move >= hold_nomove) { //saga hamle en karlisiysa
			secilen_hamle[j].x_coor = preys[j].x_coor+1;
			secilen_hamle[j].y_coor = preys[j].y_coor;
		}
		else if(up_move >= left_move && up_move >= down_move && up_move >= right_move && up_move >= hold_nomove) { // yukari hamle en karlisiysa
			secilen_hamle[j].x_coor = preys[j].x_coor;
			secilen_hamle[j].y_coor = preys[j].y_coor+1;
		}
		else if(hold_nomove >= left_move && hold_nomove >= down_move && hold_nomove >= right_move && hold_nomove >= up_move) { // durmak en karlisiysa
			secilen_hamle[j].x_coor = preys[j].x_coor;
			secilen_hamle[j].y_coor = preys[j].y_coor;
		}

	}

	return secilen_hamle;
}


void Uzaga_Git(agent_feats hunter, agent_feats prey, agent_feats * secilen_hamle, int ind) {


	agent_feats hamle;
	int left_move,down_move,right_move,up_move,hold_nomove;

	// left
	if(hunter.x_coor-1 >= 0 && grid_map[hunter.x_coor-1][hunter.y_coor] != ENGEL && grid_map[hunter.x_coor-1][hunter.y_coor] != AVCI
													&& grid_map[hunter.x_coor-1][hunter.y_coor] != AV) {
		hamle.x_coor = hunter.x_coor-1;
		hamle.y_coor = hunter.y_coor;
		left_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		left_move = -1;

	// down
	if(hunter.y_coor-1 >= 0 && grid_map[hunter.x_coor][hunter.y_coor-1] != ENGEL && grid_map[hunter.x_coor][hunter.y_coor-1] != AVCI
													&& grid_map[hunter.x_coor][hunter.y_coor-1] != AV) {
		hamle.x_coor = hunter.x_coor;
		hamle.y_coor = hunter.y_coor-1;
		down_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		down_move = -1;


	// right
	if(hunter.x_coor+1 < n && grid_map[hunter.x_coor+1][hunter.y_coor] != ENGEL && grid_map[hunter.x_coor+1][hunter.y_coor] != AVCI
													&& grid_map[hunter.x_coor+1][hunter.y_coor] != AV) {
		hamle.x_coor = hunter.x_coor+1;
		hamle.y_coor = hunter.y_coor;
		right_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		right_move = -1;

	// up
	if(hunter.y_coor+1 < n && grid_map[hunter.x_coor][hunter.y_coor+1] != ENGEL && grid_map[hunter.x_coor][hunter.y_coor+1] != AVCI
													&& grid_map[hunter.x_coor][hunter.y_coor+1] != AV) {
		hamle.x_coor = hunter.x_coor;
		hamle.y_coor = hunter.y_coor+1;
		up_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		up_move = -1;

	// yerimizde durursak ne olur
	hamle.x_coor = hunter.x_coor;
	hamle.y_coor = hunter.y_coor;
	hold_nomove = Manhattan(hamle,prey);

	if(left_move >= down_move && left_move >= right_move && left_move >= up_move && left_move >= hold_nomove) { // left
		secilen_hamle[ind].x_coor = hunter.x_coor-1;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	else if(down_move >= left_move && down_move >= right_move && down_move >= up_move && down_move >= hold_nomove) { // down
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor-1;
	}
	else if(right_move >= left_move && right_move >= down_move && right_move >= up_move && right_move >= hold_nomove) { // right
		secilen_hamle[ind].x_coor = hunter.x_coor+1;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	else if(up_move >= left_move && up_move >= down_move && up_move >= right_move && up_move >= hold_nomove) { // up
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor+1;
	}
	else if(hold_nomove >= left_move && hold_nomove >= down_move && hold_nomove >= right_move && hold_nomove >= up_move) { // stay still
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	return;
}


void Yakina_Gel(agent_feats hunter, agent_feats prey, agent_feats * secilen_hamle, int ind) {

	agent_feats hamle;
	int left_move,down_move,right_move,up_move,hold_nomove;

	// left
	if(hunter.x_coor-1 >= 0 && grid_map[hunter.x_coor-1][hunter.y_coor] != ENGEL && grid_map[hunter.x_coor-1][hunter.y_coor] != AVCI
													&& grid_map[hunter.x_coor-1][hunter.y_coor] != AV) {
		hamle.x_coor = hunter.x_coor-1;
		hamle.y_coor = hunter.y_coor;
		left_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		left_move = 2*n+1;

	// down
	if(hunter.y_coor-1 >= 0 && grid_map[hunter.x_coor][hunter.y_coor-1] != ENGEL && grid_map[hunter.x_coor][hunter.y_coor-1] != AVCI
													&& grid_map[hunter.x_coor][hunter.y_coor-1] != AV) {
		hamle.x_coor = hunter.x_coor;
		hamle.y_coor = hunter.y_coor-1;
		down_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		down_move = 2*n+1;


	// right
	if(hunter.x_coor+1 < n && grid_map[hunter.x_coor+1][hunter.y_coor] != ENGEL && grid_map[hunter.x_coor+1][hunter.y_coor] != AVCI
													&& grid_map[hunter.x_coor+1][hunter.y_coor] != AV) {
		hamle.x_coor = hunter.x_coor+1;
		hamle.y_coor = hunter.y_coor;
		right_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		right_move = 2*n+1;

	// up
	if(hunter.y_coor+1 < n && grid_map[hunter.x_coor][hunter.y_coor+1] != ENGEL && grid_map[hunter.x_coor][hunter.y_coor+1] != AVCI
													&& grid_map[hunter.x_coor][hunter.y_coor+1] != AV) {
		hamle.x_coor = hunter.x_coor;
		hamle.y_coor = hunter.y_coor+1;
		up_move = Manhattan(hamle,prey);
	}
	else // engel,sinir,av ya da avci var
		up_move = 2*n+1;

	// yerimizde durursak ne olur
	hamle.x_coor = hunter.x_coor;
	hamle.y_coor = hunter.y_coor;
	hold_nomove = Manhattan(hamle,prey);

	if(left_move <= down_move && left_move <= right_move && left_move <= up_move && left_move <= hold_nomove) { // left
		secilen_hamle[ind].x_coor = hunter.x_coor-1;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	else if(down_move <= left_move && down_move <= right_move && down_move <= up_move && down_move <= hold_nomove) { // down
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor-1;
	}
	else if(right_move <= left_move && right_move <= down_move && right_move <= up_move && right_move <= hold_nomove) { // right
		secilen_hamle[ind].x_coor = hunter.x_coor+1;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	else if(up_move <= left_move && up_move <= down_move && up_move <= right_move && up_move <= hold_nomove) { // up
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor+1;
	}
	else if(hold_nomove <= left_move && hold_nomove <= down_move && hold_nomove <= hold_nomove && hold_nomove <= up_move) { // dur
		secilen_hamle[ind].x_coor = hunter.x_coor;
		secilen_hamle[ind].y_coor = hunter.y_coor;
	}
	return;
}

agent_feats * Olenleri_Kaldir(int type) {
	int j,k;
	int olu_counter = 0;


	if(type == AV) {
		for(j = 0; j < prey_number; j++) {
			if(!preys[j].live)
				olu_counter++;
		}
		agent_feats temp[prey_number-olu_counter]; // kalan prey sayisi kadar gecici bellek acalim
		for(j = 0, k = 0; j < prey_number; j++) {
			if(preys[j].live) { // canlilari gecici bellege alalim
				temp[k].live = preys[j].live;
				temp[k].energy = preys[j].energy;
				temp[k].x_coor = preys[j].x_coor;
				temp[k].y_coor = preys[j].y_coor;
				temp[k].agent_index = preys[j].agent_index;
				k++;
			}
			else
				grid_map[preys[j].x_coor][preys[j].y_coor] = BOS;
		}
		free(preys);   //butun preyleri kaldiralim
		prey_number -= olu_counter;  // yeni prey sayisini hesaplayalim

		preys = malloc(sizeof(agent_feats)*prey_number); // yeni prey sayisi kadar memory acalim

		for(j = 0; j < prey_number; j++) {
			preys[j].x_coor = temp[j].x_coor;
			preys[j].y_coor = temp[j].y_coor;
			preys[j].live = temp[j].live;
			preys[j].energy = temp[j].energy;
			preys[j].agent_index = temp[j].agent_index;
		}
		return preys;

	}
	else {
		for(j = 0; j < hunter_number; j++){
			if(!hunters[j].live)
				olu_counter++;
		}
		agent_feats temp[hunter_number-olu_counter]; //yeni hunter sayisi kadar gecici bellek
		for(j = 0, k = 0; j < hunter_number; j++) {
			if(hunters[j].live) {
				temp[k].live = hunters[j].live;
				temp[k].energy = hunters[j].energy;
				temp[k].x_coor = hunters[j].x_coor;
				temp[k].y_coor = hunters[j].y_coor;
				temp[k].agent_index = hunters[j].agent_index;
				k++;
			}
			else
				grid_map[hunters[j].x_coor][hunters[j].y_coor] = BOS;
		}

		free(hunters);  //tum hunterlari temizle
		hunter_number -= olu_counter;  // yeni hunter sayisini hesapla

		hunters = malloc(sizeof(agent_feats)*hunter_number);  //yeni hunter sayisi kadar bellek acalim

		for(j = 0; j < hunter_number; j++) {
			 hunters[j].x_coor = temp[j].x_coor;
			 hunters[j].y_coor = temp[j].y_coor;
			 hunters[j].live = temp[j].live;
			 hunters[j].energy = temp[j].energy;
			 hunters[j].agent_index = temp[j].agent_index;
		}
		return hunters;
	}

}

agent_feats * runHunterPlan() {
	int j,k;
	agent_feats * decisions = malloc(sizeof(agent_feats)*hunter_number);

	// first, if there are adj/same cell preys eat them
	// keep the eater hunters and eaten preys, -1 means hunter j eat nothing
	printf("check for eaten preys\n");
	int eaters[hunter_number];
	for(j = 0; j < hunter_number; j++)
		eaters[j] = -1;

	for(j = 0; j < hunter_number; j++) {
		for(k = 0; k < prey_number; k++) {
			int dist = Manhattan(hunters[j],preys[k]);
			if(dist <= 1) {
				eaters[j] = k;
				break;                        //her hunter ayni anda bir prey yiyebilir
			}
		}
	}

	// for each eaten kth prey, increment the eaters energy accordingly
	int anyEaten = 0;
	for(k = 0; k < prey_number; k++) {
		int numbOfEaters = 0;
		for(j = 0; j < hunter_number; j++) {
			if(eaters[j] == k) {
				numbOfEaters++;
			}
		}

		// set the eaten prey dead
		if(numbOfEaters > 0) {
			printf("the prey at %d,%d has been eaten by %d number of hunters\n",preys[k].x_coor,preys[k].y_coor,numbOfEaters);
			preys[k].live = false;
			anyEaten = 1;
		}

		// increment the energy
		for(j = 0; j < hunter_number; j++) {
			if(eaters[j] == k) {
				hunters[j].energy += (R/numbOfEaters);
				printf("the hunter at %d,%d has eaten the prey at %d,%d and its energy is incremented by %f, becomes %f\n"
										,hunters[j].x_coor,hunters[j].y_coor,preys[k].x_coor,preys[k].y_coor,R/numbOfEaters,hunters[j].energy);
			}
		}
	}
	// remove the eaten preys from the space
	if(anyEaten)
		preys = Olenleri_Kaldir(AV);
	else
		printf("there is no eaten prey in this time step\n");
	printf("the number of remaining preys is %d\n",prey_number);

	// check for no energy hunters
	printf("check for dead hunters\n");
	int anyDead = 0;
	for(j = 0; j < hunter_number; j++) {
		if(hunters[j].energy < T) {
			printf("the hunter at %d,%d has energy below the threshold, %f, so it will be removed from the environment\n"
																				,hunters[j].x_coor,hunters[j].y_coor,hunters[j].energy);
			hunters[j].live = false;
			anyDead = 1;
		}
	}
	if(anyDead)
		hunters = Olenleri_Kaldir(AVCI);
	else
		printf("there is no dead hunter in this time step\n");
	printf("the number of remaining hunters is %d\n",hunter_number);

	// main decision loop, for each hunter dikkat (kadir)
	for(j = 0; j < hunter_number; j++) {
		printf("------- main decision loop for hunter %d,%d  -----------------------------------\n",hunters[j].x_coor,hunters[j].y_coor);
		hunters[j].close_to_prey = false;   // once etrafinda prey var bilgisini silelim

		// if no energy to move, just stay still, vision da prey varsa bile raporlamasin, close to prey false kalsin basklari gelmesin
		if(hunters[j].energy <= T+1) {
			decisions[j].x_coor = hunters[j].x_coor;
			decisions[j].y_coor = hunters[j].y_coor;
			printf("the hunter at %d,%d does not have enough energy to move, %f, so stays still\n"
														,hunters[j].x_coor,hunters[j].y_coor,hunters[j].energy);
			continue;
		}

		// check the preys in the observable area
		int preyDists[prey_number];
		for(k = 0; k < prey_number; k++)
			preyDists[k] = 2*n+1;

		int numbOfFoundPreys = 0;

		for(k = 0; k < prey_number; k++) {
			if(preys[k].x_coor <= hunters[j].x_coor+d && preys[k].x_coor >= hunters[j].x_coor-d &&
							preys[k].y_coor <= hunters[j].y_coor+d && preys[k].y_coor >= hunters[j].y_coor-d) {
				preyDists[k] = Manhattan(preys[k],hunters[j]);
				numbOfFoundPreys++;
				hunters[j].close_to_prey = true;
			}
		}

		if(numbOfFoundPreys > 0) {
			printf("the hunter at %d,%d sees %d many preys in its observable area, and will try to get close to the nearest one\n"
																				,hunters[j].x_coor,hunters[j].y_coor,numbOfFoundPreys);
			// find the nearest prey in the observable area
			int minDist = preyDists[0];
			int ind = 0;
			for(k = 0; k < prey_number; k++) {
				if(preyDists[k] < minDist) {
					ind = k;
					minDist = preyDists[k];
				}
			}
			printf("the closest prey is at %d,%d, the hunter at %d,%d will try to get close to it\n"
																	,preys[ind].x_coor,preys[ind].y_coor,hunters[j].x_coor,hunters[j].y_coor);
			// find the action that make the hunter closer to the prey,
			// same as the prey displacement maximization strategy, but minimization.
			Yakina_Gel(hunters[j],preys[ind],decisions,j);
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
				int hunterDists[hunter_number];
				for(k = 0; k < hunter_number; k++)
					hunterDists[k] = 2*n+1;

				int numbOfFoundHunters = 0;
				for(k = 0; k < hunter_number; k++) {
					if(k != j)
						if(hunters[k].x_coor <= hunters[j].x_coor+d && hunters[k].x_coor >= hunters[j].x_coor-d &&
										hunters[k].y_coor <= hunters[j].y_coor+d && hunters[k].y_coor >= hunters[j].y_coor-d) {
							hunterDists[k] = Manhattan(hunters[k],hunters[j]);
							numbOfFoundHunters++;
						}
				}


				if(numbOfFoundHunters > 0) {
					//observable alanda hunter varsa uzaklasalim
					printf("the hunter at %d,%d sees %d many hunters in its observable area, since %d is below the threshold %d, it will try to get close to the nearest one\n",hunters[j].x_coor,hunters[j].y_coor,numbOfFoundHunters,numbOfFoundHunters,d);
					int minDist = hunterDists[0];
					int ind = 0;
					for(k = 0; k < hunter_number; k++) {
						if(hunterDists[k] < minDist) {
							ind = k;
							minDist = hunterDists[k];
						}
					}

					if(hunters[ind].close_to_prey)
					  Yakina_Gel(hunters[j],hunters[ind],decisions,j);
					else
					  Uzaga_Git(hunters[j],hunters[ind],decisions,j);
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
			}
		}
	}

	return decisions;
}

void runReactiveMultiAgentPlan() {
	int j,k;
	while(hunter_number > 0 && prey_number > 0) {
		// each decides what to do first, then it is applied together.
		// as a special condition eating a prey will be done instantenously.
		agent_feats * decisionHunters = runHunterPlan(); // hunters decisions changes
																	   			// the environment by removing preys.
		agent_feats * decisionPreys = runPreyPlan();




		// apply decisions for preys

		printf("PREY DECISIONS\n");
		// collision cases
		if(prey_number <= 0)
			printf("There is no remaining prey so the plan will end at the next time step\n");
		for(j = 0; j < prey_number; j++) {
			for(k = 0; k < prey_number; k++) {
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
			if(grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] == ENGEL) {
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

					if(grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] == ENGEL) { // if still an obstacle just stay still
							decisionPreys[j].x_coor = preys[j].x_coor;
							decisionPreys[j].y_coor = preys[j].y_coor;
					}
			}

			if(preys[j].x_coor == decisionPreys[j].x_coor && preys[j].y_coor == decisionPreys[j].y_coor) {
				printf("HOLD_STILL: the prey at %d,%d moves to the position %d,%d\n"
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
			grid_map[preys[j].x_coor][preys[j].y_coor] = BOS;
			grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] = AV;
			//grid_map_index[decisionPreys[j].x_coor][decisionPreys[j].y_coor] = j;
			preys[j].x_coor = decisionPreys[j].x_coor;
			preys[j].y_coor = decisionPreys[j].y_coor;
		}


		// apply decisions for hunters

		printf("HUNTER DECISIONS\n");
		if(hunter_number <= 0)
			printf("There is no remaining hunter so the plan will end at the next time step\n");

		// collision cases
		for(j = 0; j < hunter_number; j++) {
			for(k = 0; k < hunter_number; k++) {
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
			if(grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] == ENGEL) {
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

					if(grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] == ENGEL) { // if still an obstacle just stay still
							decisionHunters[j].x_coor = hunters[j].x_coor;
							decisionHunters[j].y_coor = hunters[j].y_coor;
					}

					hunters[j].energy -= 1;	// in collision case, they do not move to anywhere, just looses energy
			}
			// decrement the energy levels accordingly
			if(hunters[j].x_coor == decisionHunters[j].x_coor && hunters[j].y_coor == decisionHunters[j].y_coor) {
				printf("HOLD_STILL: the hunter at %d,%d moves to the position %d,%d, HOLD_STILL, and "
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
			grid_map[hunters[j].x_coor][hunters[j].y_coor] = BOS;
			grid_map[decisionHunters[j].x_coor][decisionHunters[j].y_coor] = AVCI;
			//grid_map_index[decisionHunters[j].x_coor][decisionHunters[j].y_coor] = j;
			hunters[j].x_coor = decisionHunters[j].x_coor;
			hunters[j].y_coor = decisionHunters[j].y_coor;
		}



		printEnv();
		printf("%d. time step is finished, press enter to continue...\n",time);
		time++;
		Bekle();
	}

	return;
}

int main(void) {
	FILE * input_file = fopen("/home/kadir/workspace/ma_systems/HW1/environment.inp","r");
	int j,k;


	fscanf(input_file,"%d %d %f %f %f",&n,&d,&e,&R,&T);  //girdileri okuyalim

	fgetc(input_file);                  //yeni satiri okuyalim


	grid_map = malloc(sizeof(int *)*n);

	for(j = 0; j < n; j++)
		grid_map[j] = malloc(sizeof(int)*n);

	for(j = 0; j < n; j++)
		for(k = 0; k < n; k++)
			grid_map[j][k] = BOS;



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
			grid_map[x_coor-1][y_coor-1] = AVCI;
			hunter_number++;
		}
		else if(object == 'p') {
			grid_map[x_coor-1][y_coor-1] = AV;
			prey_number++;
		}
		else if(object == 'o')
			grid_map[x_coor-1][y_coor-1] = ENGEL;
		else
			printf("ERROR while reading objects from input, obstacle: ----%c----\n",object);
	}
	// save hunters and preys in a separate array
	hunters = malloc(sizeof(agent_feats)*hunter_number);
	preys = malloc(sizeof(agent_feats)*prey_number);
	int huntInd = 0;
	int preyInd = 0;
	for(j = 0; j < n; j++) {
		for(k = 0; k < n; k++) {
			if(grid_map[j][k] == AVCI) {
				//grid_map_index[j][k] = huntInd;
				hunters[huntInd].x_coor = j;
				hunters[huntInd].y_coor = k;
				hunters[huntInd].live = true;
				hunters[huntInd].energy = e;
				hunters[huntInd].agent_index = huntInd;
				huntInd++;
			}
			else if(grid_map[j][k] == AV) {
				//grid_map_index[j][k] = preyInd;
				preys[preyInd].x_coor = j;
				preys[preyInd].y_coor = k;
				preys[preyInd].live = true;
				preys[preyInd].agent_index = preyInd;
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



	printf("The first form of the environment is as below,\n");
	printf("'-' means empty cell,\n'X' means an obstacle,\n'H' means a hunter,\n'P' means a prey:\n");
	printEnv();
	printf("Simulasyonu baslatmak icin Enter a basiniz\n");

	Bekle();  // this for starting the plan

	printf("****************************************************************************************\n");
	printf("************************** Reactive planning started.  *********************************\n");
	printf("****************************************************************************************\n\n");
	runReactiveMultiAgentPlan();
	printf("****************************************************************************************\n");
	printf("************************** Reactive planning finished. *********************************\n");
	printf("****************************************************************************************\n");

	return 0;
}
