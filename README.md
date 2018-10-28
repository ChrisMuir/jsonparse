# jsonparse

R package for parsing JSON. There are already a few R packages that parse JSON data ( [jsonlite](https://github.com/jeroen/jsonlite), [rjson](https://github.com/alexcb/rjson) ), the intent behind this one is to try to build a package that is faster than the existing options. This project is very young, currently the functions can only handle values of type `int`, `double`, `logical`, and `character`.

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
library(jsonify)
```
```r
# Create json string, using package jsonify
json_str <- jsonify::to_json(
  list(
    "string_key" = "Cats", 
    "int_key" = 5L, 
    "double_key" = 99.4, 
    "bool_key" = TRUE, 
    "vector_key" = c(9L, 10L, 11L, 12L)
  )
)

# print json_str
json_str
```
```
#> [1] "{\"string_key\":[\"Cats\"],\"int_key\":[5],\"double_key\":[99.4],\"bool_key\":[true],\"vector_key\":[9,10,11,12]}"
```
```r
jsonparse::from_json(json_str)
```
```
#> $string_key
#> [1] "Cats"

#> $int_key
#> [1] 5

#> $double_key
#> [1] 99.4

#> $bool_key
#> [1] TRUE

#> $vector_key
#> [1]  9 10 11 12
```

## Benchmarks

```r
library(jsonlite)
jl_fromJSON <- jsonlite::fromJSON
library(rjson)
rj_fromJSON <- rjson::fromJSON
```

#### Test 1
```r
json_str <- jsonify::to_json(
  list(
    "ints" = 1L:100000L, 
    "doubles" = rnorm(100000), 
    "strings" = stringi::stri_rand_strings(100000, 8), 
    "bools" = sample(c(TRUE, FALSE), size = 100000, replace = TRUE)
  )
)

microbenchmark::microbenchmark(
  jsonparse = from_json(json_str), 
  rjson = rj_fromJSON(json_str), 
  jsonlite = jl_fromJSON(json_str, simplifyVector = FALSE)
)
```
```
#> Unit: milliseconds
#>      expr       min        lq      mean    median       uq       max neval
#> jsonparse  24.01423  27.23423  29.97406  29.60571  32.0372  44.45918   100
#>     rjson 100.33898 109.40579 119.47500 117.18489 126.3026 226.08668   100
#>  jsonlite 207.57313 219.68605 230.28911 226.09717 239.4743 277.65422   100
```

#### Test 2
```r
json_str <- lapply(1:10000, function(x) {
  list(
    "string_key" = "Cats", 
    "int_key" = 5L, 
    "double_key" = 99.4, 
    "bool_key" = TRUE, 
    "vector_key" = c(9L, 10L, 11L, 12L)
  )
})
json_str <- jsonify::to_json(json_str)

microbenchmark::microbenchmark(
  jsonparse = from_json(json_str), 
  rjson = rj_fromJSON(json_str), 
  jsonlite = jl_fromJSON(json_str, simplifyVector = FALSE)
)
```
```
#> Unit: milliseconds
#>      expr      min       lq     mean   median       uq      max neval
#> jsonparse 10.67509 11.50920 14.73693 12.64370 14.01555 105.5113   100
#>     rjson 28.35868 31.80194 38.61254 34.84451 41.82969 149.4932   100
#>  jsonlite 82.88986 86.76564 90.76374 90.10586 93.98836 111.0137   100
```
