#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct _som {
	float*** weights;
	int** hits;
	int x;
	int y;
	int z;
} Som;

void PrintWeights(Som* s){

	int i, j, k;

	for ( i = 0; i < s->x; i++){
		for (j = 0; j < s->y; j++){
			for (k = 0; k < s->z; k++){
		 		printf("%d ", (int)(s->weights[i][j][k]*255));
			}
			printf("\n");
		}
	}

}

void PrintHits(Som* s){
	int i, j;

	for ( i = 0; i < s->x; i++){
		for (j = 0; j < s->y; j++)
			printf("%d ", s->hits[i][j]);
		printf("\n");
	}
}

void UMatrix(Som* s){

	/*int i, j, k;

	for (i = 0; i < s->x; i++)
		for (j = 0; j < s->y; j++){

			for (k = 0; k < s->z; k++){
		}
		*/

}

Som* InitSom(int x, int y, int z){

	int i, j, k;

	Som * s = malloc(sizeof(Som));

	s->x = x; s->y = y; s->z = z;
	s->weights = malloc(sizeof(float**)*x*y);
	s->hits = malloc(sizeof(int**)*x*y);

	for (i = 0; i < x; i++){
		s->weights[i] = malloc(sizeof(float*)*y);
		s->hits[i] = malloc(sizeof(float)*y);
	}

	for (i = 0; i < x; i++)
		for (j = 0; j < y; j++){
			s->weights[i][j] = malloc(sizeof(float)*z);
			s->hits[i][j] = 0;
			for (k = 0; k < z; k++)
				s->weights[i][j][k] = (float) rand() / (float) RAND_MAX;
		}
	
	return s;
}

void FreeSom(Som* s){

	if (s == NULL)
		return;

	int i, j, k;
	for (i = 0; i< s->x; i++){
		for (j = 0; j < s->y; j++)
			free(s->weights[i][j]);
		free(s->weights[i]);
		free(s->hits[i]);
	}

	free(s->weights);
	free(s->hits);
}

void InitWeights(Som* s){

	if (s == NULL)
		return;

	int i, j, k;

	for ( i = 0; i < s->x; i++)
		for (j = 0; j < s->y; j++)
			for (k = 0; k < s->z; k++)
				s->weights[i][j][k] = (float) rand() / (float) RAND_MAX;
}

//param2 = 1000
void Train(Som* s, float** features, int nfeatures, int maxIterations, float param2, float initialLearningRate){

	if ( s == NULL || features == NULL)
		return;

	float initialRadius = (s->x)/2.0;
	float param1 = maxIterations/log(initialRadius);
	float radius = initialRadius;
	int i, j, k, n;
	n = 0;
	float learningRate = initialLearningRate;

	while ( n < maxIterations){

		int idx = rand() % nfeatures;
		float mindist = (float) RAND_MAX;
		int winnerx, winnery;

		for ( i = 0; i < s->x; i++){
			for ( j = 0; j < s->y; j++){
				float dist = 0;
				for ( k = 0; k < s->z; k++)
					dist += pow( s->weights[i][j][k] - features[idx][k], 2);
				dist = sqrt(dist);
				if (dist < mindist){
					winnerx = i;
					winnery = j;
					mindist = dist;
				}
			}
		}

		s->hits[winnerx][winnery] += 1;

		for ( i = 0; i < s->x; i++)
			for (j = 0; j < s->y; j++){
				float dist2 =  (winnerx - i)*(winnerx - i)  + (winnery - j)*(winnery - j);
				float neightborhood = expf( -(dist2*dist2)/(2*radius*radius) );
				for ( k = 0; k < s->z; k++)
					s->weights[i][j][k] += learningRate*neightborhood*(features[idx][k] - s->weights[i][j][k]);
			}

		learningRate = initialLearningRate*expf(-n/param2);
		radius = initialRadius*expf(-n/param1);
		n++;
	}
}

int main() {

	Som* s = InitSom(10, 10, 3);

	int i;
	float** features;
	features = malloc(sizeof(float*)*8);
	for (i = 0; i < 8; i++)
		features[i] = malloc(sizeof(float)*3);

	features[0][0] = 1.0;
	features[0][1] = 0.0;
	features[0][2] = 0.0;

	features[1][0] = 0.0;
	features[1][1] = 1.0;
	features[2][2] = 0.0;

	features[2][0] = 0.0;
	features[2][1] = 0.0;
	features[2][2] = 1.0;

	features[3][0] = 0.0;
	features[3][1] = 0.5;
	features[3][2] = 0.25;

	features[4][0] = 0.0;
	features[4][1] = 0.0;
	features[4][2] = 0.5;

	features[5][0] = 1.0;
	features[5][1] = 1.0;
	features[5][2] = 0.2;

	features[6][0] = 1.0;
	features[6][1] = 0.4;
	features[6][2] = 0.25;

	features[7][0] = 1.0;
	features[7][1] = 0.0;
	features[7][2] = 1.0;


	Train(s, features, 8, 10000, 1000, 0.1);
	PrintHits(s);
	FreeSom(s);
}
