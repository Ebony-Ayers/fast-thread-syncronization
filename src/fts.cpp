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

#pragma once

//platform 0: unknown
//platform 1: linux
//platform 2: windows (win32 or win64)
#ifdef __linux__
	#define FTS_PLATFORM 1
#endif
#ifdef _WIN32
	#define FTS_PLATFORM 2
#endif
#ifdef _WIN64
#define FTS_PLATFORM 2
#endif
#ifndef FTS_PLATFORM
	#pragma message "FTS Warning: no platform could be identified. Thread sleeping mechanisms will revert to std::mutex"
	#define FTS_PLATFORM 0
#endif


#include <atomic>
#if FTS_PLATFORM == 0
	#include <mutex>
#endif
#if FTS_PLATFORM == 1
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <linux/futex.h>
#endif
#if FTS_PLATFORM == 2
	#include <windows.h>
#endif
#include <iostream>
#include <thread>
#include <chrono>


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
			#if FTS_PLATFORM == 0
			std::mutex m_mutex;
			#endif
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
			#if FTS_PLATFORM == 0
			std::mutex m_mutex;
			std::atomic_bool m_unlocked;
			std::atomic_int32_t m_num_waiting;
			#endif
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


//=========================================adaptivelock========================================
void fts::adaptive_lock::lock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
		m_address = 1;
	//platform: windows
	#elif FTS_PLATFORM == 2
		auto value = 1;
		WaitOnAddress(reinterpret_cast<void*>(&m_address), &value, sizeof(value), INFINITE);
	//platform: unknown
	#elif FTS_PLATFORM == 0
		m_mutex.lock();
	#endif
}
void fts::adaptive_lock::unlock()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
		m_address = 0;
	//platform: windows
	#elif FTS_PLATFORM == 2
		m_address++;
		WakeByAddressSingle(reinterpret_cast<void*>(&m_address));
	//platform: unknown
	#elif FTS_PLATFORM == 0
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
		if (m_address == 0)
		{
			auto value = 1;
			WaitOnAddress(reinterpret_cast<void*>(&m_address), &value, sizeof(value), INFINITE);
			return true;
		}
		else return false;
	//platform: unknown
	#elif FTS_PLATFORM == 0
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
		while (true)
		{
			auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
			if (prev > 0) return;
			else
			{
				m_counter.fetch_add(1, std::memory_order_acquire);
				auto value = 1;
				WaitOnAddress(reinterpret_cast<void*>(&m_address), &value, sizeof(value), INFINITE);
			}
		}
	//platform: unknown
	#elif FTS_PLATFORM == 0
		while(true)
		{
			auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) return;
			else
			{
				m_counter.fetch_add(1, std::memory_order_acquire);
				m_mutex.lock();
			}
		}
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
		m_counter.fetch_add(1, std::memory_order_acquire);
		WakeByAddressSingle(reinterpret_cast<void*>(&m_address));
	//platform: unknown
	#elif FTS_PLATFORM == 0
		m_counter.fetch_add(1, std::memory_order_acquire);
		m_mutex.unlock();
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
		while (true)
		{
			auto prev = m_counter.fetch_sub(1, std::memory_order_acquire);
			if (prev > 0) return true;
			else
			{
				m_counter.fetch_add(1, std::memory_order_acquire);
				return false;
			}
		}
	//platform: unknown
	#elif FTS_PLATFORM == 0
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
	#endif

	return false;
}
fts::adaptive_semaphore::adaptive_semaphore()
: m_counter(1), m_address(0) {}
fts::adaptive_semaphore::adaptive_semaphore(int32_t max)
: m_counter(max), m_address(0) {}


//=========================================signal=========================================
void fts::signal::wait()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
	//platform: windows
	#elif FTS_PLATFORM == 2
		auto value = 1;
		WaitOnAddress(reinterpret_cast<void*>(&m_address), &value, sizeof(value), INFINITE);
	//platform: unknown
	#elif FTS_PLATFORM == 0
		m_num_waiting.fetch_add(1);
		m_mutex.lock();
		m_unlocked.store(true);
		m_mutex.unlock();
	#endif
}
void fts::signal::wake()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
	//platform: windows
	#elif FTS_PLATFORM == 2
		WakeByAddressSingle(reinterpret_cast<void*>(&m_address));
	//platform: unknown
	#elif FTS_PLATFORM == 0
		m_unlocked.store(false);
		m_mutex.unlock();
		while(!m_unlocked.load());
		m_mutex.lock();
		m_num_waiting.fetch_sub(1);
	#endif
}
void fts::signal::wake_all()
{
	//platform: linux
	#if FTS_PLATFORM == 1
		syscall(SYS_futex, &m_address, FUTEX_WAKE_PRIVATE, std::numeric_limits<int32_t>::max(), nullptr);
	//platform: windows
	#elif FTS_PLATFORM == 2
		WakeByAddressAll(reinterpret_cast<void*>(&m_address));
	//platform: unknown
	#elif FTS_PLATFORM == 0
		for(int32_t i = m_num_waiting.load(); i > 0; i--)
		{
			m_unlocked.store(false);
			m_mutex.unlock();
			while(!m_unlocked.load());
			m_mutex.lock();
		}
		m_num_waiting.store(0);
	#endif
}
fts::signal::signal()
: m_address(0)
#if FTS_PLATFORM == 0
, m_unlocked(false), m_num_waiting(0)
#endif
{
	#if FTS_PLATFORM == 0
	m_mutex.lock();
	#endif
}


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