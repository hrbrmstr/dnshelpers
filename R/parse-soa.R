paste0(
  c(
    "^",
    "([[:print:]]+)", # mname
    "[[:space:]]+",
    "([[:print:]]+)", # rname
    "[[:space:]]+",
    "([[:digit:]]+)", # serial
    "[[:space:]]+",
    "([[:digit:]]+)", # refresh
    "[[:space:]]+",
    "([[:digit:]]+)", # retry
    "[[:space:]]+",
    "([[:digit:]]+)", # expire
    "[[:space:]]+",
    "([[:digit:]]+)" # minimum
  ),
  collapse = ""
) -> soa_regex

#' Parse SOA DNS data reponses into a data frame
#'
#' @md
#' @param x a character vector of SOA responses
#' @return data frame (tibble)
#' @export
parse_soa <- function(x) {

  stri_trans_tolower(x) %>%
    stri_trim_both() %>%
    stri_match_all_regex(soa_regex) %>%
    map_df(~{
      set_names(
        as.list(.x[2:8]),
        c("mname", "rname", "serial", "refresh", "retry", "expire", "minimum")
      )
    }) -> out

  out$refresh <- as.numeric(out$refresh)
  out$retry <- as.numeric(out$retry)
  out$expire <- as.numeric(out$expire)
  out$minimum <- as.numeric(out$minimum)

  out

}
