// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilities/AutoResetEvent.h"
#include "Containers/Queue.h"
#include <functional>
#include <thread>
#include <mutex>

class TaskBase {
protected:
	FString _name;
	AutoResetEvent _resetEvent{};
	bool _isCompleted{ false };
	bool _isCanceled{ false };
	std::function<void()> _action{ []() {} };

public:
	TaskBase() = default;
	TaskBase(const std::function<void()>& action, const FString& name = "Untitled");
	~TaskBase();

	bool IsCompleted() const;
	bool IsCanceled() const;
	void Run();
	void Wait();
	void Cancel();


	static std::shared_ptr<TaskBase> CompletedTask();
	static std::shared_ptr<TaskBase> CanceledTask();
};

template <typename T>
class Task : public TaskBase{
private:
	T _result;

public:
	Task(const std::function<T()>& action, const FString& name="Untitled") {
		_action = [action, this]() { _result = action(); };
		_name = name;
	}

	const T& Run() {
		TaskBase::Run();
		return GetResult();
	}

	const T& GetResult() const { return _result; }
	const T& Wait() { TaskBase::Wait(); return GetResult(); }

	static std::shared_ptr<Task<T>> CompletedTask() {
		static std::shared_ptr<Task<T>> task = std::make_shared<Task<T>>([]() { return T(); }, "CompletedTask");
		if (!task->IsCompleted())
			task->Run();
		return task;
	}


	static std::shared_ptr<Task<T>> CanceledTask() {
		static std::shared_ptr<Task<T>> task = std::make_shared<Task<T>>([]() { return T(); }, "CanceledTask");
		task->Cancel();
		return task;
	}
};


class AR51SDK_API UnrealThread
{
private: 
	bool _isRunning{ false };
	TQueue<std::shared_ptr<TaskBase>, EQueueMode::Mpsc> _tasks;
	std::mutex _mutex;
	std::thread::id _mainThreadId;

private:

	static void SetMainThread();

	static UnrealThread& Instance();
	UnrealThread();
	~UnrealThread();

public:
	static bool IsMainThread();
	static void Invoke(const std::function<void()>& action, const FString& name="Untitled");
	static std::shared_ptr<TaskBase> BeginInvoke(const std::function<void()>& action, const FString& name = "Untitled");

	template <typename T>
	static T Invoke(const std::function<T()>& action, const FString& name = "Untitled") {
		return BeginInvoke<T>(action, name)->Wait();
	}

	template <typename T>
	static std::shared_ptr<Task<T>> BeginInvoke(const std::function<T()>& action, const FString& name = "Untitled"){
		std::lock_guard<std::mutex> lock(Instance()._mutex);
		if (!Instance()._isRunning) return Task<T>::CanceledTask();
		auto task = std::make_shared<Task<T>>(action);
		Instance()._tasks.Enqueue(task);
		return task;
	}

	/// <summary>
	/// Invoke the specified action on the Unreal main thread, wait for its completion and return the specified action result value,
	/// if the action was cacneled, it returns the specified canceledValue
	/// </summary>
	/// <typeparam name="T">The class/struct of the return value</typeparam>
	/// <param name="action">The action to perform</param>
	/// <param name="canceledValue">The default value in case the action was canceled</param>
	/// <returns>The specified action's result if it was exectuted, o/w the specified calceledValue</returns>
	template <typename T>
	static T Invoke(const std::function<T()>& action, const T& canceledValue) {
		auto task = UnrealThread::BeginInvoke<T>(action);
		task->Wait();
		return task->IsCompleted() ? task->GetResult() : canceledValue;
	}


	static void Tick();
	static void Start();
	static void Stop();
	static void Clear();
};
