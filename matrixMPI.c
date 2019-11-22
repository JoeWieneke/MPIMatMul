#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

void printMatrix(int size, int matrix[size][size]) {
    int k, j;

    for(k = 0; k < size; k++) {
        for(j = 0; j < size; j++) {
            printf(" %3d", matrix[k][j]);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
	MPI_Init(NULL, NULL);

	int myRank;
	int worldSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	if(argc != 2) {
		if(myRank == 0)
			printf("Need a size for the matrices\n");
		MPI_Finalize();
		return 0;
	}

	int size = atoi(argv[1]);
	int k, j, sum = 0;
	int table1[size][size];
	int t1Row[size], t3Row[size];
	int table2[size][size];
	int table3[size][size];
	int seed = 1;

	//fill and print first 2 matrices
	if(myRank == 0) {
		for(k = 0; k < size; k++) {
			for(j = 0; j < size; j++) {
				table1[k][j] = rand() % 5 + 1;
				table2[k][j] = rand() % 9 + 1;
				table3[k][j] = 0;
			}
		}
		printf("A ------------- \n");
		printMatrix(size, table1);
		printf("B ------------- \n");
		printMatrix(size, table2);
	}



	//scatter first matrix
	MPI_Scatter(table1, size*size/worldSize, MPI_INT, t1Row, size*size/worldSize, MPI_INT, 0, MPI_COMM_WORLD);
	//Broadcast table 2 to everyone
	MPI_Bcast(table2, size*size, MPI_INT, 0, MPI_COMM_WORLD);
	//Barrier
	MPI_Barrier(MPI_COMM_WORLD);


	//calculate and put answers into rows
	for(k = 0; k < size; k++) {
		for(j = 0; j < size; j++)
			sum += t1Row[j] * table2[j][k];

		t3Row[k] = sum;
		sum = 0;
	}

	//gather rows
	MPI_Gather(t3Row, size*size/worldSize, MPI_INT, table3, size*size/worldSize, MPI_INT, 0, MPI_COMM_WORLD);

	//Barrier
	MPI_Barrier(MPI_COMM_WORLD);

	//print resulting matrix
	if(myRank == 0) {
		printf("C ------------- \n");
		printMatrix(size, table3);
	}

	MPI_Finalize();
	return 0;
}
