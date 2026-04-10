from Pytolith import TagSystem
import os
import io
import re

# Set current working directory to the file's location
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

MY_H2EK_TAGS_FOLDER = r"..\..\..\..\..\..\..\tags"
VS_FOLDER = r"rasterizer\vertex_shaders_dx9"

# initialize new tag system using layouts packaged with Pytolith
system = TagSystem(MY_H2EK_TAGS_FOLDER)

dir_path = os.path.join(MY_H2EK_TAGS_FOLDER, VS_FOLDER)

# Get list of all files in directory
files = [f for f in os.listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f))]

# Make sure we only loop through vertex shaders
vertex_shaders = [f for f in files if ".vertex_shader" in f]

# names of each vertex shader type
types = ["world", "rigid", "rigid_boned", "skinned_1_bone", "skinned_2_bone", "skinned_3_bone", "skinned_4_bone", "screen", "parallel", "perpendicular", "vertical", "horizontal" ]

for f in vertex_shaders:
	path = os.path.join(dir_path, f)
	print(f)
	
	# load the vertex shader tag
	vertex_shader_tag = system.load_tag(os.path.join(VS_FOLDER, f))
	
	# loop through every vertex shader in the geometry classifications and dump the contents
	for i in range(len(vertex_shader_tag.fields.geometry_classifications_dx9.value.elements)):
		fxpath = path[path.rindex('\\') + 1:path.rindex('.')] + "_" + types[i] + ".fx"
		text_file = io.open(fxpath, 'w', newline='')
		text_file.write(vertex_shader_tag.fields.geometry_classifications_dx9.value.elements[i][2].value.decode("utf-8"))
		text_file.close()
		
		with open(fxpath, "rt") as fin:
			data = fin.read()
			data = re.sub(r"dp2\((.*?)\ , (.*?) \)", r"dot(\1.xy, \2.xy)", data)
			data = re.sub(r"dp3\((.*?)\ , (.*?) \)", r"dot(\1.xyz, \2.xyz)", data)
			data = re.sub(r"dp4\((.*?)\ , (.*?) \)", r"dot(\1.xyzw, \2.xyzw)", data)

		with open(fxpath, "wt") as fout:
			fout.write(data)



