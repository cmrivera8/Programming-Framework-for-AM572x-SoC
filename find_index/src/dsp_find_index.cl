kernel void dsp_find_index(global float *matrix, const float value, global int *index, global int *num_matches)
{
    int gid = get_global_id(0);
    if (matrix[gid] == value)
    {
        int match_index = atomic_add(num_matches, 1);
        index[match_index] = gid;
    }
}
