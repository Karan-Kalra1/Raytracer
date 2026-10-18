#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "Math.h"
#include "Timer.h"
#include "iostream"

namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{}

		Vector3 origin{};
		float fovAngle{ 90.f };
		

		Vector3 forward{ 0.266f,-0.453f,0.860f };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };
		float senstivity{ 0.1f };

		Matrix cameraToWorld{};

		Matrix CalculateCameraToWorld()
		{


			right = Vector3::Cross(Vector3::UnitY, forward);

			right.Normalize();

			up = Vector3::Cross(forward,right);

			up.Normalize();


			cameraToWorld = Matrix(right, up, forward, origin);

			
			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			float speed = 5.f;
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				speed = 8.f;
			}
			else
			{
				float speed = 5.f;
			}

			if (pKeyboardState[SDL_SCANCODE_W])
			{

				origin += forward *  speed * deltaTime;

			}
			else if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * speed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * speed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * speed * deltaTime;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if ((mouseState & SDL_BUTTON_LMASK))
			{
				if ((mouseState & SDL_BUTTON_RMASK) > 0)
				{
					origin -= up * speed * deltaTime * mouseY * senstivity;
					origin -= right * speed * deltaTime * mouseX * senstivity;
				}

				else {
					if (mouseX < 0)
					{
						totalPitch += speed / 10 * deltaTime;
					}
					else if (mouseX > 0)
					{
						totalPitch -= speed / 10 * deltaTime;
					}

					if (mouseY < 0)
					{
						fovAngle -= speed * 5 * deltaTime;
					}
					else if (mouseY > 0)
					{
						fovAngle += speed * 5 * deltaTime;
					}
				}
			}

			else if ((mouseState & SDL_BUTTON_RMASK))
			{


				if ((mouseState & SDL_BUTTON_LMASK) > 0)
				{
					origin -= up * speed * deltaTime * mouseY * senstivity;
					origin -= right * speed * deltaTime * mouseX * senstivity;
				}


				else
				{


					if (mouseX < 0)
					{
						totalPitch += speed / 10 * deltaTime;
					}
					else if (mouseX > 0)
					{
						totalPitch -= speed / 10 * deltaTime;
					}

					if (mouseY < 0)
					{
						totalYaw -= speed / 10 * deltaTime;
					}
					else if (mouseY > 0)
					{
						totalYaw += speed / 10 * deltaTime;
					}

				}
			}
			
		


			

			Matrix finalRotation{};

			finalRotation = finalRotation.CreateRotation(Vector3(totalYaw, totalPitch, 0));

			forward = finalRotation.TransformVector(Vector3::UnitZ);

			forward.Normalize();

			

			
		}
	};
}
