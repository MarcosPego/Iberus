using System.Runtime.InteropServices;

namespace IberusScripts;

[StructLayout(LayoutKind.Sequential)]
public struct IberusVec3
{
    public float X;
    public float Y;
    public float Z;

    public IberusVec3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
}

/// <summary>
/// P/Invoke declarations for Iberus engine C API.
/// </summary>
internal static class NativeBindings
{
    private const string LibraryName = "Iberus";

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_GetComponent_Transform(
        IntPtr worldPtr, ulong entityId,
        out IberusVec3 outPos, out IberusVec3 outRot, out IberusVec3 outScale);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Iberus_World_SetComponent_Transform(
        IntPtr worldPtr, ulong entityId,
        ref IberusVec3 pos, ref IberusVec3 rot, ref IberusVec3 scale);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_IsAlive(IntPtr worldPtr, ulong entityId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_HasComponent_Transform(IntPtr worldPtr, ulong entityId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern ulong Iberus_World_CreateEntity(IntPtr worldPtr);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Iberus_World_DestroyEntity(IntPtr worldPtr, ulong entityId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_Input_IsKeyPressed(int keyCode);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_HasComponent_SDF(IntPtr worldPtr, ulong entityId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_GetSDFPartCount(IntPtr worldPtr, ulong entityId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_GetSDFPartType(IntPtr worldPtr, ulong entityId, int partIndex);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_GetSDFPartPosition(IntPtr worldPtr, ulong entityId, int partIndex, out IberusVec3 outPos);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Iberus_World_SetSDFPartPosition(IntPtr worldPtr, ulong entityId, int partIndex, ref IberusVec3 pos);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Iberus_World_GetSDFPartEndpoint(IntPtr worldPtr, ulong entityId, int partIndex, out IberusVec3 outEndpoint);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Iberus_World_SetSDFPartEndpoint(IntPtr worldPtr, ulong entityId, int partIndex, ref IberusVec3 endpoint);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern float Iberus_World_GetSDFPartRadius(IntPtr worldPtr, ulong entityId, int partIndex);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern void Iberus_Debug_Log(string message);
}
