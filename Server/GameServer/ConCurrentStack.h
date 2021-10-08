#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T _Value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(_Value));
		_condVar.notify_one();
	}

	bool TryPop(T& _Value)
	{
		lock_guard<mutex> lock(_mutex);

		if (_stack.empty())
			return false;
		
		// empty -> top -> pop
		Value - stack
		_condVar.notify_one();
	}
private:

	stack<T> stack;
	mutex _mutex;
	condition_variable _condVar;
	
};

