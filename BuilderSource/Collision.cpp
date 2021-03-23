#include "Collision.h"

bool Collision::PointInSphere(const SL::Vector& sphere_centre, float sphere_radius, const SL::Vector& point) 
{
	//	Calculate the squared distance from the point to the centre of the sphere.
	SL::Vector sphere_to_point = sphere_centre.Subtract(point);
	float squared_distance = sphere_to_point.Dot(sphere_to_point);

	if (squared_distance < (sphere_radius * sphere_radius))
	{
		//	Point lies inside the sphere.
		return true;
	}
	
	//	Point does not lie inside the sphere.
	return false;
	
}

bool Collision::RayInSphere(const SL::Vector& ray_start, const SL::Vector& ray_direction, const float sphere_radius, const SL::Vector& sphere_centre)
{
	SL::Vector ray_sphere = sphere_centre.Subtract(ray_start);

	float dot = ray_sphere.Dot(ray_direction);

	if (dot < 0.0f)
	{
		//	Ray is pointing away from the sphere.
		return false;
	}
	
	//	Calculate the closest point to the sphere.
	SL::Vector projected_ray = ray_direction;
	projected_ray.Scale(dot);
	SL::Vector closest_point = ray_start.Add(projected_ray);

	return PointInSphere(sphere_centre, sphere_radius, closest_point);
}
