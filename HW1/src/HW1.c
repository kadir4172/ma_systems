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


agent_feats * Av_Kararlari() {
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

agent_feats * Avci_Kararlari() {
	int j,k;
	agent_feats * decision_to_move = malloc(sizeof(agent_feats)*hunter_number);


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


	int yem_oldu = 0;
	for(k = 0; k < prey_number; k++) {
		int yiyici_sayisi = 0;
		for(j = 0; j < hunter_number; j++) {
			if(eaters[j] == k) {
				yiyici_sayisi++;
			}
		}

		//yem olan avi oldu olarak isaretleyelim
		if(yiyici_sayisi > 0) {

			preys[k].live = false;
			yem_oldu = 1;
		}

		//yiyenlerin enerjilerini arttiralim
		int total_energy_of_eaters = 0;
		for(j = 0; j < hunter_number; j++){
			if(eaters[j] == k) {
				total_energy_of_eaters += hunters[j].energy;
		    }
		}
		for(j = 0; j < hunter_number; j++){
			if(eaters[j] == k) {
				hunters[j].energy += (R * hunters[j].energy) / total_energy_of_eaters;
			}
		}
	}
	//olenleri kaldiralim
	if(yem_oldu)
		preys = Olenleri_Kaldir(AV);


	// enerjisi biten hunter lari bulalim
	int olen_var = 0;
	for(j = 0; j < hunter_number; j++) {
		if(hunters[j].energy < T) {
			hunters[j].live = false;
			olen_var = 1;
		}
	}
	if(olen_var)
		hunters = Olenleri_Kaldir(AVCI);


	// eger yiyebilcekleri bir prey yoksa ve olmedilerse hunterlar asagidaki rutinlere gore hareket ederler
	for(j = 0; j < hunter_number; j++) {
		hunters[j].close_to_prey = false;   // once etrafinda prey var bilgisini silelim

		//yeteri kadar enerjin yoksa, vision da prey varsa bile raporlama, close to prey false kalsin basklari gelmesin
		if(hunters[j].energy <= T+1) {
			decision_to_move[j].x_coor = hunters[j].x_coor;
			decision_to_move[j].y_coor = hunters[j].y_coor;
			continue;
		}

		// yeteri kadar enerjin varsa hareketini belirlemek icin etrafindak avlari gozlemle
		int av_mesafesi[prey_number];
		for(k = 0; k < prey_number; k++)
			av_mesafesi[k] = 2*n+1;

		int yakin_av_sayisi = 0;

		for(k = 0; k < prey_number; k++) {
			if(preys[k].x_coor <= hunters[j].x_coor+d && preys[k].x_coor >= hunters[j].x_coor-d &&
							preys[k].y_coor <= hunters[j].y_coor+d && preys[k].y_coor >= hunters[j].y_coor-d) {
				av_mesafesi[k] = Manhattan(preys[k],hunters[j]);
				yakin_av_sayisi++;
				hunters[j].close_to_prey = true;
			}
		}

		if(yakin_av_sayisi > 0) {

			// birden fazla yakin av varsa en yakini bul
			int en_yakin_mesafe = av_mesafesi[0];
			int ind = 0;
			for(k = 0; k < prey_number; k++) {
				if(av_mesafesi[k] < en_yakin_mesafe) {
					ind = k;
					en_yakin_mesafe = av_mesafesi[k];
				}
			}
			Yakina_Gel(hunters[j],preys[ind],decision_to_move,j);  // j hunter i icin en yakin av a en yakin hamleyi bulalim
		}
		else {
			//gorunurde av yok, az enerjin varsa bekle birsey yapma
			if(hunters[j].energy <= T+2) {
				decision_to_move[j].x_coor = hunters[j].x_coor;
				decision_to_move[j].y_coor = hunters[j].y_coor;
			}
			//gorunurde av yok ama hunterin enerjisi var
			else {
				//en yakin avciyi bulalim
				int avci_mesafeleri[hunter_number];
				for(k = 0; k < hunter_number; k++)
					avci_mesafeleri[k] = 2*n+1;

				int bulunan_avci_sayisi = 0;
				for(k = 0; k < hunter_number; k++) {
					if(k != j)
						if(hunters[k].x_coor <= hunters[j].x_coor+d && hunters[k].x_coor >= hunters[j].x_coor-d &&
										hunters[k].y_coor <= hunters[j].y_coor+d && hunters[k].y_coor >= hunters[j].y_coor-d) {
							avci_mesafeleri[k] = Manhattan(hunters[k],hunters[j]);
							bulunan_avci_sayisi++;
						}
				}

				//observable alanda hunter varsa
				if(bulunan_avci_sayisi > 0) {
					int en_yakin_mesafe = avci_mesafeleri[0];
					int ind = 0;
					for(k = 0; k < hunter_number; k++) {
						if(avci_mesafeleri[k] < en_yakin_mesafe) {
							ind = k;
							en_yakin_mesafe = avci_mesafeleri[k];
						}
					}

					if(hunters[ind].close_to_prey)
					  Yakina_Gel(hunters[j],hunters[ind],decision_to_move,j); //eger en yakindaki avcinin hedefinde bir prey varsa ona yaklasalim
					else
					  Uzaga_Git(hunters[j],hunters[ind],decision_to_move,j); //eger en yakindaki avcinin da hedefinde prey yoksa ondan uzaklasalim
				}
				else {
					//gorunurde avci da yok, rastgele dolasalim , rastgele dolasirken boundarylere carpmadan random gezinelim
					int random_action = rand() % 5;
					if(random_action == 0) {
						if(hunters[j].x_coor-1 >= 0)
							decision_to_move[j].x_coor = hunters[j].x_coor-1;
						else
							decision_to_move[j].x_coor = hunters[j].x_coor+1;
						    decision_to_move[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 1) {
						decision_to_move[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decision_to_move[j].y_coor = hunters[j].y_coor-1;
						else
							decision_to_move[j].y_coor = hunters[j].y_coor+1;
					}
					else if(random_action == 2) {
						if(hunters[j].x_coor+1 < n)
							decision_to_move[j].x_coor = hunters[j].x_coor+1;
						else
							decision_to_move[j].x_coor = hunters[j].x_coor-1;
						decision_to_move[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 3) {
						decision_to_move[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decision_to_move[j].y_coor = hunters[j].y_coor+1;
						else
							decision_to_move[j].y_coor = hunters[j].y_coor-1;
					}
					else if(random_action == 4) {
						decision_to_move[j].x_coor = hunters[j].x_coor;
						decision_to_move[j].y_coor = hunters[j].y_coor;
					}
				}
			}
		}
	}

	return decision_to_move;
}

void runReactiveMultiAgentPlan() {
	int j,k;
	while(hunter_number > 0 && prey_number > 0) {

		agent_feats * decisionHunters = Avci_Kararlari(); //o anki duruma gore avcilarin kararlarini belirleyelim

		agent_feats * decisionPreys = Av_Kararlari();     // o anki duruma gore avlarin kararlarini belirleyelim





		//Tum kararlar ayni anda alinda, carpisma durumlarini burada handle edelim
		if(prey_number <= 0)
			printf("Ortamda av kalmadi bu adimda simulasyon sonlandirilacaktir.\n");
		for(j = 0; j < prey_number; j++) {
			for(k = 0; k < prey_number; k++) {
				if(j != k && decisionPreys[j].x_coor == decisionPreys[k].x_coor && decisionPreys[j].y_coor == decisionPreys[k].y_coor) {
					printf("COLLISION: the prey at %d,%d has collided with the prey at %d,%d while moving to %d,%d and they will spill out randomly\n"
									,preys[j].x_coor,preys[j].y_coor,preys[k].x_coor,preys[k].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
					// after a collision it moves randomly, note that it looses energy both for collision and random move
					int random_action = rand() % 5;
					if(random_action == 0) {	// up
						if(preys[j].x_coor-1 >= 0)
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(random_action == 1) { // left
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor-1 >= 0)
							decisionPreys[j].y_coor = preys[j].y_coor-1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(preys[j].x_coor+1 < n)
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor+1 < n)
							decisionPreys[j].y_coor = preys[j].y_coor+1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
						decisionPreys[j].x_coor = preys[j].x_coor;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}

					// not to collide again
					int prevact = random_action;
					random_action = rand() % 5;
					if(random_action == prevact && random_action < 4)
						random_action += 1;
					else if(random_action == prevact && random_action == 4)
						random_action -= 1;

					if(random_action == 0) {	// up
						if(preys[k].x_coor-1 >= 0)
							decisionPreys[k].x_coor = preys[k].x_coor-1;
						else
							decisionPreys[k].x_coor = preys[k].x_coor+1;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					else if(random_action == 1) { // left
						decisionPreys[k].x_coor = preys[k].x_coor;
						if(preys[k].y_coor-1 >= 0)
							decisionPreys[k].y_coor = preys[k].y_coor-1;
						else
							decisionPreys[k].y_coor = preys[k].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(preys[k].x_coor+1 < n)
							decisionPreys[k].x_coor = preys[k].x_coor+1;
						else
							decisionPreys[k].x_coor = preys[k].x_coor-1;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionPreys[k].x_coor = preys[k].x_coor;
						if(preys[k].y_coor+1 < n)
							decisionPreys[k].y_coor = preys[k].y_coor+1;
						else
							decisionPreys[k].y_coor = preys[k].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
						decisionPreys[k].x_coor = preys[k].x_coor;
						decisionPreys[k].y_coor = preys[k].y_coor;
					}
					break;
				}
			}
			if(grid_map[decisionPreys[j].x_coor][decisionPreys[j].y_coor] == ENGEL) {
				printf("COLLISION: the prey at %d,%d has collided with the obstacle at %d,%d and it will spill out randomly\n"
											,preys[j].x_coor,preys[j].y_coor,decisionPreys[j].x_coor,decisionPreys[j].y_coor);
					int random_action = rand() % 5;
					if(random_action == 0) {	// up
						if(preys[j].x_coor-1 >= 0)
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(random_action == 1) { // left
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor-1 >= 0)
							decisionPreys[j].y_coor = preys[j].y_coor-1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(preys[j].x_coor+1 < n)
							decisionPreys[j].x_coor = preys[j].x_coor+1;
						else
							decisionPreys[j].x_coor = preys[j].x_coor-1;
						decisionPreys[j].y_coor = preys[j].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionPreys[j].x_coor = preys[j].x_coor;
						if(preys[j].y_coor+1 < n)
							decisionPreys[j].y_coor = preys[j].y_coor+1;
						else
							decisionPreys[j].y_coor = preys[j].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
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
					int random_action = rand() % 5;
					if(random_action == 0) {	// up
						if(hunters[j].x_coor-1 >= 0)
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 1) { // left
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(hunters[j].x_coor+1 < n)
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
						decisionHunters[j].x_coor = hunters[j].x_coor;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					// not to collide again
					int prevact = random_action;
					random_action = rand() % 5;
					if(random_action == prevact && random_action < 4)
						random_action += 1;
					else if(random_action == prevact && random_action == 4)
						random_action -= 1;

					if(random_action == 0) {	// up
						if(hunters[k].x_coor-1 >= 0)
							decisionHunters[k].x_coor = hunters[k].x_coor-1;
						else
							decisionHunters[k].x_coor = hunters[k].x_coor+1;
						decisionHunters[k].y_coor = hunters[k].y_coor;
					}
					else if(random_action == 1) { // left
						decisionHunters[k].x_coor = hunters[k].x_coor;
						if(hunters[k].y_coor-1 >= 0)
							decisionHunters[k].y_coor = hunters[k].y_coor-1;
						else
							decisionHunters[k].y_coor = hunters[k].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(hunters[k].x_coor+1 < n)
							decisionHunters[k].x_coor = hunters[k].x_coor+1;
						else
							decisionHunters[k].x_coor = hunters[k].x_coor-1;
						decisionHunters[k].y_coor = hunters[k].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionHunters[k].x_coor = hunters[k].x_coor;
						if(hunters[k].y_coor+1 < n)
							decisionHunters[k].y_coor = hunters[k].y_coor+1;
						else
							decisionHunters[k].y_coor = hunters[k].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
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
					int random_action = rand() % 5;
					if(random_action == 0) {	// up
						if(hunters[j].x_coor-1 >= 0)
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 1) { // left
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor-1 >= 0)
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
					}
					else if(random_action == 2) {	// down
						if(hunters[j].x_coor+1 < n)
							decisionHunters[j].x_coor = hunters[j].x_coor+1;
						else
							decisionHunters[j].x_coor = hunters[j].x_coor-1;
						decisionHunters[j].y_coor = hunters[j].y_coor;
					}
					else if(random_action == 3) {	// right
						decisionHunters[j].x_coor = hunters[j].x_coor;
						if(hunters[j].y_coor+1 < n)
							decisionHunters[j].y_coor = hunters[j].y_coor+1;
						else
							decisionHunters[j].y_coor = hunters[j].y_coor-1;
					}
					else if(random_action == 4) {	// stay still
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
