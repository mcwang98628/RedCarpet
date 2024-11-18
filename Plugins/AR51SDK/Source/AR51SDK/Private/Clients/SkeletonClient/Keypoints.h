#pragma once
#include "CoreMinimal.h"

enum class Keypoints {
	Unknown = -1,
	Nose = 0,
	Neck = 1,
	RShoulder = 2,
	RElbow = 3,
	RWrist = 4,
	LShoulder = 5,
	LElbow = 6,
	LWrist = 7,
	MidHip = 8,
	RHip = 9,
	RKnee = 10,
	RAnkle = 11,
	LHip = 12,
	LKnee = 13,
	LAnkle = 14,
	REye = 15,
	LEye = 16,
	REar = 17,
	LEar = 18,
	LBigToe = 19,
	LSmallToe = 20,
	LHeel = 21,
	RBigToe = 22,
	RSmallToe = 23,
	RHeel = 24,
	Background = 25,

	UpperChest,
	Spine,

	// left fingers
	LThumbFinger0,
	LThumbFinger1,
	LThumbFinger2,
	LThumbFinger3,

	LIndexFinger0,
	LIndexFinger1,
	LIndexFinger2,
	LIndexFinger3,

	LMiddleFinger0,
	LMiddleFinger1,
	LMiddleFinger2,
	LMiddleFinger3,

	LRingFinger0,
	LRingFinger1,
	LRingFinger2,
	LRingFinger3,

	LPinkyFinger0,
	LPinkyFinger1,
	LPinkyFinger2,
	LPinkyFinger3,

	// right fingers
	RThumbFinger0,
	RThumbFinger1,
	RThumbFinger2,
	RThumbFinger3,

	RIndexFinger0,
	RIndexFinger1,
	RIndexFinger2,
	RIndexFinger3,

	RMiddleFinger0,
	RMiddleFinger1,
	RMiddleFinger2,
	RMiddleFinger3,

	RRingFinger0,
	RRingFinger1,
	RRingFinger2,
	RRingFinger3,

	RPinkyFinger0,
	RPinkyFinger1,
	RPinkyFinger2,
	RPinkyFinger3,

	Last,
};

class KeypointsHelper {
public:
	static const TArray<Keypoints>& AllKeypoints();;
};
