# C# Scripting Runtime for Iberus

This folder contains the **engine runtime** for C# scripting: Script base class, NativeBindings (P/Invoke), and ScriptBridge. Project scripts live in each project's `Assets/Scripts/` under `Game-Build/Projects/`.

## Build

1. Regenerate the solution: `premake5 vs2022` (or your generator).
2. Build the solution. The Game project post-build publishes `Game/Scripts/ScriptHost` (a stub exe referencing Game.Scripts) with `dotnet publish --self-contained` → `Game-Build/`. This bundles the .NET 9 runtime; end users do not need .NET installed.
3. When you open a project, the engine scans `Assets/Scripts/` for `.csproj` and builds them.

## Usage

1. Add a **Script** component to an entity in the Inspector.
2. Set **Assembly** to `Assets/Scripts/{ProjectName}.Scripts.dll` (e.g. `Assets/Scripts/Demo.Scripts.dll` for the Demo project). Legacy format `{ProjectName}.Scripts.dll` is auto-resolved.
3. Set **Type** to the full type name (e.g. `Demo.BasicMovementController`).
4. Press F11 to enter Game mode – scripts run when the scene is simulating.

## Creating a Script

1. Create a class in your project's `Assets/Scripts/` folder that inherits from `Script`.
2. Override `OnInit()` and/or `OnUpdate(double deltaTime)`.
3. Use `GetTransform()` and `SetTransform()` to read/write the entity’s transform.

Example:

```csharp
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
