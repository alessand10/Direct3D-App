#pragma once
#include "Common.h"


namespace TMath {
	template <typename T>
	T clamp(T min, T max, T element) {
		if (element > max) { return max; }
		else if (element < min) { return min; }
		else return element;
	}

	template <typename T>
	T truncate(T value, UINT decimalPlaces) {
		float shift = powf(10.f, decimalPlaces);
		return truncf(value * shift) / shift;
	}

	const float pi = 3.14159265359f;
	const float pi_div2 = 3.14159265359f / 2.f;
}

#define F4_TO_VEC(f) (DirectX::XMLoadFloat4(&f))
#define VEC_TO_F4(f,v) XMFLOAT4 f; DirectX::XMStoreFloat4(&f,v);
