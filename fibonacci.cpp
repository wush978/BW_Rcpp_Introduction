#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
int fibonacciRcpp(int n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  return fibonacciRcpp(n-1) + 
    fibonacciRcpp(n-2);
}