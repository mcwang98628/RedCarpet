// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>
#include <condition_variable>
//#include ""

class AR51SDK_API AutoResetEvent
{
public:
	explicit AutoResetEvent(bool initial = false);

	void Set();
	void Reset();

	bool WaitOne(bool reset = true);
	bool WaitOneFor(int milliseconds);

private:
	AutoResetEvent(const AutoResetEvent&) = delete;
	AutoResetEvent& operator=(const AutoResetEvent&) = delete; // non-copyable
	bool flag_;
	std::mutex protect_;
	std::condition_variable signal_;
};