#pragma once
#ifndef __ATOMICCIRCULARBUFFER__
#define __ATOMICCIRCULARBUFFER__
#include <array>
#include <atomic>
#include <optional>
#include <span>
template<class T>
class AtomicCircularBuffer
{
public:
	AtomicCircularBuffer(void);
	bool push_back(std::span<T> val);
	bool push_back(T val);
	std::optional<T> pop_front(void);
	size_t increment(size_t, size_t count = 1) const;
	size_t decrement(size_t, size_t count = 1) const;
	const size_t get_size()const ;
	std::atomic<size_t> debug_reads{ 0 }, debug_writes{ 0 };

private:
	static const std::size_t m_size = 1024; //power of 2 is best
	std::array<T, m_size> m_buffer;
	//T m_buffer[m_size];
	std::atomic<size_t> m_write_ptr{ 0 }, m_read_ptr{ 0 }, m_space{ m_size};
	
};
#include "AtomicCircularBuffer.hpp"
#endif


