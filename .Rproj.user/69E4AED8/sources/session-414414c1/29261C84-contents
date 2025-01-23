library(Rcpp)
sourceCpp("C:/Users/alice/Documents/ripser/ripserWrapper/src/ripserWrapper.cpp")
# Loads and compiles the C++ file into R

# Test the function
coords <- matrix(c(0, 0, 1, 1, 0, 1, 1, 0), ncol = 2, byrow = TRUE)
result <- computePersistenceBarcodesR(coords)
print(result)
