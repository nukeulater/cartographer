#pragma once 
 
/* structures */ 
 
struct s_vertex_shader_replacement_information 
{ 
	const char* tag_name; 
	const DWORD *const *const *const shader_bytecode; 
	const int16 *const *const size; 
}; 
 
/* constants */ 
 
extern const s_vertex_shader_replacement_information k_vertex_shader_replacement_information[111]; 
