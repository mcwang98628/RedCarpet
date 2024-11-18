#include "Keypoints.h"

const TArray<Keypoints>& KeypointsHelper::AllKeypoints() {
	static TArray<Keypoints> keypoints;
	if (keypoints.Num() == 0) {
		for (int i = (int)Keypoints::Unknown; i < (int)Keypoints::Last; i++)
		{
			keypoints.Add((Keypoints)i);
		}
	}
	return keypoints;
}
