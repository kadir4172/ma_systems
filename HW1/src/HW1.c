/*
 ============================================================================
 Name        : HW1.c
 Author      : Kadir Cimenci
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"

int ** grid_map;
int n,d,e,R ;
float T;

int Hunter_count = 0;
int Prey_count = 0;

#define EMPTY     0;
#define HUNTER    1;
#define PREY      2;
#define OBSTACLE  3;



typedef struct {
	bool live ;
    float energy;
	int x_coor;
	int y_coor;
} agent_info;


int main(void) {
	puts("!!!HW1!!!");
	FILE * input_file = fopen("environment.inp","r");
	int k,l;
	grid_map = malloc(sizeof(int *)*n);                    // n integer icin memory allocation
    fscanf(input_file,"%d %d %d %d %f",&n,&d,&e,&R,&T);   // ilk 5 veriyi input dosyasindan okuyalim

    fgetc(input_file);

    for(k = 0; k < n; k++){
    		grid_map[k] = malloc(sizeof(int)*n);           //array in her elemani icin n integer lik memory acalim
    }

    for(k = 0; k < n; k++){
    		for(l = 0; l < n; l++){
    			grid_map[k][l] = EMPTY;                   //butun haritayi bos dolduralim
    		}
    }

	char cell_type;
	int  x_ind, y_ind;
	while((fscanf(input_file,"%c %d %d", &cell_type, &x_ind, &y_ind)) != EOF) {
		if(cell_type == '\n') {
			fgetc(input_file);                         //yeni satira gecelim
			continue;
		}
		fgetc(input_file);

		if(cell_type == 'h') {
			grid_map[x_ind-1][y_ind-1] = HUNTER;
			Hunter_count++;
		}
		else if(cell_type == 'p') {
			grid_map[x_ind-1][y_ind-1] = PREY;
			Prey_count++;
		}
		else if(cell_type == 'o'){
			grid_map[x_ind-1][y_ind-1] = OBSTACLE;
		}
		else
			printf("Girdi dosyasinda bilinmeyen karakter saptandi");
	}




    //puts("!!!HomeWork1!!!"); /* prints !!!HomeWork1!!! */
	//return EXIT_SUCCESS;
}
