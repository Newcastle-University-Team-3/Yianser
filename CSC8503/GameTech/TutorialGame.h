#pragma once
#include "GameTechRenderer.h"
#include "OGLTexture.h"
#include "../CSC8503Common/PhysicsXSystem.h"
#include "../../AudioManager/AudioManager.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			float FPS = 0.0f;
			float framesPerSecond = 0.0f;   
			float lastTime = 0.0f;
			float previousSecond;
			float currentSecond;


			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing,
			                       const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();

			bool SelectObject();
			bool SelectXObject();

			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void CalculateFrameRate(float dt);

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius,
			                              float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsXSystem*		physicsX;
			GameWorld*			world;
	
			bool useGravity;
			bool inSelectionMode;
			bool DebugMode;

			float forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh* capsuleMesh = nullptr;
			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;

			//Coursework Meshes
			OGLMesh* charMeshA = nullptr;
			OGLMesh* charMeshB = nullptr;
			OGLMesh* enemyMesh = nullptr;
			OGLMesh* bonusMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);

			void LockCameraToObject(GameObject* o)
			{
				lockedObject = o;
			}
		};
	}
}
