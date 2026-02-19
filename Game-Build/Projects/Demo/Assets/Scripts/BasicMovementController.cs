using IberusScripts;

namespace Demo;

/// <summary>
/// WASD (XZ ground plane) + Q/E (Y up/down) movement controller for SDF entities.
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

    protected override void OnInit()
    {
    }

    protected override void OnUpdate(double deltaTime)
    {
        var (pos, rot, scale) = GetTransform();

        float dx = 0;
        float dy = 0;
        float dz = 0;
        if (IsKeyPressed(KeyW)) dz += 1;
        if (IsKeyPressed(KeyS)) dz -= 1;
        if (IsKeyPressed(KeyA)) dx += 1;
        if (IsKeyPressed(KeyD)) dx -= 1;
        if (IsKeyPressed(KeyQ)) dy -= 1;
        if (IsKeyPressed(KeyE)) dy += 1;

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

        const int CapsuleType = 3;
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
                float deltaX = diffX * inv;
                float deltaY = diffY * inv;
                float deltaZ = diffZ * inv;
                partPos.X += deltaX;
                partPos.Y += deltaY;
                partPos.Z += deltaZ;
                SetSDFPartPosition(i, partPos);

                if (GetSDFPartType(i) == CapsuleType)
                {
                    var endpoint = GetSDFPartEndpoint(i);
                    SetSDFPartEndpoint(i, new IberusVec3(
                        endpoint.X + deltaX,
                        endpoint.Y + deltaY,
                        endpoint.Z + deltaZ));
                }
            }

            lastX = pos.X + partPos.X;
            lastY = pos.Y + partPos.Y;
            lastZ = pos.Z + partPos.Z;
        }
    }
}
