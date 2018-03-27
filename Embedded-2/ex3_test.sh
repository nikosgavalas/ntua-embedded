#!/bin/bash

make	ex3

cp	input_first	t1
cp	input_first	t1_default
cp	input_sec	t2
cp	input_sec	t2_default

echo "Running for input_first"
./ex3	t1
./ex3_default	t1_default
md5sum	t1_concat_out t1_default_concat_out
md5sum	t1_len_out t1_default_len_out
md5sum	t1_sorted_out t1_default_sorted_out


echo "Running for input_sec"
./ex3	t2
./ex3_default	t2_default
md5sum	t2_concat_out t2_default_concat_out
md5sum	t2_len_out t2_default_len_out
md5sum	t2_sorted_out t2_default_sorted_out


rm	t1* t2*
