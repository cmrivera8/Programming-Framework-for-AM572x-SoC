#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include "ocl_util.h"
#include "assert.h"

//Embedded pre-compiled binaries for OpenCL kernels:
#include "dsp_skeleton.dsp_h"
//

using namespace cl;
using namespace std;

const int NumElements     = 2 * 1024 * 1024; // 2M elements
const int VecLen          = sizeof(cl_float2)/sizeof(float);
const int NumVecElements  = NumElements / VecLen; //aka WorkItems in kernel
const int WorkGroupSize   = 4 * 1024/VecLen; // 16KB/128KB of local memory per array
/*
LocalMemorySize = WorkGroupSize * sizeof(cl_float2)
LocalMemorySize = (4 * 1024/2)  * (8) = 16KB
*/

int main(int argc, char *argv[])
{
    //OpenCL APIs in a try-catch block to detect errors
    try {
        // OpenCL kernel part 
        // Create an OpenCL context with the accelerator device
        Context context(CL_DEVICE_TYPE_ACCELERATOR);
        vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        
        // Build kernel from pre-compiled binary, with embedded binary in .dsp_h file (dsp_skeleton.dsp_h)
        Program::Binaries binary(1, make_pair(dsp_skeleton_dsp_bin,
                                                sizeof(dsp_skeleton_dsp_bin)));
        Program             program = Program(context, devices, binary);
        program.build(devices);

        // Create an OpenCL command queue
        CommandQueue Q(context, devices[0]);

        // Allocate arrays in contiguous shared memory to avoid copies when
        // dispatching from ARM to DSP
        // Example: 

        cl_float *x = (cl_float *)__malloc_ddr(sizeof(float) * NumElements);
        cl_float *y = (cl_float *)__malloc_ddr(sizeof(float) * NumElements);

        assert (x != NULL);
        assert (y != NULL);

        // Initialize the vectors/matrices
        srand(42);
        for (int i=0; i < NumElements; ++i) x[i] = rand() % 5 + 1;
        for (int i=0; i < NumElements; ++i) y[i] = 0;

        // Perform computation on OpenCL device (DSP)
        // Creation of OpenCL buffers to pass as arguments to the kernel.
        // Example:
        
        const int BufSize = sizeof(float) * NumElements;
        Buffer bufx(context, (cl_mem_flags) CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,  BufSize, x);
        Buffer bufy(context, (cl_mem_flags) CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, BufSize, y);
        

        // Create a kernel object and set its arguments.
        // Example: 
        // Considering a kernel object with the following prototype:
        /*
        kernel void dsp_skeleton(global const float2 *x,
                        global       float2 *y,
                        local        float2 *lx,
                        local        float2 *ly)
        */
        
        Kernel kernel(program, "dsp_skeleton");

        kernel.setArg(0, bufx);
        kernel.setArg(1, bufy);        
        kernel.setArg(2, __local(sizeof(cl_float2)*WorkGroupSize));
        kernel.setArg(3, __local(sizeof(cl_float2)*WorkGroupSize));

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

        //Free memory
        __free_ddr(x);
        __free_ddr(y);
    }
    catch (Error& err)
    {
        cerr << "ERROR: " << err.what() << "(" << err.err() << ", "
             << ocl_decode_error(err.err()) << ")" << endl;
        exit(1);
    }

    return 0;
}