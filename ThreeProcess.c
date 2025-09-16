#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int sum(int **matrix, const int size) {
    int sum = 0;
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            sum += matrix[i][j];
        }
    }
    return sum;
}

int sum_of_diagonal(int **matrix, const int size) {
    int sum = 0;
    for(int i = 0; i < size; ++i) {
        sum += matrix[i][i];
    }
    return sum;
}

int main()
{
    int size = 0;
    printf("Enter size of matrix: ");
    scanf("%d", &size);

    int **matrix = malloc(sizeof(int*) * size);
    if(!matrix) {
        printf("Failed to allocate memory! \n");
        return -1;
    }

    for(int i = 0; i < size; ++i) {
        matrix[i] = malloc(sizeof(int) * size);
        if(!matrix[i]) {
            printf("Failed to allocate memory!\n");
            for (int j = 0; j < i; ++j) {
                free(matrix[j]);
            }
            free(matrix);
            return -1;
        }
    }

    printf("Enter matrix: \n");
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            scanf("%d", &matrix[i][j]);
        }
    }

    pid_t pid1 = fork();

    if(pid1 == -1) {
        perror("Fork failed");
        return -1;
    }
    else if(pid1 == 0) {
        printf("sum of matrix is: %d\n", sum(matrix, size));
    }
    else {
        pid_t pid2 = fork();
        
        if(pid2 == -1) {
            perror("Fork failed");
            return -1;
        }

        if(pid2 == 0) {
            printf("sum of the main diagonal is: %d\n", sum_of_diagonal(matrix, size));
        }

        else {
            pid_t pid3 = fork();
            
            if(pid3 == -1) {
                perror("Fork failed\n");
                return -1;
            }
            else if(pid3 == 0) {
                printf("Upper triangle part:\n");
                for(int i = 0; i < size; ++i) {
                    for(int j = i; j < size; ++j) {
                        printf("%d ", matrix[i][j]);
                    }
                    printf("\n");
                }
            }
            else {
                wait(NULL);
                wait(NULL);
                wait(NULL);
                
                for(int i = 0; i < size; ++i) {
                    free(matrix[i]);
                }
                free(matrix);

                return 0;
            }
        }
    }
}