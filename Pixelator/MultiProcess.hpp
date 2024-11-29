#pragma once

#include "iostream"
#include <thread>
#include <vector>

template <typename T, typename Func>
class ThreadHelper
{  
public:
	static void RunOnThreads(const std::vector<T>& elems, const Func& func)
	{

	};
};