#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <string>
#include <vector>
#include "unreal.grpc.h"

class Utils {

public:
	/// <summary>
	/// Converts the given AR51::PlatformType enum value to its corresponding string representation.
	/// </summary>
	/// <param name="platformType">The AR51::PlatformType enum value to convert.</param>
	/// <returns>The string representation of the AR51::PlatformType. Returns "Unknown" if the input platformType is not recognized.</returns>
	static FString ToString(AR51::PlatformType platformType) {
		switch (platformType) {
			case AR51::PlatformType::PC: return "PC";
			case AR51::PlatformType::HoloLens: return "HoloLens";
			case AR51::PlatformType::HoloLens2: return "HoloLens2";
			case AR51::PlatformType::Android: return "Android";
			case AR51::PlatformType::MagicLeap: return "MagicLeap";
			case AR51::PlatformType::OculusRift: return "OculusRift";
			case AR51::PlatformType::OculusQuest: return "OculusQuest";
			case AR51::PlatformType::HtcVive: return "HtcVive";
			case AR51::PlatformType::OpenXrTethered: return "OpenXrTethered";
			case AR51::PlatformType::OpenXrMobile: return "OpenXrMobile";
			case AR51::PlatformType::PicoNeo3Pro: return "PicoNeo3Pro";
			case AR51::PlatformType::OvrTethered: return "OvrTethered";
			case AR51::PlatformType::HtcFocus3: return "HtcFocus3";
		};
		return "Unknonw";
	}


	static FVector ProjectOnVector(FVector from, FVector to, FVector projectMe)
	{
		auto h = to;
		auto k = projectMe;
		auto f = from;

		auto kf = k - f;
		auto hf = h - f;

		// Find knee's nearest point on hip to foot vector
		auto o = hf * FVector::DotProduct(hf, kf) / (hf.Size() * hf.Size()) + f;
		return o;
	}

	static FQuat CreateRotation(const FVector& right, const FVector& up) {
		return CreateRotation(right, up, FVector::CrossProduct(right.GetSafeNormal(), up.GetSafeNormal()));
	}

	static FQuat CreateRotation(const FVector& right, const FVector& up, const FVector& forward)
	{
		return FMatrix(forward.GetSafeNormal(), right.GetSafeNormal(), up.GetSafeNormal(), FVector::ZeroVector).ToQuat();
	}


	/// <summary>
	/// Returns the angle in degrees between from and to.
	/// </summary>
	/// <param name="from">The vector from which the angular difference is measured.</param>
	/// <param name="to">The vector to which the angular difference is measured.</param>
	/// <returns>The angle in degrees between the two vectors.</returns>
	static float Angle(const FVector& from, const FVector& to)
	{
		float num = (float)FMath::Sqrt(from.SizeSquared() * to.SizeSquared());
		if (num < 1E-15f)
		{
			return 0.f;
		}

		float num2 = FMath::Clamp(FVector::DotProduct(from, to) / num, -1.f, 1.f);
		return (float)FMath::Acos(num2) * 57.29578f;
	}

	/// <summary>
	/// Returns the signed angle in degrees between from and to.
	/// </summary>
	/// <param name="from">The vector from which the angular difference is measured.</param>
	/// <param name="to">The vector to which the angular difference is measured.</param>
	/// <param name="axis">A vector around which the other vectors are rotated.</param>
	static float SignedAngle(const FVector& from, const FVector& to, const FVector& axis) {

		float num = Angle(from, to);
		float num2 = from.Y * to.Z - from.Z * to.Y;
		float num3 = from.Z * to.X - from.X * to.Z;
		float num4 = from.X * to.Y - from.Y * to.X;
		float num5 = FMath::Sign(axis.X * num2 + axis.Y * num3 + axis.Z * num4);
		return num * num5;
	}
};