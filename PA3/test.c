#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_DIM 10  // Adjustable matrix dimension

void generate_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = i + 1;  // Fill matrix sequentially for easy debugging
    }
}

void generate_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = i + 1;
    }
}

void print_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

void print_vector(double *vector, int size, const char *name) {
    printf("%s:\n", name);
    for (int i = 0; i < size; i++) {
        printf("%6.2f\n", vector[i]);
    }
    printf("\n");
}

void matvec_mult(double *local_matrix, double *vector, double *local_result, int local_rows, int cols) {
    for (int i = 0; i < local_rows; i++) {
        local_result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            local_result[i] += local_matrix[i * cols + j] * vector[j];
        }
    }
}

int main(int argc, char **argv) {
    int rank, size;
    double *matrix = NULL, *vector = NULL, *result = NULL;
    double *local_matrix, *local_result;
    int *sendcounts, *displs;
    int local_rows;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector = (double *)malloc(MATRIX_DIM * sizeof(double));

    if (rank == 0) {
        matrix = (double *)malloc(MATRIX_DIM * MATRIX_DIM * sizeof(double));
        result = (double *)malloc(MATRIX_DIM * sizeof(double));
        generate_matrix(matrix, MATRIX_DIM, MATRIX_DIM);
        generate_vector(vector, MATRIX_DIM);

        printf("Matrix:\n");
        print_matrix(matrix, MATRIX_DIM, MATRIX_DIM);
        print_vector(vector, MATRIX_DIM, "Vector");
    }

    // Broadcast vector to all processes
    MPI_Bcast(vector, MATRIX_DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Allocate memory for sendcounts and displacements
    sendcounts = (int *)malloc(size * sizeof(int));
    displs = (int *)malloc(size * sizeof(int));

    // Compute row distribution for Scatterv()
    int rows_assigned = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (MATRIX_DIM / size) * MATRIX_DIM;
        if (i < MATRIX_DIM % size) {
            sendcounts[i] += MATRIX_DIM;  // Extra row for first few processes
        }
        displs[i] = rows_assigned * MATRIX_DIM;
        rows_assigned += sendcounts[i] / MATRIX_DIM;
    }

    local_rows = sendcounts[rank] / MATRIX_DIM;
    local_matrix = (double *)malloc(local_rows * MATRIX_DIM * sizeof(double));
    local_result = (double *)malloc(local_rows * sizeof(double));

    if (local_matrix == NULL || local_result == NULL) {
        fprintf(stderr, "Rank %d: Memory allocation failed\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Scatter matrix rows
    MPI_Scatterv(matrix, sendcounts, displs, MPI_DOUBLE,
                 local_matrix, local_rows * MATRIX_DIM, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    // Debug: Print received matrix portion
    printf("Rank %d received %d rows of matrix:\n", rank, local_rows);
    print_matrix(local_matrix, local_rows, MATRIX_DIM);

    if (rank == 0)
    {
        // Perform matrix-vector multiplication
        matvec_mult(local_matrix, vector, local_result, local_rows, MATRIX_DIM);

        // Debug: Print local result before gathering
        print_vector(local_result, local_rows, "Local Result");
    }

    else
    {
        // Perform matrix-vector multiplication
        matvec_mult(local_matrix, vector, local_result, local_rows, MATRIX_DIM);

    // Debug: Print local result before gathering
        print_vector(local_result, local_rows, "Local Result");
    }

    // Gather results
    MPI_Gatherv(local_result, local_rows, MPI_DOUBLE,
                result, sendcounts, displs, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nResult Vector:\n");
        print_vector(result, MATRIX_DIM, "Result");
    }

    free(vector);
    free(local_matrix);
    free(local_result);
    free(sendcounts);
    free(displs);

    if (rank == 0) {
        free(matrix);
        free(result);
    }

    MPI_Finalize();
    return 0;
}