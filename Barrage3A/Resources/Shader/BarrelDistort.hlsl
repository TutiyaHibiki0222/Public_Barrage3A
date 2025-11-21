struct PS_INPUT
{
    float4 DiffuseColor : COLOR0;
    float4 SpecularColor : COLOR1;
    float2 TextureCoord0 : TEXCOORD0;
    float2 TextureCoord1 : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 Output : COLOR0;
};

sampler Texture : register(s0);

float BarrelPower : register(c0);
float t : register(c1);
float mousex : register(c2);
float mousey : register(c3);
float HeatStrength : register(c4);
float Time : register(c5);

float HorizontalBumpStrength = 0.1; // 横方向の凹凸の強さ

float aspectRatio : register(c6);

float SmoothRandom(float theta)
{
    return
        sin(theta * 10.0 + Time * 3.0) * 0.05 +
        sin(theta * 15.0 + Time * 5.0 + 1.0) * 0.03 +
        sin(theta * 25.0 + Time * 7.0 + 2.0) * 0.015;
}

// 凹凸の強さ自体も動的に変える関数
float BumpStrengthDynamic(float minStrength, float amplitude)
{
    // sinの値を0〜1に正規化して、minStrength〜minStrength+amplitudeの範囲で揺らぐ
    return minStrength + amplitude * (sin(Time * 2.0) * 0.5 + 0.5);
}


PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;

    float2 uv = PSInput.TextureCoord0.xy;
    float2 xy = float2(2.0 * uv.x - 1.0, (2.0 * uv.y - 1.0) * aspectRatio);

    float2 mouse = 2.0 * float2(mousex, mousey) - 1.0;
    mouse.y *= aspectRatio;

    float2 xy2 = t * (xy - mouse);
    float dist = length(xy2);

    float effectStrength = saturate(1.2 - dist);

    float theta = atan2(xy2.y, xy2.x);

    float bumpStrength = BumpStrengthDynamic(0.35,0.65);

    float radiusDisturb = SmoothRandom(theta) * effectStrength * bumpStrength;

    float baseRadius = 1.0 + radiusDisturb;

    float2 CalcPos;
    float4 TextureColor;

    if (dist >= baseRadius)
    {
        CalcPos = uv;
        TextureColor = tex2D(Texture, CalcPos);
    }
    else
    {
        float centerExpand = pow(1.0 - dist / baseRadius, 2.0) * 0.3;
        float dist2 = pow(dist / baseRadius, BarrelPower) - centerExpand;
        dist2 = max(dist2, 0.0);

        CalcPos.x = dist2 * baseRadius * cos(theta);
        CalcPos.y = dist2 * baseRadius * sin(theta);
        CalcPos *= 1.0 / t;
        CalcPos += mouse;

        CalcPos.y /= aspectRatio;

        float2 heatOffset;
        heatOffset.x = sin(uv.y * 40.0 + Time * 5.0) * 0.005 * HeatStrength;
        heatOffset.y = cos(uv.x * 60.0 + Time * 7.0) * 0.003 * HeatStrength;
        CalcPos += heatOffset;

        CalcPos = 0.5 * (CalcPos + 1.0);
        CalcPos = clamp(CalcPos, 0.0, 1.0);

        TextureColor = tex2D(Texture, CalcPos);
        TextureColor.rgb -= clamp(0.3 * (1.0 - dist / baseRadius), 0.0, 0.5);
    }

    PSOutput.Output = TextureColor;
    return PSOutput;
}
