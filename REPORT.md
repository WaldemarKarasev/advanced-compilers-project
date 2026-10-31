# Project. Stage 1: Garbage Collector

**Author:** Vladimir Karasev  
**Repository:** <https://github.com/WaldemarKarasev/advanced-compilers-project>  
**Submission commit:** [`5a010fe`](https://github.com/WaldemarKarasev/advanced-compilers-project/commit/5a010fe57ef1ed28f08871b0526b202fcd4cb7a7) 

**Russian version:** [REPORT_RU.md](REPORT_RU.md) - AI-assisted translation.

## Report navigation

- [Build and run](REPORT.md#build-and-run)
- [Algorithm and implementation](REPORT.md#algorithm-and-implementation)
- [Parameters](REPORT.md#parameters)
- [GC statistics and state output](REPORT.md#statistics-and-state-output)
- [Testing](REPORT.md#testing)
- [Known limitations](REPORT.md#known-limitations)
- [Bonus part](REPORT.md#bonus-part)
- [AI use](REPORT.md#ai-use)
- [Sources](REPORT.md#sources)


## Build and Run

### Environment

The project uses CMake, a C compiler, a C++ compiler, and `just` for the convenience recipes in `justfile`. CMake downloads GoogleTest 1.12.1 with `FetchContent` when tests are enabled. The project requests the C11 and C++20 language standards.

The environment used for the submitted run:

| Tool or platform | Version / value |
|---|---|
| Operating system and architecture | `7.0.0-38-generic 24.04.4-Ubuntu x86_64` |
| CMake | `3.28.3` |
| C compiler | `gcc-13` |
| C++ compiler | `g++13` |
| just | `1.46.0` |
| GoogleTest | `release-1.12.1` |

### Build and run

The `justfile` provides the standard workflow:

```sh
just configure
just build
just test
```

`just test` configures and builds the project, then runs the unit and integration tests with CTest.

The sanitizer workflow enables AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
just sanitize-configure
just sanitize-build
just sanitize-test
```

Or run the complete sanitizer workflow with `just sanitize-test`. The recipe configures and builds `build_sanitize` before running CTest.

The equivalent standard CMake commands are:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTING=ON
cmake --build build --parallel 10
STELLA_GC_STEP_BUDGET=1 ctest --test-dir build --output-on-failure
```

## Algorithm and Implementation

The collector is an incremental Cheney-Baker copying collector. It uses two equal-sized semispaces and does not implement generations. At any time, one space is the from-space and the other is the to-space.

When there is room and no cycle is active, allocation proceeds in from-space. When from-space cannot satisfy an allocation, the collector begins a cycle. It clears to-space, forwards objects reachable from dynamic and permanent roots, and initializes the Cheney scan pointer. Forwarding copies each reachable object once and stores the destination address in the old object's `gc_word`. Repeated visits use that forwarding address and preserve sharing and cycles.

The collector then scans copied objects incrementally. Each step processes at most the configured number of objects. While a cycle is active, new allocations are placed at the high end of to-space, while copied reachable objects grow from the low end. This lets the mutator continue allocating during incremental tracing. When the scan pointer reaches the copied-object boundary, the spaces are swapped and the cycle completes.

Managed-field reads advance an incremental step. During an active cycle, the read barrier forwards a referenced object if it still resides in from-space and updates the field. Allocations, managed writes, and compatible object updates also advance collection work. Primitive-field access does not need pointer tracing. Writes to static objects are rejected.

Important invariants include:

- Every managed pointer that must survive a collection is held in a registered dynamic or permanent root, or is found by scanning a reachable object's managed fields.
- A forwarded object has one canonical destination copy, recorded in its old `gc_word`.
- The scanner only scans the low region of to-space containing forwarded objects. New mutator allocations use the high region.
- A cycle finishes only when scanning catches up with the forwarded-object region, only then are the spaces swapped.
- `update_object` changes the descriptor and active fields in place, requires the new descriptor to have the same slot capacity, and clears inactive slots.

Implementation map:

| Concern | Main implementation files |
|---|---|
| C++ bindings for the Stella GC C API | `bindings/gc.hpp` |
| GC API entry points, initialization, allocation, field access, update, diagnostics | `gc/cheney-baker/gc.cpp` |
| Two semispace allocation and swapping | `gc/cheney-baker/heap.cpp`, `gc/cheney-baker/heap.hpp` |
| Object layout and descriptor validation | `gc/cheney-baker/object.cpp`, `gc/cheney-baker/object.hpp` |
| Forwarding and Cheney scanning | `gc/cheney-baker/copying.cpp`, `gc/cheney-baker/copying.hpp` |
| Cycle phases and incremental steps | `gc/cheney-baker/collection.cpp`, `gc/cheney-baker/collection.hpp` |
| Dynamic and permanent roots | `gc/cheney-baker/roots.cpp`, `gc/cheney-baker/roots.hpp` |
| Step-budget parsing and object/state printing helpers | `gc/cheney-baker/gc_helpers.hpp` |
| Collector state and counters | `gc/cheney-baker/gc_state.hpp`, `gc/cheney-baker/gc_stats.hpp` |

## Parameters

| Parameter | Meaning and setting | Valid values and default |
|---|---|---|
| `max_heap_bytes` | Supplied through `StellaGcConfig` at initialization. It is the capacity of **each** semispace. The collector allocates two semispaces, so the reserved semispace storage is approximately `2 * max_heap_bytes`, excluding other runtime allocations. The command-line runtime accepts it as the optional second argument. | Must be greater than zero. Runtime default: 64 MiB per semispace. An object larger than one semispace cannot be allocated. |
| `STELLA_GC_STEP_BUDGET` | Environment variable that sets the maximum number of objects scanned by one ordinary incremental step. | Positive decimal integer representable as `size_t`, default: `1`. Empty, zero, negative, non-numeric, or overflowing values cause an ABI violation. |
| `STELLA_GC_STATS` | Requests statistics from `runtime/main.c` before shutdown. | Enabled when set to a non-empty value other than `0`, otherwise disabled. |
| `STELLA_GC_STATE` | Requests a heap and collector-state dump from `runtime/main.c` before shutdown. | Enabled when set to a non-empty value other than `0`, otherwise disabled. |

The step budget controls work per incremental step, not the total amount of work in a cycle. If available space is exhausted while a cycle is active, allocation may perform a full collection outside the regular step budge,; `alloc budget exceeded` counts allocation calls that needed this fallback.

## GC Statistics and State Output

`STELLA_GC_STATS=1` prints statistics to `stderr`. Allocation totals are cumulative for the process. `occupied bytes` is the collector's current accounting value. Each allocation adds its size, and completing a cycle replaces it with the bytes copied plus allocations made during that cycle. `peak occupied bytes` is the maximum value recorded so far.

For example, a recorded run of `chat_cheney_baker` with input `10`, heap argument `4096`, and step budget `1` printed the result `45` and these statistics:

```text
45
=== Stella Cheney-Baker GC statistics ===
allocated bytes: 7080
allocated objects: 210

completed cycles: 2
normal incremental steps: 141
alloc budget exceeded: 0

occupied bytes: 3624
peak occupied bytes: 5144

managed reads: 245
managed writes: 0
primitive reads: 0
primitive writes: 0

read barrier activations: 17
write barrier activations: 0

dynamic root depth: 2
peak dynamic root depth: 94
permanent roots: 0

step budget: 1
```

## Testing

The test suite has three configurations: GoogleTest unit tests for internal and public GC behavior, integration tests that run the C output of Stella examples, and AddressSanitizer/UndefinedBehaviorSanitizer builds of the same suites.

### Representative tests

The current test suite contains 60 unit and 15 Stella integration tests. The following table groups the unit tests by the behavior they cover. It does not list every test individually.

| Unit-test suite | Properties checked |
|---|---|
| `GcSmokeTest` | The GC API can be initialized and shut down from a linked test executable. |
| `CheneyBakerCopyingTest` | Copying, forwarding-pointer reuse, preservation of sharing, static objects, and skipping primitive fields. |
| `GcStepBudgetTest` | Default and valid budgets: ejection of empty, zero, negative, non-numeric, and overflowing values. |
| `GcAllocationTest` | Object layout and initialization: invalid, static, or over-capacity allocation descriptors. |
| `GcCollectionTest` | Reclamation of garbage while preserving a rooted cycle. |
| `GcDiagnosticsTest` | Statistics, roots, and heap-state output: rejection of null output streams. |
| `GcFieldAccessTest` | Managed and primitive reads/writes, index bounds, and static-object write rejection. |
| `GcLifecycleTest` | Initialization/shutdown rules and rejection of invalid lifecycle operations. |
| `GcRootDeathTest` | Shutdown is rejected with dynamic roots and allowed with permanent roots. |
| `GcRootTest` | LIFO root stack, duplicate registrations, and aliases to the same object. |
| `GcUpdateObjectTest` | Managed/primitive updates, clearing inactive slots, and rejection of incompatible updates. |
| `GcSmallHeapStressTest` | Repeated garbage allocation while retaining a rooted self-cycle in a tiny heap. |

The Stella integration cases are grouped by workload below. Exact input arguments, heap sizes, budgets, expected output, and statistics checks are specified in [`tests/integration/CMakeLists.txt`](tests/integration/CMakeLists.txt).

| Integration-test name(s) | Properties checked |
|---|---|
| `StellaDoubleInput5`, `StellaHistoryInput5`, `StellaPlaylistInput5` | Expected scalar results for arithmetic, object mutation, and list processing. |
| `StellaChatInput10Budget1`, `StellaChatInput10Budget2`, `StellaChatInput10Budget4` | The same garbage-producing workload returns the same result with budgets `1`, `2`, and `4`, collection must occur. |
| `StellaExtraLargeThenSmall`, `StellaExtraReplacementChurn`, `StellaExtraKeepTail` | Allocation churn and reclamation while retaining the required live result. |
| `StellaExtraTwoLiveLists`, `StellaExtraLiveListTraversal`, `StellaExtraSharedAlias`, `StellaExtraNestedLists`, `StellaExtraKeepOneTreeBranch` | Multiple live lists, traversal, shared aliases, nested lists, and retaining one tree branch. |
| `StellaExtraClosureCapture` | Captured managed data remains reachable through a closure. |

Total: 75 registered cases (60 unit and 15 integration).

Some integration tests set `STELLA_GC_STATS=1` and fail if no collection cycle completes. Those tests check both the expected program result and that the workload actually reaches the collector. The chat example is run with step budgets `1`, `2`, and `4`. Churn-style examples use a small heap and budget `1` to force collection.

`StellaChatInput10Budget1`, `StellaChatInput10Budget2`, and `StellaChatInput10Budget4` run the `chat` example with input `10` and a 4096-byte semispace. Its result is `45`, the sum of the batch sizes from `0` through `9`. Each test checks this result and enables statistics. The CTest configuration also fails if `completed cycles: 0`, so the test confirms that the collector actually ran under each budget.

`GcSmallHeapStressTest.PreservesRootedCycleWhileReclaimingGarbage` configures a heap large enough for only four one-slot objects. It roots one object, makes its managed field point back to itself, and performs 64 more allocations without retaining those new objects. The test repeatedly checks that reading the rooted object's field still returns the root. It passes only if garbage is reclaimed and the self-cycle remains valid.

### Reproduction commands and results

Run these commands from the repository root on the final submission commit and record the exact CTest summary:

```sh
just test
just sanitize-test
```

The configured integration tests and their heap sizes, arguments, budgets, expected outputs, and statistics checks are listed in `tests/integration/CMakeLists.txt`. Unit test targets are registered in `tests/gc/CMakeLists.txt` and its subdirectories.

Run the following commands from the repository root:

```sh
just test
just sanitize-test
```

The regular CTest log supplied for this run reports **75/75 tests passed**. The sanitizer build and test run also completed successfully. The sanitizer CTest summary was not included in the attached log.

| Configuration | Reproduction settings |
|---|---|
| Regular tests | `just test`; Debug build in `build`; `BUILD_TESTING=ON`; 10 build jobs; default `STELLA_GC_STEP_BUDGET=1`. |
| ASan/UBSan tests | `just sanitize-test`; Debug build in `build_sanitize`; C and C++ compiled with `-fsanitize=address,undefined -fno-omit-frame-pointer`; executables linked with `-fsanitize=address,undefined`. |

`BUILD_DIR`, `BUILD_TYPE`, `JOBS`, `SANITIZE_BUILD_DIR`, and `STELLA_GC_STEP_BUDGET` can be overridden through the environment. The exact integration inputs and heap sizes are listed in `tests/integration/CMakeLists.txt`; the build and test recipes are in `justfile`.

## Shared tests

No tests from other students were used or shared.

## Known Limitations

- The collector uses a fixed-size pair of semispaces. It does not grow the heap after initialization.
- The implementation is incremental and non-generational.
- The collector state is global and the implementation is not designed for concurrent mutator threads.
- If the live set does not fit in a semispace, collection cannot make the requested allocation succeed and the runtime reports out-of-memory.
- Statistics and state output are diagnostic text formats, not a stable machine-readable interface.


## Bonus Part

The optional parameter study in task 6.1 was not included in this submission. Therefore, no `bonus/` scripts, CSV measurements, or AI transcript are provided.

## AI Use

The garbage-collector implementation in `gc/cheney-baker/` was written by the author. AI assistance was used for test design and its registration, build/test configuration, and documentation.

The following commits are identified from the repository history as AI-assisted or explicitly AI-generated. 

| Commit | AI-assisted work | Model |
|---|---|---|
| [`0ba4b65`](https://github.com/WaldemarKarasev/advanced-compilers-project/commit/0ba4b65c0dfb620b106a0c498cdfde8265a009e6) | AI-assisted creation of nine additional Stella integration workloads covering allocation churn, object replacement, multiple live lists, traversal, shared aliases, retained tails, nested lists, closure captures, and retained tree branches. Added the corresponding Stella-compiler-generated C files and a Docker-based compilation script (`scripts/compile_stella.sh`) |GPT-5.6 Sol |
| [`da52a45`](https://github.com/WaldemarKarasev/advanced-compilers-project/commit/da52a45dc811fbaecd5fc2f32b85e69b5463915d) | AI-generated GoogleTest cases for the internal `Forward` and `ScanOne` operations, covering null and static objects, object copying and layout, repeated forwarding, shared references, and preservation of primitive fields. Registered the copying tests in CMake and adjusted a shared test helper. | GPT-5.6 Sol |
| [`bc943a6`](https://github.com/WaldemarKarasev/advanced-compilers-project/commit/bc943a6f0458d5a9af75bbc50f1700859e78a24b) | AI-generated GoogleTest cases for dynamic and permanent roots: push/pop operations, LIFO ordering, root value changes, duplicate or conflicting registrations, aliases, and shutdown behavior. Registered the root tests in CMake. | GPT-5.6 Sol |
| [`787e476`](https://github.com/WaldemarKarasev/advanced-compilers-project/commit/787e476adaac8cf901ad27d6479b26c34a65edbf) | AI-generated GoogleTest cases for object allocation, memory layout, field initialization, and rejection of invalid object descriptors. Added a reusable descriptor helper and registered the allocation tests in CMake. | GPT-5.6 Sol |

The author is responsible for understanding and defending every test included in this report.

## Sources
- Advanced compilers lectures notes
- Appel, Andrew W. (1998). Modern Compiler Implementation in ML. Cambridge University Press.