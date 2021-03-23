#pragma once
#include "vector.h"
 
namespace SL
{
	class Matrix4x4
	{
	public:
		Matrix4x4();
		const float GetValue(int row, int column);
		void SetRow(int row, Vector values);
		void SetMatrix(Vector row0, Vector row1, Vector row2, Vector row3);
		void SetIdentity();
		Matrix4x4 RotationAxisAngle(Vector axis_normalised, float angle);
		Matrix4x4& Multiply4x3(Matrix4x4 m);
		Vector GetRow(int row);
	private:
		float values_[4][4];
	};
}
