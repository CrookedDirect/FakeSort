# Correctness tests

Run from the repository root:

```sh
node tests/test.js
g++ -O3 -std=c++17 -fopenmp src/FakeSort.cpp -o fake_sort
./fake_sort
g++ -O3 -std=c++17 -fopenmp tests/test.cpp -o test_cpp
./test_cpp
```

Historical release verification records: R02_results.txt and R03_results.txt. Expected totals: JS 44759; C++ self-test 3870, exhaustive 59046, parallel configurations 360, rejection 21. Counts include overlapping cases. These tests are not proofs, performance measurements, or race-detector checks. See records for API limitations.
