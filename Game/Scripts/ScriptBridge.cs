using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

namespace GameScripts;

/// <summary>
/// Bridge between native host and script instances. Methods are called via P/Invoke
/// from hostfxr load_assembly_and_get_function_pointer.
/// </summary>
public static class ScriptBridge
{
    private static readonly Dictionary<IntPtr, Script> Instances = new();
    private static IntPtr _nextHandle = (IntPtr)1;

    /// <param name="assemblyPathPtr">Full path to script assembly (e.g. project's Demo.Scripts.dll).</param>
    /// <param name="typeNamePtr">Full type name (e.g. Demo.BasicMovementController).</param>
    [UnmanagedCallersOnly]
    public static IntPtr CreateInstance(IntPtr assemblyPathPtr, IntPtr typeNamePtr)
    {
        if (typeNamePtr == IntPtr.Zero)
        {
            return IntPtr.Zero;
        }
        string typeName = Marshal.PtrToStringUTF8(typeNamePtr) ?? "";
        if (string.IsNullOrEmpty(typeName))
        {
            return IntPtr.Zero;
        }
        try
        {
            Type? type;
            if (assemblyPathPtr != IntPtr.Zero)
            {
                string assemblyPath = Marshal.PtrToStringUTF8(assemblyPathPtr) ?? "";
                if (!string.IsNullOrEmpty(assemblyPath))
                {
                    var asm = Assembly.LoadFrom(assemblyPath);
                    type = asm.GetType(typeName);
                    if (type == null)
                    {
                        type = asm.GetExportedTypes().FirstOrDefault(t => t.FullName == typeName || t.Name == typeName);
                    }
                }
                else
                {
                    type = Type.GetType(typeName) ?? Type.GetType(typeName + ", Game.Scripts");
                }
            }
            else
            {
                type = Type.GetType(typeName) ?? Type.GetType(typeName + ", Game.Scripts");
            }
            if (type == null || !typeof(Script).IsAssignableFrom(type))
            {
                return IntPtr.Zero;
            }
            var script = (Script?)Activator.CreateInstance(type);
            if (script == null)
            {
                return IntPtr.Zero;
            }
            IntPtr handle = _nextHandle;
            unchecked { _nextHandle = (IntPtr)((long)_nextHandle + 1); }
            Instances[handle] = script;
            return handle;
        }
        catch
        {
            return IntPtr.Zero;
        }
    }

    [UnmanagedCallersOnly]
    public static void Init(IntPtr handle, ulong entityId, IntPtr worldPtr)
    {
        if (handle == IntPtr.Zero || !Instances.TryGetValue(handle, out var script))
        {
            return;
        }
        script.__Init(entityId, worldPtr);
    }

    [UnmanagedCallersOnly]
    public static void Update(IntPtr handle, ulong entityId, IntPtr worldPtr, double deltaTime)
    {
        if (handle == IntPtr.Zero || !Instances.TryGetValue(handle, out var script))
        {
            return;
        }
        script.__Update(deltaTime);
    }

    [UnmanagedCallersOnly]
    public static void DestroyInstance(IntPtr handle)
    {
        if (handle != IntPtr.Zero)
        {
            Instances.Remove(handle);
        }
    }
}
