// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <assert.h>
#include "Debug.h"

template <typename T>
class ISingleton
{
private:
    static T* _instance; //Removed inline variable initialization which is not compatible prior to c++17 (unreal 4.27)

protected:
	ISingleton() {
        if (!_instance)
        {
            LOG("Singleton instance is already set.");
        }           
        _instance = static_cast<T*>(this);
    }

public:

    ~ISingleton()
    {
        if (_instance == this)
        {
            _instance = nullptr;
        }
    }

    static T* Instance() {
        if (!_instance)
        {
            LOG("Singleton instance of %s is not set. Make sure it is added to the map.");
        }

        TSoftObjectPtr<T> ptr(_instance);
        if (!ptr.IsValid()) {
            LOG("Error: Invalid soft pointer to singleton!");
            _instance = nullptr;
        }
        return _instance;
    }
};

// Definition of the static member
template<typename T>
T* ISingleton<T>::_instance = nullptr;
