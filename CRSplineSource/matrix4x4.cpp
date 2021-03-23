#include "matrix4x4.h"
#include <math.h>

namespace SL
{
	Matrix4x4::Matrix4x4()
	{
		// Init matrix values.
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				values_[y][x] = 0.0f;
			}
		}
	}

	//	Return a matrix for a rotation of theta degrees about an axis
	//		Row-major.
	Matrix4x4 Matrix4x4::RotationAxisAngle(Vector axis_normalised, float angle)
	{
		//	Define constants.
		const float x = axis_normalised.X();
		const float y = axis_normalised.Y();
		const float z = axis_normalised.Z();
		const float cos_theta = cos(angle);
		const float sin_theta = sin(angle);
		const float inv_cos_theta = 1.0f - cos_theta;

		//	First Row.
		values_[0][0] = (x * x * inv_cos_theta) + cos_theta;
		values_[0][1] = (x * y * inv_cos_theta) + (z * sin_theta);
		values_[0][2] = (x * z * inv_cos_theta) - (y * sin_theta);
		values_[0][3] = 0.0f;

		//	Second Row.
		values_[1][0] = (x * y * inv_cos_theta) - (z * sin_theta);
		values_[1][1] = (y * y * inv_cos_theta) + cos_theta;
		values_[1][2] = (y * z * inv_cos_theta) + (x * sin_theta);
		values_[1][3] = 0.0f;

		//	Third Row.
		values_[2][0] = (x * z * inv_cos_theta) + (y * sin_theta);
		values_[2][1] = (y * z * inv_cos_theta) - (x * sin_theta);
		values_[2][2] = (z * z * inv_cos_theta) + cos_theta;
		values_[2][3] = 0.0f;

		//	Fourth Row.
		values_[3][0] = 0.0f;
		values_[3][1] = 0.0f;
		values_[3][2] = 0.0f;
		values_[3][3] = 1.0f;

		Matrix4x4 result;
		result.SetRow(0, Vector(values_[0][0], values_[0][1], values_[0][2], values_[0][3]));
		result.SetRow(1, Vector(values_[1][0], values_[1][1], values_[1][2], values_[1][3]));
		result.SetRow(2, Vector(values_[2][0], values_[2][1], values_[2][2], values_[2][3]));
		result.SetRow(3, Vector(values_[3][0], values_[3][1], values_[3][2], values_[3][3]));
		return result;

	}

	//	Multiply this 4x4 matrix by a 4x3 matrix. Result is a 4x3 matrix.
	Matrix4x4& Matrix4x4::Multiply4x3(Matrix4x4 m)
	{
		Matrix4x4 result;

		for (int y = 0; y < 4; y++)
		{
			Vector row;
			row.SetX((values_[y][0] * m.GetValue(0, 0)) + (values_[y][1] * m.GetValue(1, 0)) + (values_[y][2] * m.GetValue(2, 0)) + (values_[y][3] * m.GetValue(3, 0)));
			row.SetY((values_[y][0] * m.GetValue(0, 1)) + (values_[y][1] * m.GetValue(1, 1)) + (values_[y][2] * m.GetValue(2, 1)) + (values_[y][3] * m.GetValue(3, 1)));
			row.SetZ((values_[y][0] * m.GetValue(0, 2)) + (values_[y][1] * m.GetValue(1, 2)) + (values_[y][2] * m.GetValue(2, 2)) + (values_[y][3] * m.GetValue(3, 2)));
			result.SetRow(y, row);
		}

		return result;
	}

	const float Matrix4x4::GetValue(int row, int column)
	{
		return values_[row][column];
	}

	void Matrix4x4::SetRow(int row, Vector values)
	{
		values_[row][0] = values.X();
		values_[row][1] = values.Y();
		values_[row][2] = values.Z();
		values_[row][3] = values.W();
	}

	void Matrix4x4::SetIdentity()
	{
		Vector row1(1.0f, 0.0f, 0.0f, 0.0f);
		Vector row2(0.0f, 1.0f, 0.0f, 0.0f);
		Vector row3(0.0f, 0.0f, 1.0f, 0.0f);
		Vector row4(0.0f, 0.0f, 0.0f, 1.0f);
		SetRow(0, row1);
		SetRow(1, row2);
		SetRow(2, row3);
		SetRow(3, row4);
	}

	void Matrix4x4::SetMatrix(Vector row0, Vector row1, Vector row2, Vector row3)
	{
		SetRow(0, row0);
		SetRow(1, row1);
		SetRow(2, row2);
		SetRow(3, row3);
	}

	Vector Matrix4x4::GetRow(int row)
	{
		Vector result(values_[row][0], values_[row][1], values_[row][2], values_[row][3]);
		return result;
	}
}
