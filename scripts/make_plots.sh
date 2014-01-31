#!/bin/bash
python plot_aggregate_results.py subset-trials.eps /home/velezj/projects/gits/p2l-system/build/bin/jj-coverage-01subset-1find.agg /home/velezj/projects/gits/p2l-system/build/bin/ex-igmm001-01subset-06centered-1find.agg /home/velezj/projects/gits/p2l-system/build/bin/jj-005mean-01subset-debugplan-06initcentered-1find.agg

python plot_aggregate_results.py full-trials.eps /home/velezj/projects/gits/p2l-system/build/bin/ex-coverage-full-06centered-1find.agg /home/velezj/projects/gits/p2l-system/build/bin/ex-igmm001-full-06centered-1find.agg /home/velezj/projects/gits/p2l-system/build/bin/ex-mean008-full-06centered-1find.agg
