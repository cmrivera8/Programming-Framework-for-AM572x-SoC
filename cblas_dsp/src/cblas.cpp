#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include "ocl_util.h"
#include "assert.h"
#include "cblas.h"

//Embedded pre-compiled binaries for OpenCL kernels:

//

using namespace cl;
using namespace std;

#include <time.h>
struct timespec t0, t1;
#define tick()  clock_gettime(CLOCK_MONOTONIC, &t0);
#define tock() (clock_gettime(CLOCK_MONOTONIC, &t1), \
                        t1.tv_sec - t0.tv_sec + (t1.tv_nsec - t0.tv_nsec) / 1e9)

void printMatrix(double *matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << matrix[i * cols + j] << " ";
        }
        cout << endl;
    }
}

void printMatrices(double *A, double *B, double *C, int m, int k, int n)
{
    if(m > 5){return;}
    cout << "Matrix A:" << endl;
    printMatrix(A, m, k);
    cout << endl;

    cout << "Matrix B:" << endl;
    printMatrix(B, k, n);
    cout << endl;

    cout << "Matrix C:" << endl;
    printMatrix(C, m, n);
    cout << endl;
}

void save_to_file(int NumElements, float time1, float time2, float improvement, int matches) {
  FILE *fp;
  fp = fopen("data.txt", "a");
  fprintf(fp, "%d %f %f %f %d\n", NumElements, time1, time2, improvement, matches);
  fclose(fp);
}

int main(int argc, char *argv[])
{
    //OpenCL APIs in a try-catch block to detect errors
    try {
        // Allocate arrays in contiguous shared memory to avoid copies when
        // dispatching from ARM to DSP
        // Example: 
        int input = atoi(argv[1]);
        int m, k, n;
        m = k = n = input;//2000;
        double alpha = 0.7; 
        double beta  = 1.3; 
        double *A = (double *)__malloc_ddr(sizeof(double) * m*k);
        double *B = (double *)__malloc_ddr(sizeof(double) * k*n);
        double *C = (double *)__malloc_ddr(sizeof(double) * m*n);

        assert (A != NULL);
        assert (B != NULL);
        assert (C != NULL);

        // Initialize the vectors/matrices
        srand(42);
        for (int i=0; i < (m*k); ++i) A[i] = rand() % 5 + 1;
        for (int i=0; i < (k*n); ++i) B[i] = rand() % 5 + 1;
        for (int i=0; i < (m*n); ++i) C[i] = 0;

        //Print before calculation:
        printMatrices(A,B,C,m,k,n);

        double secs = 0.0;
        tick();
        /* To compute on ARM or DSP:
        Forcing BLAS level 3 execution on ARM:
            export TI_CBLAS_OFFLOAD=000;
        Forcing BLAS level 3 execution on DSP:
            export TI_CBLAS_OFFLOAD=001;
        Optimal BLAS level 3 execution on ARM or DSP:
            export TI_CBLAS_OFFLOAD=002; 
        */
        setenv("TI_CBLAS_OFFLOAD", "001", 0); // 0 = no overwritten of variable if exist

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, k, B, n, beta, C, n);

        secs += tock();
        cout << "\nExecution time: "<< secs << endl;
        
        //Save time result in file
        int offload=atoi(getenv("TI_CBLAS_OFFLOAD"));
        save_to_file(m, secs, 0, 0, offload);

        //Print after calculation:
        printMatrices(A,B,C,m,k,n);

        //Free memory
        __free_ddr(A);
        __free_ddr(B);
        __free_ddr(C);
    }
    catch (Error& err)
    {
        cerr << "ERROR: " << err.what() << "(" << err.err() << ", "
             << ocl_decode_error(err.err()) << ")" << endl;
        exit(1);
    }

    return 0;
}