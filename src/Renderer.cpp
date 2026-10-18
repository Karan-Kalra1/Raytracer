
#define PARALLEL_EXECUTION

//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <execution>

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"


using namespace dae;



Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{




	Camera& camera = pScene->GetCamera();

	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	const float fov_angle = camera.fovAngle * TO_RADIANS;

	const float fov = tan(fov_angle / 2.f);

#if defined(PARALLEL_EXECUTION)

	//PARALLEL LOGIC

	const uint32_t amountOfPixels{ static_cast<uint32_t>(m_Width * m_Height) };
	std::vector<uint32_t> pixelIndices{};
	pixelIndices.reserve(amountOfPixels);

	for (uint32_t index{}; index < amountOfPixels; ++index) pixelIndices.emplace_back(index);

	std::for_each(std::execution::par, pixelIndices.begin() , pixelIndices.end() ,
		[&](const uint32_t i) {

		RenderPixel(pScene, i, fov, aspectRatio, cameraToWorld, camera.origin);

		});




#else 

	//no threading

	const uint32_t amountOfPixels{ static_cast<uint32_t>(m_Width * m_Height) };

	for (uint32_t pixelIndex{}; pixelIndex < amountOfPixels; ++pixelIndex)
	{

		RenderPixel(pScene, pixelIndex, fov, aspectRatio, cameraToWorld, camera.origin);
	}



#endif

	//END


	SDL_UpdateWindowSurface(m_pWindow);




			

		
}




void Renderer::RenderPixel(const Scene* pScene, uint32_t pixelIndex, const float fov, const  float aspectRatio, const Matrix& cameraToWorld, const Vector3 cameraOrigin) const
{
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t px{ pixelIndex % m_Width };
	const uint32_t py{ pixelIndex / m_Width };

	float x = (2.f * ((static_cast<float>(px) + 0.5f) / static_cast<float>(m_Width)) - 1.f) * aspectRatio * fov;

	float y = (1.f - 2.f * ((static_cast<float>(py) + 0.5f) / static_cast<float>(m_Height))) * fov;


	Vector3 rayDirection{ 0,0,1 };

	rayDirection = Vector3{ x , y , 1.f };



	rayDirection = cameraToWorld.TransformVector(rayDirection);

	rayDirection.Normalize();

	Ray hitRay{ cameraOrigin ,rayDirection };


	ColorRGB finalColor{};
	HitRecord closestHit{};

	
	pScene->GetClosestHit(hitRay, closestHit);

	if (closestHit.didHit)
	{




		for (const Light& light : lights)
		{





			Vector3 lightDirection{ LightUtils::GetDirectionToLight(light , closestHit.origin) };


			float lightDistance = lightDirection.Normalize();

			Vector3 viewRay{ closestHit.origin,cameraOrigin };

			viewRay.Normalize();

			float cosine{ Vector3::Dot(closestHit.normal, lightDirection) };



			Ray lightRay{ closestHit.origin + closestHit.normal * .001f, lightDirection };

			lightRay.min = 0.01f;
			lightRay.max = lightDistance;

			if (cosine < 0)
			{
				continue;
			}

			
			if ( m_ShadowsEnabled && pScene->DoesHit(lightRay))
			{
				continue;
			}


			

			

			

			


			switch (m_LightingMode)
			{
			case LightingMode::ObservedArea:
			
				finalColor += {cosine, cosine, cosine};
				break;
			

			case LightingMode::Radiance:
			{
				ColorRGB radiance{ LightUtils::GetRadiance(light, closestHit.origin) };
				finalColor += radiance;
				break;
			}

			case LightingMode::BRDF:
			{
				ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, lightDirection , viewRay) };

				finalColor += BRDF;
				break;
			}

			case LightingMode::Combined:
			{
				ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, lightDirection , viewRay) };
				ColorRGB radiance{ LightUtils::GetRadiance(light, closestHit.origin) };

				finalColor += radiance * BRDF * cosine;
				break;
			}
			}



		}

	}

	finalColor.MaxToOne();



//Update Color in Buffer


	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

	

	

}




bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	switch (m_LightingMode)
	{
	case LightingMode::ObservedArea:
		m_LightingMode = LightingMode::Radiance;

		break;

	case LightingMode::Radiance:

		m_LightingMode = LightingMode::BRDF;
		break;
	
	case LightingMode::BRDF:

		m_LightingMode = LightingMode::Combined;
			break;

	case LightingMode::Combined:

		m_LightingMode = LightingMode::ObservedArea;
		break;
	}

}
