#include <Rcpp.h>
using namespace Rcpp;

// compute sums of distances for dcov V- and U-statistics
// and energy statistics without storing distance matrices

double        Gini_sum(NumericMatrix x);
double        Gini_sum2(NumericMatrix x, NumericMatrix y);
double        sum_paired_dist(NumericMatrix x, NumericMatrix y);
double        sum_dist3(NumericMatrix x, NumericMatrix y);
NumericVector rowSums_dist(NumericMatrix x);
          
// [[Rcpp::export]]
double Gini_sum(NumericMatrix x) {
  /*
  sum all Euclidean distances |x_i - x_j|
  without storing a distance matrix 
  */
  int i, j, k, n=x.nrow(), d=x.ncol();
  double sum = 0.0, dsum, dif;

  for (i=0; i<n; i++) {
    for (j=0; j<i; j++) {
      if (d == 1) {
        sum += fabs(x(i, 0) - x(j, 0));
      } else {
        dsum = 0.0;
        for (k=0; k<d; k++) {
          dif = x(i, k) - x(j, k);
          dsum += dif*dif;
        }
        sum += sqrt(dsum);
      }
    }
  }
  
  return 2 * sum; 
}

// [[Rcpp::export]]
double Gini_sum2(NumericMatrix x, NumericMatrix y) {
  /*
  sum all Euclidean distances |x_i - y_j|
  without storing distance matrices 
  */
  int i, j, k, n=x.nrow(), m=y.nrow(), p=x.ncol(), q=y.ncol();
  double sum = 0.0, dsum, dif;
  
  if (p != q) {
    Rprintf("distance not defined for unequal dims: %d %d\n", p, q);
    Rf_warning("Gini_sum2: unequal dimensions, returning NA");
    return NA_REAL;
  }
  
  for (i=0; i<n; i++) {
    for (j=0; j<m; j++) {
      if (p == 1) {
        sum += fabs(x(i, 0) - y(j, 0));
      } else {
        dsum = 0.0;
        for (k=0; k<p; k++) {
          dif = x(i, k) - y(j, k);
          dsum += dif*dif;
        }
        sum += sqrt(dsum);
      }
    }
  }
  return sum;
}

// [[Rcpp::export]]
double sum_paired_dist(NumericMatrix x, NumericMatrix y) {
  /*
  sum all products a_{ij}b_{ij} = |x_i - x_j||y_i - y_j|
  without storing distance matrices 
  used in computing the dcov^2 V-statistic or U-statistic 
  */
  int i, j, k, n=x.nrow(), m=y.nrow(), p=x.ncol(), q=y.ncol();
  double a, b, sum = 0.0, dif;
  
  if (m != n) {
    Rprintf("not defined for unequal samples sizes: %d %d\n", n, m);
    Rf_warning("sum_paired_dist: unequal samples sizes, returning NA");
    return NA_REAL;
  }
  
  if (p == 1 && q == 1) {
    sum = 0.0;
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        a = fabs(x(i, 0) - x(j, 0));
        b = fabs(y(i, 0) - y(j, 0));
        sum += a * b;
      }
    }
    return sum;
  }
      
  // for random vectors
  sum = 0.0;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
        a = 0.0;
        b = 0.0;
        for (k=0; k<p; k++) {
          dif = x(i, k) - x(j, k);
          a += dif * dif;
        }
        for (k=0; k<q; k++) {
          dif = y(i, k) - y(j, k);
          b += dif * dif;
        }
        sum += sqrt(a) * sqrt(b);
      }
    }
  
  return sum;
}

// [[Rcpp::export]]
double sum_dist3(NumericMatrix x, NumericMatrix y) {
  /*
  sum_{i,j,k=1}^n = |x_i - x_j||y_i - y_k|   (triples i,j,k)
  without storing distance matrices
  used in computing the dcov^2 V-statistic or U-statistic
  */
  int i, j, k, L, n=x.nrow(), m=y.nrow(), p=x.ncol(), q=y.ncol();
  double a, b, sum = 0.0, dif;
  
  if (m != n) {
    Rprintf("not defined for unequal samples sizes: %d %d\n", n, m);
    Rf_warning("sum_dist3: unequal samples sizes, returning NA");
    return NA_REAL;
  }
  
  if (p == 1 && q == 1) {
    sum = 0.0;
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        for (L=0; L<n; L++) {
          a = fabs(x(i, 0) - x(j, 0));
          b = fabs(y(i, 0) - y(L, 0));
          sum += a * b;
        }
      }
    }
    return sum;
  }
  
  // for random vectors
  sum = 0.0;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      for (L=0; L<n; L++) {
        a = 0.0;
        b = 0.0;
        for (k=0; k<p; k++) {
          dif = x(i, k) - x(j, k);
          a += dif * dif;
        }
        for (k=0; k<q; k++) {
          dif = y(i, k) - y(L, k);
          b += dif * dif;
        }
        sum += sqrt(a) * sqrt(b);
      }
    }
  }
  
  return sum;
}

// [[Rcpp::export]]
NumericVector rowSums_dist(NumericMatrix x) {
  /*
  sum the rows of a distance matrix A = (a_{ij}) = (|x_i - x_j|)
  without creating the matrix 
  return the vector of row sums (a_{i.}) = (a_{1.}, ..., a_{n.})^T
  */
  int i, j, k, n=x.nrow(), p=x.ncol();
  double a, dif;
  NumericVector rowsums(n);      // A is n by n
  
  if (p == 1) {
    for (i=0; i<n; i++) {
      a = 0.0;
      for (j=0; j<n; j++) {
        a += fabs(x(i, 0) - x(j, 0));
      }
      rowsums(i) = a;
    }
    return rowsums;
  }
  
  // for random vectors
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      a = 0.0;
      for (k=0; k<p; k++) {
        dif = x(i, k) - x(j, k);
        a += dif * dif;
      }
      rowsums(i) += sqrt(a);
    }
  }
  return rowsums;
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//

/*** R
x <- as.matrix(iris[1:50, 1:2])
y <- as.matrix(iris[1:50, 3:4])
n <- 50

aidot <- rowSums_dist(x)
bidot <- rowSums_dist(y)
a <- sum(aidot)
b <- sum(bidot)
ab3 <- sum_dist3(x, y)
ab <- sum_paired_dist(x, y)
ab
2 * sum(dist(x)*dist(y))

a
Gini_sum(x)

v <- ab / n^2 + a*b/n^4 - 2*ab3/n^3
v
energy::dcov(x, y)^2
*/