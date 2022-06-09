#include "LockCircularBuffer.h"
template<class T>
LockCircularBuffer<T>::LockCircularBuffer() {

}

template<class T>
inline bool LockCircularBuffer<T>::push_back(T val) {
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
inline std::optional<T> LockCircularBuffer<T>::pop_front(void) { //use optional later
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
inline size_t LockCircularBuffer<T>::increment(size_t idx) {
	return ((idx + 1) % m_size);
}

template<class T>
inline const size_t LockCircularBuffer<T>::get_size() const {
	return m_size;
}