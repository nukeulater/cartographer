struct VS_OUTPUT
{
    float4 oD0 : COLOR0;
    float4 oT0 : TEXCOORD0;
    float4 oT1 : TEXCOORD1;
    float4 oT2 : TEXCOORD2;
    float4 oPos : POSITION;
    float4 oT7 : TEXCOORD7;
};
//global vs 2.0 vertex shader header for asm conversions. (preceded by VS_INPUT/VS_OUTPUT struct)
//not used if #define HLSL is at the top of the .vsc file.

float4 c[96*2+16] : register(c0);  // grab all the xbox specific constant register space

void hsge(out float4 dest, float4 src0, float4 src1)
{
	dest.x = (src0.x >= src1.x) ? 1.0f : 0.0f;
	dest.y = (src0.y >= src1.y) ? 1.0f : 0.0f;
	dest.z = (src0.z >= src1.z) ? 1.0f : 0.0f;
	dest.w = (src0.w >= src1.w) ? 1.0f : 0.0f;
}

void hslt(out float4 dest, float4 src0, float4 src1)
{
	dest.x = (src0.x < src1.x) ? 1.0f : 0.0f;
	dest.y = (src0.y < src1.y) ? 1.0f : 0.0f;
	dest.z = (src0.z < src1.z) ? 1.0f : 0.0f;
	dest.w = (src0.w < src1.w) ? 1.0f : 0.0f;
}

void hsincos(out float2 dest, in float angle)
{
	sincos(angle, dest.y, dest.x);
}

float dp2(float4 a, float4 b)
{
	return a.x*b.x+a.y*b.y;
}

float dp3(float4 a, float4 b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

float dp4(float4 a, float4 b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
}

VS_OUTPUT main(//input registers
	float4 va_position: POSITION0,
	float4 va_node_indices: BLENDINDICES0,
	float4 va_node_weights: BLENDWEIGHT0,
	float4 va_texcoord: TEXCOORD0,
	float4 va_normal: NORMAL0,
	float4 va_binormal: BINORMAL0,
	float4 va_tangent: TANGENT0,
	float4 va_anisotropic_binormal: BINORMAL1,
	float4 va_incident_radiosity: COLOR1,
	float4 va_secondary_texcoord: TEXCOORD1,
	float4 va_isq_secondary_position: POSITION1,
	float4 va_isq_secondary_node_indices: BLENDINDICES1,
	float4 va_isq_secondary_node_weights: BLENDWEIGHT1,
	float4 va_isq_select: TEXCOORD2,
	float4 va_color: COLOR0,
	float4 va_tint_factor: COLOR2,
	float4 va_dsq_plane: TEXCOORD3,
	float4 va_billboard_offset: TEXCOORD4,
	float4 va_billboard_axis: TEXCOORD5,
	float4 va_pca_cluster_id: TEXCOORD6,
	float4 va_pca_weights: TEXCOORD7) 
{
	VS_OUTPUT output;

//temps    
    float4 r0 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r1 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r2 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r3 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r4 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r5 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r6 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r7 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r8 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r9 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r10 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 r11 = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 r12 = float4(1.0f, 1.0f, 1.0f, 1.0f);
	int4 a0;  //only use .x
	
//output registers
    float4 oT0 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT1 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT2 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT3 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT4 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT5 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT6 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oT7 = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float oFog = 1.0f;
	float oPts = 1.0f;
    
    float4 oD0 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 oD1 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
	float4 oPos = float4(1.0f, 1.0f, 1.0f, 1.0f);


	




















{
	float temptemp;
	temptemp = dot(va_position.xyz, c[177].xyz);
 	temptemp = temptemp + c[177].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.x=temp.x;
}
}
{
	float temptemp;
	temptemp = dot(va_position.xyz, c[178].xyz);
 	temptemp = temptemp + c[178].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.y=temp.y;
}
}
{
	float temptemp;
	temptemp = dot(va_position.xyz, c[179].xyz);
 	temptemp = temptemp + c[179].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.z=temp.z;
}
}
{
	float temptemp;
	temptemp = dot(va_position.xyz, c[180].xyz);
 	temptemp = temptemp + c[180].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.w=temp.w;
}
}


{
    float4 temp = (float4)(va_color );
    	oD0.x=temp.x;
    	oD0.y=temp.y;
    	oD0.z=temp.z;
    	oD0.w=temp.w;
}


{
    float4 temp = (float4)(c[183].xxxx * va_position.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)((va_texcoord * c[183].yyyy ) + r10.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[186].xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy + c[184].zwww );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[181].xyyy );
    	oT0.x=temp.x;
    	oT0.y=temp.y;
}


{
    float4 temp = (float4)(c[183].zzzz * va_position.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)((va_texcoord * c[183].wwww ) + r10.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[186].zwww );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy + c[185].xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[182].xyyy );
    	oT1.x=temp.x;
    	oT1.y=temp.y;
}


{
    float4 temp = (float4)(c[184].xxxx * va_position.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)((va_texcoord * c[184].yyyy ) + r10.xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[187].xyyy );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy + c[185].zwww );
    	r10.x=temp.x;
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(r10.xyyy * c[182].zwww );
    	oT2.x=temp.x;
    	oT2.y=temp.y;
}


output.oD0=oD0;
output.oT0=oT0;
output.oT1=oT1;
output.oT2=oT2;
output.oPos=oPos;
// cram anything additional here that needs to happen at the end of all vertex shaders.

	output.oT7.xyzw = oPos; //float4(oPos.z, oPos.w); 
	

	return output;
}
