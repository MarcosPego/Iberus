#include "Enginepch.h"
#include "ScriptAPI.h"
#include "World.h"
#include "Components.h"
#include "EntityId.h"
#include "MathUtils.h"
#include "Engine.h"
#include "InputManager.h"
#include "KeyCode.h"
#include "Log.h"

using namespace Math;

extern "C" {

int Iberus_World_GetComponent_Transform(void* worldPtr, uint64_t entityId,
    Iberus_Vec3* outPos, Iberus_Vec3* outRot, Iberus_Vec3* outScale) {
    if (!worldPtr || !outPos || !outRot || !outScale) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* t = world->GetComponent<Iberus::TransformComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!t) {
        return 0;
    }
    outPos->x = t->Position.x;
    outPos->y = t->Position.y;
    outPos->z = t->Position.z;
    outRot->x = t->Rotation.x;
    outRot->y = t->Rotation.y;
    outRot->z = t->Rotation.z;
    outScale->x = t->Scale.x;
    outScale->y = t->Scale.y;
    outScale->z = t->Scale.z;
    return 1;
}

void Iberus_World_SetComponent_Transform(void* worldPtr, uint64_t entityId,
    const Iberus_Vec3* pos, const Iberus_Vec3* rot, const Iberus_Vec3* scale) {
    if (!worldPtr) {
        return;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* t = world->GetComponent<Iberus::TransformComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!t) {
        return;
    }
    if (pos) {
        t->Position.x = pos->x;
        t->Position.y = pos->y;
        t->Position.z = pos->z;
    }
    if (rot) {
        t->Rotation.x = rot->x;
        t->Rotation.y = rot->y;
        t->Rotation.z = rot->z;
    }
    if (scale) {
        t->Scale.x = scale->x;
        t->Scale.y = scale->y;
        t->Scale.z = scale->z;
    }
}

int Iberus_World_IsAlive(void* worldPtr, uint64_t entityId) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    return world->IsAlive(static_cast<Iberus::EntityId>(entityId)) ? 1 : 0;
}

int Iberus_World_HasComponent_Transform(void* worldPtr, uint64_t entityId) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    return world->HasComponent<Iberus::TransformComponent>(static_cast<Iberus::EntityId>(entityId)) ? 1 : 0;
}

uint64_t Iberus_World_CreateEntity(void* worldPtr) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    return static_cast<uint64_t>(world->CreateEntity());
}

void Iberus_World_DestroyEntity(void* worldPtr, uint64_t entityId) {
    if (!worldPtr) {
        return;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    world->DestroyEntity(static_cast<Iberus::EntityId>(entityId));
}

int Iberus_Input_IsKeyPressed(int keyCode) {
    return Iberus::Engine::Instance()->GetInputManager().IsKeyPressed(static_cast<Iberus::KeyCode>(keyCode)) ? 1 : 0;
}

int Iberus_World_HasComponent_SDF(void* worldPtr, uint64_t entityId) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    return world->HasComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId)) ? 1 : 0;
}

int Iberus_World_GetSDFPartCount(void* worldPtr, uint64_t entityId) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    return sdf ? static_cast<int>(sdf->Parts.size()) : 0;
}

int Iberus_World_GetSDFPartType(void* worldPtr, uint64_t entityId, int partIndex) {
    if (!worldPtr) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return 0;
    }
    return sdf->Parts[static_cast<size_t>(partIndex)].Type;
}

int Iberus_World_GetSDFPartPosition(void* worldPtr, uint64_t entityId, int partIndex, Iberus_Vec3* outPos) {
    if (!worldPtr || !outPos) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return 0;
    }
    const auto& part = sdf->Parts[static_cast<size_t>(partIndex)];
    outPos->x = part.Transform.Position.x;
    outPos->y = part.Transform.Position.y;
    outPos->z = part.Transform.Position.z;
    return 1;
}

void Iberus_World_SetSDFPartPosition(void* worldPtr, uint64_t entityId, int partIndex, const Iberus_Vec3* pos) {
    if (!worldPtr || !pos) {
        return;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return;
    }
    auto& part = sdf->Parts[static_cast<size_t>(partIndex)];
    part.Transform.Position.x = pos->x;
    part.Transform.Position.y = pos->y;
    part.Transform.Position.z = pos->z;
}

int Iberus_World_GetSDFPartEndpoint(void* worldPtr, uint64_t entityId, int partIndex, Iberus_Vec3* outEndpoint) {
    if (!worldPtr || !outEndpoint) {
        return 0;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return 0;
    }
    const auto& part = sdf->Parts[static_cast<size_t>(partIndex)];
    outEndpoint->x = part.Endpoint.x;
    outEndpoint->y = part.Endpoint.y;
    outEndpoint->z = part.Endpoint.z;
    return 1;
}

void Iberus_World_SetSDFPartEndpoint(void* worldPtr, uint64_t entityId, int partIndex, const Iberus_Vec3* endpoint) {
    if (!worldPtr || !endpoint) {
        return;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return;
    }
    auto& part = sdf->Parts[static_cast<size_t>(partIndex)];
    part.Endpoint.x = endpoint->x;
    part.Endpoint.y = endpoint->y;
    part.Endpoint.z = endpoint->z;
}

float Iberus_World_GetSDFPartRadius(void* worldPtr, uint64_t entityId, int partIndex) {
    if (!worldPtr) {
        return 0.0f;
    }
    auto* world = static_cast<Iberus::World*>(worldPtr);
    auto* sdf = world->GetComponent<Iberus::SDFComponent>(static_cast<Iberus::EntityId>(entityId));
    if (!sdf || partIndex < 0 || static_cast<size_t>(partIndex) >= sdf->Parts.size()) {
        return 0.0f;
    }
    return sdf->Parts[static_cast<size_t>(partIndex)].Radius;
}

void Iberus_Debug_Log(const char* message) {
    if (message) {
        Iberus::Log::GetClientLogger()->info("[Script] {}", message);
    }
}

}
