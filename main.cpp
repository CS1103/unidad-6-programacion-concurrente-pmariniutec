#include <iostream>
#include "matrix.h"
#include "matrix_threads.h"

constexpr long long size = 1024;

int main() {

  {
	Matrix<int> m1{size, size};
	Matrix<int> m2{size, size};

  }

  {
	PMatrix<int> m1{size, size, 2};
	m1.randomFill();
	PMatrix<int> m2{size, size, 2};
	m2.randomFill();

	auto m3 = m1 * m2;
  }
  {
	PMatrix<int> m1{size, size, 4};
	m1.randomFill();
	PMatrix<int> m2{size, size, 4};
	m2.randomFill();

	auto m3 = m1 * m2;
  }
  {
	PMatrix<int> m1{size, size, 8};
	m1.randomFill();
	PMatrix<int> m2{size, size, 8};
	m2.randomFill();

	auto m3 = m1 * m2;
  }
  {
	PMatrix<int> m1{size, size, 16};
	m1.randomFill();
	PMatrix<int> m2{size, size, 16};
	m2.randomFill();

	auto m3 = m1 * m2;
  }
}
