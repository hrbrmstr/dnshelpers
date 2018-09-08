
# dnshelpers

Tools to Process ‘DNS’ Response Data

## Description

The internet domain name system (‘DNS’) supports a wide variety of
response types besides simple ‘IP’ addresses or domain names. Methods
are provided to process a myriad of other response data elements
including ‘DMARC’ (‘TXT’), ‘SPF’ (‘TXT’), ‘MX’ and more.

## What’s Inside The Tin

The following functions are implemented:

  - `parse_dmarc`: Parse DMARCv1 DNS data reponses into a data frame
  - `parse_mx`: Parse MX DNS data reponses into a data frame
  - `parse_soa`: Parse SOA DNS data reponses into a data frame
  - `parse_spf`: Parse SPF TXT DNS data reponses into a list

## Installation

``` r
devtools::install_github("hrbrmstr/dnshelpers")
```

## Usage

``` r
library(dnshelpers)
library(gdns)
library(tidyverse)

# current verison
packageVersion("dnshelpers")
```

    ## [1] '0.1.0'

``` r
targets <- c("senate.gov", "equifax.com", "rapid7.com")

bulk_query(targets, "MX") %>% 
  bind_cols(parse_mx(.$data))
```

    ## # A tibble: 7 x 7
    ##   name          type   TTL data                        query       preference data1                   
    ##   <chr>        <int> <int> <chr>                       <chr>            <int> <chr>                   
    ## 1 equifax.com.    15   315 20 smtp2.equifax.com.       equifax.com         20 smtp2.equifax.com.      
    ## 2 equifax.com.    15   315 10 smtp.equifax.com.        equifax.com         10 smtp.equifax.com.       
    ## 3 rapid7.com.     15   299 1 aspmx.l.google.com.       rapid7.com           1 aspmx.l.google.com.     
    ## 4 rapid7.com.     15   299 10 alt3.aspmx.l.google.com. rapid7.com          10 alt3.aspmx.l.google.com.
    ## 5 rapid7.com.     15   299 10 alt4.aspmx.l.google.com. rapid7.com          10 alt4.aspmx.l.google.com.
    ## 6 rapid7.com.     15   299 5 alt1.aspmx.l.google.com.  rapid7.com           5 alt1.aspmx.l.google.com.
    ## 7 rapid7.com.     15   299 5 alt2.aspmx.l.google.com.  rapid7.com           5 alt2.aspmx.l.google.com.

``` r
bulk_query(targets, "SOA") %>% 
  bind_cols(parse_soa(.$data))
```

    ## # A tibble: 3 x 12
    ##   name          type   TTL data                      query   mname     rname       serial  refresh retry expire minimum
    ##   <chr>        <int> <int> <chr>                     <chr>   <chr>     <chr>       <chr>     <dbl> <dbl>  <dbl>   <dbl>
    ## 1 senate.gov.      6 10799 sen-dmzp.senate.gov. saa… senate… sen-dmzp… saanoc.saa… 200810…    1800   900 2.59e6     900
    ## 2 equifax.com.     6   315 ns0.equifax.com. hostmas… equifa… ns0.equi… hostmaster… 201808…   10800  3600 2.42e6     900
    ## 3 rapid7.com.      6   899 ns-1653.awsdns-14.co.uk.… rapid7… ns-1653.… awsdns-hos… 1          7200   900 1.21e6   86400

``` r
bulk_query(sprintf("_dmarc.%s", targets), "TXT") %>% 
  filter(str_detect(str_to_lower(data), "dmarc1")) %>% 
  bind_cols(parse_dmarc(.$data))
```

    ## # A tibble: 2 x 13
    ##   name                type   TTL data                   query   v     p     sp    rua        ruf      rf    pct   fo   
    ##   <chr>              <int> <int> <chr>                  <chr>   <chr> <chr> <chr> <chr>      <chr>    <chr> <chr> <chr>
    ## 1 _dmarc.senate.gov.    16 10214 "\"v=DMARC1;\"\"p=rej… _dmarc… DMAR… reje… none  mailto:sa… mailto:… afrf  100   <NA> 
    ## 2 _dmarc.rapid7.com.    16   299 "\"v=DMARC1; p=none; … _dmarc… DMAR… none  <NA>  mailto:dm… mailto:… <NA>  <NA>  0

``` r
bulk_query(targets, "TXT") %>% 
  filter(str_detect(str_to_lower(data), "v=spf")) %>% 
  .$data %>% 
  parse_spf() %>% 
  str()
```

    ## List of 3
    ##  $ :List of 4
    ##   ..$ mechasims :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  1 obs. of  3 variables:
    ##   .. .. ..$ qualifier: chr "-"
    ##   .. .. ..$ mechanism: chr "all"
    ##   .. .. ..$ data     : chr NA
    ##   ..$ modifiers :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  0 obs. of  0 variables
    ##   ..$ valid     : logi TRUE
    ##   ..$ spf_record: chr "\"v=spf1 -all\""
    ##  $ :List of 4
    ##   ..$ mechasims :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  17 obs. of  3 variables:
    ##   .. .. ..$ qualifier: chr [1:17] "+" "+" "+" "+" ...
    ##   .. .. ..$ mechanism: chr [1:17] "ip4" "ip4" "ip4" "ip4" ...
    ##   .. .. ..$ data     : chr [1:17] "216.46.96.0/24" "12.104.201.0/24" "12.178.18.150/32" "68.233.76.14/32" ...
    ##   ..$ modifiers :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  0 obs. of  0 variables
    ##   ..$ valid     : logi TRUE
    ##   ..$ spf_record: chr "\"v=spf1 ip4:216.46.96.0/24 ip4:12.104.201.0/24 ip4:12.178.18.150/32 ip4:68.233.76.14/32 ip4:198.60.22.202/32 i"| __truncated__
    ##  $ :List of 4
    ##   ..$ mechasims :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  13 obs. of  3 variables:
    ##   .. .. ..$ qualifier: chr [1:13] "+" "+" "+" "+" ...
    ##   .. .. ..$ mechanism: chr [1:13] "ip4" "ip4" "ip4" "ip4" ...
    ##   .. .. ..$ data     : chr [1:13] "208.118.237.0/24" "208.118.227.0/25" "64.125.235.5" "64.125.235.6" ...
    ##   ..$ modifiers :List of 1
    ##   .. ..$ :Classes 'tbl_df', 'tbl' and 'data.frame':  0 obs. of  0 variables
    ##   ..$ valid     : logi TRUE
    ##   ..$ spf_record: chr "\"v=spf1 ip4:208.118.237.0/24 ip4:208.118.227.0/25 ip4:64.125.235.5 ip4:64.125.235.6 ip4:205.201.137.229 ip4:20"| __truncated__
