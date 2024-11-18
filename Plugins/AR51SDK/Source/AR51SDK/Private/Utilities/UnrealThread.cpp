// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealThread.h"

void UnrealThread::SetMainThread() {
	std::lock_guard<std::mutex> lock(Instance()._mutex);
	Instance()._mainThreadId = std::this_thread::get_id();
}

UnrealThread& UnrealThread::Instance() {
	static UnrealThread instance;
	return instance;
}

UnrealThread::UnrealThread()
{
}

UnrealThread::~UnrealThread()
{
	Clear();
}

bool UnrealThread::IsMainThread() {
	auto& instance = Instance();
	std::lock_guard<std::mutex> lock(instance._mutex);
	return std::this_thread::get_id() == instance._mainThreadId;
}

void UnrealThread::Invoke(const std::function<void()>& action, const FString& name) { BeginInvoke(action, name)->Wait(); }

std::shared_ptr<TaskBase> UnrealThread::BeginInvoke(const std::function<void()>& action, const FString& name) {
	std::lock_guard<std::mutex> lock(Instance()._mutex);
	if (!Instance()._isRunning) return TaskBase::CanceledTask();
	auto task = std::make_shared<TaskBase>(action, name);
	Instance()._tasks.Enqueue(task);
	return task;
}



void UnrealThread::Tick() {
	SetMainThread();
	std::shared_ptr<TaskBase> task;

	while (!Instance()._tasks.IsEmpty()) {
		auto peek = Instance()._tasks.Peek();
		if (peek) peek->get()->Run();
		Instance()._tasks.Dequeue(task);
	}
}

void UnrealThread::Start() {
	std::lock_guard<std::mutex> lock(Instance()._mutex);
	Instance()._isRunning = true;
	Clear();
}

void UnrealThread::Stop() {
	std::lock_guard<std::mutex> lock(Instance()._mutex);
	Instance()._isRunning = false;
	Clear();
}

void UnrealThread::Clear() {
	std::shared_ptr<TaskBase> task;
	while (Instance()._tasks.Dequeue(task))
		task->Cancel();
}

TaskBase::TaskBase(const std::function<void()>& action, const FString& name) {
	_action = action;
	_name = name;
}

TaskBase::~TaskBase() { Cancel(); }

bool TaskBase::IsCompleted() const { return _isCompleted; }
bool TaskBase::IsCanceled() const { return _isCanceled; }

void TaskBase::Run() {
	_action();
	_isCompleted = true;
	_isCanceled = false;
	_resetEvent.Set();
}

void TaskBase::Wait() { if (_isCompleted || _isCanceled) return; _resetEvent.WaitOne(false); }

void TaskBase::Cancel() { _isCanceled = true; _resetEvent.Set(); }

std::shared_ptr<TaskBase> TaskBase::CompletedTask() {
	static std::shared_ptr<TaskBase> task = std::make_shared<TaskBase>([]() {}, "CompletedTask");
	if (!task->IsCompleted())
		task->Run();
	return task;
}

std::shared_ptr<TaskBase> TaskBase::CanceledTask() {
	static std::shared_ptr<TaskBase> task = std::make_shared<TaskBase>([]() {}, "CanceledTask");
	task->Cancel();
	return task;
}
