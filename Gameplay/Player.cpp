#include "Player.h"
#include "PlayerController.h"
#include "../CSC8503/CSC8503Common/PhysicsXSystem.h"
#include "PlayerState.h"
#include "WeaponState.h"
Player::Player(PlayerRole colour, AbilityContainer* aCont, GameObjectType type,bool localplayer)
{
	forward = Quaternion(transform.GetOrientation()) * Vector3(0, 0, 1);
	right = Vector3::Cross(Vector3(0, 1, 0), forward);
	shootDir = forward;
	pColour = colour;
	isLocalPlayer = localplayer;
	AssignRole(aCont);
	this->type = type;
	SetupStateMachine();
}

Player::~Player() {


}

void Player::SetUp()
{
	auto physics = new ComponentPhysics();
	physics->phyObj = GetPhysicsXObject();
	PhyProperties properties = PhyProperties();
	properties.type = PhyProperties::Character;
	properties.transform = PhysXConvert::TransformToPxTransform(GetTransform());
	properties.Mass = 10.0f;
	properties.positionOffset = Vector3(0,4.1,0);

	Vector3 scale = GetTransform().GetScale();
	properties.volume = new PxCapsuleGeometry(PhysXConvert::Vector3ToPxVec3(scale).x,
		PhysXConvert::Vector3ToPxVec3(scale).y);

	physics->phyObj->properties = properties;
	PushComponent(physics);

	if (isLocalPlayer) {
		const auto input = new ComponentInput();
		input->user_interface = new PlayerController();

		PushComponent(input);

		auto camera = new ComponentCamera();
		camera->gO = this;

		camera->camera = new Camera();
		camera->camera->SetNearPlane(0.1f);
		camera->camera->SetFarPlane(500.0f);
		camera->camera->SetPitch(-15.0f);

		PushComponent(camera);
	}
}
void Player::Move() {
	if (lastInput.movement_direction == Vector2(0, 1)) {
		physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(forward)*speed);
	}
	if (lastInput.movement_direction == Vector2(0, -1)) {
		physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-forward) * speed);
	}
	if (lastInput.movement_direction == Vector2(1, 0)) {
		physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(right) * speed);
	}
	if (lastInput.movement_direction == Vector2(-1, 0)) {
		physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-right) * speed);
	}
}
void Player::Jump(float dt) {
	physicsXObject->CMove(PxVec3(0, 1, 0) * cos(timeStack.jumpingTimeStack));
	timeStack.jumpingTimeStack += dt * 3;
}
void Player::Dash(float dt) {
	if (lastInput.movement_direction == Vector3(0,0,1)) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(forward) * 3.0f, 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	if (lastInput.movement_direction == Vector3(0,0,-1)) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(-forward) * 3.0f, 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	if (lastInput.movement_direction == Vector3(1,0, 0)) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(right) * 3.0f, 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	if (lastInput.movement_direction == Vector3(-1,0,0)) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(-right) * 3.0f, 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	timeStack.dashingTimeStack += dt;
}
void Player::Openfire() {
	if (ammo > 0) {
		YiEventSystem::GetMe()->PushEvent(PLAYER_OPEN_FIRE, GetWorldID());
		ammo--;
	}
}
float Player::TakeDamage(float dmg) {
	health = health - dmg < 0 ? 0 : health - dmg;
	return health;
}
bool Player::IsDead() {
	return health == 0 ? true : false;
}
void Player::Respawn() {
	YiEventSystem::GetMe()->PushEvent(PLAYER_RESPWAN,GetWorldID());
}

// Give damage to palyer a
void Player::GiveDamage(float dmg, Player* a) {
	a->TakeDamage(dmg);
	if (a->IsDead() == true) {
		teamKill++;
	}
}

void Player::Reload() {
	isReloading = false;
	if (ammo >= 0 && ammo < maxAmmo) {
		isReloading = true;
		ammo = maxAmmo;
		// Finish reload
		isReloading = false;
	}
}

void Player::AssignRole(AbilityContainer* aCont)
{
	std::string colour;

	switch (pColour) {
	case PlayerRole_red:
		colour = "Red";
		abilities[0] = aCont->allAbilities[0];
		abilities[1] = aCont->allAbilities[1];
		break;
	case PlayerRole_green:
		colour = "Green";
		abilities[0] = aCont->allAbilities[2];
		abilities[1] = aCont->allAbilities[3];
		break;
	case PlayerRole_blue:
		colour = "Blue";
		abilities[0] = aCont->allAbilities[4];
		abilities[1] = aCont->allAbilities[5];
		break;
	}
}

void Player::SetupStateMachine()
{
	Idle* idle = new Idle();
	idle->userdata = this;
	playerState = new PushdownMachine(idle);

	Hold* hold = new Hold();
	hold->userdata = this;
	weaponState = new PushdownMachine(hold);
}
void Player::Update(float dt) {
	ComponentGameObject::Update(dt);
	transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(lastInput.look_direction.x,
		lastInput.look_direction.y,0));
	if (GetComponentCamera()) {
		forward = GetComponentCamera()->camera->GetThirdPersonOrientation() * Vector3(0, 0, -1);
		Vector2 screenSize = Window::GetWindow()->GetScreenSize();
		Vector3 target = PhysicsXSystem::getMe()->ScreenToWorld(*GetComponentCamera()->camera, screenSize / 2.0f,false);
		shootDir = (target-transform.GetPosition()).Normalised();
	}
	right = Vector3::Cross(forward, Vector3(0, 1, 0));
	if(GetComponentInput())
		lastInput = GetComponentInput()->user_interface->get_inputs();
	playerState->Update(dt);
	weaponState->Update(dt);
	timeStack.dashCooldown -= dt;
	timeStack.respawnCooldown -= dt;
}

void Player::OnBlue() {
	if (isOnBlue == true) {
		speed = speed * 2;
	}
}

void Player::OnGreen() {
	if (isOnGreen == true) {
		health += 30;
		if (health >= 100) {
			health = 100;
		}
	}
}