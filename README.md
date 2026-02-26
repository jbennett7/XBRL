# XBRL

An R package for extracting structured financial data from XBRL instance
documents and their associated Discoverable Taxonomy Sets (DTS). Supports
both traditional XBRL and inline XBRL (iXBRL).

## Installation

The package is not on CRAN. Install from GitHub:

```r
# install.packages("remotes")
remotes::install_github("jbennett7/XBRL")
```

**System requirement:** libxml2 >= 2.9.1 must be installed.

```sh
# Debian/Ubuntu
sudo apt-get install libxml2-dev

# Fedora/RHEL
sudo dnf install libxml2-devel

# macOS (Homebrew)
brew install libxml2
```

## Quick Start

```r
library(XBRL)

# Parse a local or remote XBRL filing in one call
result <- xbrlDoAll("path/to/instance.xml")

# Access the extracted DataFrames
head(result$fact)       # reported values
head(result$context)    # reporting periods and entities
head(result$element)    # taxonomy element definitions
head(result$label)      # human-readable labels
```

Remote files are downloaded and cached automatically:

```r
result <- xbrlDoAll(
  "https://example.com/filings/aapl-20230930.htm",
  cache.dir = "xbrl_cache",
  verbose   = TRUE
)
```

Export all tables to CSV by supplying a filename prefix:

```r
xbrlDoAll("instance.xml", prefix.out = "output/aapl")
# writes: output/aapl_facts.csv, output/aapl_contexts.csv, ...
```

## Output Structure

`xbrlDoAll()` returns a named list of eleven data frames:

| Name | Contents |
|------|----------|
| `fact` | Reported values with contextRef, unitRef, decimals, element name |
| `context` | Reporting periods: entity identifier, startDate, endDate, instant flag |
| `dimension` | Dimensional context members (contextId, dimension, value) — one row per member |
| `element` | Schema element declarations: type, periodType, balance, substitutionGroup |
| `role` | Role type definitions from the taxonomy |
| `label` | Human-readable element labels with language and role |
| `presentation` | Presentation arc relationships (parent/child ordering) |
| `definition` | Definition arc relationships (dimensional structure) |
| `calculation` | Calculation arc relationships (arithmetic checks) |
| `unit` | Unit definitions (measure, numerator/denominator for ratios) |
| `footnote` | Footnotes linked to specific facts |

Dimensional contexts are returned as a normalized long table: each
`<xbrldi:explicitMember>` or `<xbrldi:typedMember>` element becomes one row
in `$dimension`, so there is no limit on the number of dimensions per context.

## API Reference

### `xbrlDoAll()`

```r
xbrlDoAll(
  file.inst,
  cache.dir          = "xbrl.Cache",
  prefix.out         = NULL,
  verbose            = FALSE,
  delete.cached.inst = TRUE
)
```

| Argument | Description |
|----------|-------------|
| `file.inst` | Path or URL to an XBRL instance document |
| `cache.dir` | Directory for caching downloaded files. `NULL` disables caching. |
| `prefix.out` | If non-NULL, write each data frame to `{prefix}_{name}.csv` |
| `verbose` | Print processing log to the console |
| `delete.cached.inst` | Delete the cached copy of a remote instance after processing |

### `XBRL()` — Low-Level Object

For fine-grained control, use the closure-based `XBRL()` object directly:

```r
xbrl <- XBRL()
xbrl$setVerbose(TRUE)
xbrl$setCacheDir("my_cache")
xbrl$openInstance("instance.xml")
xbrl$processSchema(xbrl$getSchemaName())
xbrl$processContexts()
xbrl$processFacts()
xbrl$processUnits()
xbrl$processFootnotes()
result <- xbrl$getResults()
xbrl$closeInstance()
```

## Architecture

The package is built on libxml2 (via Rcpp) for fast XML parsing. Each
concern (facts, contexts, labels, arcs, units) is handled by a dedicated
C++ source file that queries the parsed document with XPath and returns a
data frame. Schema and linkbase discovery is recursive, with cycle detection
to handle complex multi-file DTS structures.

## Credits

Originally authored by Roberto Bertolusso and Riaz Arbi. Currently
maintained by Joseph Bennett. Their contributions are preserved and credited
in the DESCRIPTION file.

## License

GPL (>= 2)
