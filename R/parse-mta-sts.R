#' Parse MTA-STS DNS data reponses into a list
#'
#' @md
#' @param x a character vector of MTA-STS responses
#' @return list (that will collape nicely into a data frame with `dply::bind_rows()`
#'         or `data.table::rbindlist(..., fill=TRUE)`).
#' @export
parse_mta_sts <- function(x) {

  stri_trans_tolower(x) %>%
    stri_trim_both() %>%
    stri_split_regex(";[[:space:]]*", simplify = FALSE) %>%
    lapply(function(.x) {
      tmp <- sapply(.x, stri_trim_both, USE.NAMES = FALSE)
      tmp <- tmp[tmp != ""]
      tmp <- tmp[stri_detect_fixed(tmp, "=")]
      if (length(tmp) == 0) return(data.frame(v=NA_character_, id=NA_character_, stringsAsFactors = FALSE))
      tmp <- stri_split_fixed(tmp, "=", 2, simplify = TRUE)
      tmp <- as.data.frame(as.list(set_names(tmp[,2], tmp[,1])), stringsAsFactors=FALSE)
      tmp
    })

}
?dns_h