# C# Scripting API (Iberus)

This folder contains the **engine scripting runtime** for Iberus: Script base class, NativeBindings (P/Invoke), and ScriptBridge. Project scripts live in each project's `Assets/Scripts/` under `Game-Build/Projects/`.

## Build

1. Build the solution. The Game project post-build publishes `Iberus/Scripts/ScriptHost` (a stub exe referencing Iberus.Scripts) with `dotnet publish` → `Game-Build/`. Requires .NET 9 installed.
2. When you open a project, the engine scans `Assets/Scripts/` for `.csproj`, copies `Iberus.Scripts.dll` into that folder, and runs `dotnet build` per project.

## Usage

1. Add a **Script** component to an entity in the Inspector.
2. Set **Assembly** to `Assets/Scripts/{ProjectName}.Scripts.dll` (e.g. `Assets/Scripts/Demo.Scripts.dll` for the Demo project).
3. Set **Type** to the full type name (e.g. `Demo.BasicMovementController`).
4. Press F11 to enter Game mode – scripts run when the scene is simulating.

## Creating a Script

1. Create a class in your project's `Assets/Scripts/` folder that inherits from `Script` (from `IberusScripts`).
2. Override `OnInit()` and/or `OnUpdate(double deltaTime)`.
3. Use `GetTransform()` and `SetTransform()` to read/write the entity's transform.

Example:

```csharp
using IberusScripts;

public class MyBehaviour : Script
{
    protected override void OnUpdate(double deltaTime)
    {
        var (pos, rot, scale) = GetTransform();
        pos.X += 1.0f * (float)deltaTime;
        SetTransform(pos, rot, scale);
    }
}
```

## Hot-Reload

Scripts are unloaded when leaving Game mode (F11) and reloaded when entering again. Edit scripts, switch modes to test changes.
