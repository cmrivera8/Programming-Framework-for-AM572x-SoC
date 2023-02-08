kernel void dsp_cblas(global const float2 *x,
                global       float2 *y,
                local        float2 *lx,
                local        float2 *ly)
{
    int grp_id    = get_group_id(0);
    int num_elems = get_local_size(0);

    // Initiate copy of input arrays from global to local memory
    event_t ev1 = async_work_group_copy(lx, x+grp_id*num_elems, num_elems, 0);

    // Wait for copies to complete
    wait_group_events(1, &ev1);
 
    // Perform compute
    int lid    = get_local_id(0);
    ly[lid] = lx[lid] * lx[lid];

    // Initiate copy of results from local to global memory & wait for 
    // completion
    event_t ev2 = async_work_group_copy(y+grp_id*num_elems, ly, num_elems, 0);
    wait_group_events(1, &ev2);
}
