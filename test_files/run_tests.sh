#!/usr/bin/env bash

declare -a file_extensions=("ob" "ext" "ent")
prefix_of_extension="expected"

tests_folder=$PWD
cd ..
make
cd $tests_folder

for as_file in *.as
do
  file_prefix=as_file
  file_prefix=${as_file/.as/}
  echo "Running assembler on $as_file:"
  printf "\n"
  echo "-------------------------- Assembler Output: --------------------------"
  ../assembler $file_prefix
  echo "-------------------------- End Of Assembler Output --------------------------"
  printf "\n"
  echo "Loking for diff in $as_file:"
    for i in "${file_extensions[@]}"
    do
      echo "  Loking for diff in $file_prefix.$i:"
      diff "$file_prefix.$i" "$file_prefix.$prefix_of_extension.$i"
    done
      printf "\n\n"
done
