#ifndef PMATRIX_H
#define PMATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <random>
#include <chrono>
#include <thread>

#define UPPER 100
#define LOWER 0

template<typename T>
class PMatrix {
  public:
    PMatrix(std::size_t width, std::size_t height);
	~PMatrix();

    PMatrix(const PMatrix &rhs);

	PMatrix& operator=(const PMatrix &rhs);
	PMatrix operator*(const PMatrix &rhs);

	void print();
	void randomFill();

  private:
	// NOTE: Threaded multiplication helper
	template<typename X>
	friend void threadedMultiplication(PMatrix<X> &ret, const PMatrix<X> m1, const PMatrix<X> m2, const std::size_t threadID);
	std::size_t m_NumThreads = 3;

	std::size_t getIndex(std::size_t i, std::size_t j) const;

	std::size_t m_Columns, m_Rows;
	std::vector<T> data;
};

template<typename X>
void threadedMultiplication(PMatrix<X> &ret, const PMatrix<X> m1, const PMatrix<X> m2, const std::size_t threadID) {
  const std::size_t numElements = (m1.m_Columns * m1.m_Rows);
  const std::size_t numOperations = numElements / m1.m_NumThreads;
  const std::size_t restOperations = numElements % m1.m_NumThreads;

  std::size_t start_op, end_op;

  if (threadID == 0) {
	start_op = numOperations * threadID;
	end_op = (numOperations * (threadID + 1)) + restOperations;
  } else {
	start_op = numOperations * threadID + restOperations;
	end_op = (numOperations * (threadID + 1)) + restOperations;
  }

  for (std::size_t op = start_op; op < end_op; ++op) {
	const std::size_t row = op % m2.m_Rows;
	const std::size_t col = op / m1.m_Columns;
	// std::cout << "Thread " << threadID << ": rc " << row << ' ' << col << '\n';
	X temp = 0;
	for (std::size_t i = 0; i < m1.m_Columns; ++i) {
	  temp += m1.data.at(m1.getIndex(row, i)) * m2.data.at(m2.getIndex(i, col));
	  // std::cout << "Thread " << threadID << ": " << m1.getIndex(row, i) << " * " << m2.getIndex(i, col) << ' ' << ret.getIndex(row, col) << '\n';
	}
	// std::cout << "Thread " << threadID << ": " << ret.getIndex(row, col) << ' ' << temp << '\n';
	ret.data.insert(ret.data.begin() + ret.getIndex(row, col), temp);
  }
}


template<typename T>
PMatrix<T>::PMatrix(std::size_t width, std::size_t height) : m_Columns(width), m_Rows(height) {
  data.resize(m_Columns * m_Rows);
  // randomFill();
}

template<typename T>
PMatrix<T>::PMatrix(const PMatrix &rhs) {
  if (rhs.m_Columns > 0 && rhs.m_Rows > 0) {
	m_Columns = rhs.m_Columns;
	m_Rows = rhs.m_Rows;
	data = rhs.data;
  }
}

template<typename T>
PMatrix<T>::~PMatrix() { }

template<typename T>
void PMatrix<T>::randomFill() {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_real_distribution<T> unif(LOWER, UPPER);

  for (std::size_t i = 0; i < m_Rows; ++i) {
	for (std::size_t j = 0; j < m_Columns; ++j) {
	  data.insert(data.begin() + getIndex(i, j), unif(re));
	}
  }
}

// Template specialization to use uniform_std::size_t_distribution.
template<>
void PMatrix<int>::randomFill() {

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<int> unif(LOWER, UPPER);

  for (std::size_t i = 0; i < m_Rows; ++i) {
	for (std::size_t j = 0; j < m_Columns; ++j) {
	  data.insert(data.begin() + getIndex(i, j), unif(re));
	}
  }
}

template<typename T>
std::size_t PMatrix<T>::getIndex(std::size_t i, std::size_t j) const {
  return i * m_Columns + j;
}

template<typename T>
void PMatrix<T>::print() {
  for (std::size_t i = 0; i < m_Rows; ++i) {
	for (std::size_t j = 0; j < m_Columns; ++j) {
	  std::cout << data.at(getIndex(i, j)) << ' ';
	}
	std::cout << '\n';
  }
}

template<typename T>
PMatrix<T>& PMatrix<T>::operator=(const PMatrix<T>& rhs) {
  if (rhs.m_Columns > 0 && rhs.m_Rows > 0) {
	m_Columns = rhs.m_Columns;
	m_Rows = rhs.m_Rows;
	data = rhs.data;
  }
  return *this;
}

template<typename T>
PMatrix<T> PMatrix<T>::operator*(const PMatrix &rhs) {
  if(m_Rows != rhs.m_Columns) {
	throw std::runtime_error("Row Count on A doesn't match Column Count on B");
  }

  PMatrix<T> ret {m_Columns, rhs.m_Rows};

  std::thread threads[m_NumThreads];

  for (std::size_t i = 0; i < m_NumThreads; ++i) {
	threads[i] = std::thread(threadedMultiplication<T>, std::ref(ret), *this, rhs, i);
  }
  for (std::size_t i = 0; i < m_NumThreads; ++i) {
	threads[i].join();
  }

  return ret;
}

#endif
