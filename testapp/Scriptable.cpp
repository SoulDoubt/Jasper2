#include "Scriptable.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Scene.h"
#include "Sphere.h"
#include "PhysicsCollider.h"
#include "CharacterController.h"


//void Jasper::ShooterScript::LateUpdate()
//{
//	if (CharacterController* player = dynamic_cast<CharacterController*>(GetGameObject())) {
//		auto world = player->GetScene()->GetPhysicsWorld();
//		auto btWorld = world->GetBtWorld();
//		auto vm = player->GetScene()->GetCamera().GetViewMatrix().Inverted();
//		auto proj = player->GetScene()->GetProjectionMatrix();
//		auto mod = Matrix4();
//		mod.SetToIdentity();
//		auto mvp = proj * vm * mod;
//		world->debugDrawer->mvpMatrix = mvp;
//		for (auto& rh : RayHits) {
//			world->debugDrawer->drawLine(rh.from, rh.to, { 1.f, 1.f, 1.f });
//		}
//	}
//}
//
//void Jasper::ShooterScript::Shoot()
//{
//	printf("Shooting a sphere...\n");
//	auto gameObject = GetGameObject();
//	if (CharacterController* player = dynamic_cast<CharacterController*>(gameObject)) {
//		auto scene = gameObject->GetScene();
//		auto pw = scene->GetPhysicsWorld();
//		auto transform = player->GetWorldTransform();
//		//transform.SetIdentity();
//		auto rootNode = gameObject->GetScene()->GetRootNode();
//		auto mat = scene->GetMaterialCache().GetResourceByName("red_material");
//		auto sphere = rootNode->AttachNewChild<GameObject>("Sphere");
//		Mesh* mesh = nullptr;
//		if ((mesh = scene->GetMeshCache().GetResourceByName("shoot_sphere")) == nullptr) {
//			// need to create the mesh instance
//			mesh = scene->CreateMesh<Sphere>("shot_sphere", 0.1f);			
//			mesh->Initialize();
//			mesh->Color = { 0.f, 1.f, 1.f, 1.f };
//		}
//
//		auto mr = sphere->AttachNewComponent<MeshRenderer>("shot_sphere_renderer", mesh, mat);
//		mr->Initialize();
//		sphere->SetLocalTransform(transform);
//
//		auto collider = sphere->AttachNewComponent<SphereCollider>("Sphere_collider", mesh->GetHalfExtents(), pw);
//		collider->Mass = 0.2f;
//		collider->Restitution = 0.87f;
//		sphere->AttachNewComponent<DestroyScript>("Destroy_Script");
//		sphere->Awake();
//		sphere->Children().clear();
//		scene->GetRenderer()->RegisterGameObject(sphere);
//		auto rb = collider->GetRigidBody();
//		
//		Vector3 center_offset = { 0.f, 0.f, 0.f };
//		Vector3 f = Normalize(player->GetForwardsVector()) * Force;
//		rb->setLinearVelocity(f.AsBtVector3());
//		//collider->GetRigidBody()->applyForce(f.AsBtVector3(), center_offset.AsBtVector3());
//	}
//	//printf("%s has %d children", gameObject->GetName().c_str(), gameObject->Children().size());
//
//	
//
//}
//
//void Jasper::ShooterScript::ShootRay()
//{
//	if (CharacterController* player = dynamic_cast<CharacterController*>(GetGameObject())) {
//		btVector3 direction = player->GetForwardsVector().AsBtVector3();
//		btVector3 from = player->GetWorldTransform().Position.AsBtVector3();
//		btVector3 to = from + (1000 * (direction));
//		auto world = player->GetScene()->GetPhysicsWorld();
//		auto btWorld = world->GetBtWorld();
//
//		btCollisionWorld::ClosestRayResultCallback cb(from, to);
//		btWorld->rayTest(from, to, cb);
//
//		if (cb.hasHit()) {
//			auto point = cb.m_hitPointWorld;
//			auto normal = cb.m_hitNormalWorld;
//			auto collider = (btRigidBody*)cb.m_collisionObject;
//			auto offset = point - collider->getWorldTransform().getOrigin();
//			collider->activate();
//			collider->applyImpulse(-normal * 500.f, offset);
//			RayHits.emplace_back(RayHit{ from, point });
//		}
//		
//
//	}
//}
