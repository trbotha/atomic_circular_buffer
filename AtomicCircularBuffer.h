#pragma once
#ifndef __ATOMICCIRCULARBUFFER__
#define __ATOMICCIRCULARBUFFER__
#include<array>
#include <atomic>
#include <optional>
template<class T>
class AtomicCircularBuffer
{
public:
	AtomicCircularBuffer(void);
	bool push_back(T val);
	std::optional<T> pop_front(void);
	size_t increment(size_t) const;
	size_t decrement(size_t) const;
	const size_t get_size()const ;
	std::atomic<size_t> debug_reads{ 0 }, debug_writes{ 0 };

private:
	static const std::size_t m_size = 1024; //power of 2 is best
	std::array<T, m_size> m_buffer;
	std::atomic<size_t> m_write_ptr{ 0 }, m_read_ptr{ 0 }, m_space{ m_size };
	
};
#include "AtomicCircularBuffer.hpp"
#endif


