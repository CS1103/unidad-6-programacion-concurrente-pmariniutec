#ifndef PMATRIX_H
#define PMATRIX_H


#include <chrono>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <random>
#include <thread>

#define UPPER 100
#define LOWER 0


template<typename T>
class PMatrix {
  public:
    PMatrix(long long width, long long height, long long threads);
	~PMatrix();

    PMatrix(const PMatrix &rhs);

	PMatrix& operator=(const PMatrix &rhs);
	PMatrix operator*(const PMatrix &rhs);

	void print();
	void randomFill();

  private:
	// NOTE: Threaded multiplication helper
	template<typename X>
	friend void threadedMultiplication(PMatrix<X> &ret, const PMatrix<X> &m1, const PMatrix<X> &m2, const long long threadID);
	long long m_NumThreads;

	long long m_Columns, m_Rows;
	std::vector<std::vector<T>> data;
};

template<typename X>
void threadedMultiplication(PMatrix<X> &ret, const PMatrix<X> &m1, const PMatrix<X> &m2, const long long threadID) {
  const long long numElements = m1.m_Columns * m1.m_Rows;
  const long long numOperations = numElements / m1.m_NumThreads;
  const long long restOperations = numElements % m1.m_NumThreads;

  long long start_op, end_op;

  if (threadID == 0) {
	start_op = numOperations * threadID;
	end_op = (numOperations * (threadID + 1)) + restOperations;
  } else {
	start_op = numOperations * threadID + restOperations;
	end_op = (numOperations * (threadID + 1)) + restOperations;
  }

  for (long long op = start_op; op < end_op; ++op) {
	const long long row = op % m2.m_Rows;
	const long long col = op / m1.m_Columns;
	X temp = 0;
	for (long long i = 0; i < m1.m_Columns; ++i) {
	  temp += m1.data.at(row).at(i) * m2.data.at(i).at(col);
	}
	ret.data[row][col] = temp;
  }
}


template<typename T>
PMatrix<T>::PMatrix(long long width, long long height, long long threads) : m_Columns(width), m_Rows(height), m_NumThreads(threads) {
  data.resize(m_Rows);
  for (int i = 0; i < data.size(); ++i) {
    data[i].resize(m_Columns);
  }
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

  for (long long i = 0; i < m_Rows; ++i) {
	for (long long j = 0; j < m_Columns; ++j) {
	  data[i][j] = unif(re);
	}
  }
}

// Template specialization to use uniform_long long_distribution.
template<>
void PMatrix<int>::randomFill() {

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<int> unif(LOWER, UPPER);

  for (long long i = 0; i < m_Rows; ++i) {
	for (long long j = 0; j < m_Columns; ++j) {
	  data[i][j] = unif(re);
	}
  }
}

template<typename T>
void PMatrix<T>::print() {
  for (long long i = 0; i < m_Rows; ++i) {
	for (long long j = 0; j < m_Columns; ++j) {
	  std::cout << data[i][j] << ' ';
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

  PMatrix<T> ret {m_Columns, rhs.m_Rows, m_NumThreads};


  auto t1 = std::chrono::high_resolution_clock::now();

  std::thread threads[m_NumThreads];

  for (long long i = 0; i < m_NumThreads; ++i) {
	threads[i] = std::thread(threadedMultiplication<T>, std::ref(ret), std::ref(*this), std::ref(rhs), i);
  }
  for (long long i = 0; i < m_NumThreads; ++i) {
	threads[i].join();
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
  std::cout << "Time with " << m_NumThreads << " threads: " << duration << " ms" << '\n';

  return ret;
}

#endif
