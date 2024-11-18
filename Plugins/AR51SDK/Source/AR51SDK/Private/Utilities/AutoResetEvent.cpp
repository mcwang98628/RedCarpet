// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/AutoResetEvent.h"
#include <chrono>

AutoResetEvent::AutoResetEvent(bool initial)
	: flag_(initial), protect_(), signal_()
{
}

void AutoResetEvent::Set()
{
	std::lock_guard<std::mutex> lock(protect_);
	flag_ = true;
	signal_.notify_one();
}

void AutoResetEvent::Reset()
{
	std::lock_guard<std::mutex> lock(protect_);
	flag_ = false;
}

bool AutoResetEvent::WaitOne(bool reset)
{
	std::unique_lock<std::mutex> lk(protect_);
	while (!flag_) // prevent spurious wakeups from doing harm
		signal_.wait(lk);
	if(reset) flag_ = false; // waiting resets the flag
	return true;
}

bool AutoResetEvent::WaitOneFor(int milliseconds)
{
	std::unique_lock<std::mutex> lk(protect_);

	if(signal_.wait_for(lk, std::chrono::milliseconds(milliseconds)) == std::cv_status::timeout) {
		return false;
	}
	flag_ = false; // waiting resets the flag
	return true;
}