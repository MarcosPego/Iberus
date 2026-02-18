using System.Runtime.InteropServices;

namespace GameScripts;

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

    protected virtual void OnInit() { }
    protected virtual void OnUpdate(double deltaTime) { }

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

    protected IberusVec3 GetSDFPartPosition(int partIndex)
    {
        NativeBindings.Iberus_World_GetSDFPartPosition(WorldPtr, EntityId, partIndex, out var pos);
        return pos;
    }

    protected void SetSDFPartPosition(int partIndex, IberusVec3 pos)
    {
        NativeBindings.Iberus_World_SetSDFPartPosition(WorldPtr, EntityId, partIndex, ref pos);
    }

    /// <summary>Log a debug message to the engine console.</summary>
    protected void Log(string message)
    {
        if (!string.IsNullOrEmpty(message))
        {
            NativeBindings.Iberus_Debug_Log(message);
        }
    }
}
