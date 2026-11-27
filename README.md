# Fake Sort v1

[한국어](README.ko.md)

[Runner](demo/index.html) · [Visualizer](demo/visualizer.html) · [Technical documentation](docs/technical.md) · [Benchmarks](benchmarks/README.md) · [Tests](tests/README.md)

Fake Sort is an experimental comparison-based sorting algorithm that partitions numeric arrays into contiguous rooms, alternates two buffers for unresolved elements, and fixes completed regions in the final output array. It is not an in-place or stable sort.

## How it works

1. A holds the input and final output; B is a scratch buffer.
2. Choose the value at the middle index of a room as its pivot (not the median value).
3. Write smaller values from the left and larger values from the right into the destination buffer.
4. After scanning the room, fill the remaining central region in A with the pivot value. Finalize singleton rooms in A as well.
5. Process only unresolved child rooms, alternating source and destination buffers at each level.

Each partition preserves the multiset of numeric values and creates strictly smaller, disjoint child rooms. This gives termination and, by induction, ascending numeric order in A. Equal-key record order and signed-zero identity are not preserved by contract.

## Modes

| Mode | JS option | C++ ID | Features |
|---|---|---:|---|
| Original | `original` | 0 | Basic sequential wave partitioning |
| Pure | `pure` | 5 | D0, E1, BD; no C8 |
| Advanced | `advanced` | 1 | D0, C8, E1, BD |
| Optimized | `optimized` | 2 | Advanced with room-list reuse |
| Wave | — | 3 | Advanced partition kernel, OpenMP waves |
| Async | — | 4 | Advanced partition kernel, OpenMP room tasks |

- D0: if a child exceeds 7/8 of its parent, use the quarter index for its next pivot.
- C8: sorting networks for rooms of 2–8 elements.
- E1: skip rewriting an all-equal room already in A.
- BD: scan forward through the pivot index, then backward from the end, reversing comparison order in the second scan. This is sequential, not two-thread partitioning.

All JS modes accept caller-provided scratch storage. Only JS Optimized reuses room lists. C++ Optimized and Pure reuse room lists within a call; the public C++ API allocates its own scratch storage. Async defaults to a task cutoff of 2048 elements; smaller child rooms are processed locally.

## Complexity

| Case | Time |
|---|---|
| Best (all equal) | Θ(N) |
| Balanced partitions / ordinary random-input assumption | Θ(N log N) |
| Worst | Θ(N²) |
| Auxiliary space | O(N) |

D0 does not eliminate the quadratic worst case. The first large partition is sequential even in the parallel modes. Task management, allocation and extra memory can outweigh the benefits on small or unbalanced inputs.

## JavaScript

Requires a modern JavaScript runtime; CommonJS export is provided. From the repository root:

```js
const { fakeSort } = require('./src/FakeSort.js');
const a = [3, 1, 2, 1];
fakeSort(a, { mode: 'pure' });
console.log(a); // [1, 1, 2, 3]
```

`fakeSort(a, {mode = 'optimized', scratch = null} = {})` modifies and returns `a`. Use ordinary writable dense `Array` or `Float64Array` containing finite numbers. Optional scratch must be writable, of either supported type, at least as long as the input, and must not be the input or share its typed-array backing buffer. Frozen scratch is not reliably rejected; scratch validation is skipped for inputs shorter than two elements. NaN and infinities are rejected.

## C++

Requires C++17 and OpenMP. Build and run the included self-test from the repository root:

```sh
g++ -O3 -std=c++17 -fopenmp src/FakeSort.cpp -o fake_sort
./fake_sort
```

To use the implementation in a program, define `FAKE_SORT_LIBRARY` before including the source to omit its self-test main:

```cpp
#define FAKE_SORT_LIBRARY
#include "src/FakeSort.cpp"
int main() {
    std::vector<double> a{3, 1, 2, 1};
    fake_sort(a, 5);
}
```

`fake_sort(a, mode=2, threads=4, cutoff=2048)` sorts finite doubles and modifies the input vector using auxiliary storage. Threads must be positive and cutoff at least 2. Wave ignores cutoff after validation. The requested thread count is not a guarantee of actual concurrency. Pure uses signed `int` products in its imbalance test; conservatively restrict Pure to `N <= INT_MAX / 8` (268,435,455 for 32-bit int). Very large inputs have not been validated.

## Verification

The release verification logs record 44,759 JS checks and 63,297 C++ checks, including overlapping cases and 11 / 21 input-rejection checks respectively. JS was checked with Node.js 22.16.0; C++ with GCC 14.2.0 on Linux using OpenMP. These are finite tests, not proof of race freedom or universal correctness. Signed zero is treated as numerically equal in JS tests.

```sh
node tests/test.js
g++ -O3 -std=c++17 -fopenmp tests/test.cpp -o test_cpp
./test_cpp
```

## Performance and limitations

Historical research measurements suggest benefits on some duplicate-heavy and sawtooth inputs and some parallel workloads. Other implementations can be faster on random, sorted, mountain or valley inputs. The historical comparisons depend on the specific implementations and environment; no new performance measurements were made during this verification. The readable JS implementation is not guaranteed to match the speed of the research benchmark kernels. No hardware-counter measurements, race-detector checks, or cross-platform/browser compatibility tests are claimed.

Fake Sort is an experimental project, not a claim of academic novelty, patentability or universal superiority.

## License

Released under the [MIT License](LICENSE).
