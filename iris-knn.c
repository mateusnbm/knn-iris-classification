//
//  main.c
//  c-sandbox
//
//  Created by Mateus Nunes de B Magalhaes on 3/27/18.
//  Copyright © 2018 mateusnbm. All rights reserved.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum species {
    
    kSpeciesSetosa,
    kSpeciesVersicolor,
    kSpeciesVirginica
    
} Species;

typedef struct plant {
    
    Species family;
    float sepalLength;
    float sepalWidth;
    float petalLength;
    float petalWidth;
    
} Plant;

Plant *testSet[1000];
Plant *trainingSet[1000];

unsigned int testSetSize = 0;
unsigned int trainingSetSize = 0;

char *plantName(Species s) {
    
    return s == kSpeciesSetosa ? "setosa" : (s == kSpeciesVirginica ? "virginica" : "versicolor");
    
}

void printPlant(Plant *a) {
    
    printf(
        "%.1f, %.1f, %.1f, %.1f, Iris-%s\n",
        a->sepalLength,
        a->sepalWidth,
        a->petalLength,
        a->petalWidth,
        plantName(a->family)
        );
    
}

void fillSetsFromFile(char filename[], float trainingRatio) {
    
    char buffer[1024];
    FILE *f = fopen(filename, "r");
    
    while(fgets(buffer, 1024, f) != NULL) {
        
        Plant *p = (Plant *) malloc(sizeof(Plant));
        
        p->family = buffer[24] == 'o' ? kSpeciesSetosa : buffer[24] == 's' ? kSpeciesVersicolor : kSpeciesVirginica;
        p->sepalLength = (buffer[0]-'0') + ((buffer[2]-'0')/10.0);
        p->sepalWidth = (buffer[4]-'0') + ((buffer[6]-'0')/10.0);
        p->petalLength = (buffer[8]-'0') + ((buffer[10]-'0')/10.0);
        p->petalWidth = (buffer[12]-'0') + ((buffer[14]-'0')/10.0);
        
        if (trainingSetSize/(testSetSize+trainingSetSize+1.0) > trainingRatio) {
            
            testSet[testSetSize++] = p;
            
        }else{
            
            trainingSet[trainingSetSize++] = p;
            
        }
        
    }
    
    fclose(f);
    
}

float euclideanDistance(Plant *a, Plant *b) {
    
    float d = 0;
    
    d += powf((a->sepalLength - b->sepalLength), 2);
    d += powf((a->sepalWidth - b->sepalWidth), 2);
    d += powf((a->petalLength - b->petalLength), 2);
    d += powf((a->petalWidth - b->petalWidth), 2);
    
    return sqrtf(d);
    
}

Plant **computeNeighbors(Plant *item, int k) {
    
    int j, l, m, n;
    float d;
    float *distances = (float *) calloc(k, sizeof(float));
    Plant **neighbors = (Plant **) malloc(k * sizeof(Plant *));
    
    for (j = 0; j < trainingSetSize; j++) {
        
        d = euclideanDistance(item, trainingSet[j]);
        
        for (l = 0; l < k && neighbors[l] != NULL && distances[l] < d; l++);
        
        if (neighbors[l] != NULL && l < k) {
            
            for (m = (l+1), n = 1; m < k; m++, n++) {
                
                distances[k-n] = distances[k-n-1];
                neighbors[k-n] = neighbors[k-n-1];
                
            }
            
            neighbors[l] = NULL;
            
        }
        
        if (neighbors[l] == NULL) {
            
            distances[l] = d;
            neighbors[l] = trainingSet[j];
            
        }
        
    }
    
    return neighbors;
    
}

float knn(int k, int log) {
    
    int i, j, l, result = 0;
    
    for (i = 0; i < testSetSize; i++) {
        
        // Compute k nearest neighbors.
        
        Plant **neighbors = computeNeighbors(testSet[i], k);
        
        // Increase the successful predictions count if at least half of
        // the neighbors belong to the same class of the base individual.
        
        for (j = 0, l = 0; j < k; j++) {
            
            l += (neighbors[j]->family == testSet[i]->family ? 1 : 0);
            
        }
        
        result += ((l > (k/2)) ? 1 : 0);
        
        // Print the individual, neighbors and prediction outcome.
        
        if (log) {
            
            printf("Item: %i:\n", i);
            printPlant(testSet[i]);
            printf("Neighbors:\n");
            
            for (j = 0; j < k; j++) {
                
                printPlant(neighbors[j]);
            }
            
            printf("Correct: %s.\n", ((l > (k/2)) ? "YES" : "NO"));
            printf("\n");
            
        }
        
    }
    
    return (float) result/testSetSize;
    
}

void cleanSet(Plant *set[], int size) {
    
    int i;
    
    for (i = 0; i < size; i++) {
        
        free(set[i]);
        
    }
    
}

int main(int argc, const char * argv[]) {
    
    int i;
    int ksl = 9;
    int ks[] = {1, 2, 3, 5, 7, 9, 11, 13, 15};
    
    fillSetsFromFile("iris.txt", 0.67);
    
    for (i = 0; i < ksl; i++) {
        
        printf("k: %i %f\n", ks[i], knn(ks[i], 0));
        
    }
    
    cleanSet(testSet, testSetSize);
    cleanSet(trainingSet, trainingSetSize);
    
    return 0;
    
}
