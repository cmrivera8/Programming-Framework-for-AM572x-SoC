#include <iostream>
#include <CL/cl.h>

using namespace std;

string formatWithCommas(long long number) {
    string result;

    int count = 0;
    for (auto i = number; i > 0; i /= 10) {
        if (count % 3 == 0 && count != 0) {
            result = ',' + result;
        }

        result = (char)('0' + i % 10) + result;
        ++count;
    }

    return result;
}

int main() {
    cl_uint num_devices;
    clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    cl_device_id *devices = new cl_device_id[num_devices];
    clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

    for (int i = 0; i < num_devices; i++) {
        cl_device_id device = devices[i];

        // Get device name
        char device_name[1024];
        clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);

        // Get device vendor
        char device_vendor[1024];
        clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(device_vendor), device_vendor, NULL);

        // Get number of compute units
        cl_uint num_compute_units;
        clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &num_compute_units, NULL);

        // Get local memory size
        cl_ulong local_mem_size;
        clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_mem_size, NULL);

        size_t workgroup_size;
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &workgroup_size, NULL);

        cl_uint workitem_dimensions;
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &workitem_dimensions, NULL);

        size_t *workitem_sizes = new size_t[workitem_dimensions];
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * workitem_dimensions, workitem_sizes, NULL);

        cout << "Device: " << i << endl;
        cout << "Device Name: " << device_name << endl;
        cout << "Device Vendor: " << device_vendor << endl;
        cout << "Number of Compute Units: " << num_compute_units << endl;
        cout << "Local Memory Size: " << local_mem_size/1024 << " KB (" << local_mem_size << " bytes)" << endl;
        cout << "Max Work Group Size: " << formatWithCommas(workgroup_size) << " Work Items" << endl;
        cout << "Max Work Item Dimensions: " << formatWithCommas(workitem_dimensions) << endl;
        cout << "Max Work Items assigned to each dimension: ";
        for (int j = 0; j < workitem_dimensions; j++) {
            cout << "\n Dimension " << j << ": " << formatWithCommas(workitem_sizes[j]) << " Work Items";
        }
        cout << endl << endl;
        delete [] workitem_sizes;
    }

    delete [] devices;

    return 0;
}