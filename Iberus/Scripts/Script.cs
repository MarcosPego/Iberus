using System.Runtime.InteropServices;

namespace IberusScripts;

/// <summary>
/// Base class for C# scripts. Override OnInit and OnUpdate.
/// </summary>
public abstract class Script
{
    protected ulong EntityId { get; private set; }
    protected IntPtr WorldPtr { get; private set; }

    internal void __Init(ulong entityId, IntPtr worldPtr)
    {
        EntityId = entityId;
        WorldPtr = worldPtr;
        OnInit();
    }

    internal void __Update(double deltaTime)
    {
        OnUpdate(deltaTime);
    }

    internal void __OnEntityChanged()
    {
        OnEntityChanged();
    }

    protected virtual void OnInit() { }
    protected virtual void OnUpdate(double deltaTime) { }

    /// <summary>Called when the entity's structure was changed externally (e.g. creature creator preset applied).</summary>
    protected virtual void OnEntityChanged() { }

    protected (IberusVec3 pos, IberusVec3 rot, IberusVec3 scale) GetTransform()
    {
        NativeBindings.Iberus_World_GetComponent_Transform(WorldPtr, EntityId,
            out var pos, out var rot, out var scale);
        return (pos, rot, scale);
    }

    protected void SetTransform(IberusVec3 pos, IberusVec3 rot, IberusVec3 scale)
    {
        NativeBindings.Iberus_World_SetComponent_Transform(WorldPtr, EntityId, ref pos, ref rot, ref scale);
    }

    protected bool IsAlive()
    {
        return NativeBindings.Iberus_World_IsAlive(WorldPtr, EntityId) != 0;
    }

    protected bool HasTransform()
    {
        return NativeBindings.Iberus_World_HasComponent_Transform(WorldPtr, EntityId) != 0;
    }

    protected bool IsKeyPressed(int keyCode)
    {
        return NativeBindings.Iberus_Input_IsKeyPressed(keyCode) != 0;
    }

    protected bool HasSDF() => NativeBindings.Iberus_World_HasComponent_SDF(WorldPtr, EntityId) != 0;

    protected int GetSDFPartCount() => NativeBindings.Iberus_World_GetSDFPartCount(WorldPtr, EntityId);

    protected int GetSDFPartType(int partIndex) => NativeBindings.Iberus_World_GetSDFPartType(WorldPtr, EntityId, partIndex);

    protected IberusVec3 GetSDFPartPosition(int partIndex)
    {
        NativeBindings.Iberus_World_GetSDFPartPosition(WorldPtr, EntityId, partIndex, out var pos);
        return pos;
    }

    protected void SetSDFPartPosition(int partIndex, IberusVec3 pos)
    {
        NativeBindings.Iberus_World_SetSDFPartPosition(WorldPtr, EntityId, partIndex, ref pos);
    }

    protected IberusVec3 GetSDFPartEndpoint(int partIndex)
    {
        NativeBindings.Iberus_World_GetSDFPartEndpoint(WorldPtr, EntityId, partIndex, out var endpoint);
        return endpoint;
    }

    protected void SetSDFPartEndpoint(int partIndex, IberusVec3 endpoint)
    {
        NativeBindings.Iberus_World_SetSDFPartEndpoint(WorldPtr, EntityId, partIndex, ref endpoint);
    }

    protected float GetSDFPartRadius(int partIndex) =>
        NativeBindings.Iberus_World_GetSDFPartRadius(WorldPtr, EntityId, partIndex);

    /// <summary>Log a debug message to the engine console.</summary>
    protected void Log(string message)
    {
        if (!string.IsNullOrEmpty(message))
        {
            NativeBindings.Iberus_Debug_Log(message);
        }
    }
}
