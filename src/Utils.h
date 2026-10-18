#pragma once
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			
			float quadratic_A{Vector3::Dot(ray.direction,ray.direction)};
			float quadratic_B{ Vector3::Dot(2.f * ray.direction , (ray.origin - sphere.origin)) };
			float quadratic_C{ Vector3::Dot((ray.origin - sphere.origin) , (ray.origin - sphere.origin)) - (sphere.radius * sphere.radius)};

			float discriminant{ (quadratic_B * quadratic_B) - (4 * quadratic_A * quadratic_C) };
			
			if (discriminant > 0.f)
			{
				
				hitRecord.t = (-quadratic_B - sqrt(discriminant)) / 2 * quadratic_A;

				if (hitRecord.t < ray.min)
				{
					hitRecord.t = (-quadratic_B + sqrt(discriminant)) / 2 * quadratic_A;
				}

				if (hitRecord.t > ray.min && hitRecord.t < ray.max)
				{
					hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
					hitRecord.normal = Vector3{ hitRecord.origin - sphere.origin }.Normalized();
					hitRecord.didHit = true;

					return true;
				}

			}
			
			
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			


			hitRecord.t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);

			if (hitRecord.t > ray.min && hitRecord.t < ray.max)
			{
				hitRecord.didHit = true;
				hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
				hitRecord.normal =  plane.normal;

				
				

				return true;
			}

			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			




			if (ignoreHitRecord == true)
			{


				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && Vector3::Dot(triangle.normal, ray.direction) > 0)
				{
					hitRecord.didHit = false;
					return false;
				}

				if (triangle.cullMode == TriangleCullMode::BackFaceCulling && Vector3::Dot(triangle.normal, ray.direction) < 0)
				{
					hitRecord.didHit = false;
					return false;
				}

			}


			else {



				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && Vector3::Dot(triangle.normal, ray.direction) < 0)
				{
					hitRecord.didHit = false;
					return false;
				}

				if (triangle.cullMode == TriangleCullMode::BackFaceCulling && Vector3::Dot(triangle.normal, ray.direction) > 0)
				{
					hitRecord.didHit = false;
					return false;
				}

			}

			

			





			constexpr float epsilon = std::numeric_limits<float>::epsilon();

			Vector3 first_edge{ triangle.v1 - triangle.v0 };
			Vector3 second_edge{ triangle.v2 - triangle.v0 };

			Vector3 ray_cross_edge2{ Vector3::Cross(ray.direction,second_edge) };

			float det = Vector3::Dot(first_edge, ray_cross_edge2);

			if (det > -epsilon && det < epsilon)
			{
				return false;

			}

			float inv_det = 1.0f / det;

			Vector3 s = ray.origin - triangle.v0;

			float u = inv_det * Vector3::Dot(s, ray_cross_edge2);

			if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u - 1) > epsilon))
			{
				return false;

			}

			Vector3 s_cross_edge1{ Vector3::Cross(s,first_edge) };

			float v = inv_det * Vector3::Dot(ray.direction, s_cross_edge1);

			if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
			{
				return false;

			}

			hitRecord.t = inv_det * Vector3::Dot(second_edge, s_cross_edge1);

			if (hitRecord.t > ray.min && hitRecord.t < ray.max)
			{
				hitRecord.didHit = true;
				hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;





				return true;

			}

			else {

				hitRecord.didHit = false;
				return false;

			}

			
		}

		

	

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}







#pragma endregion


		


#pragma region TriangeMesh HitTest


		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			const float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) * ray.direction.x;
			const float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) * ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			const float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) * ray.direction.y;
			const float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) * ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			const float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) * ray.direction.z;
			const float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) * ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}




		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			if (SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}

				int vertice_counter{};
				int current_triangle_number{};

				Vector3 vertice[3];

				for (const int& indice : mesh.indices)
				{
					vertice[vertice_counter] = mesh.transformedPositions[indice];
					vertice_counter++;

					if (vertice_counter == 3)
					{
						vertice_counter = 0;

						Triangle triangle(vertice[0], vertice[1], vertice[2], mesh.transformedNormals[current_triangle_number]);
						triangle.cullMode = mesh.cullMode;

							if (HitTest_Triangle(triangle, ray, hitRecord,ignoreHitRecord))
							{
								return true;
							}
								

						current_triangle_number++;
					}

				}
			

			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			

			return { Vector3{ origin, light.origin} };

		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			
			Vector3 radius{ light.origin - target };

			return { light.color * (light.intensity / std::max(0.f,Vector3::Dot(radius,radius)))};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}