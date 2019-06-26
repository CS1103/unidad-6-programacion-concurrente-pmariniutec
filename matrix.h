#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <random>
#include <chrono>

#define UPPER 100
#define LOWER 0

template<typename T>
class Matrix {
  public:
    Matrix(std::size_t width, std::size_t height);

    Matrix(const Matrix &rhs);

	Matrix& operator=(const Matrix &rhs);
	Matrix operator*(const Matrix &rhs);

	void print();

  private:
	void randomFill();
	std::size_t getIndex(std::size_t i, std::size_t j);
	std::size_t m_Columns, m_Rows;
	std::vector<T> data;
};

template<typename T>
Matrix<T>::Matrix(std::size_t width, std::size_t height) : m_Columns(width), m_Rows(height) {
  data.resize(m_Columns * m_Rows);
  randomFill();
}

template<typename T>
Matrix<T>::Matrix(const Matrix &rhs) {
  if (rhs.m_Columns > 0 && rhs.m_Rows > 0) {
	m_Columns = rhs.m_Columns;
	m_Rows = rhs.m_Rows;
	data = rhs.data;
  }
}

template<typename T>
void Matrix<T>::randomFill() {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_real_distribution<T> unif(LOWER, UPPER);

  for (int i = 0; i < m_Rows; ++i) {
	for (int j = 0; j < m_Columns; ++j) {
	  data.insert(data.begin() + getIndex(i, j), unif(re));
	}
  }
}

// Template specialization to use uniform_int_distribution.
template<>
void Matrix<int>::randomFill() {

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<int> unif(LOWER, UPPER);

  for (int i = 0; i < m_Rows; ++i) {
	for (int j = 0; j < m_Columns; ++j) {
	  data.insert(data.begin() + getIndex(i, j), unif(re));
	}
  }
}

template<typename T>
std::size_t Matrix<T>::getIndex(std::size_t i, std::size_t j) {
  return i * m_Columns + j;
}

template<typename T>
void Matrix<T>::print() {
  for (int i = 0; i < m_Rows; ++i) {
	for (int j = 0; j < m_Columns; ++j) {
	  std::cout << data.at(getIndex(i, j)) << ' ';
	}
	std::cout << '\n';
  }
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs) {
  if (rhs.m_Columns > 0 && rhs.m_Rows > 0) {
	m_Columns = rhs.m_Columns;
	m_Rows = rhs.m_Rows;
	data = rhs.data;
  }
  return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix &rhs) {
  if(m_Rows != rhs.m_Columns) {
	throw std::runtime_error("Row Count on A doesn't match Column Count on B");
  }

  Matrix<T> ret {m_Columns, rhs.m_Rows};

  auto t1 = std::chrono::high_resolution_clock::now();

  for (std::size_t i = 0; i < m_Columns; ++i) {
	for (std::size_t j = 0; j < rhs.m_Rows; ++j) {
	  auto temp = data.at(i * m_Rows) * rhs.data.at(j);
	  for (std::size_t k = 1; k < m_Rows; ++k) {
		temp += data.at(i * m_Rows + k) * rhs.data.at(k * rhs.m_Rows + j);
	  }
	  ret.data.insert(ret.data.begin() + getIndex(i, j), temp);
	}
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
  std::cout << "Time with " << 1 << " threads: " << duration << " ms" << '\n';

  return ret;
}

#endif
