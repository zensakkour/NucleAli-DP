# NucleAli-DP Architecture Overview

## Core Components

- `alignment.c`
  - Distance computation (`dist_naif`, `dist_1`, `Dist_2`)
  - Alignment reconstruction (`sol_1`, `SOL_2`)
  - Cost-model utilities (`cout_substitution`, `calcul_cout`)

- `DATA_struct.c`
  - Domain objects:
    - `duo_chaine` for input sequence pairs
    - `Align` for alignment results
  - Allocation/free helpers and sanity checks

- `instance_io.c`
  - `.adn` parser
  - Instance directory indexing and ordering
  - Process memory usage helper

- `main.c`
  - CLI orchestration and solver selection (`sol1` / `sol2`)

## Algorithmic Flow

1. Parse instance file (`lire_genome`)
2. Select solver
3. Compute optimal edit distance
4. Reconstruct optimal alignment
5. Report metrics (distance, runtime, alignment length)

## Complexity Summary

- Naive recursion (`dist_naif`): exponential, baseline reference only
- `Dist_1` + `sol_1`: `O(n*m)` time, `O(n*m)` memory
- `Dist_2` + `SOL_2`: `O(n*m)` time, `O(m)` working memory for distance layers

## Quality Gates

- Assertions on computed alignment cost vs. optimal distance
- Sanity verification against original sequences
- Dedicated benchmark executables for runtime and memory behavior



