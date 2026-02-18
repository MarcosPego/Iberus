using IberusScripts;

namespace Demo;

/// <summary>
/// WASD + Q/E movement controller for SDF entities.
/// Moves the root and updates SDF part positions with tension-based follow.
/// </summary>
public class BasicMovementController : Script
{
    private const float MoveSpeed = 0.05f;
    private const float MaxTension = 2.0f;

    // KeyCode values (aligned with engine)
    private const int KeyW = 87;
    private const int KeyS = 83;
    private const int KeyA = 65;
    private const int KeyD = 68;
    private const int KeyQ = 81;
    private const int KeyE = 69;

    private int _frameCount;

    protected override void OnInit()
    {
        Log($"BasicMovementController OnInit Entity={EntityId}");
    }

    protected override void OnUpdate(double deltaTime)
    {
        _frameCount++;
        var (pos, rot, scale) = GetTransform();

        // Debug: log every 60 frames with state
        if (_frameCount % 60 == 1)
        {
            bool w = IsKeyPressed(KeyW), s = IsKeyPressed(KeyS), a = IsKeyPressed(KeyA);
            bool d = IsKeyPressed(KeyD), q = IsKeyPressed(KeyQ), e = IsKeyPressed(KeyE);
            Log($"BasicMovementController Update frame={_frameCount} pos=({pos.X:F2},{pos.Y:F2},{pos.Z:F2}) keys W={w} A={a} S={s} D={d} Q={q} E={e}");
        }

        float dx = 0;
        float dy = 0;
        float dz = 0;
        if (IsKeyPressed(KeyW)) dy += 1;
        if (IsKeyPressed(KeyS)) dy -= 1;
        if (IsKeyPressed(KeyA)) dx += 1;
        if (IsKeyPressed(KeyD)) dx -= 1;
        if (IsKeyPressed(KeyQ)) dz += 1;
        if (IsKeyPressed(KeyE)) dz -= 1;

        float len = MathF.Sqrt(dx * dx + dy * dy + dz * dz);
        if (len > 0)
        {
            float inv = MoveSpeed / len;
            pos.X += dx * inv;
            pos.Y += dy * inv;
            pos.Z += dz * inv;
            SetTransform(pos, rot, scale);
        }

        if (!HasSDF())
        {
            return;
        }

        int partCount = GetSDFPartCount();
        float lastX = pos.X;
        float lastY = pos.Y;
        float lastZ = pos.Z;

        for (int i = 0; i < partCount; i++)
        {
            var partPos = GetSDFPartPosition(i);
            float partWorldX = pos.X + partPos.X;
            float partWorldY = pos.Y + partPos.Y;
            float partWorldZ = pos.Z + partPos.Z;

            float diffX = lastX - partWorldX;
            float diffY = lastY - partWorldY;
            float diffZ = lastZ - partWorldZ;
            float dist = MathF.Sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);

            if (dist > MaxTension)
            {
                float inv = MoveSpeed / dist;
                partPos.X += diffX * inv;
                partPos.Y += diffY * inv;
                partPos.Z += diffZ * inv;
                SetSDFPartPosition(i, partPos);
            }

            lastX = pos.X + partPos.X;
            lastY = pos.Y + partPos.Y;
            lastZ = pos.Z + partPos.Z;
        }
    }
}
