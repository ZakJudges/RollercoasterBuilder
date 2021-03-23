#include "matrix3x3.h"
#include <math.h>

namespace SL
{
	Matrix3x3::Matrix3x3()
	{
		SetIdentity();
	}

	Matrix3x3::Matrix3x3(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
	{
		values_[0][0] = _00; values_[0][1] = _01; values_[0][2] = _02;
		values_[1][0] = _10; values_[1][1] = _11; values_[1][2] = _12;
		values_[2][0] = _20; values_[2][1] = _21; values_[2][2] = _22;
	}

	Matrix3x3 Matrix3x3::RotationAxisAngle(Vector axis_normalised, float angle)
	{
		//	Define constants.
		const float x = axis_normalised.X();
		const float y = axis_normalised.Y();
		const float z = axis_normalised.Z();
		const float cos_theta = cosf(angle);
		const float sin_theta = sinf(angle);
		const float inv_cos_theta = 1.0f - cos_theta;

		//	First Row.
		values_[0][0] = (x * x * inv_cos_theta) + cos_theta;
		values_[0][1] = (x * y * inv_cos_theta) + (z * sin_theta);
		values_[0][2] = (x * z * inv_cos_theta) - (y * sin_theta);

		//	Second Row.
		values_[1][0] = (x * y * inv_cos_theta) - (z * sin_theta);
		values_[1][1] = (y * y * inv_cos_theta) + cos_theta;
		values_[1][2] = (y * z * inv_cos_theta) + (x * sin_theta);

		//	Third Row.
		values_[2][0] = (x * z * inv_cos_theta) + (y * sin_theta);
		values_[2][1] = (y * z * inv_cos_theta) - (x * sin_theta);
		values_[2][2] = (z * z * inv_cos_theta) + cos_theta;

		return Matrix3x3(values_[0][0], values_[0][1], values_[0][2], values_[1][0], values_[1][1], values_[1][2], values_[2][0], values_[2][1], values_[2][2]);
	}

	Matrix3x3 Matrix3x3::RotationY(float angle_degrees)
	{
		const float theta = angle_degrees * 0.0174533f;
		const float cos_theta = cosf(theta);
		const float sin_theta = sinf(theta);

		//	First row:
		values_[0][0] = cos_theta; 
		values_[0][1] = 0.0f; 
		values_[0][2] = sin_theta * -1.0f;
		
		//	Second row:
		values_[1][0] = 0.0f; 
		values_[1][1] = 1.0f; 
		values_[1][2] = 0.0f;

		//	Third row:
		values_[2][0] = sin_theta;
		values_[2][1] = 0.0f;
		values_[2][2] = cos_theta;

		return Matrix3x3(values_[0][0], values_[0][1], values_[0][2], values_[1][0], values_[1][1], values_[1][2], values_[2][0], values_[2][1], values_[2][2]);
	}

	//	Transform the input vector by this matrix.
	Vector Matrix3x3::TransformVector(Vector vector)
	{
		//	Row-major.
		float x = (values_[0][0] * vector.X()) + (values_[1][0] * vector.Y()) + (values_[2][0] * vector.Z());
		float y = (values_[0][1] * vector.X()) + (values_[1][1] * vector.Y()) + (values_[2][1] * vector.Z());
		float z = (values_[0][2] * vector.X()) + (values_[1][2] * vector.Y()) + (values_[2][2] * vector.Z());

		Vector result(x, y, z);

		return result;
	}

	const float Matrix3x3::GetValue(int row, int column)
	{
		return values_[row][column];
	}

	void Matrix3x3::SetRow(int row, Vector values)
	{
		values_[row][0] = values.X();
		values_[row][1] = values.Y();
		values_[row][2] = values.Z();
	}

	void Matrix3x3::SetIdentity()
	{
		Vector row1(1.0f, 0.0f, 0.0f);
		Vector row2(0.0f, 1.0f, 0.0f);
		Vector row3(0.0f, 0.0f, 1.0f);
		SetRow(0, row1);
		SetRow(1, row2);
		SetRow(2, row3);
	}

	void Matrix3x3::SetMatrix(Vector row0, Vector row1, Vector row2)
	{
		SetRow(0, row0);
		SetRow(1, row1);
		SetRow(2, row2);
	}
}
