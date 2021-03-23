//	Class that creates a collection of Catmull-Rom splines that are continuous.
//		Text file contains all the control points.

#pragma once

#include "crspline.h"
#include <vector>
#include "matrix3x3.h"

namespace SL
{
	class CRSplineController
	{
	public:
		CRSplineController(int spline_resolution);
		~CRSplineController();
		
		Vector GetPoint(const float t);
		Vector GetPointAtDistance(const float d);
		float GetTimeAtDistance(const float d);
		Vector GetTangent(const float t);
		inline float GetArcLength() { return arc_length_; }

		bool AddSegment(CRSpline* segment, const float tension, bool match_tangent = false);
		void RemoveBack();
		void ClearSegments();
		void CalculateSplineLength();
		inline int GetSegmentCount() { return segments_.size(); }
		CRSpline* JoinSelf();

	private:
		std::vector<float> spline_times_;
		std::vector<float> spline_lengths_;
		std::vector<Vector> control_points_;
		std::vector<CRSpline*> segments_;
		Matrix3x3 segment_rotation_store_;

		float arc_length_;
		int spline_resolution_;
	private:
		int GetCurrentSegment(float t);
		int FindLengthIndex(float length);

	};
}

