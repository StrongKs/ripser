#' Compute something using ripser
#'
#' A longer description of what `ripser_test()` does.
#'
#' @param input Describe the `input` parameter here.
#' @return Describe what the function returns.
#' @export
ripser_test <- function(input) {
  .Call('_ripserAPI_ripser_test', PACKAGE = 'ripserAPI', input)
}
