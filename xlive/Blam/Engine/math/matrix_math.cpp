#include "stdafx.h"
#include "matrix_math.h"

real_matrix3x3* matrix3x3_from_forward_and_up(real_matrix3x3* matrix, const real_vector3d* forward, const real_vector3d* up)
{
	matrix->forward = *forward;
	cross_product3d(up, forward, &matrix->left);
	matrix->up = *up;
	return matrix;
}

real_matrix3x3* matrix3x3_rotation_from_quaternion(real_matrix3x3* matrix, const real_quaternion* quaternion)
{
	const real32 dot_product = dot_product4d_quaternion(quaternion, quaternion);

	const real32 scalar = (dot_product > k_real_epsilon ? 2.0f / dot_product : 0.0f);

	real_vector3d scaled_vector;
	scale_vector3d(&quaternion->v, scalar, &scaled_vector);

	real_vector3d w_scaled_vector;
	scale_vector3d(&scaled_vector, quaternion->w, &w_scaled_vector);

	real_vector3d i_scaled_vector;
	scale_vector3d(&scaled_vector, quaternion->v.i, &i_scaled_vector);

	real32 j_scaled_vector_j = quaternion->v.j * scaled_vector.j;
	real32 j_scaled_vector_k = quaternion->v.j * scaled_vector.k;
	real32 k_scaled_vector_k = quaternion->v.k * scaled_vector.k;

	matrix->forward.i = 1.0f - (k_scaled_vector_k + j_scaled_vector_j);
	matrix->left.i = i_scaled_vector.j - w_scaled_vector.k;
	matrix->up.i = i_scaled_vector.k + w_scaled_vector.j;
	matrix->forward.j = i_scaled_vector.j + w_scaled_vector.k;
	matrix->left.j = 1.0f - (k_scaled_vector_k + i_scaled_vector.i);
	matrix->up.j = j_scaled_vector_k - w_scaled_vector.i;
	matrix->forward.k = i_scaled_vector.k - w_scaled_vector.j;
	matrix->left.k = j_scaled_vector_k + w_scaled_vector.i;
	matrix->up.k = 1.0f - (j_scaled_vector_j + i_scaled_vector.i);
	return matrix;
}

real_quaternion* matrix3x3_rotation_to_quaternion(const real_matrix3x3* matrix, real_quaternion* quaternion)
{
	real32 v1 = matrix->forward.i + matrix->left.j + matrix->up.k;
	if (v1 <= 0.0f)
	{
		uint32 i = matrix->forward.i < matrix->left.j;
		if (matrix->v.n[4 * i] < matrix->up.k)
		{
			i = 2;
		}

		int32 i_1 = (i + 1) % 3;
		int32 i_2 = (i + 2) % 3;
		real32 forward_result = matrix->v.n[4 * i] - matrix->v.n[4 * i_1] - matrix->v.n[4 * i_2] + 1.0f;
		real32 forward_result_sqroot = square_root(forward_result);

		quaternion->v.n[i] = forward_result_sqroot * 0.5f;

		// Make sure value set is greater than epsilon
		ASSERT(quaternion->v.n[i] > k_real_epsilon);

		real32 scalar = 0.25f / quaternion->v.n[i];

		quaternion->w = (matrix->matrix[i_1][i_2] - matrix->matrix[i_2][i_1]) * scalar;
		quaternion->v.n[i_1] = (matrix->matrix[i][i_1] + matrix->matrix[i_1][i]) * scalar;
		quaternion->v.n[i_2] = (matrix->matrix[i][i_2] + matrix->matrix[i_2][i]) * scalar;
	}
	else
	{
		real32 v1_root = square_root(v1 + 1.0f);
		quaternion->w = v1_root * 0.5f;
		
		// Make sure w is greater than epsilon
		ASSERT(quaternion->w > k_real_epsilon);

		real32 scalar = 0.25f / quaternion->w;
		quaternion->v.i = (matrix->left.k - matrix->up.j) * scalar;
		quaternion->v.j = (matrix->up.i - matrix->forward.k) * scalar;
		quaternion->v.k = (matrix->forward.j - matrix->left.i) * scalar;
	}

	if (quaternion->w < 0.0f)
	{
		quaternion->v.i = -quaternion->v.i;
		quaternion->v.j = -quaternion->v.j;
		quaternion->v.k = -quaternion->v.k;
		quaternion->w = -quaternion->w;
	}
	return quaternion;
}


void matrix4x3_from_orientation(real_matrix4x3* matrix, const real_orientation* orientation)
{
	matrix4x3_rotation_from_quaternion(matrix, &orientation->rotation);
	matrix->position = orientation->translation;
	matrix->scale = orientation->scale;
	return;
}

void matrix4x3_rotation_from_quaternion(real_matrix4x3* matrix, const real_quaternion* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	matrix->scale = 1.0f;
	return;
}

real_quaternion* matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out)
{
	return matrix3x3_rotation_to_quaternion(&matrix->vectors, out);
}

void matrix4x3_rotation_from_vectors(real_matrix4x3* matrix, const real_vector3d* forward, const real_vector3d* up)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *forward;
	cross_product3d(up, forward, &matrix->vectors.left);
	matrix->vectors.up = *up;
	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	return;
}

void matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up)
{
	matrix4x3_rotation_from_vectors(matrix, forward, up);
	matrix->position = *position;
	return;
}

void matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* output)
{
	if (input->scale != 0.0f)
	{
		real_point3d inverse_pos = { -input->position.x, -input->position.y, -input->position.z };
		if (input->scale == 1.f)
		{
			output->scale = 1.f;
		}
		else
		{
			output->scale = 1.f / input->scale;
			inverse_pos.x *= output->scale;
			inverse_pos.y *= output->scale;
			inverse_pos.z *= output->scale;
		}

		output->vectors.forward.i = input->vectors.forward.i;
		output->vectors.left.j = input->vectors.left.j;
		output->vectors.up.k = input->vectors.up.k;

		output->vectors.left.i = input->vectors.forward.j;
		output->vectors.forward.j = input->vectors.left.i;
		
		output->vectors.up.i = input->vectors.forward.k;
		output->vectors.forward.k = input->vectors.up.i;

		output->vectors.up.j = input->vectors.left.k;
		output->vectors.left.k = input->vectors.up.j;

		output->position.x = output->vectors.forward.i * inverse_pos.x + output->vectors.left.i * inverse_pos.y + output->vectors.up.i * inverse_pos.z;
		output->position.y = output->vectors.forward.j * inverse_pos.x + output->vectors.left.j * inverse_pos.y + output->vectors.up.j * inverse_pos.z;
		output->position.z = output->vectors.forward.k * inverse_pos.x + output->vectors.left.k * inverse_pos.y + output->vectors.up.k * inverse_pos.z;
	}
	else
	{
		csmemset(output, 0, sizeof(real_matrix4x3));
	}

	return;
}

void matrix4x3_inverse_transform_normal(real_matrix4x3* input, real_vector3d* input_vector, real_vector3d* out_vector)
{
	INVOKE(0x77D35, 0, matrix4x3_inverse_transform_normal, input, input_vector, out_vector);
}

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out)
{
	INVOKE(0x78209, 0x75DFF, matrix4x3_multiply, matrix1, matrix2, out);
	return;
}

void matrix4x3_translation(real_matrix4x3* matrix, const real_point3d* position)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *global_forward3d;
	matrix->vectors.left = *global_left3d;
	matrix->vectors.up = *global_up3d;
	matrix->position = *position;
	return;
}

real_matrix4x3* matrix4x3_identity(real_matrix4x3* matrix)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *global_forward3d;
	matrix->vectors.left = *global_left3d;
	matrix->vectors.up = *global_up3d;
	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	return matrix;
}

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, const real_point3d* point, const real_quaternion* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	matrix->scale = 1.0f;
	matrix->position = *point;
	return;
}

void matrix4x3_to_point_and_vectors(real_matrix4x3* matrix, real_point3d* position, real_vector3d* forward, real_vector3d* up)
{
	*forward = matrix->vectors.forward;
	*up = matrix->vectors.up;
	*position = matrix->position;
	return;
}

real_point3d* matrix4x3_transform_point(const real_matrix4x3* matrix, const real_point3d* point, real_point3d* result)
{
	real_point3d scaled_point = *point; 
	if (matrix->scale != 1.0f)
	{
		scaled_point.x *= matrix->scale;
		scaled_point.y *= matrix->scale;
		scaled_point.z *= matrix->scale;
	}

	result->x = (((matrix->vectors.up.i * scaled_point.z) + (matrix->vectors.left.i * scaled_point.y)) + (matrix->vectors.forward.i * scaled_point.x)) + matrix->position.x;
	result->y = (((matrix->vectors.up.j * scaled_point.z) + (matrix->vectors.left.j * scaled_point.y)) + (matrix->vectors.forward.j * scaled_point.x)) + matrix->position.y;
	result->z = (((matrix->vectors.up.k * scaled_point.z) + (matrix->vectors.left.k * scaled_point.y)) + (matrix->vectors.forward.k * scaled_point.x)) + matrix->position.z;
	return result;
}

real_vector3d* matrix4x3_transform_vector(const real_matrix4x3* matrix, const real_vector3d *vector, real_vector3d* result)
{
	real_vector3d scaled_vector = *vector;
	if (matrix->scale != 1.0f)
	{
		scale_vector3d(&scaled_vector, matrix->scale, &scaled_vector);
	}

	result->i = (((matrix->vectors.up.i * scaled_vector.k) + (matrix->vectors.left.i * scaled_vector.j)) + (matrix->vectors.forward.i * scaled_vector.i)) + matrix->position.x;
	result->j = (((matrix->vectors.up.j * scaled_vector.k) + (matrix->vectors.left.j * scaled_vector.j)) + (matrix->vectors.forward.j * scaled_vector.i)) + matrix->position.y;
	result->k = (((matrix->vectors.up.k * scaled_vector.k) + (matrix->vectors.left.k * scaled_vector.j)) + (matrix->vectors.forward.k * scaled_vector.i)) + matrix->position.z;
	return result;
}

real_matrix4x3* matrix4x3_rotation_from_angles(real_matrix4x3* matrix, real32 i, real32 j, real32 k)
{
	const real_vector3d cosine_vector { cos(i), cos(j), cos(k) };
	const real_vector3d sine_vector{ sin(i), sin(j), sin(k) };

	const real32 j_cos_k = sine_vector.j * cosine_vector.k;
	const real32 sin_jk = sine_vector.j * sine_vector.k;

	matrix->scale = 1.0f;
	matrix->vectors.forward.i = cosine_vector.i * cosine_vector.j;
	matrix->vectors.forward.j = sine_vector.i * cosine_vector.k - sin_jk * cosine_vector.i;
	matrix->vectors.forward.k = j_cos_k * cosine_vector.i + sine_vector.i * sine_vector.k;
	
	matrix->vectors.left.i = -cosine_vector.j * sine_vector.i;
	matrix->vectors.left.j = sin_jk * sine_vector.i + cosine_vector.i * cosine_vector.k;
	matrix->vectors.left.k = cosine_vector.i * sine_vector.k - sine_vector.i * j_cos_k;
	
	matrix->vectors.up.i = -sine_vector.j;
	matrix->vectors.up.j = -sine_vector.k * cosine_vector.j;
	matrix->vectors.up.k = cosine_vector.j * cosine_vector.k;

	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	return matrix;
}

void __cdecl matrix4x3_rotation_from_axis_and_angle(real_matrix4x3* matrix, real_vector3d* vector, real32 axis, real32 angle)
{
	return INVOKE(0x775C1, 0x751B7, matrix4x3_rotation_from_axis_and_angle, matrix, vector, axis, angle);
}

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, real32 fractional_ticks, real_matrix4x3* out_mat)
{
	real_quaternion q1_previous, q2_target, q3_interpolated;
	// first we convert matrix to quaternion rotation
	matrix4x3_rotation_to_quaternion(previous, &q1_previous);
	matrix4x3_rotation_to_quaternion(target, &q2_target);
	// interpolate the rotation quaternion
	fast_quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
	// and convert back to matrix format, since that's what the game uses
	matrix4x3_rotation_from_quaternion(out_mat, &q3_interpolated);
	// interpolate scale and the position of the node
	scale_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
	points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
	return;
}

void __cdecl matrix3x3_multiply(real_matrix3x3* input_matrix_1, real_matrix3x3* input_matrix_2, real_matrix3x3* out_matrix)
{
	INVOKE(0x78645, 0x7623B, matrix3x3_multiply, input_matrix_1, input_matrix_2, out_matrix);
}

void matrix3x3_transform_vector(const real_matrix3x3* input_matrix, real_vector3d* input_vector, real_vector3d* out_vector)
{
	INVOKE(0x78800, 0x763F6, matrix3x3_transform_vector, input_matrix, input_vector, out_vector);
}

real_matrix3x3* matrix3x3_from_angles(real_matrix3x3* matrix, real32 i, real32 j, real32 k)
{
	const real_vector3d cosine_vector{ cos(i), cos(j), cos(k) };
	const real_vector3d sine_vector{ sin(i), sin(j), sin(k) };


	real32 ci_ck = cosine_vector.i * cosine_vector.k;
	real32 ci_sk = cosine_vector.i * sine_vector.k;

	real32 si_ck = sine_vector.i * cosine_vector.k;
	real32 si_sk = sine_vector.i * sine_vector.k;

	matrix->forward.i = cosine_vector.i * cosine_vector.j;
	matrix->forward.j = cosine_vector.j * sine_vector.i;
	matrix->forward.k = sine_vector.j;
	matrix->left.i = -(ci_sk * cosine_vector.j) - si_ck;
	matrix->left.j = ci_ck - (si_sk * cosine_vector.j);
	matrix->left.k = cosine_vector.j * sine_vector.k;
	matrix->up.i = si_sk - (ci_ck * cosine_vector.j);
	matrix->up.j = -(si_ck * cosine_vector.j) - ci_sk;
	matrix->up.k = cosine_vector.j * cosine_vector.k;
	return matrix;
}

real_vector3d* matrix4x3_transform_normal(const real_matrix4x3* matrix, const real_vector3d* normal, real_vector3d* result)
{
	const real_vector3d copy = *normal;
	result->i = ((copy.i * matrix->vectors.forward.i) + (copy.j * matrix->vectors.left.i)) + (copy.k * matrix->vectors.up.i);
	result->j = ((copy.i * matrix->vectors.forward.j) + (copy.j * matrix->vectors.left.j)) + (copy.k * matrix->vectors.up.j);
	result->k = ((copy.i * matrix->vectors.forward.k) + (copy.j * matrix->vectors.left.k)) + (copy.k * matrix->vectors.up.k);
	return result;
}

real_plane3d* matrix4x3_transform_plane(const real_matrix4x3* matrix, const real_plane3d* plane, real_plane3d* result)
{
	matrix4x3_transform_normal(matrix, &plane->n, &result->n);
	const real32 scale_product = matrix->scale * plane->d;
	result->d = scale_product + dot_product3d((real_vector3d*)&matrix->position, &result->n);
	return result;
}
