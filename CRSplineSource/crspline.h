#pragma once

#include "vector.h"
#include "matrix4x4.h"

namespace SL
{
	class CRSpline
	{
	private:
		Vector control_points_[4];
		Vector coefficients_[4];
		bool is_used_;
		bool is_parent_;
	public:
		CRSpline();
		Matrix4x4 CalculateCoefficients(const float tension);
		void SetControlPoints(const Vector p0, const Vector p1, const Vector p2, const Vector p3);
		void SetControlPoint(const Vector point, int element);
		Vector GetPoint(const float t);
		Vector GetTangent(const float t);
		Vector GetControlPoint(int element);

		inline Vector GetCoefficient(int element) { return coefficients_[element]; }
		inline Vector GetSplineStart() { return control_points_[1]; }
		inline Vector GetSplineEnd() { return control_points_[2]; }

		inline void SetUsed(bool used) { is_used_ = used; }
		inline bool IsUsed() { return is_used_; }
		inline void SetIsParent(bool f) { is_parent_ = f; }
		inline bool IsParent() { return is_parent_; }

	};
}
