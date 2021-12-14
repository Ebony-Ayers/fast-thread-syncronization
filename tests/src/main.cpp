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
	fts::Signal s;

	std::thread t(foo, &s);
	s.wait();
	t.detach();
	
	return 0;
}
