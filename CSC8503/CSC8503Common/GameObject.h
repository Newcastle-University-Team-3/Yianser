#pragma once
#include "Transform.h"
#include "..//../Gameplay/eGameObjectType.h"
#include "PhysicsXObject.h"

#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL
{
	namespace CSC8503
	{
		class NetworkObject;
		class GameObject
		{
		public:
			GameObject(string name = "");
			~GameObject();
			bool IsActive() const
			{
				return isActive;
			}

			Transform& GetTransform()
			{
				return transform;
			}

			RenderObject* GetRenderObject() const
			{
				return renderObject;
			}

			PhysicsXObject* GetPhysicsXObject() const
			{
				return physicsXObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject)
			{
				renderObject = newObject;
			}

			void SetPhysicsXObject(PhysicsXObject* newObject)
			{
				physicsXObject = newObject;
			}

			void SetNetworkObject(int id);

			void SetNetworkObject(NetworkObject* object) {
				networkObject = object;
			}

			const string& GetName() const
			{
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) { std::cout << "11" << std::endl; }

			virtual void OnCollisionEnd(GameObject* otherObject) {}
			
			bool GetBroadphaseAABB(Vector3& outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID)
			{
				worldID = newID;
			}

			int GetWorldID() const
			{
				return worldID;
			}

			GameObjectType type;

		protected:
			Transform transform;

			PhysicsXObject* physicsXObject;
			RenderObject* renderObject;
			NetworkObject* networkObject;

			bool isActive;
			int worldID;
			string name;

			Vector3 broadphaseAABB;
		};
	}
}
