/*Basic lock based circular buffer
* Uses statically allocated std::array for buffer
* Uses space variable to determine space not read/write pointer
* Can push single value or array requires c++20 for std::span
* Theunis R. Botha 10 June 2022
*/

#pragma once
#ifndef __LOCKCIRCULARBUFFER__
#define __LOCKCIRCULARBUFFER__
#include <array>
#include <mutex>
#include <optional>
#include <atomic>  //just used for debug write read counter which are atomic
#include <span>
template<class T>
class LockCircularBuffer
{
public:
	LockCircularBuffer(void) noexcept;
	bool push_back(std::span<T> val) noexcept;
	bool push_back(T val) noexcept;
	std::optional<T> pop_front(void) noexcept;
	size_t increment(size_t idx, size_t count = 1) const noexcept;
	size_t decrement(size_t idx, size_t count = 1) const noexcept;
	const size_t get_size()const noexcept;
	std::atomic<size_t> debug_reads{ 0 }, debug_writes{ 0 };

private:
	static const std::size_t m_size = 1024; //power of 2 is best
	std::array<T, m_size> m_buffer;
	//T m_buffer[m_size];
	//std::atomic<size_t> m_write_ptr{ 0 }, m_read_ptr{ 0 }, m_space{ m_size };
	size_t m_write_ptr{ 0 }, m_read_ptr{ 0 }, m_space{ m_size };
	std::mutex m_mutex;

};
#include "LockCircularBuffer.hpp"
#endif


