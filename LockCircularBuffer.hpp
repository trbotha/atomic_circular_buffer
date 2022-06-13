#include "LockCircularBuffer.h"
template<class T>
LockCircularBuffer<T>::LockCircularBuffer() noexcept {

}

template<class T>
inline bool LockCircularBuffer<T>::push_back(std::span<T> values) noexcept {
	std::lock_guard lk(m_mutex);
	//check for space if available write data
	if (values.size() <= m_space) {
		const size_t idx = min(m_size - m_write_ptr, values.size());
		//either write all elements or to just end of array
		for (size_t i = 0; i < idx; ++i)
			m_buffer[m_write_ptr + i] = values[i];

		//write none or remainder of array from start
		const int32_t idx2 = (int32_t)values.size() - idx;
		for (int32_t i = 0; i < idx2; ++i)
			m_buffer[i] = values[idx + i];

		m_write_ptr = increment(m_write_ptr, values.size());
		m_space -= values.size();
		debug_writes+= values.size();
		return true;
	}

	return false;
}

template<class T>
inline bool LockCircularBuffer<T>::push_back(T val) noexcept {
	std::lock_guard lk(m_mutex);
	if (m_space > 0) {		
		m_buffer[m_write_ptr] = val;
		m_write_ptr = increment(m_write_ptr);
		m_space--;
		debug_writes++;
		return true;
	}

	return false;
}

template<class T>
inline std::optional<T> LockCircularBuffer<T>::pop_front(void) noexcept {
	std::lock_guard lk(m_mutex);
	if (m_space == m_size) { //no elements
		return std::nullopt;
	}
	m_space++;
	debug_reads++;
	size_t read_ptr = m_read_ptr;
	m_read_ptr = increment(m_read_ptr);
	return m_buffer[read_ptr];
}

template<class T>
inline size_t LockCircularBuffer<T>::increment(size_t idx, size_t count) const noexcept {
	return ((idx + count) % m_size);
}

template<class T>
inline size_t LockCircularBuffer<T>::decrement(size_t idx, size_t count) const noexcept {
	return ((idx - count) % m_size);
}

template<class T>
inline const size_t LockCircularBuffer<T>::get_size() const noexcept {
	return m_size;
}