# jsonparse

R package for parsing JSON. There are already a few R packages that parse JSON data ( [jsonlite](https://github.com/jeroen/jsonlite), [rjson](https://github.com/alexcb/rjson) ), the intent behind this one is to try to build a package that is faster than the existing options. This project is very young, currently the functions can only handle values of type `int`, `double`, `logical`, and `character`.

This package is built using the [rapidjson](https://github.com/Tencent/rapidjson) C++ library (via the [rapidjsonr](https://github.com/SymbolixAU/rapidjsonr) R package), and [Rcpp](https://github.com/RcppCore/Rcpp).

As an additional resource, check out the [jsonify](https://github.com/SymbolixAU/jsonify) package, which uses the rapidjson library to convert R objects to json.

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
    "string_key" = "cats", 
    "int_key" = 5L, 
    "double_key" = 99.4, 
    "bool_key" = TRUE, 
    "vector_key" = c(9L, 10L, 11L, 12L), 
    "list_key" = list("dogs", 55.3)
  )
)

# print json_str
json_str
```
```
#> {"string_key":["cats"],"int_key":[5],"double_key":[99.4],"bool_key":[true],"vector_key":[9,10,11,12],"list_key":[["dogs"],[55.3]]}

```
```r
jsonparse::from_json(json_str)
```
```
#> $string_key
#> [1] "cats"

#> $int_key
#> [1] 5

#> $double_key
#> [1] 99.4

#> $bool_key
#> [1] TRUE

#> $vector_key
#> [1]  9 10 11 12

#> $list_key
#> $list_key[[1]]
#> [1] "dogs"

#> $list_key[[2]]
#> [1] 55.3
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
    "string_key" = "cats", 
    "int_key" = 5L, 
    "double_key" = 99.4, 
    "bool_key" = TRUE, 
    "vector_key" = c(9L, 10L, 11L, 12L), 
    "list_key" = list("dogs", 55.3)
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
#>      expr       min        lq      mean    median        uq      max neval
#> jsonparse  15.64111  17.31473  22.00976  19.56679  22.38043 105.1796   100
#>     rjson  39.75470  47.17560  57.10193  52.63964  60.26783 168.0407   100
#>  jsonlite 106.23252 111.21012 118.63235 115.01797 119.71804 238.5919   100
```
