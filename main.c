#include <stdio.h>
#include <stdlib.h>
#include "plotter.h"


int importContours(ContourNode* contour){
    FILE *FP;
    char buffer[255];
    int * xpos = calloc(5000,sizeof(int));
    int * ypos = calloc(5000,sizeof(int));
    FP = fopen("./contours.txt","r");
    int points = 0;
    int contour_count = 0;
    int width = strtol(fgets(buffer, sizeof(buffer), FP), (char **)NULL, 10);   
    int height = strtol(fgets(buffer, sizeof(buffer), FP), (char **)NULL, 10);   
    while(fgets(buffer, sizeof(buffer), FP)){
        if(strstr(buffer, "---")){
            contour->width = width;
            contour->height = height;
            contour->x_positions = xpos; 
            contour->y_positions = ypos;
            contour->size = points;
            contour->next = (ContourNode *) malloc(sizeof(ContourNode));
            contour->end = 0;
            contour = (ContourNode *) contour->next;
            points = 0;
            contour->end = 1;
            xpos = calloc(5000,sizeof(int));
            ypos = calloc(5000,sizeof(int));
            printf("%d\n", contour_count++);
            continue;
        }
        char *token = strtok(buffer, " ");
        int count = 0;
        while(token != NULL){
            if (count == 0){
                xpos[points] = strtol(token, (char **)NULL, 10);
            }
            if (count == 1){
                ypos[points] = strtol(token, (char **)NULL, 10);
            }
            count++;
            token = strtok(NULL," ");
        }
        points++;
    }
}

int printList(ContourNode * HEAD){
    ContourNode * temp = HEAD;
    int count = 0;
    do{
        printf("%d\n", count++);
        printf("-------\n");
        int i = 0;
        for(i = 0; i < temp->size; i++){
            printf("x[1]: %d, ", temp->x_positions[i]);
            printf("y[1]: %d\n", temp->y_positions[i]);
        }
        printf("Points: %d\n", temp->size);
        printf("Points: %d\n", temp->end);
        printf("-------\n\n");
        temp = (ContourNode *)temp->next;
    }while(!temp->end);
}


int main(){
    init();
    ContourNode* linked_list = malloc(sizeof(ContourNode)*5);
    importContours(linked_list);
    drawImage(linked_list);
    printList(linked_list);
}


