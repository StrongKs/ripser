read_lower_tri <- function(file){
  mat <- read.csv(file, header=FALSE, blank.lines.skip=FALSE)
  as.dist(mat)
}
read_lower_tri("downloads/...")

test_mat <- matrix(c(
  0, 1, 2,
  1, 0, 3,
  2, 3, 0
), nrow=3, byrow=TRUE)
