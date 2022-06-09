#include <stdio.h>
#include <thread>
#include <windows.h>

#include "AtomicCircularBuffer.h"
#include "LockCircularBuffer.h"

void insert_elements(size_t id, AtomicCircularBuffer<double>& atomic_buffer);
void read_elements(size_t id, AtomicCircularBuffer<double>& atomic_buffer);
void insert_elements2(size_t id, LockCircularBuffer<double>& lock_buffer);
void read_elements2(size_t id, LockCircularBuffer<double>& lock_buffer);

std::atomic<uint8_t> stop{ 0 };
const size_t num_threads_write = 1;
const size_t num_threads_read = 1;
size_t ReadsPerThread[num_threads_read] = { 0 };
size_t WritesPerThread[num_threads_write] = { 0 };

int main(void) {
	size_t t = ((size_t)0 - 1) % 1024;
	AtomicCircularBuffer<double> atomic_buffer;
	LockCircularBuffer<double> lock_buffer;
	for (size_t i = 0; i < 1024; ++i) 
		atomic_buffer.push_back(5.5);
	std::optional<double> val = atomic_buffer.pop_front();
	val = atomic_buffer.pop_front();



	std::thread t_w[num_threads_write];
	for (size_t i = 0; i < num_threads_write; ++i) {
		t_w[i] = std::thread(insert_elements, i, std::ref(atomic_buffer));
	}
	std::thread t_r[num_threads_read];
	for (size_t i = 0; i < num_threads_read; ++i) {
		t_r[i] = std::thread(read_elements,i, std::ref(atomic_buffer));
	}

	Sleep(1000);
	stop = 1;
	Sleep(1);
	for (size_t i = 0; i < num_threads_read; ++i) {
		t_r[i].join();
		printf(" Thread = %i, Reads = %i\n", i, ReadsPerThread[i]);
		ReadsPerThread[i] = 0;
	}
	for (size_t i = 0; i < num_threads_write; ++i) {
		t_w[i].join();
		printf(" Thread = %i, Writes = %i\n", i, WritesPerThread[i]);
		WritesPerThread[i] = 0;
	}

	size_t writes{ atomic_buffer.debug_writes };
	size_t	reads{ atomic_buffer.debug_reads };
	size_t	diff{ atomic_buffer.debug_writes - atomic_buffer.debug_reads };
	printf("number of inserts = %f, Reads = %f diff %i\n", (double)writes / 1'000'000., (double)reads / 1'000'000., diff);
	stop = 0;
	std::thread t_w2[num_threads_write];
	for (size_t i = 0; i < num_threads_write; ++i) {
		t_w[i] = std::thread(insert_elements2, i, std::ref(lock_buffer));
	}
	for (size_t i = 0; i < num_threads_read; ++i) {
		t_r[i] = std::thread(read_elements2, i, std::ref(lock_buffer));
	}

	Sleep(1000);
	stop = 1;
	Sleep(1);
	for (size_t i = 0; i < num_threads_read; ++i) {
		t_r[i].join();
		printf(" Thread = %i, Reads = %i\n", i, ReadsPerThread[i]);
	}
	for (size_t i = 0; i < num_threads_write; ++i) {
		t_w[i].join();
		printf(" Thread = %i, Writes = %i\n", i, WritesPerThread[i]);
	}
	writes = lock_buffer.debug_writes ;
	reads =  lock_buffer.debug_reads;
	diff = lock_buffer.debug_writes - lock_buffer.debug_reads;
	printf("number of inserts = %f, Reads = %f diff %i\n", (double)writes/1'000'000., (double)reads / 1'000'000., diff);


	return 0;

}


void read_elements(size_t id, AtomicCircularBuffer<double>& atomic_buffer) {
	const size_t size = 30;
	while (!stop) {
		for (size_t i = 0; i < size; ++i) {
			std::optional<double> val = atomic_buffer.pop_front();
			if (val != std::nullopt) {
				ReadsPerThread[id]++;
			}
		}
	}
}

void insert_elements(size_t id, AtomicCircularBuffer<double> &atomic_buffer) {
	const size_t size = 30;
	while (!stop) {
		//for (size_t i = 0; i < size; ++i) {
			if (atomic_buffer.push_back(5.5)){
				//printf("Thread %i inserted %i\n", id, i);
				WritesPerThread[id]++;
			}

	//	}
	}
	
}

void read_elements2(size_t id, LockCircularBuffer<double>& lock_buffer) {
	const size_t size = 30;
	while (!stop) {
	//	for (size_t i = 0; i < size; ++i) {
		std::optional<double> val = lock_buffer.pop_front();
		if (val != std::nullopt) {
			ReadsPerThread[id]++;
		}
	//	}
	}
}

void insert_elements2(size_t id, LockCircularBuffer<double>& lock_buffer) {
	const size_t size = 30;
	while (!stop) {
	//	for (size_t i = 0; i < size; ++i) {
			if (lock_buffer.push_back(5.5)) {
				WritesPerThread[id]++;
				//printf("Thread %i inserted %i\n", id, i);
			}

	//	}
	}

}