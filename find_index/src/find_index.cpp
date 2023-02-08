#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include "ocl_util.h"
#include "assert.h"
#include <math.h>
#include <stdlib.h>

//Embedded pre-compiled binaries for OpenCL kernels:
#include "dsp_find_index.dsp_h"
//

using namespace cl;
using namespace std;

/*
LocalMemorySize = WorkGroupSize * sizeof(cl_float2)
LocalMemorySize = (4 * 1024/2)  * (8) = 16KB
*/

#include <time.h>
struct timespec t0, t1;
#define tick()  clock_gettime(CLOCK_MONOTONIC, &t0);
#define tock() (clock_gettime(CLOCK_MONOTONIC, &t1), \
                        t1.tv_sec - t0.tv_sec + (t1.tv_nsec - t0.tv_nsec) / 1e9)

// void arm_find(float arr[], int n, int x, int index[], int *count) {
//   *count = 0;
//   for (int i = 0; i < n; i++) {
//     if (arr[i] == x) {
//       index[(*count)++] = i;
//     }
//   }
// }

#include <omp.h>
void arm_find(float arr[], int n, int x, int index[], int *count) {
  *count = 0;
  int local_count = 0;
  #pragma omp parallel for num_threads(2) shared(local_count)
  for (int i = 0; i < n; i++) {
    if (arr[i] == x) {
      int tmp = local_count;
      index[tmp] = i;
      #pragma omp atomic
      local_count++;
    }
  }
  *count = local_count;
}

void save_to_file(int NumElements, float time1, float time2, float improvement, int matches) {
  FILE *fp;
  fp = fopen("data.txt", "a");
  fprintf(fp, "%d %f %f %f %d\n", NumElements, time1, time2, improvement, matches);
  fclose(fp);
}

void generate_random_and_save(int n, double a, double b) {
  FILE *fp;
  fp = fopen("data_input.csv", "w");
  srand(time(NULL));
  for (int i = 0; i < n; i++) {
    double x = a + (b - a) * (double)rand() / RAND_MAX;
    fprintf(fp, "%f\n", x);
  }
  fclose(fp);
}

void read_from_file(float *array, int size) {
  FILE *fp;
  fp = fopen("data.csv", "r");
  for (int i = 0; i < size; i++) {
    fscanf(fp, "%lf", &array[i]);
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
    double NumWorkGroups   = 10.0;

    double start = NumWorkGroups;
    double end = 16e6;
    int steps = 100;
    double step_size = (log(end) - log(start)) / steps;
    int lastIteration = 0;
    for (int i = 0; i <= steps; i++) {
      int currentIteration = ceil(exp(log(start) + i * step_size)/NumWorkGroups)*NumWorkGroups;
      // printf("Number of elements: %.2f\n", currentIteration);

      //Avoid repeated values
      if(lastIteration==currentIteration){continue;}

      lastIteration=currentIteration;

      int NumElements     = currentIteration;
      int VecLen          = sizeof(float)/sizeof(float);
      int NumVecElements  = NumElements / VecLen; //aka WorkItems in kernel
      int WorkGroupSize   = NumElements/NumWorkGroups; // 16KB/128KB of local memory per array
      //OpenCL APIs in a try-catch block to detect errors
      try {
          // OpenCL kernel part 
          // Create an OpenCL context with the accelerator device
          Context context(CL_DEVICE_TYPE_ACCELERATOR);
          vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
          
          // Build kernel from pre-compiled binary, with embedded binary in .dsp_h file (dsp_find_index.dsp_h)
          Program::Binaries binary(1, make_pair(dsp_find_index_dsp_bin,
                                                  sizeof(dsp_find_index_dsp_bin)));
          Program             program = Program(context, devices, binary);
          program.build(devices);

          // Create an OpenCL command queue
          CommandQueue Q(context, devices[0]);

          // Allocate arrays in contiguous shared memory to avoid copies when
          // dispatching from ARM to DSP
          // Example: 

          float *matrix = (float *)__malloc_ddr(sizeof(float) * NumElements);
          int *index = (int *)__malloc_ddr(sizeof(int) * NumElements);
          int * num_matches = (int *)__malloc_ddr(sizeof(int));

          assert (matrix != NULL);
          assert (index != NULL);
          assert (num_matches != NULL);

          //Variables
          float value = 88;
          
          index[0] = 0;

          // Initialize the vectors/matrices
          srand(time(NULL));
          for (int i=0; i < NumElements; ++i) matrix[i] = rand() % 1000 + 1;
          // matrix[55] = 88;
          // matrix[NumElements-1] = 88;
          for (int i=0; i < NumElements; ++i) index[i] = 0;
          num_matches[0]=0;          

          // Perform computation on OpenCL device (DSP)
          // Creation of OpenCL buffers to pass as arguments to the kernel.
          // Example:
          
          Buffer bufMatrix(context, (cl_mem_flags) CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,  sizeof(float) * NumElements, matrix);

          Buffer bufIndex(context, (cl_mem_flags) CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, sizeof(int) * NumElements, index);

          Buffer bufMatches(context, (cl_mem_flags) CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, sizeof(int) * NumElements, num_matches);
          

          // Create a kernel object and set its arguments.
          // Example: 
          // Considering a kernel object with the following prototype:
          /*
          kernel void dsp_find_index(global const float2 *x,
                          global       float2 *y,
                          local        float2 *lx,
                          local        float2 *ly)
          */
          
          Kernel kernel(program, "dsp_find_index");

          kernel.setArg(0, bufMatrix);
          kernel.setArg(1, value);
          kernel.setArg(2, bufIndex);       
          kernel.setArg(3, bufMatches);    

          //Record time:
          double secs_dsp = 0.0;
          double secs_arm = 0.0;
          tick();

          // Dispatch the kernel
          Event ev1;
          Q.enqueueNDRangeKernel(kernel, NullRange, NDRange(NumVecElements),
                              NDRange(WorkGroupSize), NULL, &ev1);
          /*The "enqueueNDRangeKernel" command will enqueue to the OpenCL queue named "Q" a kernel object named "kernel".

          The second argument is the offset, assumed to be 0 in all dimensions. The NullRange object will satisfy that 0 specification.

          The third argument is the "global size" and it specifies a wish to execute 1024 instances of the work-item specified in the kernel source associated with the kernel object kernel.

          The fourth argument is the local size and it specifies how many of the work-items should be grouped into a work-group. In this case, it is specified to be 128 work-items per work-group.
          
          Since there are 1024 total work-items and 128 work-items per work-group, these is a total of 8 work-groups (1024 / 128 = 8).
          
          It is also possible for the global size and local size to be specified in 2 or 3 dimensions. For example a 2D kernel enqueue may look like

          Q.enqueueNDRangeKernel(K, NullRange, NDRange(640, 480), NDRange(640, 1))
          */

          // Wait fo the kernel to complete execution
          ev1.wait();

          //Elapsed time:
          secs_dsp += tock();
          // cout << "\nExecution time (DSP): "<< secs_dsp << " seconds"<< endl;

          // Print results:
          int num_matches_value = *num_matches;
          // printf("Value: %f \n", (double) value);
          // printf("Number of matches: %d \n", num_matches_value);
          // printf("Index of %f: \n", (double) value);
          for(int i=0; i<num_matches_value;i++){
              // printf("%d\n",index[i]);
          }
          //Free memory
          __free_ddr(matrix);
          __free_ddr(index);
          __free_ddr(num_matches);

          //Save DSP matches count
          int dsp_matches = num_matches_value;

          //Record time:
          secs_arm = 0.0;
          tick();
          //Done with ARM:
          arm_find(matrix, NumElements, value, index, num_matches);

          //Elapsed time:
          secs_arm += tock();
          // cout << "\nExecution time (ARM): "<< secs_arm << " seconds"<< endl;

          // Print results:
          num_matches_value = *num_matches;
          // printf("Value: %f \n", (double) value);
          // printf("Number of matches: %d \n", num_matches_value);
          // printf("Index of %f: \n", (double) value);
          for(int i=0; i<num_matches_value;i++){
              // printf("%d\n",index[i]);
          }

          // Improvement:
          // printf("\nDSP is %f times faster than ARM.\n", secs_arm/secs_dsp);

          //Save ARM matches count
          int arm_matches = num_matches_value;
          
          //Compare the results given by OpenCL and by ARM:
          if (dsp_matches != arm_matches) {
            printf("Error! computation results differ.\n");
            save_to_file(NumElements, secs_arm, secs_dsp, secs_arm/secs_dsp, 0);
            continue;
          }

          //Save results in a file to plot
          save_to_file(NumElements, secs_arm, secs_dsp, secs_arm/secs_dsp,num_matches_value);
        }
        catch (Error& err)
        {
            cerr << "ERROR: " << err.what() << "(" << err.err() << ", "
                << ocl_decode_error(err.err()) << ")" << endl;
            exit(1);
        }
    }

    return 0;
}

