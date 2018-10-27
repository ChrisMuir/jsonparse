# jsonparse

R package for parsing JSON. There are already a few R packages that parse JSON data ( [jsonlite](https://github.com/jeroen/jsonlite), [rjson](https://github.com/alexcb/rjson)), the intent behind this one is to try to build a package that is faster than the existing options. This project is very young, currently the functions can only handle scalar values of type `int`, `double`, `logical`, and `character`.

This package is built using [rapidjson](https://github.com/Tencent/rapidjson) (via the [rapidjsonr](https://github.com/SymbolixAU/rapidjsonr) R package), and [Rcpp](https://github.com/RcppCore/Rcpp).

Please [report](https://github.com/ChrisMuir/jsonparse/issues) issues, comments, or feature requests.

## Installation

Install from this repo:

``` r
# install.packages("devtools")
devtools::install_github("ChrisMuir/jsonparse")
```

## Example Usage

```r
library(jsonparse)
```

```r
json_str <- "{\"int_key\":8, \"double_key\":99.5, \"logical_key\":true, \"string_key\":\"cats\"}"
jsonparse::from_json(json_str)
```
```
#> $int_key
#> [1] 8

#> $double_key
#> [1] 99.5

#> $logical_key
#> [1] TRUE

#> $string_key
#> [1] "cats"
```

## Benchmarks

```r
library(jsonlite)
jl_fromJSON <- jsonlite::fromJSON
library(rjson)
rj_fromJSON <- rjson::fromJSON

# Function to create a long json string, with structure similar to the 
# example json string from the example above.
create_json_str <- function(vals, size) {
  out <- vapply(as.character(seq_len(size)), function(x) {
    curr_val <- unlist(sample(vals, 1))
    if (is.logical(curr_val)) {
      return(paste(paste0('"', x, '"'), tolower(as.character(curr_val)), sep = ":"))
    }
    if (is.character(curr_val)) {
      return(paste(paste0('"', x, '"'), paste0('"', curr_val, '"'), sep = ":"))
    }
    paste(paste0('"', x, '"'), curr_val, sep = ":")
  }, character(1), USE.NAMES = FALSE)
  
  paste0("{", paste(out, collapse = ", "), "}")
}

vals <- list(8, 99.5, TRUE, "cats")
json_str <- create_json_str(vals, 100000)
```
```r
# Ensure the output of jsonparse::from_json() matches that of jsonlite::fromJSON()
identical(jl_fromJSON(json_str), from_json(json_str))
```
```
#> TRUE
```
```r
microbenchmark::microbenchmark(
  jsonparse = from_json(json_str), 
  rjson = rj_fromJSON(json_str), 
  jsonlite = jl_fromJSON(json_str), 
  times = 50
)
```
```
Unit: milliseconds
      expr       min        lq      mean    median        uq       max neval
 jsonparse  16.69483  18.53344  21.56793  19.87237  21.99968  47.27006    50
     rjson  38.60249  41.15548  45.02022  44.41045  48.38988  56.59249    50
  jsonlite 331.38482 343.32524 369.25288 358.70981 369.31804 486.69618    50
```
