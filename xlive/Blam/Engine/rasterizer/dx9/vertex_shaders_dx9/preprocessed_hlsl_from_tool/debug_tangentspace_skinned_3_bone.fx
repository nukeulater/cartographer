struct VS_OUTPUT
{
    float4 oT0 : TEXCOORD0;
    float4 oT1 : TEXCOORD1;
    float4 oT2 : TEXCOORD2;
    float4 oT3 : TEXCOORD3;
    float4 oPos : POSITION;
    float4 oT7 : TEXCOORD7;
};
//global vs 2.0 vertex shader header for asm conversions. (preceded by VS_INPUT/VS_OUTPUT struct)
//not used if #define HLSL is at the top of the .vsc file.

//#pragma ruledisable 0x02040101
//#pragma ruledisable 0x02040102
//#pragma ruledisable 0x02040103
//#pragma ruledisable 0x04010100
//#pragma ruledisable 0x04010101
//#pragma ruledisable 0x04010102
//#pragma ruledisable 0x04010103
//#pragma ruledisable 0x04010104
//

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
    float4 oPos = float4(1.0f, 1.0f, 1.0f, 1.0f);
#define r12 oPos
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
    


	



#define _POSITION	r0
#define _XFORM_X	r4 
#define _XFORM_Y	r5 
#define _XFORM_Z	r6 
{
    float4 temp = (float4)(va_node_indices * c[7].wwww  );
    	r0.x=temp.x;
    	r0.y=temp.y;
    	r0.z=temp.z;
    	r0.w=temp.w;
}
{
    float4 temp = (float4)(r0.zzzz );
    	a0.x=temp.x;
}
	r4  = va_node_weights.zzzz * c[a0.x + 50] ;
 	r5  = va_node_weights.zzzz * c[a0.x + 51] ;
 	r6  = va_node_weights.zzzz * c[a0.x + 52] ;
 {
    float4 temp = (float4)(r0.yyyy );
    	a0.x=temp.x;
}
	r4  = (va_node_weights.yyyy * c[a0.x + 50] ) + r4  ;
 	r5  = (va_node_weights.yyyy * c[a0.x + 51] ) + r5  ;
 	r6  = (va_node_weights.yyyy * c[a0.x + 52] ) + r6  ;
 {
    float4 temp = (float4)(r0.xxxx );
    	a0.x=temp.x;
}
	
{
    float4 temp = (float4)(-va_node_weights.zzzz + c[4].wwww  );
    	r9.x=temp.x;
}
{
    float4 temp = (float4)(-va_node_weights.yyyy + r9.xxxx );
    	r9.x=temp.x;
}
	r4  = (r9.xxxx * c[a0.x + 50] ) + r4  ;
 	r5  = (r9.xxxx * c[a0.x + 51] ) + r5  ;
 	r6  = (r9.xxxx * c[a0.x + 52] ) + r6  ;
 
{
    float4 temp = (float4)(va_position * c[170].xyzz );
    	r10.x=temp.x;
    	r10.y=temp.y;
    	r10.z=temp.z;
}
{
    float4 temp = (float4)(r10 + c[171].xyzz );
    	r10.x=temp.x;
    	r10.y=temp.y;
    	r10.z=temp.z;
}
{
    float4 temp = (float4)(dp3(r10 , _XFORM_X ));
    	r0.x=temp.x;
}
{
    float4 temp = (float4)(r0.xxxx + _XFORM_X.wwww );
    	r0.x=temp.x;
}
{
    float4 temp = (float4)(dp3(r10 , _XFORM_Y ));
    	r0.y=temp.y;
}
{
    float4 temp = (float4)(r0.yyyy + _XFORM_Y.wwww );
    	r0.y=temp.y;
}
{
    float4 temp = (float4)(dp3(r10 , _XFORM_Z ));
    	r0.z=temp.z;
}
{
    float4 temp = (float4)(r0.zzzz + _XFORM_Z.wwww );
    	r0.z=temp.z;
}

#define _NORMAL		r1
#define _BINORMAL	r2
#define _TANGENT	r3

	
{
    float4 temp = (float4)(dp3(va_tangent , _XFORM_X ));
    	_TANGENT.x=temp.x;
}
{
    float4 temp = (float4)(dp3(va_tangent , _XFORM_Y ));
    	_TANGENT.y=temp.y;
}
{
    float4 temp = (float4)(dp3(va_tangent , _XFORM_Z ));
    	_TANGENT.z=temp.z;
}
{
    float4 temp = (float4)(dp3(va_binormal , _XFORM_X ));
    	_BINORMAL.x=temp.x;
}
{
    float4 temp = (float4)(dp3(va_binormal , _XFORM_Y ));
    	_BINORMAL.y=temp.y;
}
{
    float4 temp = (float4)(dp3(va_binormal , _XFORM_Z ));
    	_BINORMAL.z=temp.z;
}
{
    float4 temp = (float4)(dp3(va_normal , _XFORM_X ));
    	_NORMAL.x=temp.x;
}
{
    float4 temp = (float4)(dp3(va_normal , _XFORM_Y ));
    	_NORMAL.y=temp.y;
}
{
    float4 temp = (float4)(dp3(va_normal , _XFORM_Z ));
    	_NORMAL.z=temp.z;
}


{
	float temptemp;
	temptemp = dp3(_POSITION , c[0] );
 	temptemp = temptemp + c[0].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.x=temp.x;
}
}
{
	float temptemp;
	temptemp = dp3(_POSITION , c[1] );
 	temptemp = temptemp + c[1].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.y=temp.y;
}
}
{
	float temptemp;
	temptemp = dp3(_POSITION , c[2] );
 	temptemp = temptemp + c[2].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.z=temp.z;
}
}
{
	float temptemp;
	temptemp = dp3(_POSITION , c[3] );
 	temptemp = temptemp + c[3].wwww ;
 {
    float4 temp = (float4)(temptemp );
    	oPos.w=temp.w;
}
}

{
    float4 temp = (float4)(_TANGENT );
    	r7.x=temp.x;
    	r7.y=temp.y;
    	r7.z=temp.z;
}
{
    float4 temp = (float4)(_BINORMAL );
    	r8.x=temp.x;
    	r8.y=temp.y;
    	r8.z=temp.z;
}
{
    float4 temp = (float4)(_NORMAL );
    	r9.x=temp.x;
    	r9.y=temp.y;
    	r9.z=temp.z;
}
{
    float4 temp = (float4)(dp3(r7 , r7 ));
    	r10.x=temp.x;
}
{
    float4 temp = (float4)(dp3(r8 , r8 ));
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(dp3(r9 , r9 ));
    	r10.z=temp.z;
}


{
    float4 temp = (float4)(r7 );
    	oT0.x=temp.x;
    	oT0.y=temp.y;
    	oT0.z=temp.z;
}
{
    float4 temp = (float4)(r8 );
    	oT1.x=temp.x;
    	oT1.y=temp.y;
    	oT1.z=temp.z;
}
{
    float4 temp = (float4)(r9 );
    	oT2.x=temp.x;
    	oT2.y=temp.y;
    	oT2.z=temp.z;
}
{
    float4 temp = (float4)(r10 );
    	oT3.x=temp.x;
    	oT3.y=temp.y;
    	oT3.z=temp.z;
}


{
    float4 temp = (float4)(rsqrt(r10.xxxx ));
    	r10.x=temp.x;
}
{
    float4 temp = (float4)(rsqrt(r10.yyyy ));
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(rsqrt(r10.zzzz ));
    	r10.z=temp.z;
}
{
    float4 temp = (float4)(r7 * r10.xxxx );
    	r7.x=temp.x;
    	r7.y=temp.y;
    	r7.z=temp.z;
}
{
    float4 temp = (float4)(r8 * r10.yyyy );
    	r8.x=temp.x;
    	r8.y=temp.y;
    	r8.z=temp.z;
}
{
    float4 temp = (float4)(r9 * r10.zzzz );
    	r9.x=temp.x;
    	r9.y=temp.y;
    	r9.z=temp.z;
}
{
    float4 temp = (float4)(dp3(r7 , r8 ));
    	r10.x=temp.x;
}
{
    float4 temp = (float4)(dp3(r8 , r9 ));
    	r10.y=temp.y;
}
{
    float4 temp = (float4)(dp3(r9 , r7 ));
    	r10.z=temp.z;
}
{
    float4 temp = (float4)((r10.xxxx * c[5].wwww  ) + c[5].wwww  );
    	oT0.w=temp.w;
}
{
    float4 temp = (float4)((r10.yyyy * c[5].wwww  ) + c[5].wwww  );
    	oT1.w=temp.w;
}
{
    float4 temp = (float4)((r10.zzzz * c[5].wwww  ) + c[5].wwww  );
    	oT2.w=temp.w;
}


output.oT0=oT0;
output.oT1=oT1;
output.oT2=oT2;
output.oT3=oT3;
output.oPos=oPos;
// cram anything additional here that needs to happen at the end of all vertex shaders.

	output.oT7.xyzw = oPos; //float4(oPos.z, oPos.w); 
	

	return output;
}
