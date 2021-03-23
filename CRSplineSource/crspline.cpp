#include "crspline.h"
#include <math.h>

namespace SL
{
	//	To use spline, set control points and then calculate the coefficients.
	CRSpline::CRSpline() 
	{
		//	If true then spline segment is managed by the SplineController, which will handle memory of the segment.
		//		If false, the memory must be deallocated outside of the SplineController.
		is_used_ = false;

		//	Is the head segment in a series of segments.
		//		Used to ensure that the overall shape of multiple segment splines remains the same when transforming.
		is_parent_ = true;
	}

	Vector CRSpline::GetPoint(const float t_)
	{
		Vector point;
		float t = t_;
		if (t < 0)
		{
			t = 0;
		}
		else if (t > 1)
		{
			t = 1;
		}

		point.SetX(coefficients_[0].X() + (coefficients_[1].X() * t) + (coefficients_[2].X() * t * t) + (coefficients_[3].X() * t * t * t));
		point.SetY(coefficients_[0].Y() + (coefficients_[1].Y() * t) + (coefficients_[2].Y() * t * t) + (coefficients_[3].Y() * t * t * t));
		point.SetZ(coefficients_[0].Z() + (coefficients_[1].Z() * t) + (coefficients_[2].Z() * t * t) + (coefficients_[3].Z() * t * t * t));
		return point;
	}

	//	Forward.
	Vector CRSpline::GetTangent(const float t_)
	{
		float t = t_;
		if (t < 0)
		{
			t = 0;
		}
		else if (t > 1)
		{
			t = 1;
		}

		Vector tangent;
		tangent.SetX(coefficients_[1].X() + (2.0f * coefficients_[2].X() * t) + (3.0f * coefficients_[3].X() * t * t));
		tangent.SetY(coefficients_[1].Y() + (2.0f * coefficients_[2].Y() * t) + (3.0f * coefficients_[3].Y() * t * t));
		tangent.SetZ(coefficients_[1].Z() + (2.0f * coefficients_[2].Z() * t) + (3.0f * coefficients_[3].Z() * t * t));

		return tangent.Normalised();
	}

	//	Calculate the coefficient vectors for this spline segment.
	Matrix4x4 CRSpline::CalculateCoefficients(const float tension)
	{
		Vector row0(0.0f, 1.0f, 0.0f, 0.0f);
		Vector row1(tension * -1.0f, 0.0f, tension, 0.0f);
		Vector row2(2.0f * tension, tension - 3.0f, 3.0f - (2.0f * tension), tension * -1.0f);
		Vector row3(tension * -1.0f, 2.0f - tension, tension - 2.0f, tension);
		Matrix4x4 cr_matrix;
		cr_matrix.SetMatrix(row0, row1, row2, row3);

		Matrix4x4 control_point_matrix;
		control_point_matrix.SetMatrix(control_points_[0], control_points_[1], control_points_[2], control_points_[3]);

		Matrix4x4 coefficient_matrix;
		coefficient_matrix = cr_matrix.Multiply4x3(control_point_matrix);
		for (int i = 0; i < 4; i++)
		{
			coefficients_[i] = coefficient_matrix.GetRow(i);
		}

		return coefficient_matrix;
	}

	void CRSpline::SetControlPoints(const Vector p0, const Vector p1, const Vector p2, const Vector p3)
	{
		control_points_[0] = p0;
		control_points_[1] = p1;
		control_points_[2] = p2;
		control_points_[3] = p3;
	}

	void CRSpline::SetControlPoint(Vector point, int element)
	{
		control_points_[element] = point;
	}

	Vector CRSpline::GetControlPoint(int element)
	{
	return control_points_[element];
	}
}
