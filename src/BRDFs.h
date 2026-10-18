#pragma once
#include "Math.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			
			
			return { (kd* cd) /PI};

			
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			
			
			

			return { (kd * cd) / PI };
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			
			Vector3 reflect{ -l - 2 *(Vector3::Dot(-l,n)) * n };

			float cosine { Vector3::Dot(reflect,v) };

			cosine = std::max(cosine, 0.f);

			float phong = ks * powf(cosine, exp);

			return {ColorRGB(phong,phong,phong) };
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			
			
			
			float dot { Vector3::Dot(h,v) };

			dot = std::max(dot, 0.f);

			float dot_pow{powf( (1.f - dot) ,5.f)};
			

			auto fresnel{ f0 + ( (ColorRGB{ 1.f,1.f,1.f } - f0) * dot_pow ) };
			
			return {fresnel};
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			
			float a{ Square(roughness)};
			float a_square{ Square(a)};

			float dot{ Vector3::Dot(n,h) };

			dot = std::max(dot, 0.f);

			float dot_square{ Square(dot)};

			float denominator{ dot_square * (a_square - 1.f) + 1.f };

			return { a_square / ( PI * Square(denominator )) };
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			

			float dot{ Vector3::Dot(n,v) };

			dot = std::max(dot, 0.f);

			float a{ Square(roughness) };
			

			float k{ Square(a + 1.f) / 8.f};

			float denominator{ (dot * (1.f - k)) + k };

			return { dot / denominator };
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			


			float smith{ GeometryFunction_SchlickGGX(n, v, roughness ) * GeometryFunction_SchlickGGX(n, l, roughness) };
		

			return { smith };
		}

	}
}