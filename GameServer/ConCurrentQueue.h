#pragma once

#include <mutex>

template<typename T>
class LcokQueue
{

public:

	LcokQueue() {}
	LcokQueue(const LcokQueue&) = delete;
	LcokQueue& operator=(const LcokQueue&) = delete;


	void Push(T _Value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(_Value));
		_condVar.notify_one();
	}

	bool TryPop(T& _Value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		// empty -> top -> pop
		_Value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& _Value) // 데이터가 있을때만 pop
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] { return _queue.empty() == false; });
		_Value = std::move(_queue.front());
		_queue.pop();
	}


private:

	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;

};

