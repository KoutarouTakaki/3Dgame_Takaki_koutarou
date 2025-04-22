

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �����p�o�b�t�@
cbuffer Fuchi : register(b6)
{
    float4		fuchiColor;
	int			fuchi;
    float2		fuchiTime;
	int			fill;
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// �f�B�]���u�p�o�b�t�@
cbuffer Dissolve : register(b8)
{
    float4 dissolveColor; // �f�B�]���u���̃G�t�F�N�g�F
    float DissolveTime;
    int DissolveFlug;
    float dissolveThreshold; // 0.0�`1.0�ŃG�t�F�N�g�̐i�s�
	
    float Dummy; //16byte���E�p
};

// �e�N�X�`���u�����h�p�o�b�t�@
cbuffer TexBlend : register(b9)
{
    int TexBlendFlug; // �t���O
    float2 TexBlendTime;
    float TexBlendRate; // �u�����h�̊���
};

Texture2D diffuseTexture : register(t0); // ���C���e�N�X�`��
Texture2D noiseTexture : register(t1); // �m�C�Y�e�N�X�`��

Texture2D baseTexture : register(t2); // �x�[�X�e�N�X�`��
Texture2D blendTexture : register(t3); // �u�����h�e�N�X�`��

SamplerState samplerState : register(s0);


//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}


	if (Light.Enable == 0)
	{
        color = color * Material.Diffuse;
    }
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	
	//�t�H�O
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

//�e�N�X�`���u�����h
    if (TexBlendFlug == 1)
    {
        float2 MoveUV = inTexCoord + TexBlendTime;

        float4 blendColor = blendTexture.Sample(g_SamplerState, MoveUV);

		//�x�[�X�J���[�ƃu�����h�J���[��������1.0f�ɂȂ�悤�Ƀu�����h����Ă���o�� -> �x�[�X�J���[ 0.4f :�u�����h�J���[ 0.6f�p
        //outDiffuse.rgb = color.rgb * (1.0f - TexBlendRate) + blendColor.rgb * TexBlendRate;
        //outDiffuse.a = color.a * (1.0f - TexBlendRate) + blendColor.a * TexBlendRate;
		
		//�x�[�X�J���[�Ɉ��̊����̃u�����h�J���[�𑫂��Ă���o�� -> �x�[�X�J���[ 1.0f :�u�����h�J���[ 0.6f�p
        outDiffuse.rgb = color.rgb + blendColor.rgb * TexBlendRate;
        outDiffuse.a = color.a - (blendColor.a * TexBlendRate) / 10;
    }

	//�����
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if (angle > -0.3f)
		if ((angle < 0.5f)&&(angle > -0.5f))
		{
            outDiffuse = fuchiColor;
        }
	}
	
	//�f�B�]���u
    if (DissolveFlug == 1)
    {
        float noiseValue = noiseTexture.Sample(g_SamplerState, inTexCoord).r;

        // �m�C�Y�l��臒l���r
        if (noiseValue < dissolveThreshold)
        {
            if (dissolveThreshold - noiseValue < 0.03f)
            {
				// �����鉏�̐F��ς���
                outDiffuse = dissolveColor;

            }
            else
            {
				// �f�B�]���u�G�t�F�N�g�F��K�p
                discard;
            }
        }
    }
	
}

