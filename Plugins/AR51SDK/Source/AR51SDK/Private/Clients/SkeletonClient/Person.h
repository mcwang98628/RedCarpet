// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "unreal.grpc.h"

#include "Converter.h"
#include "Debug.h"
#include "Keypoints.h"
#include <AR51Character.h>

class Person
{
private:
	FString _id;
	UAR51Character* _character;
	AR51::SkeletonReply _lastSkeleton;
	TArray<FVector> _rawPositions;
	float _lastUpdateTime;

public:
	const FString& GetId() const;
	UAR51Character* GetCharacter() const;
	AActor* GetModel() const;

	const AR51::SkeletonReply& GetLastSkeleton() const;
	void SetLastSkeleton(const AR51::SkeletonReply& skeleton, float timestamp);

	/// <summary>
	/// Since the raw ankle position (i.e. cvs ankle pos estimation) is not in the same place as the virtual character
	/// We shorten the calf length using the calf ratio by movind the ankle position along the knee to ankle vector, i.e. if the calf ration is 1, the ankle position will not change
	/// if we set the calf ratio to 0.5, the ankle new position will be in right in the middle of the calf i.e. 0.5 * (knee + ankle)
	/// FORMULA: new ankle = knee + calfRatio * (ankle - knee)
	/// </summary>
	/// <param name="calfRatio"></param>
	void FixCalfRatio(float calfRatio);

	const TArray<FVector>& GetRawPositions() const;

	float GetLastUpdateTime() const;

	float GetTimeFromLastUpdate(float currentTime) const;

public:
	Person(const FString& id, UAR51Character* character);
	~Person();
};
