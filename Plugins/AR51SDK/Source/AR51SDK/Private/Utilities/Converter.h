#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <string>
#include <vector>
#include "unreal.grpc.h"
#include "PlatformType.h"
#include <SocketSubsystem.h>


/// <summary>
/// Converts the given Unreal Engine FString to a standard C++ string using UTF-8 encoding.
/// </summary>
/// <param name="fString">The FString to convert.</param>
/// <returns>A standard C++ string representing the original FString.</returns>
inline std::string ToString(const FString& fString) { return std::string(TCHAR_TO_UTF8(*fString)); }

/// <summary>
/// Converts the given standard C++ string to an Unreal Engine FString.
/// </summary>
/// <param name="str">The standard C++ string to convert.</param>
/// <returns>An FString representing the original standard C++ string.</returns>
inline FString ToFString(const std::string& str) { return FString(str.c_str()); }

class Converter {
private:

	template <typename T>
	static TArray<T> ToTArray(const std::string& bytes, TArray<T>*) {
		TArray<T> v((T*)bytes.data(), bytes.size() / sizeof(T));
		return v;
	}

	template <typename T>
	static std::vector<T> ToVector(const std::string& bytes, std::vector<T>*) {
		std::vector<T> v(bytes.size() / sizeof(T));
		std::memcpy(v.data(), bytes.data(), bytes.size());
		return v;
	}

	template <typename T>
	static T ToStruct(const std::string& bytes, T*) {
		T item;
		if (bytes.size() == sizeof(T))
			std::memcpy(&item, bytes.data(), bytes.size());
		return item;
	}



public:

	static void ToStruct(const TArray<float>& v, FVector& result, int offset = 0) { result = { v[0 + offset], v[1+ offset], v[2+ offset] }; }
	static void ToStruct(const TArray<float>& v, FQuat& result, int offset = 0) { result = { v[0 + offset], v[1 + offset], v[2 + offset], v[3 + offset]}; }
	static void ToStruct(const TArray<float>& v, FMatrix& result, int offset = 0) {
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				result.M[i][j] = v[i * 4 + j + offset];
			}
		}
	}

	static TArray<float> ToFloats(const FVector& v) { return { (float)v.X, (float)v.Y, (float)v.Z }; }

	static TArray<float> ToFloats(const FQuat& v) { return { (float)v.X, (float)v.Y, (float)v.Z, (float)v.W }; }

	static TArray<float> ToFloats(const FMatrix& v) { return {
		(float)v.M[0][1], (float)v.M[0][1], (float)v.M[0][2], (float)v.M[0][3],
		(float)v.M[1][1], (float)v.M[1][1], (float)v.M[1][2], (float)v.M[1][3],
		(float)v.M[2][1], (float)v.M[2][1], (float)v.M[2][2], (float)v.M[2][3],
		(float)v.M[3][1], (float)v.M[3][1], (float)v.M[3][2], (float)v.M[3][3], }; 
	}

	template <typename T>
	static TArray<float> ToFloats(const TArray<T>& values) {
		TArray<float> v;
		for (auto item : values)
		{
			auto floats = ToFloats(item);
			v.Append(floats);
		}
		return v;
	}

	static FString GetEndpoint(const std::string& ip, int port) { return ToFString(ip + ":" + std::to_string(port)); }

	static FString GetEndpoint(const FString& ip, int port) { return GetEndpoint(ToString(ip), port); }

	template <typename T>
	static std::vector<T> ToVector(const std::string& bytes) { return ToVector(bytes, (std::vector<T>*)(0)); }

	// We wrap around this function because when compiling on Android it cannot do specialiaztion when the function are static
	// The error was:  "error: explicit specialization cannot have a storage class [-Werror]"
	template <typename T>
	static TArray<T> ToTArray(const std::string& bytes) { 
		static Converter c;
		return c.ToTArrayCore<T>(bytes);
	}

	template <typename T>
	TArray<T> ToTArrayCore(const std::string& bytes) { return ToTArray(bytes, (TArray<T>*)(0)); }

#ifdef UE_5_0_OR_LATER	

	template <>
	TArray<FVector> ToTArrayCore<FVector>(const std::string& bytes) {
		auto values = ToTArray<float>(bytes);
		TArray<FVector> result;
		for (size_t i = 0; i < values.Num() / 3; i++)
		{
			FVector v;
			ToStruct(values, v, i * 3);
			result.Add(v);
		}
		return result;
	}

	template <>
	TArray<FQuat> ToTArrayCore<FQuat>(const std::string& bytes) {
		auto values = ToTArray<float>(bytes);
		TArray<FQuat> result;
		for (size_t i = 0; i < values.Num() / 4; i++)
		{
			FQuat v;
			ToStruct(values, v, i * 4);
			result.Add(v);
		}
		return result;
	}

	template <>
	TArray<FMatrix> ToTArrayCore<FMatrix>(const std::string& bytes) {
		auto values = ToTArray<float>(bytes);
		TArray<FMatrix> result;
		for (size_t i = 0; i < values.Num() / 16; i++)
		{
			FMatrix v;
			ToStruct(values, v, i * 16);
			result.Add(v);
		}
		return result;
	}
#endif

	// We wrap around this function because when compiling on Android it cannot do specialiaztion when the function are static
	// The error was:  "error: explicit specialization cannot have a storage class [-Werror]"
	template <typename T>
	static T ToStruct(const std::string& bytes) {
		static Converter c;
		return c.ToStructCore<T>(bytes);
	}

	template <typename T>
	T ToStructCore(const std::string& bytes) { return ToStruct(bytes, static_cast<T*>(0)); }
#ifdef UE_5_0_OR_LATER

	template <>
	FVector ToStructCore(const std::string& bytes) {
		FVector v;
		auto values = ToTArray<float>(bytes);
		ToStruct(values, v);
		return v;
	}

	template <>
	FQuat ToStructCore(const std::string& bytes) {
		FQuat v;
		auto values = ToTArray<float>(bytes);
		ToStruct(values, v);
		return v;
	}

	template <>
	FMatrix ToStructCore(const std::string& bytes) {
		FMatrix v;
		auto values = ToTArray<float>(bytes);
		ToStruct(values, v);
		return v;
	}
#endif

	// We wrap around this function because when compiling on Android it cannot do specialiaztion when the function are static
	// The error was:  "error: explicit specialization cannot have a storage class [-Werror]"
	template <typename T>
	static std::string Serialize(const T& value) {
		static Converter c;
		return c.SerializeCore<T>(value);
	}

	template <typename T>
	std::string SerializeCore(const T& value) {
		std::string bytes(sizeof(T), ' ');
		std::memcpy((void*)bytes.data(), &value, bytes.size());
		return bytes;
	}

#ifdef UE_5_0_OR_LATER
	template<>
	std::string SerializeCore<FVector>(const FVector& value) {
		auto v = ToFloats(value);
		auto s = Serialize(v);
		return s;
	}

	template<>
	std::string SerializeCore<FQuat>(const FQuat& value) {
		auto v = ToFloats(value);
		auto s = Serialize(v);
		return s;
	}

	template<>
	std::string SerializeCore<FMatrix>(const FMatrix& value) {
		auto v = ToFloats(value);
		auto s = Serialize(v);
		return s;
	}
#endif



	template <typename T>
	static std::string Serialize(const std::vector<T>& v) {
		char* ptr = (char*)v.data();
		auto count = v.size() * sizeof(T);
		std::string s(ptr, count);

		auto c = s;
	}

	// We wrap around this function because when compiling on Android it cannot do specialiaztion when the function are static
// The error was:  "error: explicit specialization cannot have a storage class [-Werror]"
	template <typename T>
	static std::string Serialize(const TArray<T>& v) {
		static Converter c;
		return c.SerializeCore<T>(v);
	}

	template <typename T>
	std::string SerializeCore(const TArray<T>& v) {
		char* ptr = (char*)v.GetData();
		auto count = v.Num() * sizeof(T);
		std::string s(ptr, count);
		return s;
	}

#ifdef UE_5_0_OR_LATER

	template <>
	std::string SerializeCore<FVector>(const TArray<FVector>& v) { return Serialize(ToFloats(v)); }

	template <>
	std::string SerializeCore<FQuat>(const TArray<FQuat>& v) { return Serialize(ToFloats(v)); }

	template <>
	std::string SerializeCore<FMatrix>(const TArray<FMatrix>& v) { return Serialize(ToFloats(v)); }
#endif


};