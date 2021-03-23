#pragma once
#include "vector.h"

namespace SL
{
	class Matrix3x3
	{
	public:
		Matrix3x3();
		Matrix3x3(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22);
		const float GetValue(int row, int column);
		void SetRow(int row, Vector values);
		void SetIdentity();
		void SetMatrix(Vector row0, Vector row1, Vector row2);
		Matrix3x3 RotationAxisAngle(Vector axis_normalised, float angle);
		Matrix3x3 RotationY(float angle_degrees);
		Vector TransformVector(Vector vector);

	private:
		float values_[3][3];
	};
}
