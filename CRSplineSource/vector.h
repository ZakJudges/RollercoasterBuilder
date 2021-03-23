#pragma once

namespace SL
{
	class Vector
	{
	public:
		Vector();
		Vector(float x, float y, float z);
		Vector(float x, float y, float z, float w);
		static Vector Up();
		static Vector Right();
		static Vector Forward();
		Vector Add(Vector v) const;
		Vector Subtract(Vector v) const;
		Vector Cross(Vector& v) const;
		Vector Flip() const;
		float Dot(Vector v) const;
		float LengthSquared() const;
		float GetLength() const;
		void Normalise();
		Vector Normalised() const;
		void Set(float x, float y, float z);
		void Set(float x, float y, float z, float w);
		void Scale(float scale);
		Vector Scaled(float scale);
		inline float X() { return x_; }
		inline float Y() { return y_; }
		inline float Z() { return z_; }
		inline float W() { return w_; }
		inline void SetX(float x) { x_ = x; }
		inline void SetY(float y) { y_ = y; }
		inline void SetZ(float z) { z_ = z; }
		inline void SetW(float w) { w_ = w; }

	private:
		float x_;
		float y_;
		float z_;
		float w_;
	};
}
