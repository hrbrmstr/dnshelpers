.dmarc_keys <- c("v", "p", "sp", "adkim", "aspdf", "pct", "fo", "rf", "ri", "rua", "ruf")

#' Parse DMARCv1 DNS data reponses into a data frame
#'
#' @md
#' @param x a character vector of DMARCv1 responses
#' @return data frame (tibble)
#' @export
parse_dmarc <- function(x) {

  x <- stri_trans_tolower(x)
  x <- stri_replace_all_regex(x, '[\\\\"]', ' ')
  x <- stri_trim_both(x)

  map_df(x, function(dmarc_rec) {
    if (is.na(x)) {
      list(is_valid = FALSE)
    } else if (!stri_detect_fixed(dmarc_rec, "v=dmarc1")) {
      list(is_valid = FALSE)
    } else {
      fields <- stri_split_regex(dmarc_rec, ";[[:space:]]*")[[1]]
      rec <- stri_split_fixed(fields, "=", 2, simplify = TRUE)
      rec <- as.list(set_names(stri_trim_both(rec[,2]), stri_trim_both(rec[,1])))

      is_valid <- all(names(rec) %in% .dmarc_keys)
      ok_names <- intersect(.dmarc_keys, names(rec))

      rec <- rec[ok_names]
      rec$is_valid <- is_valid

      rec
    }
  }) -> out

  class(out) <- c("tbl_df", "tbl", "data.frame")
  out

}