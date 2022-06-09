#include "AtomicCircularBuffer.h"
template<class T>
AtomicCircularBuffer<T>::AtomicCircularBuffer() {

}

template<class T>
inline bool AtomicCircularBuffer<T>::push_back(T val){
	if (m_space > 0) {
		size_t  write_ptr = m_write_ptr;
		size_t  write_ptr_next = increment(write_ptr);
		if(write_ptr_next == m_read_ptr )//this loses one slot but makes implementation easier
			return false;
		std::atomic<size_t> test = 0;

		if (m_write_ptr.compare_exchange_weak(write_ptr, write_ptr_next)) {
			m_buffer[write_ptr] = val;
			//printf("pointer = %i\n", write_ptr);
			//m_space.store(decrement(m_space), std::memory_order_release);  //memory_order_release = nothing before this can be done after in memory
			//m_space = decrement(m_space);
			m_space.fetch_sub(1, std::memory_order_release);
			//m_space--;
			debug_writes++;
			return true;
		}
	}

	return false;
}

template<class T>
inline std::optional<T> AtomicCircularBuffer<T>::pop_front(void){ //use optional later
	if (m_space.load(std::memory_order_acquire) == m_size) { //no elements memory_order_acquire= nothing after this can be done before in memory
		return std::nullopt;
	}
	size_t  read_ptr = m_read_ptr;
	size_t  read_ptr_next = increment(read_ptr);
	if (increment(read_ptr_next) == m_write_ptr)
		return std::nullopt;
	if (m_read_ptr.compare_exchange_weak(read_ptr, read_ptr_next)) {
		m_space++;
		debug_reads++;
		return m_buffer[read_ptr];
	}
	return std::nullopt;
}

template<class T>
inline size_t AtomicCircularBuffer<T>::increment(size_t idx) const{
	return ((idx+1) %m_size);
}

template<class T>
inline size_t AtomicCircularBuffer<T>::decrement(size_t idx) const {
	return ((idx - 1) % m_size);
}

template<class T>
inline const size_t AtomicCircularBuffer<T>::get_size() const {
	return m_size;
}