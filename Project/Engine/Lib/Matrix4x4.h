#pragma once


#ifdef _DEBUG
#include <imgui.h>
#include <string>
#endif // _DEBUG

#include <Vector3.h>


struct Matrix4x4
{
	float m[4][4];

	static Matrix4x4 Identity() {
		return Matrix4x4{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	}

	Vector3 GetScale() const;

	inline void operator*=(const Matrix4x4& _mat)
	{
		Matrix4x4 result;
		Matrix4x4 _m1 = *this;
		Matrix4x4 _m2 = _mat;
		result.m[0][0] = _m1.m[0][0] * _m2.m[0][0] + _m1.m[0][1] * _m2.m[1][0] + _m1.m[0][2] * _m2.m[2][0] + _m1.m[0][3] * _m2.m[3][0];
		result.m[0][1] = _m1.m[0][0] * _m2.m[0][1] + _m1.m[0][1] * _m2.m[1][1] + _m1.m[0][2] * _m2.m[2][1] + _m1.m[0][3] * _m2.m[3][1];
		result.m[0][2] = _m1.m[0][0] * _m2.m[0][2] + _m1.m[0][1] * _m2.m[1][2] + _m1.m[0][2] * _m2.m[2][2] + _m1.m[0][3] * _m2.m[3][2];
		result.m[0][3] = _m1.m[0][0] * _m2.m[0][3] + _m1.m[0][1] * _m2.m[1][3] + _m1.m[0][2] * _m2.m[2][3] + _m1.m[0][3] * _m2.m[3][3];

		result.m[1][0] = _m1.m[1][0] * _m2.m[0][0] + _m1.m[1][1] * _m2.m[1][0] + _m1.m[1][2] * _m2.m[2][0] + _m1.m[1][3] * _m2.m[3][0];
		result.m[1][1] = _m1.m[1][0] * _m2.m[0][1] + _m1.m[1][1] * _m2.m[1][1] + _m1.m[1][2] * _m2.m[2][1] + _m1.m[1][3] * _m2.m[3][1];
		result.m[1][2] = _m1.m[1][0] * _m2.m[0][2] + _m1.m[1][1] * _m2.m[1][2] + _m1.m[1][2] * _m2.m[2][2] + _m1.m[1][3] * _m2.m[3][2];
		result.m[1][3] = _m1.m[1][0] * _m2.m[0][3] + _m1.m[1][1] * _m2.m[1][3] + _m1.m[1][2] * _m2.m[2][3] + _m1.m[1][3] * _m2.m[3][3];

		result.m[2][0] = _m1.m[2][0] * _m2.m[0][0] + _m1.m[2][1] * _m2.m[1][0] + _m1.m[2][2] * _m2.m[2][0] + _m1.m[2][3] * _m2.m[3][0];
		result.m[2][1] = _m1.m[2][0] * _m2.m[0][1] + _m1.m[2][1] * _m2.m[1][1] + _m1.m[2][2] * _m2.m[2][1] + _m1.m[2][3] * _m2.m[3][1];
		result.m[2][2] = _m1.m[2][0] * _m2.m[0][2] + _m1.m[2][1] * _m2.m[1][2] + _m1.m[2][2] * _m2.m[2][2] + _m1.m[2][3] * _m2.m[3][2];
		result.m[2][3] = _m1.m[2][0] * _m2.m[0][3] + _m1.m[2][1] * _m2.m[1][3] + _m1.m[2][2] * _m2.m[2][3] + _m1.m[2][3] * _m2.m[3][3];

		result.m[3][0] = _m1.m[3][0] * _m2.m[0][0] + _m1.m[3][1] * _m2.m[1][0] + _m1.m[3][2] * _m2.m[2][0] + _m1.m[3][3] * _m2.m[3][0];
		result.m[3][1] = _m1.m[3][0] * _m2.m[0][1] + _m1.m[3][1] * _m2.m[1][1] + _m1.m[3][2] * _m2.m[2][1] + _m1.m[3][3] * _m2.m[3][1];
		result.m[3][2] = _m1.m[3][0] * _m2.m[0][2] + _m1.m[3][1] * _m2.m[1][2] + _m1.m[3][2] * _m2.m[2][2] + _m1.m[3][3] * _m2.m[3][2];
		result.m[3][3] = _m1.m[3][0] * _m2.m[0][3] + _m1.m[3][1] * _m2.m[1][3] + _m1.m[3][2] * _m2.m[2][3] + _m1.m[3][3] * _m2.m[3][3];
		*this = result;
	}

#ifdef _DEBUG
	void ShowData(const std::string& _label, bool _newWindow = true) const;
#endif // _DEBUG


};