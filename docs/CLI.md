# NucleAli-DP CLI Reference

## Commands

- Single instance: `--input <path>`
- Batch mode: `--batch-dir <directory>`

Exactly one of `--input` or `--batch-dir` must be provided.

## Options

- `--solver <sol1|sol2>`: choose alignment solver (default `sol2`)
- `--show-alignment`: print aligned strings (single instance only)
- `--verify`: validate alignment integrity and computed distance
- `--json`: emit JSON output
- `--csv <path>`: write batch report as CSV

## Cost Parameters

- `--cost-ins <int>`
- `--cost-del <int>`
- `--cost-sub-concordant <int>`
- `--cost-sub-nonconcordant <int>`

All cost values must be non-negative integers.
