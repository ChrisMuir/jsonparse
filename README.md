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

microbenchmark::microbenchmark(
  jsonparse = from_json(json_str), 
  rjson = rj_fromJSON(json_str), 
  jsonlite = jl_fromJSON(json_str)
)
```
```
Unit: microseconds
     expr    min     lq     mean median      uq     max neval
jsonparse  3.202  3.806  5.30057  4.558  5.7970  32.433   100
    rjson  4.704  5.535  7.27236  7.257  7.8545  30.201   100
 jsonlite 60.706 63.274 73.61564 65.784 72.1105 423.252   100
```
