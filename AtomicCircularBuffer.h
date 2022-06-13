/*Basic lock free circular buffer
* Uses statically allocated std::array for buffer can only use size-1 
* Uses atomically compare_exchange_weak to determine whether one can
* read or write data. Can push single value or array requires c++20 for std::span
* Theunis R. Botha 10 June 2022
*/
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
	AtomicCircularBuffer(void) noexcept;
	bool push_back(std::span<T> val) noexcept;
	bool push_back(T val) noexcept;
	std::optional<T> pop_front(void) noexcept;
	size_t increment(size_t, size_t count = 1) const noexcept;
	size_t decrement(size_t, size_t count = 1) const noexcept;
	const size_t get_size()const noexcept;
	std::atomic<size_t> debug_reads{ 0 }, debug_writes{ 0 };

private:
	constexpr static std::size_t m_size = 1024; //power of 2 is best
	std::array<T, m_size> m_buffer;
	//T m_buffer[m_size];
	std::atomic<size_t> m_write_ptr{ 0 }, m_read_ptr{ 0 }, m_space{ m_size};
	
};
#include "AtomicCircularBuffer.hpp"
#endif


