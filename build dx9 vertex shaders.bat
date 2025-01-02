@echo off
:: Change Current Directory to the location of this batch file 
CD /D %~dp0
set vertex_shader_dir="./tags/rasterizer/vertex_shaders_dx9/"
GOTO:MAIN

:compile_shader
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: The part where we compile the shader bytecode
		.\bin\fxc.exe /O3 /Gis /nologo /T %~1 %~3 /Fo ./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/%~2_%~1.vso ./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/%~2.fx 2> nul
    ENDLOCAL
EXIT /B 0

:replace_content
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: Instead of "unsigned char" we want "static const unsigned char"
		:: Also, remove __compiled prefix
		:: Remove _vso suffix
		.\bin\w64devkit\bin\sed.exe -b -i -e "s/_vso\[/\[/g" -e "s/unsigned char __xlive_Blam_Engine_rasterizer_dx9_vertex_shaders_dx9_preprocessed_hlsl_from_tool_compiled_/static const unsigned char k_/g" %~1
	ENDLOCAL
EXIT /B 0

:add_shader_bytecode_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: Get a C array of file data for our compiled shader
		.\bin\w64devkit\bin\xxd.exe -i ./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/%~2_%~1.vso >> %~3
		:: Delete the last line in the file to remove length global
		.\bin\w64devkit\bin\sed.exe -b -i "$d" %~3						
	ENDLOCAL
EXIT /B 0

:add_shader
	SETLOCAL ENABLEDELAYEDEXPANSION
		call:compile_shader "vs_2_0" %~1 %~2
		call:compile_shader "vs_3_0" %~1 %~2
		
		set file="./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/%~1.h"
		
		echo #pragma once > %file%
		echo. >> %file%
		
		call:add_shader_bytecode_to_file "vs_2_0" %~1 %file%		
		echo. >> %file%
		call:add_shader_bytecode_to_file "vs_3_0" %~1 %file%
		set "file="
    ENDLOCAL
EXIT /B 0

:add_vertex_shaders
	SETLOCAL ENABLEDELAYEDEXPANSION
		for %%i in (./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/*.fx) do call::add_shader %%~ni
		for %%i in (./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/*.h) do call::replace_content ./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/%%i
    ENDLOCAL
EXIT /B 0

:cleanup
	SETLOCAL ENABLEDELAYEDEXPANSION
		del xlive\Blam\Engine\rasterizer\dx9\vertex_shaders_dx9\preprocessed_hlsl_from_tool\compiled\*.vso
    ENDLOCAL
EXIT /B 0

:add_includes_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		echo 	adding includes...
		for %%i in (./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/*.fx) do echo #include "rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/%%~ni.h" >> %~1
		echo. >> %file%
    ENDLOCAL
EXIT /B 0


:add_bytecode_reference_to_table:
	SETLOCAL ENABLEDELAYEDEXPANSION
		set hfile=".\xlive\Blam\Engine\rasterizer\dx9\vertex_shaders_dx9\preprocessed_hlsl_from_tool\compiled\%~2_%~4.h"
		:: if the file exists and we find static in the header then we know we can add this to the table
		:: if the file exists and we don't find static in the header we add NULL since this shader does not exist
		if exist %hfile% ( findstr static %hfile% > nul && ( echo 	^(DWORD^*^)k_%~2_%~4_%~3, >> %~1 ) || ( echo 	NULL, >> %~1 ) )
		set "hfile="
    ENDLOCAL
EXIT /B 0

:add_bytecode_size_reference_to_table:
	SETLOCAL ENABLEDELAYEDEXPANSION
		set hfile=".\xlive\Blam\Engine\rasterizer\dx9\vertex_shaders_dx9\preprocessed_hlsl_from_tool\compiled\%~2_%~4.h"
		:: if the file exists and we find static in the header then we know we can add this to the table
		:: if the file exists and we don't find static in the header we add 0 since this shader does not exist
		if exist %hfile% ( findstr static %hfile% > nul && ( echo 	sizeof^(k_%~2_%~4_%~3^), >> %~1 ) || ( echo 	0, >> %~1 ) )
		set "hfile="
    ENDLOCAL
EXIT /B 0

:add_bytecode_contents_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		:: initialize array for vertex shader classification types
		set items[0]=world
		set items[1]=rigid
		set items[2]=rigid_boned
		set items[3]=skinned_1_bone
		set items[4]=skinned_2_bone
		set items[5]=skinned_3_bone
		set items[6]=skinned_4_bone
		set items[7]=screen
		set items[8]=parallel
		set items[9]=perpendicular
		set items[10]=vertical
		set items[11]=horizontal
		
		:: bytecode references
		echo const DWORD *const k_%~2_bytecode_%~3[] >> %~1
		echo { >> %~1
		for /L %%i in (0,1,11) do ( call:add_bytecode_reference_to_table %file% %~2 %~3 !items[%%i]! )
		echo }; >> %~1
		echo. >> %file%
		
		:: size references
		echo const int16 k_%~2_bytecode_%~3_size[] >> %~1
		echo { >> %~1
		for /L %%i in (0,1,11) do ( call:add_bytecode_size_reference_to_table %file% %~2 %~3 !items[%%i]! )
		echo }; >> %~1
		echo. >> %file%
    ENDLOCAL
EXIT /B 0

:add_bytecode_table_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		call:add_bytecode_contents_to_file %~1 %~2 vs_2_0
		call:add_bytecode_contents_to_file %~1 %~2 vs_3_0		
		
		echo const DWORD *const *const k_%~2_bytecode[2] >> %~1
		echo { >> %~1
		echo 	k_%~2_bytecode_vs_2_0, >> %~1
		echo 	k_%~2_bytecode_vs_3_0 >> %~1
		echo }; >> %~1
		echo. >> %file%
		
		echo const int16 *const k_%~2_bytecode_size[2] >> %~1
		echo { >> %~1
		echo 	k_%~2_bytecode_vs_2_0_size, >> %~1
		echo 	k_%~2_bytecode_vs_3_0_size >> %~1
		echo }; >> %~1
		echo. >> %file%
    ENDLOCAL
EXIT /B 0

:add_master_table_to_file
	SETLOCAL ENABLEDELAYEDEXPANSION
		echo 	adding master table...
		echo const s_vertex_shader_replacement_information k_vertex_shader_replacement_information[111] >> %~1
		echo { >> %~1
	
:: Loop through every vertex shader and create the master table entry with the following:
:: vertex shader name
:: bytecode
:: bytecode size
		for %%i in (%vertex_shader_dir%*.vertex_shader) do ( ^
echo 	{ >> %~1 & ^
echo 		"%%~ni", >> %~1 & ^
echo 		k_%%~ni_bytecode, >> %~1 & ^
echo 		k_%%~ni_bytecode_size >> %~1 & ^
echo 	}, >> %~1 ^
)
		
		echo }; >> %~1
    ENDLOCAL
EXIT /B 0


:create_shader_replacement_table
	SETLOCAL ENABLEDELAYEDEXPANSION
		
		echo Building CPP file...
		
		:: Cpp
		set file="./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/_compiled_shader_table.cpp"

		echo #include "stdafx.h" > %file%
		echo #include "_compiled_shader_table.h" >> %file%
		echo. >> %file%

		:: Add includes for all vertex shaders
		call:add_includes_to_file %file%
				
		:: Add tables for bytecode and size
		echo 	adding tables...
		for %%i in (%vertex_shader_dir%*.vertex_shader) do call:add_bytecode_table_to_file %file% %%~ni

		call:add_master_table_to_file %file%
		
		echo Building Header file...
		
		:: Header
		set file="./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/_compiled_shader_table.h"

		:: Add structure
		echo #pragma once > %file%
		echo. >> %file%
		echo /* structures */ >> %file%
		echo. >> %file%
		echo struct s_vertex_shader_replacement_information >> %file%
		echo { >> %file%
		echo 	const char* tag_name; >> %file%
		echo 	const DWORD *const *const *const shader_bytecode; >> %file%
		echo 	const int16 *const *const size; >> %file%
		echo }; >> %file%
		echo. >> %file%
		
		:: add constants
		echo /* constants */ >> %file%
		echo. >> %file%
		echo extern const s_vertex_shader_replacement_information k_vertex_shader_replacement_information[111]; >> %file%
		
		set "file="
    ENDLOCAL
EXIT /B 0

:dump_vertex_shaders
	SETLOCAL ENABLEDELAYEDEXPANSION
		echo Running Python script to dump vertex shaders from tags
		python "./xlive/Blam/Engine/rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/_vertex_shader_dump.py"
    ENDLOCAL
EXIT /B 0

:MAIN
cd 

call::dump_vertex_shaders
call::add_vertex_shaders
call::cleanup
call::create_shader_replacement_table

pause
