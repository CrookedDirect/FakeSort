# Fake Sort v1 — Technical reference

[Project overview](../README.md) · [한국어 README](../README.ko.md)

## 1. Scope

Fake Sort is an experimental numeric comparison sort. Its core combines direct partition writes, alternating buffers for unresolved ranges, and permanent placement of completed ranges in the output array. This reference describes the released implementations, not a claim of academic novelty or universal performance superiority.

## 2. State and invariants

A is the mutable input and final output. B is auxiliary storage of at least N elements. A room is an inclusive index interval `[l,r]` with a parity bit and an imbalance flag. Parity false selects A as source and B as destination; true reverses these roles. Initially the whole input is one room with both bits false. Empty and singleton inputs need no partition.

At each room boundary:

- Unresolved rooms occupy disjoint intervals, each containing its authoritative values in the buffer selected by parity.
- Completed positions in A are final and excluded from future rooms.
- Every room contains precisely the numeric multiset assigned to its interval by its ancestors. Values in earlier intervals are no greater than those in later intervals.
- The other buffer may contain stale values; it is not an additional authoritative copy.

## 3. Partition kernel

For a room of length m, select the pivot value p at index `l + floor((r-l)/d)`, where d is 2 normally and 4 when the imbalance flag is active. This is an index selection, not a median or quartile calculation on values.

Set w=l and z=r. Scan each source element once for classification (in addition to reading the pivot). Write values less than p to destination[w++] and values greater than p to destination[z--]. Equal values require no temporary list. After all source elements have been classified, fill A[w..z] with p, except for the E1 case below.

The two unresolved child intervals are `[l,w-1]` and `[z+1,r]`. Empty children are discarded. Singleton children are finalized in A, copying from B only when necessary. In modes using C8, children of length 2–8 are sorted directly into A. Other children inherit reversed parity and a newly computed imbalance flag.

### Reference pseudocode

```text
rooms = [(0, N-1, false, false)]
while rooms is not empty:
    next = empty list
    for each (l, r, flag, parity) in rooms:
        src, dst = (B, A) if parity else (A, B)
        p = src[l + floor((r-l)/(4 if flag else 2))]
        w, z = l, r
        for i in classification_order(l, r):
            v = src[i]
            if v < p: dst[w] = v; w += 1
            else if v > p: dst[z] = v; z -= 1
        if not (E1 enabled and src is A and w == l and z == r):
            fill A[w..z] with p
        for each child [x,y] in [(l,w-1),(z+1,r)]:
            k = y-x+1
            if k <= 0: continue
            if k == 1: finalize dst[x] in A
            else if C8 enabled and k <= 8: network-sort dst[x..y] into A
            else: append (x,y,D0 enabled and 8*k > 7*(r-l+1),not parity) to next
    rooms = next
```

The pseudocode omits validation, the initial C8 fast path, room-list allocation details and BD's reversed comparison order. It describes the sequential wave structure, not the asynchronous scheduler.

### Worked example (Original)

Input A = [4,1,3,2,5]. The middle-index pivot is 3. The first partition writes smaller values 1,2 to B[0..1] and larger values 4,5 from the right, producing B[3..4] = [5,4]. A[2] is finalized as 3. B[2] is irrelevant. The two remaining rooms read B and write A; their singleton children and pivot regions complete A = [1,2,3,4,5].

## 4. Optimizations

- **D0:** A child of length k receives the imbalance flag exactly when 8k > 7m, where m is its parent's length. Its next pivot uses the quarter index. The flag is recomputed for each child; it does not guarantee balanced partitions.
- **C8:** Fixed compare-exchange networks handle lengths 2–8, including the entire input when applicable. The network loads values before writing the sorted result to A. The comparator counts for lengths 2 through 8 are 1, 3, 5, 9, 12, 16 and 19.
- **E1:** If every value equals p and the source is A, omit the central fill. When the source is B, the central region must still be written to A.
- **BD:** Scan from l through the pivot index inclusive, testing less-than first; then scan from r down to just above the pivot index, testing greater-than first. The two scans are sequential. This changes classification order, not the partition predicate or final numeric order.
- **Room-list reuse:** Exchange current and next lists and clear the old list for reuse. This avoids creating a fresh list object each wave, but does not promise zero allocation or a speedup. C++ vectors can retain capacity; JS engine storage behavior is implementation-dependent.

## 5. Correctness and termination

For finite numeric values, each source element belongs to exactly one of the less-than, equal-to or greater-than classes. Let their counts be L,E,G. Since the pivot comes from the room, E >= 1 and L+E+G=m. After classification w=l+L and z=r-G, so z-w+1=E. The two destination write ranges cannot overlap; the central fill preserves the numeric multiset. E1 is valid because the entire source room already contains the final equal values in A.

Every left value is strictly smaller than p and every right value strictly greater. Thus both child intervals have their correct relative rank ranges. They are disjoint and strictly smaller than their parent. Finalizing singleton children and sorting small children with their fixed networks preserves these properties. Induction on room length proves ascending numeric order and multiset preservation, assuming valid writable storage, correct numeric comparisons and the implemented sorting networks. Finite decreasing room lengths imply termination. The argument concerns numeric equality, not preservation of signed-zero identity or record order.

## 6. Complexity

A partition of m elements performs Theta(m) work, with at most two classification comparisons per element, excluding validation and fixed-size network work. The total sequential partition work is proportional to the sum of processed room lengths.

| Condition | Time |
|---|---|
| All values equal | Theta(N) |
| Balanced partitions | Theta(N log N) |
| Ordinary random-input model with sufficiently balanced expected splits | Expected Theta(N log N) |
| Repeated highly unbalanced splits | O(N²), with quadratic worst-case behavior not excluded by D0 |

The random-input statement is conditional, not a distribution-independent average-case guarantee. B uses O(N) storage, and disjoint active room lists require O(N) auxiliary metadata. Fixed networks use constant temporary storage per room. The sequential implementation is not in-place. Parallel scheduling adds runtime-dependent task and per-thread metadata; the sequential auxiliary bound is not a measured bound on total OpenMP runtime memory.

## 7. Public modes and implementation differences

| Mode | JS | C++ ID | Kernel / scheduler |
|---|---|---:|---|
| Original | original | 0 | Basic sequential wave |
| Pure | pure | 5 | D0 + E1 + BD, no C8 |
| Advanced | advanced | 1 | D0 + C8 + E1 + BD |
| Optimized | optimized | 2 | Advanced with room-list reuse |
| Wave | — | 3 | Advanced kernel, OpenMP waves |
| Async | — | 4 | Advanced kernel, OpenMP tasks |

JS Pure does not reuse room lists; C++ Pure does. All JS modes accept optional caller-owned scratch. The C++ public function allocates its own scratch within each call. Optimized does not change the partition decisions relative to Advanced. Historical comparison helpers in the C++ source are not public Fake Sort modes.

## 8. Parallel scheduling (C++)

Wave uses a static OpenMP work-sharing loop over current rooms. Workers collect children in thread-local lists. After the loop barrier, a single worker merges the lists and exchanges the wave lists; the single-region barrier publishes the next wave.

Async starts the root in a single region inside a taskgroup. A child of length at least cutoff becomes an OpenMP task; smaller unresolved children are processed locally using an explicit pending-room stack. The default cutoff is 2048. The root partition is sequential in both modes. Children can run independently because their index intervals are disjoint and parent classification finishes before child dispatch. This is the structural rationale for independence, not a race-detector result. Requested thread counts do not guarantee actual concurrency. Wave validates but does not use cutoff.

## 9. API contracts and known limitations

See the README for build and usage examples. JS accepts ordinary writable dense Array or Float64Array of finite numbers; scratch must be writable, sufficiently long, distinct from input and not share the input typed-array backing buffer. Frozen scratch is not reliably rejected. Scratch validation is skipped for N < 2. C++ accepts finite doubles, mode IDs 0–5, positive thread counts and cutoff >= 2. Its public size guard is 2147483647 elements, but Pure uses signed int products in the imbalance test: conservatively restrict Pure to N <= INT_MAX/8 (268435455 for 32-bit int). This potential overflow has not been reproduced on huge arrays or fixed in this release.

Equal-key record stability and signed-zero identity are not guaranteed. The API is numeric rather than a general comparator/record API. Extra memory, management overhead, unbalanced partitions and sequential root work can limit performance. No universal speed advantage is claimed.

## 10. Verification and demos

The recorded release checks comprise 44759 JS checks (including 11 rejection checks) and 63297 C++ checks (including 21 rejection checks). Cases overlap. See [JS test](../tests/test.js), [C++ test](../tests/test.cpp), [R02 log](../tests/R02_results.txt) and [R03 log](../tests/R03_results.txt) for scope and reproduction. These are finite tests, not a proof of race freedom. No new timing results are introduced here.

The [runner](../demo/index.html) executes the JS kernel. The [visualizer](../demo/visualizer.html) illustrates room processing and logical operations; animation interleaving is not CPU parallelism and displayed counters are not hardware measurements. The project author reported both demos working during release preparation; cross-browser and mobile compatibility have not been established.
