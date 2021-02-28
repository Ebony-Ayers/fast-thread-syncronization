//MIT License
//
//Copyright (c) 2021 Ebony Ayers
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <atomic>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>

//platform 0: unknown
//platform 1: linux
//platform 2: windows (win32)
#ifdef __linux__
	#define FTS_PLATFORM 1
#endif
#ifdef _WIN32
	#define FTS_PLATFORM 2
#endif
#ifndef FTS_PLATFORM
	#pragma message "FTS Warning: no platform could be identified. Thread sleeping mechanisms will revert to std::mutex"
	#define FTS_PLATFORM 0
#endif

namespace fts
{
	class I_lockable_object
	{
		public:
			virtual void lock() = 0;
			virtual void unlock() = 0;
			virtual bool try_lock() = 0;

			I_lockable_object() = default;
			virtual ~I_lockable_object() = default;
			I_lockable_object(const I_lockable_object&) = default;
			I_lockable_object(I_lockable_object&&) = default;
			I_lockable_object& operator=(const I_lockable_object&) = default;
			I_lockable_object& operator=(I_lockable_object&&) = default;
	};
	
	class spin_lock : I_lockable_object
	{
		public:
			void lock() override;
			void unlock() override;
			bool try_lock() override;

			spin_lock();
		
		private:
			std::atomic_bool m_is_locked;
	};
	class adaptive_lock : I_lockable_object
	{
		public:
			void lock() override;
			void unlock() override;
			bool try_lock() override;

			adaptive_lock();
		
		private:
			int32_t m_address;
			#if FTS_PLATFORM == 0
			std::mutex m_mutex;
			#endif
	};
	class spin_semaphore : I_lockable_object
	{
		public:
			void lock() override;
			void unlock() override;
			bool try_lock() override;

			spin_semaphore();
			spin_semaphore(int32_t max);
		
		private:
			std::atomic_int32_t m_counter;
	};
	class adaptive_semaphore : I_lockable_object
	{
		public:
			void lock() override;
			void unlock() override;
			bool try_lock() override;

			adaptive_semaphore();
			adaptive_semaphore(int32_t max);
		
		private:
			std::atomic_int32_t m_counter;
			int32_t m_address;
	};

	class I_signalling_object
	{
		public:
			virtual void wait() = 0;
			virtual void wake() = 0;
			virtual void wake_all() = 0;

			I_signalling_object() = default;
			virtual ~I_signalling_object() = default;
			I_signalling_object(const I_signalling_object&) = default;
			I_signalling_object(I_signalling_object&&) = default;
			I_signalling_object& operator=(const I_signalling_object&) = default;
			I_signalling_object& operator=(I_signalling_object&&) = default;
	};

	class signal : I_signalling_object
	{
		public:
			void wait() override;
			void wake() override;
			void wake_all() override;

			signal();
		
		private:
			int32_t m_address;
	};
	class spin_signal : I_signalling_object
	{
		public:
			void wait() override;
			void wake() override;
			void wake_all() override;

			spin_signal();
		
		private:
			std::atomic_char m_is_waiting;
	};
}


//=========================================spinlock=========================================
void fts::spin_lock::lock()
{
	while(true)
	{
		if(!m_is_locked.exchange(true, std::memory_order_acquire)) break;
		while(m_is_locked.load(std::memory_order_relaxed));
	}
}
void fts::spin_lock::unlock()
{
	m_is_locked.store(false, std::memory_order_release);
}
bool fts::spin_lock::try_lock()
{
	return !m_is_locked.exchange(true, std::memory_order_acquire);
}
fts::spin_lock::spin_lock()
: m_is_locked(false) {}


//=========================================adaptivelock=========================================
void fts::adaptive_lock::lock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
		m_address = 1;
	//platform: windows
	#elif FTS_PLATFORM == 2
		std::cout << "ungaurded code" << std::endl;
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		m_mutex.lock();
	#endif
}
void fts::adaptive_lock::unlock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		m_address = 0;
		syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
	//platform: windows
	#elif FTS_PLATFORM == 2
		std::cout << "ungaurded code" << std::endl;
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		m_mutex.unlock();
	#endif
}
bool fts::adaptive_lock::try_lock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		if(m_address == 0)
		{
			syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
			return true;
		}
		else return false;
	//platform: windows
	#elif FTS_PLATFORM == 2
		std::cout << "ungaurded code" << std::endl;
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		return m_mutex.try_lock();
	#endif

	return false;
}
fts::adaptive_lock::adaptive_lock()
: m_address(0) {}


//=========================================spinsemaphore=========================================
void fts::spin_semaphore::lock()
{
	while(true)
	{
		auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
		if(prev > 0) return;
		else
		{
			m_counter.fetch_add(1, std::memory_order_acquire);
			while(m_counter.load(std::memory_order_relaxed) < 1);
		}
	}
}
void fts::spin_semaphore::unlock()
{
	m_counter.fetch_add(1, std::memory_order_acquire);
}
bool fts::spin_semaphore::try_lock()
{
	while(true)
	{
		auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
		if(prev > 0) return true;
		else
		{
			m_counter.fetch_add(1, std::memory_order_acquire);
			return false;
		}
	}
}
fts::spin_semaphore::spin_semaphore()
: m_counter(1) {}
fts::spin_semaphore::spin_semaphore(int32_t max)
: m_counter(max) {}


//=========================================adaptivesemaphore=========================================
void fts::adaptive_semaphore::lock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		while(true)
		{
			auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) return;
			else
			{
				m_counter.fetch_add(1, std::memory_order_acquire);
				syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
			}
		}
	//platform: windows
	#elif FTS_PLATFORM == 2
		
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		
	#endif
}
void fts::adaptive_semaphore::unlock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		m_counter.fetch_add(1, std::memory_order_acquire);
		syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
	//platform: windows
	#elif FTS_PLATFORM == 2
		
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		
	#endif
}
bool fts::adaptive_semaphore::try_lock()
{
	

	//platform: linux
	#if FTS_PLATFORM == 1
		while(true)
		{
			auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) return true;
			else
			{
				m_counter.fetch_add(1, std::memory_order_acquire);
				return false;
			}
		}
	//platform: windows
	#elif FTS_PLATFORM == 2
		
	//platform: unknown
	#elif FTS_PLATFORM == unknown
		
	#endif
}
fts::adaptive_semaphore::adaptive_semaphore()
: m_counter(1), m_address(0) {}
fts::adaptive_semaphore::adaptive_semaphore(int32_t max)
: m_counter(max), m_address(0) {}


//=========================================signal=========================================
void fts::signal::wait()
{
	syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
}
void fts::signal::wake()
{
	syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
}
void fts::signal::wake_all()
{
	syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, std::numeric_limits<int32_t>::max(), nullptr);
}
fts::signal::signal()
: m_address(0) {}


//=========================================spinsignal=========================================
void fts::spin_signal::wait()
{
	m_is_waiting.store(2, std::memory_order_release);
	while(true)
	{
		if(m_is_waiting.load(std::memory_order_relaxed) == 1)
		{
			m_is_waiting.fetch_add(1, std::memory_order_acquire);
			return;
		}
		else if(m_is_waiting.load(std::memory_order_relaxed) == 0)
		{
			return;
		}
	}
}
void fts::spin_signal::wake()
{
	m_is_waiting.store(1, std::memory_order_release);
}
void fts::spin_signal::wake_all()
{
	m_is_waiting.store(0, std::memory_order_release);
}
fts::spin_signal::spin_signal()
: m_is_waiting(false) {}

















void func1(fts::spin_semaphore* lock)
{
	lock->lock();
	std::cout << "start of func1\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	std::cout << "end of func1\n";
	lock->unlock();
}

void func2(fts::spin_semaphore* lock)
{
	lock->lock();
	std::cout << "start of func2\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::cout << "middel of func2\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::cout << "end of func2\n";
	lock->unlock();
}

void func3(fts::spin_semaphore* lock)
{
	lock->lock();
	std::cout << "start of func3\n";
	std::this_thread::sleep_for(std::chrono::microseconds(3333));
	std::cout << "middel of func3\n";
	std::this_thread::sleep_for(std::chrono::microseconds(3333));
	std::cout << "middel of func3\n";
	std::this_thread::sleep_for(std::chrono::microseconds(3333));
	std::cout << "end of func3\n";
	lock->unlock();
}

void func4(fts::spin_semaphore* lock)
{
	lock->lock();
	std::cout << "start of func4\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	std::cout << "middel of func4\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	std::cout << "middel of func4\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	std::cout << "middel of func4\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	std::cout << "end of func4\n";
	lock->unlock();
}

void waiter(fts::signal* sig)
{
	std::cout << "waiting for the signal" << std::endl;
	sig->wait();
	std::cout << "got the signal" << std::endl;
}

int main(int, char* [])
{
	fts::spin_semaphore lock;
	
	for(int i = 0; i < 4; i++)
	{
		std::thread t1(func1, &lock);
		std::thread t2(func2, &lock);
		std::thread t3(func3, &lock);
		std::thread t4(func4, &lock);

		t1.join();
		t2.join();
		t3.join();
		t4.join();
	}

	fts::signal sig;

	std::thread ts(waiter, &sig);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	sig.wake();

	ts.join();

	return 0;
}