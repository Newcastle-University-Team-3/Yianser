#include "Player.h"
#include "PlayerController.h"
#include "../CSC8503/CSC8503Common/PhysicsXSystem.h"
#include "../CSC8503/CSC8503Common/State.h"
#include "../CSC8503/CSC8503Common/StateTransition.h"

Player::Player(PlayerRole colour, AbilityContainer* aCont, GameObjectType type)
{
	forward = Quaternion(transform.GetOrientation()) * Vector3(0, 0, 1);
	right = Vector3::Cross(Vector3(0, 1, 0), forward);
	shootDir = forward;
	pColour = colour;
	AssignRole(aCont);
	this->type = type;
	SetupStateMachine();
}

Player::~Player() {
	for (auto i : abilities)
		delete i;

	delete bullet;
}

void Player::SetUp()
{
	auto physics = new ComponentPhysics();
	physics->phyObj = GetPhysicsXObject();
	PhyProperties properties = PhyProperties();
	properties.type = PhyProperties::Character;
	properties.transform = PhysXConvert::TransformToPxTransform(GetTransform());
	properties.Mass = 10.0f;

	Vector3 scale = GetTransform().GetScale() / 2.0f;
	properties.volume = new PxBoxGeometry(PhysXConvert::Vector3ToPxVec3(scale));

	physics->phyObj->properties = properties;
	PushComponent(physics);

	const auto input = new ComponentInput();
	input->user_interface = new PlayerController();
	input->updateCallback = [this](float dt) {
		forward = transform.GetOrientation() * Vector3(0, 0, 1);
		right = Vector3::Cross(forward, Vector3(0,1,0));
   		lastInput = GetComponentInput()->user_interface->get_inputs();
		playerState->Update(dt);
	};
	PushComponent(input);

	auto camera = new ComponentCamera();
	camera->gO = this;

	camera->camera = new Camera();
	camera->camera->SetNearPlane(0.1f);
	camera->camera->SetFarPlane(500.0f);
	camera->camera->SetPitch(-15.0f);
	camera->camera->SetYaw(180);

	PushComponent(camera);
}
void Player::Move(Vector2 dir) {

	// Move forward
	if (dir.y > 0) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(forward), 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	// Move backward
	if (dir.y < 0) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(forward) * (-1), 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
	// Move left
	if (dir.x < 0) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(right) * (-1), 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	};
	// Move right
	if (dir.x > 0) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(right), 0.0001f, 0.2,
			PxControllerFilters(), NULL);
	}
}


void Player::Dash() {
	if (dashCooldown <= 0.0f) {
		physicsXObject->controller->move(PhysXConvert::Vector3ToPxVec3(forward) * 20.0f, 0.0001f, 0.2,
			PxControllerFilters(), NULL);
		dashCooldown = 2.0f;
	}
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
		bullet = new Bullet(static_cast<GameObjectType>(this->type + 1), PlayerRole_red);
		break;
	case PlayerRole_green:
		colour = "Green";
		abilities[0] = aCont->allAbilities[2];
		abilities[1] = aCont->allAbilities[3];
		bullet = new Bullet(static_cast<GameObjectType>(this->type + 1), PlayerRole_green);
		break;
	case PlayerRole_blue:
		colour = "Blue";
		abilities[0] = aCont->allAbilities[4];
		abilities[1] = aCont->allAbilities[5];
		bullet = new Bullet(static_cast<GameObjectType>(this->type + 1), PlayerRole_blue);
		break;
	}
}

void Player::SetupStateMachine()
{
	playerState = new StateMachine();

	State* Idle = new State([&](float dt) -> void {
	});
	State* Walk = new State([&](float dt) -> void {
		if (lastInput.movement_direction == Vector2(0, 1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(forward));
		}
		if (lastInput.movement_direction == Vector2(0, -1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-forward));
		}
		if (lastInput.movement_direction == Vector2(1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(right));
		}
		if (lastInput.movement_direction == Vector2(-1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-right));
		}
	});
	State* StandingJump = new State([&](float dt) -> void {
		if (lastInput.movement_direction == Vector2(0, 1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(forward));
		}
		if (lastInput.movement_direction == Vector2(0, -1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-forward));
		}
		if (lastInput.movement_direction == Vector2(1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(right));
		}
		if (lastInput.movement_direction == Vector2(-1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-right));
		}
		if (CurrentHeight < JumpHeight) {
				CurrentHeight += 0.1f;
				physicsXObject->CMove(PxVec3(0, 1, 0) * CurrentJumpspeed);
				CurrentJumpspeed -= 0.1f;
			}
		else {
				physicsXObject->CMove(PxVec3(0, 1, 0) * -CurrentJumpspeed);
				CurrentJumpspeed += 0.1f;
			}
		});
	State* DoubleJump = new State([&](float dt) -> void {
		if (lastInput.movement_direction == Vector2(0, 1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(forward));
		}
		if (lastInput.movement_direction == Vector2(0, -1)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-forward));
		}
		if (lastInput.movement_direction == Vector2(1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(right));
		}
		if (lastInput.movement_direction == Vector2(-1, 0)) {
			physicsXObject->CMove(PhysXConvert::Vector3ToPxVec3(-right));
		}
		if (CurrentHeight < JumpHeight) {
			CurrentHeight += 0.1f;
			physicsXObject->CMove(PxVec3(0, 1, 0) * CurrentJumpspeed);
			CurrentJumpspeed -= 0.1f;
		}
		else {
			physicsXObject->CMove(PxVec3(0, 1, 0) * -CurrentJumpspeed);
			CurrentJumpspeed += 0.1f;
		}
	});
	State* Dashing = new State([&](float dt) -> void {
		
		});

	StateTransition* IdleToStandingJump = new StateTransition(Idle, StandingJump, [&](void)->bool {
		if (lastInput.buttons[jump] and isGrounded)
		{
			isGrounded = false;
			return true;
		}
		return false;
		});
	StateTransition* WalkToStandingJump = new StateTransition(Walk, StandingJump, [&](void)->bool {
		if (lastInput.buttons[jump] and isGrounded)
		{
			isGrounded = false;
			return true;
		}
		return false;
		});
	StateTransition* StandingJumpToIdle = new StateTransition(StandingJump, Idle, [&](void)->bool {
		if (isGrounded)
		{
			CurrentHeight = 0.0f;
			CurrentJumpspeed = 2.0f;
			return true;
		}
		return false;
		});
	StateTransition* DoubleJumpToIdle = new StateTransition(DoubleJump, Idle, [&](void)->bool {
		if (isGrounded)
		{
 			CurrentHeight = 0.0f;
			CurrentJumpspeed = 2.0f;
			return true;
		}
		return false;
		});
	StateTransition* StandingJumpToDoubleJump = new StateTransition(StandingJump, DoubleJump, [&](void)->bool {
		if (lastInput.buttons[jump]) {
			CurrentHeight = 0.0f;
			CurrentJumpspeed = 2.0f;
			return true;
		}
		return false;
		});
	StateTransition* IdleToWalk = new StateTransition(Idle, Walk, [&](void)->bool {
		return lastInput.movement_direction not_eq Vector2();
		});
	StateTransition* WalkToIdle = new StateTransition(Walk, Idle, [&](void)->bool {
		return lastInput.movement_direction == Vector2();
		});

	
	playerState->AddState(Idle);
	playerState->AddState(Walk);
	playerState->AddState(StandingJump);
	playerState->AddState(DoubleJump);
	playerState->AddTransition(IdleToStandingJump);
	playerState->AddTransition(StandingJumpToIdle);
	playerState->AddTransition(StandingJumpToDoubleJump);
	playerState->AddTransition(DoubleJumpToIdle);
	playerState->AddTransition(WalkToStandingJump);
	playerState->AddTransition(IdleToWalk);
	playerState->AddTransition(WalkToIdle);

	//automata 
}