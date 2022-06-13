#include "AtomicCircularBuffer.h"
template<class T>
AtomicCircularBuffer<T>::AtomicCircularBuffer() noexcept {

}

template<class T>
inline bool AtomicCircularBuffer<T>::push_back(std::span<T> values) noexcept {
	//perform weak check for space, space is limited to m_size-1 (can be removed)
	if (m_space > values.size()) {
		//perform stronger check assuming write_ptr value, if changed write does not occur
		//if read_ptr has changed then more space is available and no problem occurs
		size_t  write_ptr = m_write_ptr;
		size_t  read_ptr = m_read_ptr;
		size_t  write_ptr_next = increment(write_ptr, values.size());
		bool  fail_case1 = ((write_ptr < read_ptr) && (write_ptr_next >= read_ptr)); //juming over read pointer
		bool  fail_case2 = ((write_ptr > read_ptr) && ((values.size() -(write_ptr - read_ptr)) < m_size)); //
		
		if (fail_case1 | fail_case2)
			return false;

		//check if write_ptr has been changed if not space has been allocated to us
		if (m_write_ptr.compare_exchange_weak(write_ptr, write_ptr_next)) {
			//either write all elements or to just end of array
			const size_t idx = min(m_size- write_ptr, values.size());
			for(size_t i = 0; i < idx; ++i)
				m_buffer[write_ptr+i] = values[i];

			//write none or remainder of array from start
			//signed to prevent overflow.
			const int32_t idx2 = (int32_t)values.size() - idx;
			for (int32_t i = 0; i < idx2; ++i)
				m_buffer[i] = values[idx+i];

			m_space.fetch_sub(values.size(), std::memory_order_release);
			debug_writes+=values.size();
			return true;
		}
	}

	return false;
}

template<class T>
inline bool AtomicCircularBuffer<T>::push_back(T val) noexcept {
	//perform weak check for space, space is limited to m_size-1 (can be removed)
	if (m_space > 1) {
		//perform stronger check assuming write_ptr value, if changed write does not occur
		//if read_ptr has changed then more space is available and no problem occurs
		size_t  write_ptr = m_write_ptr;
		size_t  write_ptr_next = increment(write_ptr);
		if(write_ptr_next == m_read_ptr )//this loses one slot but makes implementation easier
			return false;

		//check if write_ptr has been changed if not space has been allocated to us
		if (m_write_ptr.compare_exchange_weak(write_ptr, write_ptr_next)) {
			m_buffer[write_ptr] = val;
			m_space.fetch_sub(1, std::memory_order_release);
			debug_writes++;
			return true;
		}
	}

	return false;
}



template<class T>
inline std::optional<T> AtomicCircularBuffer<T>::pop_front(void) noexcept {
	//perform weak check (can be removed)
	std::optional<T> value{};
	if (m_space.load(std::memory_order_acquire) == m_size) { //no elements memory_order_acquire= nothing after this can be done before in memory
		return value;
	}
	//perform checks assuming read_ptr 
	size_t  read_ptr = m_read_ptr;
	size_t  read_ptr_next = increment(read_ptr);
	if (read_ptr == m_write_ptr)
		return value;
	value = m_buffer[read_ptr]; //read value now as when read_ptr updated value could be overwritten by write
	//if read_ptr unchanged then value can be read
	if (m_read_ptr.compare_exchange_weak(read_ptr, read_ptr_next)) {
		m_space++;
		debug_reads++;
		//return m_buffer[read_ptr];
		return value;
	}
	return value;
}

template<class T>
inline size_t AtomicCircularBuffer<T>::increment(size_t idx, size_t count) const noexcept {
	return ((idx+ count) %m_size);
}

template<class T>
inline size_t AtomicCircularBuffer<T>::decrement(size_t idx, size_t count) const noexcept {
	return ((idx - count) % m_size);
}

template<class T>
inline const size_t AtomicCircularBuffer<T>::get_size() const noexcept {
	return m_size;
}