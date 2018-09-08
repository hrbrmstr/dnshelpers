.parse_spf <- function(x) {

  orig <- x

  x <- stri_trans_tolower(x)
  x <- stri_replace_all_regex(x, "^[[:punct:][[:space:]]]+|[[:punct:][:space:]]+$", "")

  if (!stri_detect_regex(x, "^v=spf1")) {

    return(list(
      spf_record = orig,
      valid = FALSE
    ))

  } else {

    x <- stri_replace_all_regex(x, "^[^v]*|[[:punct:][:space:]]$", "")
    x <- unlist(strsplit(x, " +"))[-1]

    keep(x, ~{
      stri_detect_regex(.x, "^[[:punct:]]*(all|ip4|ip6|a|mx|ptr|exists|include)")
    }) %>%
      stri_match_all_regex(
        "^([[:punct:]]{0,1})(all|ip4|ip6|a|mx|ptr|exists|include):{0,1}(.*)"
      ) %>%
      map_df(~{
        set_names(as.list(.[2:4]), c("qualifier", "mechanism", "data"))
      }) -> mechs

    if (length(mechs) == 0) {
      mechs <- data.frame(stringsAsFactors=FALSE)
    } else {
      mechs$qualifier <- ifelse(mechs$qualifier == "", "+", mechs$qualifier)
      mechs$data <- ifelse(mechs$data == "", NA_character_, mechs$data)
    }

    class(mechs) <- c("tbl_df", "tbl", "data.frame")

    keep(x, ~{
      stri_detect_regex(.x, "^(redirect|exp)")
    }) %>%
      stri_match_all_regex("^(redirect|exp)=(.*)") %>%
      map_df(~{
        set_names(as.list(.[2:3]), c("modifier", "data"))
      }) -> mods

    if (length(mods) == 0) mods <- data.frame(stringsAsFactors=FALSE)

    class(mods) <- c("tbl_df", "tbl", "data.frame")

    list(
      mechasims = list(mechs),
      modifiers = list(mods),
      valid = TRUE,
      spf_record = orig
    )

  }

}

#' Parse SPF TXT DNS data reponses into a list
#'
#' @md
#' @param x a character vector of DMARCv1 responses
#' @return list (that will collape nicely into a data frame with `dply::bind_rows()`
#'         or `data.table::rbindlist(..., fill=TRUE)`).
#' @export
parse_spf <- function(x) {
  map(x, .parse_spf)
}
