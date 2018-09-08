#' Parse DMARCv1 DNS data reponses into a data frame
#'
#' @md
#' @param x a character vector of DMARCv1 responses
#' @return data frame (tibble)
#' @export
parse_dmarc <- function(x) {

  map_df(x, ~{
    .x <- trimws(gsub('[\\\\"]+', " ", .x))
    strsplit(.x, ";[[:space:]]*") %>%
      unlist() %>%
      map(strsplit, "=") %>%
      map(~{
        as.list(
          set_names(
            .x[[1]][2], # values
            .x[[1]][1]  # names
          )
        )
      }) %>%
      unlist(recursive = FALSE) %>%
      as.data.frame(stringsAsFactors=FALSE) -> out
    class(out) <- c("tbl_df", "tbl", "data.frame")
    out
  })

}