using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

namespace IberusScripts;

/// <summary>
/// Bridge between native host and script instances. Methods are called via P/Invoke
/// from hostfxr load_assembly_and_get_function_pointer with UNMANAGEDCALLERSONLY_METHOD.
/// </summary>
public static class ScriptBridge
{
    private static readonly Dictionary<IntPtr, Script> Instances = new();
    private static IntPtr _nextHandle = (IntPtr)1;
    private static string? _scriptAssemblyDir;
    private static bool _assemblyResolveRegistered;

    private static void EnsureAssemblyResolve()
    {
        if (_assemblyResolveRegistered)
        {
            return;
        }
        _assemblyResolveRegistered = true;
        var executingAsm = Assembly.GetExecutingAssembly();
        AppDomain.CurrentDomain.AssemblyResolve += (_, args) =>
        {
            var aname = new AssemblyName(args.Name);
            // Always return the already-loaded Iberus.Scripts (from App) so Script types match
            if (string.Equals(aname.Name, "Iberus.Scripts", StringComparison.OrdinalIgnoreCase))
            {
                return executingAsm;
            }
            var dirs = new List<string?>();
            if (!string.IsNullOrEmpty(_scriptAssemblyDir))
            {
                dirs.Add(_scriptAssemblyDir);
            }
            if (args.RequestingAssembly?.Location is string loc)
            {
                dirs.Add(Path.GetDirectoryName(loc));
            }
            var execDir = Path.GetDirectoryName(executingAsm.Location);
            if (!string.IsNullOrEmpty(execDir))
            {
                dirs.Add(execDir);
            }
            foreach (var dir in dirs.Where(d => !string.IsNullOrEmpty(d)))
            {
                var path = Path.Combine(dir!, aname.Name + ".dll");
                if (File.Exists(path))
                {
                    return Assembly.LoadFrom(path);
                }
            }
            return null;
        };
    }

    private static void Log(string msg)
    {
        try { NativeBindings.Iberus_Debug_Log("[ScriptBridge] " + msg); } catch { }
    }

    /// <param name="assemblyPathPtr">Full path to script assembly (UTF-8).</param>
    /// <param name="typeNamePtr">Full type name (UTF-8).</param>
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
            Log("CreateInstance: typeName empty");
            return IntPtr.Zero;
        }
        try
        {
            EnsureAssemblyResolve();
            string? assemblyPath = assemblyPathPtr != IntPtr.Zero ? Marshal.PtrToStringUTF8(assemblyPathPtr) : null;
            Log($"CreateInstance: assemblyPath={(assemblyPath ?? "(null)")} typeName={typeName}");
            if (!string.IsNullOrEmpty(assemblyPath))
            {
                if (!File.Exists(assemblyPath))
                {
                    Log($"CreateInstance: assembly file not found: {assemblyPath}");
                    return IntPtr.Zero;
                }
                _scriptAssemblyDir = Path.GetDirectoryName(Path.GetFullPath(assemblyPath));
                var asm = Assembly.LoadFrom(assemblyPath);
                var type = asm.GetType(typeName)
                    ?? asm.GetExportedTypes().FirstOrDefault(t => t.FullName == typeName || t.Name == typeName);
                if (type == null)
                {
                    var exported = string.Join(", ", asm.GetExportedTypes().Select(t => t.FullName ?? t.Name));
                    Log($"CreateInstance: type '{typeName}' not found in assembly. Exported: [{exported}]");
                    return IntPtr.Zero;
                }
                if (!typeof(Script).IsAssignableFrom(type))
                {
                    Log($"CreateInstance: type '{type.FullName}' is not assignable from Script");
                    return IntPtr.Zero;
                }
                var script = (Script?)Activator.CreateInstance(type);
                if (script == null)
                {
                    Log("CreateInstance: Activator.CreateInstance returned null");
                    return IntPtr.Zero;
                }
                IntPtr handle = _nextHandle;
                unchecked { _nextHandle = (IntPtr)((long)_nextHandle + 1); }
                Instances[handle] = script;
                return handle;
            }
            var fallbackType = Type.GetType(typeName) ?? Type.GetType(typeName + ", Iberus.Scripts");
            if (fallbackType == null || !typeof(Script).IsAssignableFrom(fallbackType))
            {
                return IntPtr.Zero;
            }
            var fbScript = (Script?)Activator.CreateInstance(fallbackType);
            if (fbScript == null)
            {
                return IntPtr.Zero;
            }
            IntPtr h = _nextHandle;
            unchecked { _nextHandle = (IntPtr)((long)_nextHandle + 1); }
            Instances[h] = fbScript;
            return h;
        }
        catch (Exception ex)
        {
            Log($"CreateInstance exception: {ex.GetType().Name}: {ex.Message}");
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
