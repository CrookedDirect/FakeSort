# Historical FINAL-PURE benchmark

PWS is the historical name of Fake Sort. The original research source and recorded output are preserved unchanged. These are historical kernel measurements, not measurements of the public API or JavaScript implementation.

Build and run from this directory:

```sh
g++ -O3 -std=c++17 -fopenmp PWS_FINAL_PURE.cpp -o benchmark
./benchmark > new_results.txt
```

The command is a reproduction recipe, not a claim about the exact historical compiler flags. The original machine configuration is not established by these two files. Runs may be slow.

Sizes: 256, 1000, 10000, 100000; seeds: 12345, 54321, 98765. Nine patterns (IDs 0–8): random, ascending, descending, nearly sorted, equal, duplicate32, mountain, sawtooth, valley. One warm-up and five timed repetitions per available configuration, rotated order. Times in milliseconds; input copy and reference comparison outside timing. Scratch is preallocated for applicable kernels. Four requested threads for Async4 and QS4, one for Async1; task cutoff 2048. Actual thread counts were not recorded. Three-way QS uses adaptive pivot selection; Hoare does not. The valley pattern is abs(i-N/2), not a general adversarial-input guarantee.

Raw columns: N, pattern ID, seed, Original, Pure, three-way QS, Hoare, Merge, Heap, Async4, QS4, Async1, Insertion, Bubble. Last two only for N <= 1000. -1 means skipped (Original on large mountain/valley). The recorded final line is PASS 6444, including warm-ups. No race detector or hardware counters are reported.

relative.csv contains the median across three seeds of 100 * algorithm median time / Pure median time for each seed. Lower is faster; blank means unavailable. All nine patterns are retained, including equal. No new timings or statistical significance claims. Results depend on implementation, machine and workload; not evidence of universal superiority.
