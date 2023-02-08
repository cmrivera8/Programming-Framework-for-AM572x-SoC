#!/bin/bash

#List of files to "ignore" (i.e change their extension temporarily to be ignored by Makefile)

# function to change extension from "initial_ext" to "final_ext":
change_extension(){
  local initial_ext=$1
  local final_ext=$2
  local files=("$@")

  for file in "${files[@]:2}"
  do
    if [ -f "./src/${file}$initial_ext" ]; then
      mv ./src/"$file"$initial_ext ./src/"${file%$inial_ext}$final_ext"
    else
      echo "File ./src/${file}$initial_ext does not exist."
    fi
  done
}

# main function
#source ../ignore_files.sh 1 -> change extensions
#source ../ignore_files.sh 0 -> revert changes
main() {
  local files_cpp=
  local files_c=
  local files_cl=

  #Example of simple Cross-compilation of OpenCL project
  # files_cpp+=(matmul_arm matmul_ocl)
  # files_c+=(main_simple my_functions)
  # files_cl+=

  #Advanced example of sgemm
  # files_cpp+=(main)
  # files_c+=(data_move sgemm_kernel sgemm)
  # files_cl+=(kernel)

  #Mathlib example
  # files_cpp+=(main_mathlib)
  # files_c+=()
  # files_cl+=(dsp_compute)

  #Others
  files_cpp+=(skeleton pathplan main_mathlib)
  files_c+=()
  files_cl+=(dsp_compute)

  if [[ $1 -eq 1 ]];
  then
    # Change .cpp to .x
    change_extension ".cpp" ".x" "${files_cpp[@]}"
    # Change .c to .z
    change_extension ".c" ".z" "${files_c[@]}"
    # Change .cl to .y
    change_extension ".cl" ".y" "${files_cl[@]}"
  else
    # Change .x back to .cpp
    change_extension ".x" ".cpp" "${files_cpp[@]}"
    # Change .z back to .c
    change_extension ".z" ".c" "${files_c[@]}"
    # Change .y to .cl
    change_extension ".y" ".cl" "${files_cl[@]}"
  fi
}

# run main function
main $1