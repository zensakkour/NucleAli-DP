# NucleAli-DP (Nucleotide Alignment Dynamic Programming)

NucleAli-DP is a C17 DNA sequence alignment engine that computes exact weighted edit distance and optimal alignments for nucleotide strings (`A`, `C`, `T`, `G`).

## Core Capabilities

- Exact edit distance computation
- Two exact solvers:
  - `sol1`: full dynamic programming matrix + traceback
  - `sol2`: divide-and-conquer alignment with reduced memory usage
- Single-instance and batch CLI execution
- Optional JSON output for automation
- Optional CSV reporting for batch runs
- Configurable scoring model at runtime
- Built-in alignment verification mode

## Scoring Model (defaults)

- Insertion: `2`
- Deletion: `2`
- Substitution:
  - `0` if identical
  - `3` if concordant (`A<->T`, `C<->G`)
  - `4` otherwise

## Repository Structure

- `src/alignment.c`, `src/alignment.h`: alignment algorithms and scoring
- `src/instance_io.c`, `src/instance_io.h`: `.adn` parser, directory indexing, memory metrics
- `src/DATA_struct.c`, `src/DATA_struct.h`: domain structures and helpers
- `src/main.c`: CLI application
- `tests/`: benchmark and validation executables
- `instances/`: input datasets
- `data/`: benchmark outputs
- `docs/`: technical documentation
- `.github/workflows/ci.yml`: CI pipeline

## Build

```bash
cmake -S . -B build
cmake --build build --config Release
```

## CLI Usage

Single instance:

```bash
./build/align_cli --input instances/Inst_0000010_7.adn --solver sol2 --show-alignment --verify
```

Batch mode:

```bash
./build/align_cli --batch-dir instances --solver sol2 --csv data/batch_report.csv
```

JSON output:

```bash
./build/align_cli --input instances/Inst_0000010_7.adn --json
```

Custom scoring:

```bash
./build/align_cli \
  --input instances/Inst_0000010_7.adn \
  --cost-ins 2 \
  --cost-del 2 \
  --cost-sub-concordant 3 \
  --cost-sub-nonconcordant 4
```

## Testing

```bash
ctest --test-dir build --output-on-failure
```

## CI

GitHub Actions runs configure/build/test on:

- `ubuntu-latest`
- `windows-latest`
