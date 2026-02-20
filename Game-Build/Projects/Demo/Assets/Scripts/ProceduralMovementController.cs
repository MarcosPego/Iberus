using IberusScripts;

namespace Demo;

/// <summary>
/// Procedural animation movement: WASD moves the head only.
/// SDF parts form a chain that follows behind with tension-based fixed push.
/// </summary>
public class ProceduralMovementController : Script
{
    private const float HeadMoveSpeed = 0.06f;
    private const float MaxTension = 2.0f;
    private const float MinPush = 0.08f;

    // KeyCode values (aligned with engine)
    private const int KeyW = 87;
    private const int KeyS = 83;
    private const int KeyA = 65;
    private const int KeyD = 68;

    private float[] partWorldX = [];
    private float[] partWorldY = [];
    private float[] partWorldZ = [];
    private float lastLayoutSignature;

    protected override void OnInit()
    {
    }

    protected override void OnEntityChanged()
    {
        partWorldX = [];
        partWorldY = [];
        partWorldZ = [];
        lastLayoutSignature = 0;
    }

    protected override void OnUpdate(double deltaTime)
    {
        var (pos, rot, scale) = GetTransform();

        // WASD moves only the head (XZ plane)
        float dx = 0;
        float dz = 0;
        if (IsKeyPressed(KeyW)) dz += 1;
        if (IsKeyPressed(KeyS)) dz -= 1;
        if (IsKeyPressed(KeyA)) dx += 1;
        if (IsKeyPressed(KeyD)) dx -= 1;

        float len = MathF.Sqrt(dx * dx + dz * dz);
        if (len > 0)
        {
            float inv = HeadMoveSpeed / len;
            pos.X += dx * inv;
            pos.Z += dz * inv;
            SetTransform(pos, rot, scale);
        }

        if (!HasSDF())
        {
            return;
        }

        int partCount = GetSDFPartCount();
        if (partCount == 0)
        {
            return;
        }

        // Layout signature: detect creature swap even with same part count
        float layoutSig = 0;
        for (int k = 0; k < partCount; k++)
        {
            var p = GetSDFPartPosition(k);
            layoutSig += p.X + p.Y + p.Z + GetSDFPartRadius(k);
        }
        bool needReinit = partWorldX.Length != partCount || MathF.Abs(layoutSig - lastLayoutSignature) > 0.001f;
        if (needReinit)
        {
            lastLayoutSignature = layoutSig;
            partWorldX = new float[partCount];
            partWorldY = new float[partCount];
            partWorldZ = new float[partCount];
            for (int i = 0; i < partCount; i++)
            {
                var partPos = GetSDFPartPosition(i);
                partWorldX[i] = pos.X + partPos.X;
                partWorldY[i] = pos.Y + partPos.Y;
                partWorldZ[i] = pos.Z + partPos.Z;
            }
        }

        const int CapsuleType = 3;

        // Chain runs head-first (creature creator puts head at high indices): process partCount-1 down to 0
        float lastX = pos.X;
        float lastY = pos.Y;
        float lastZ = pos.Z;
        float lastRadius = 0.5f;
        if (partCount > 0)
        {
            int firstIdx = partCount - 1;
            if (GetSDFPartType(firstIdx) == CapsuleType)
            {
                var p0 = GetSDFPartPosition(firstIdx);
                var e0 = GetSDFPartEndpoint(firstIdx);
                float segX = e0.X - p0.X;
                float segY = e0.Y - p0.Y;
                float segZ = e0.Z - p0.Z;
                lastRadius = MathF.Sqrt(segX * segX + segY * segY + segZ * segZ);
            }
            else
            {
                lastRadius = GetSDFPartRadius(firstIdx);
            }
        }

        for (int rev = 0; rev < partCount; rev++)
        {
            int i = partCount - 1 - rev;
            float partRadius = GetSDFPartRadius(i);
            var oldPartPos = GetSDFPartPosition(i);

            float capsuleLen = 0;
            if (GetSDFPartType(i) == CapsuleType)
            {
                var ep = GetSDFPartEndpoint(i);
                float sx = ep.X - oldPartPos.X;
                float sy = ep.Y - oldPartPos.Y;
                float sz = ep.Z - oldPartPos.Z;
                capsuleLen = MathF.Sqrt(sx * sx + sy * sy + sz * sz);
            }

            float maxDist = MathF.Max(lastRadius + partRadius, capsuleLen);

            float diffX = lastX - partWorldX[i];
            float diffY = lastY - partWorldY[i];
            float diffZ = lastZ - partWorldZ[i];
            float dist = MathF.Sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);

            float threshold = MathF.Min(MaxTension, maxDist);
            if (dist > threshold)
            {
                float inv = MinPush / dist;
                partWorldX[i] += diffX * inv;
                partWorldY[i] += diffY * inv;
                partWorldZ[i] += diffZ * inv;
            }

            float leaderX = lastX;
            float leaderY = lastY;
            float leaderZ = lastZ;
            lastX = partWorldX[i];
            lastY = partWorldY[i];
            lastZ = partWorldZ[i];
            lastRadius = capsuleLen > 0.0001f ? capsuleLen : partRadius;

            var newPartPos = new IberusVec3(
                partWorldX[i] - pos.X,
                partWorldY[i] - pos.Y,
                partWorldZ[i] - pos.Z);
            SetSDFPartPosition(i, newPartPos);

            // Capsule: point End toward leader, preserve segment length so chain follows trail
            if (GetSDFPartType(i) == CapsuleType && capsuleLen > 0.0001f)
            {
                float dirX = leaderX - partWorldX[i];
                float dirY = leaderY - partWorldY[i];
                float dirZ = leaderZ - partWorldZ[i];
                float dirLen = MathF.Sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
                if (dirLen > 0.0001f)
                {
                    float dirScale = capsuleLen / dirLen;
                    SetSDFPartEndpoint(i, new IberusVec3(
                        newPartPos.X + dirX * dirScale,
                        newPartPos.Y + dirY * dirScale,
                        newPartPos.Z + dirZ * dirScale));
                }
            }
        }
    }
}
