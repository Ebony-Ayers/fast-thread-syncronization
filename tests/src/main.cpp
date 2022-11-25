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

void func(fts::AdaptiveLock* l)
{
	l->unlock();
}

int main(int /*argc*/, const char** /*argv*/)
{
	fts::AdaptiveLock ally;

	{
		FTS_GENERIC_LOCKGUARD(&ally);
		//fts::GenericLockGuard lockGuard(&ally);
	}

	ally.lock();

	std::thread t(func, &ally);

	ally.lock();
	ally.unlock();

	t.join();

	return 0;
}
