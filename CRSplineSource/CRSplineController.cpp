#include "CRSplineController.h"

#include "matrix3x3.h"
#include <cmath>

namespace SL
{


	//	Spline will be created from seperate spline segments.
	CRSplineController::CRSplineController(int spline_resolution) : arc_length_(0.0f), spline_resolution_(spline_resolution)
	{
	}

	CRSplineController::~CRSplineController()
	{
		for (int i = 0; i < segments_.size(); i++)
		{
			if (segments_.at(i))
			{
				delete segments_.at(i);
				segments_.at(i) = 0;
			}
		}
	}

	void CRSplineController::RemoveBack()
	{
		if (segments_.size() == 0)
		{
			return;
		}

		CRSpline* segment_to_remove = segments_.back();
		if (segment_to_remove)
		{
			segments_.pop_back();
			delete segment_to_remove;
			segment_to_remove = 0;
		}

		//	The length of the curve has changed. So reparameterisation is needed again.
		CalculateSplineLength();
	}

	void CRSplineController::ClearSegments()
	{
		segments_.clear();
	}

	bool CRSplineController::AddSegment(CRSpline* segment, const float tension, bool match_tangent)
	{
		if (!segment)
		{
			return false;
		}

		if(segment->IsParent())
		{
			//	Reset the parent segment rotation.
			segment_rotation_store_.SetIdentity();
		}

		//	Attach the segment to the existing spline.
		if (!segments_.empty())
		{
			Vector p0, p1, p2, p3;
			p0 = segment->GetControlPoint(0);
			p1 = segment->GetControlPoint(1);
			p2 = segment->GetControlPoint(2);
			p3 = segment->GetControlPoint(3);

			if (!segment->IsParent())
			{
				//	Apply parent rotation to this segment.
				p0 = segment_rotation_store_.TransformVector(p0);
				p1 = segment_rotation_store_.TransformVector(p1);
				p2 = segment_rotation_store_.TransformVector(p2);
				p3 = segment_rotation_store_.TransformVector(p3);
			}

			if (match_tangent)
			{
				Matrix3x3 rotation_matrix;

				//	Tangents *must* be normalised for dot product comparison to work.
				Vector target_tangent = segments_.back()->GetControlPoint(3).Subtract(segments_.back()->GetControlPoint(1)).Normalised();
				Vector current_tangent = p2.Subtract(p0).Normalised();

				float dot = current_tangent.Dot(target_tangent);

				//	Dot = -1 implies tangents are opposites, so flip the direction of points.
				//		treat -0.98 as -1.0 to account for floating point error.
				if (dot <= -0.98f)
				{
					rotation_matrix.RotationY(180.0f);
				}
				//	Tangents face in different directions, so rotate the new spline segment such that the tangents will match.
				//		Dot = 1 implies that tangents face in same direction.
				else if(dot > -0.98f && dot < 1.0f)
				{
					Vector axis = current_tangent.Cross(target_tangent).Normalised();

					//	Invalid axis of rotation, so do not add the segment.
					if (std::isnan(axis.LengthSquared()))
					{
						return false;
					}
					
					float angle = acosf(dot);

					rotation_matrix.RotationAxisAngle(axis, angle);
				}

				p0 = rotation_matrix.TransformVector(p0);
				p1 = rotation_matrix.TransformVector(p1);
				p2 = rotation_matrix.TransformVector(p2);
				p3 = rotation_matrix.TransformVector(p3);

				if (segment->IsParent())
				{
					segment_rotation_store_ = rotation_matrix;
				}
			}

			//	Calculate the offset needed to join the segment onto the end of the spline.
			Vector direction_to_join_point = segments_.back()->GetSplineEnd().Subtract(p1);

			p0 = p0.Add(direction_to_join_point);
			p1 = p1.Add(direction_to_join_point);
			p2 = p2.Add(direction_to_join_point);
			p3 = p3.Add(direction_to_join_point);

			segment->SetControlPoints(p0, p1, p2, p3);
		}

		segments_.push_back(segment);

		//	Used = true means that the spline controller is responsible for memory management of the segment that was added to it.
		segment->SetUsed(true);

		//	To do: Make calculation of coefficients and distance reparameterisation occur after spline is finished, to improve run-time performance.
		segment->CalculateCoefficients(tension);

		CalculateSplineLength();

		return true;
	}

	//	Get a point from the splines, t normalised from 0:1
	Vector CRSplineController::GetPoint(const float t_)
	{
		if (segments_.size() == 0)
		{
			return Vector(0.0f, 0.0f, 0.0f);
		}

		float t = t_;

		int min = GetCurrentSegment(t);
		int max = min + 1;

		const float global_t = t * segments_.size();

		//	Work out the local value of t in this segment to match the value relative to the full spline.
		const float local_t = (global_t - min) / (max - min);

		//	Segment number given by min.
		Vector point = segments_.at(min)->GetPoint(local_t);

		return point;
	}

	//	Get point on the spline from parameter d [0,1], representing distance travelled along the curve.
	Vector CRSplineController::GetPointAtDistance(const float d)
	{
		if (segments_.size() == 0)
		{
			return Vector(0.0f, 0.0f, 0.0f);
		}

		float t = GetTimeAtDistance(d);

		Vector point = GetPoint(t);

		return point;
	}

	float CRSplineController::GetTimeAtDistance(const float d)
	{
		if (segments_.size() == 0)
		{
			return 0.0f;
		}

		float desired_length = d * arc_length_;

		int index = FindLengthIndex(desired_length);
		int left = index;
		int right = index + 1;
		if ((desired_length <= spline_lengths_[index]) && (index != 0))
		{
			right = index;
			left = index - 1;
		}

		//	Calculate how far along the segment left->right the desired length is. [0,1]
		float s = (desired_length - spline_lengths_[left]) / (spline_lengths_[right] - spline_lengths_[left]);


		//	Find the corresponding value of t for the desired length.
		float t = spline_times_[left] + (s * (spline_times_[right] - spline_times_[left]));
		
		return t;
	}

	Vector CRSplineController::GetTangent(const float t_)
	{
		if (segments_.size() == 0)
		{
			return Vector(0.0f, 0.0f, 0.0f);
		}

		float t = t_;

		int min = GetCurrentSegment(t);
		int max = min + 1;

		const float global_t = t * segments_.size();

		//	Work out the local value of t in this segment to match the value relative to the full spline.
		const float local_t = (global_t - min) / (max - min);

		//	Segment number given by min.
		Vector tangent = segments_.at(min)->GetTangent(local_t);

		return tangent;
	}

	//	To be called after every segment addition or removal.
	//		Records distance along curve at each t.
	//			To Do: Make spline resolution specific to each spline segment.
	//				Would allow alteration of resolution based on spline length, whilst keeping short-lengthed splines low resoultion.
	void CRSplineController::CalculateSplineLength()
	{
		if (segments_.empty())
		{
			arc_length_ = 0.0f;
			return;
		}
		//	Remove the existing times and lengths from the spline.
		spline_times_.clear();
		spline_lengths_.clear();

		std::vector<Vector> spline_points;

		const float increment = 1.0f / spline_resolution_;
		float t = 0.0f;
		float length = 0.0f;

		for (int i = 0; i < spline_resolution_; i++)
		{
			Vector point = GetPoint(t);
			spline_points.push_back(point);

			spline_times_.push_back(t);
			t += increment;

			//	Calculate length by approximating space between points as a straight line.
			if (i == 0)
			{
				spline_lengths_.push_back(length);
			}
			else
			{
				Vector p0 = spline_points[i - 1];
				Vector p1 = spline_points[i];
				Vector direction = p1.Subtract(p0);
				float magnitude = direction.GetLength();
				length += magnitude;
				spline_lengths_.push_back(length);
			}
		}
		arc_length_ = length;
	}

	//	Return the index number whose value matches or is closest to the desired length.
	//		Binary search.
	int CRSplineController::FindLengthIndex(float length)
	{
		int mid = spline_resolution_ / 2;
		int left = 0;
		int right = spline_resolution_;

		while (left <= right)
		{
			mid = (left + right) / 2;

			if (spline_lengths_[mid] == length)
			{
				return mid;
			}

			if (spline_lengths_[mid] < length)
			{
				//	length is right of mid.
				left = mid + 1;
			}
			else
			{
				//	length is left of mid.
				right = mid - 1;
			}
		}
		return mid;
	}

	//	Attach the end of this spline to the start of this spline.
	CRSpline* CRSplineController::JoinSelf()
	{
		if (segments_.size() < 2)
		{
			return nullptr;
		}
		
		//	Create a spline segment that joins the end of the last spline to the start of the first spline:
		Vector p0, p1, p2, p3;

		//	Get the points that the spline segment will be drawn between.
		p1 = segments_.back()->GetControlPoint(2);
		p2 = segments_.front()->GetControlPoint(1);

		//	Calculate p0, such that the tangent at p1 matches the tangent at the end of the spline.
		p0 = segments_.back()->GetControlPoint(1).Subtract(segments_.back()->GetControlPoint(3)).Add(p2);

		//	Calculate p3, such that the tangent at p2 matches the tangent at the start of the spline.
		p3 = segments_.front()->GetControlPoint(2).Subtract(segments_.front()->GetControlPoint(0)).Add(p1);

		CRSpline* new_segment = new CRSpline();
		new_segment->SetControlPoints(p0, p1, p2, p3);

		return new_segment;
	}

	//	Given a value of t, work out which spline segment this value of t lies in.
	int CRSplineController::GetCurrentSegment(float t)
	{
		const float num_segments = segments_.size();

		//	Work out which segment t is in:
		const float global_t = t * num_segments;

		int rounded1 = std::round(global_t);
		int rounded2 = std::round(global_t + 0.5f);

		int min = 0;
		int max = 0;

		if (rounded1 == rounded2)
		{
			//	Rounded Up.
			max = rounded1;
			min = rounded1 - 1;
		}
		else
		{
			//	Rounded Down.
			min = rounded1;
			max = rounded1 + 1;
		}

		//	Check if the end of the spline chain has been reached to prevent out of bound access.
		if (min > num_segments - 1)
		{
			min -= 1;
			max -= 1;
		}

		return min;
	}
}

