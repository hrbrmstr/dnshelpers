#' Parse MX DNS data reponses into a data frame
#'
#' @md
#' @param x a character vector of MX responses
#' @return data frame (tibble)
#' @export
parse_mx <- function(x) {

  stri_trans_tolower(x) %>%
    stri_trim_both() %>%
    stri_match_all_regex("^([[:digit:]]+)[[:space:]]+(.*)") %>%
    map_df(~{
      list(preference = as.integer(.x[2]), data = .x[3])
    })

}
