#include "../../src/fts.hpp"
#include <chrono>
#include <thread>

void foo(fts::Signal* signal)
{
	std::cout << "thread start" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::cout << "thread end" << std::endl;
	signal->wake();
}

int main(int /*argc*/, const char** /*argv*/)
{
	fts::ReadWriteLock wrl;

	wrl.readLock();
	std::cout << "1" << std::endl;
	wrl.readLock();
	std::cout << "2" << std::endl;
	wrl.readLock();
	std::cout << "3" << std::endl;

	wrl.readUnlock();
	wrl.readUnlock();
	wrl.readUnlock();

	wrl.writeLock();
	wrl.writeUnlock();
	wrl.writeLock();
	wrl.writeUnlock();

	fts::SpinSemaphore sem(1);

	sem.lock();
	//sem.removeCounter();
	sem.unlock();

	fts::SemaphoreDestoryCounterLockGuard lg(sem);
	lg.~SemaphoreDestoryCounterLockGuard();

	std::cout << "num counters " << sem.numCounters() << std::endl;

	auto result = sem.try_lock();
	std::cout << result << std::endl;

	return 0;
}
