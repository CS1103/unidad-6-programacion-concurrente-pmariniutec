#include <iostream>
#include "matrix.h"
#include "matrix_threads.h"

int main() {
  /*
  Matrix<int> m1{3, 3};
  Matrix<int> m2{3, 3};

  auto m3 = m1 * m2;
  m1.print();
  m2.print();
  m3.print();

  */
  std::cout << "\nThreaded Matrix:\n";

  PMatrix<int> m4{3, 3};
  m4.randomFill();
  PMatrix<int> m5{3, 3};
  m5.randomFill();

  auto m6 = m4 * m5;
  m4.print();
  m5.print();
  m6.print();
}
