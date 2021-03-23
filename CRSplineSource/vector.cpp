#include "vector.h"
#include <math.h>

namespace SL
{
	Vector::Vector()
	{
		x_ = 0.0f;
		y_ = 0.0f;
		z_ = 0.0f;
		w_ = 0.0f;
	}

	Vector::Vector(float x, float y, float z)
	{
		x_ = x;
		y_ = y;
		z_ = z;
		w_ = 0.0f;
	}

	Vector::Vector(float x, float y, float z, float w)
	{
		x_ = x;
		y_ = y;
		z_ = z;
		w_ = w;
	}

	Vector Vector::Up()
	{
		return Vector(0.0f, 1.0f, 0.0f);
	}

	Vector Vector::Right()
	{
		return Vector(1.0f, 0.0f, 0.0f);
	}

	Vector Vector::Forward()
	{
		return  Vector(0.0f, 0.0f, 1.0f);
	}

	Vector Vector::Add(Vector v) const 
	{
		return Vector(x_ + v.X(), y_ + v.Y(), z_ + v.Z());
	}

	Vector Vector::Subtract(Vector v) const
	{
		return Vector(x_ - v.X(), y_ - v.Y(), z_ - v.Z());
	}

	Vector Vector::Flip() const
	{
		return Vector(x_ * -1.0f, y_ * -1.0f, z_ * -1.0f, w_ * -1.0f);
	}

	Vector Vector::Cross(Vector& v) const
	{
		float x = y_ * v.Z() - z_ * v.Y();
		float y = z_ * v.X() - x_ * v.Z();
		float z = x_ * v.Y() - y_ * v.X();

		Vector result(x, y, z);

		return result;
	}

	float Vector::Dot(Vector v) const
	{
		return x_ * v.X() + y_ * v.Y() + z_ * v.Z();
	}

	float Vector::LengthSquared() const
	{
		Vector v(x_, y_, z_);
		return Dot(v);
	}

	float Vector::GetLength() const
	{
		float length = sqrt(LengthSquared());
		return length;
	}

	//	Normalise this vector.
	void Vector::Normalise()
	{
		float length = GetLength();

		x_ /= length;
		y_ /= length;
		z_ /= length;
	}

	Vector Vector::Normalised() const
	{
		float length = GetLength();

		float x = x_ / length;
		float y = y_ / length;
		float z = z_ / length;

		Vector v(x, y, z);

		return v;
	}

	void Vector::Set(float x, float y, float z)
	{
		x_ = x;
		y_ = y;
		z_ = z;
		w_ = 0.0f;
	}

	void Vector::Set(float x, float y, float z, float w)
	{
		x_ = x;
		y_ = y;
		z_ = z;
		w_ = w;
	}

	void Vector::Scale(float scale)
	{
		x_ *= scale;
		y_ *= scale;
		z_ *= scale;
		w_ *= scale;
	}

	Vector Vector::Scaled(float scale)
	{
		Vector s;
		s.Set(x_, y_, z_, w_);
		s.Scale(scale);

		return s;
	}
}

