import bpy
import struct
import bmesh
import math
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:]  # get all args after "--"

print(argv)  # --> ['example', 'args', '123']

filepath = argv[0]

print("Running nvm export...")

with open(filepath, 'w') as f:
	# magic and version information
	f.write("omp 1\n")
			
	obj = bpy.data.objects['NavMesh']
	obj.select_set(True)
	obj.hide_set(False)

	verts = []
	indices = []
	
	me = obj.data
	me.calc_loop_triangles()
	
	for tri in me.loop_triangles:
		for i in range(3):
			vert_index = tri.vertices[i]
			loop_index = tri.loops[i]

			pos = me.vertices[vert_index].co
			
			foundOne = False
			vertI = 0
			while vertI < len(verts):
				if verts[vertI] == (pos):
					foundOne = True
					indices.append(vertI)
					break
				vertI += 1
			
			if foundOne == False:
				# normals
				# verts.append((pos, norm, uv))
				verts.append(pos)
				indices.append(len(verts) - 1)
	for vert in verts:
		f.write("v " + str(vert.x) +  " " + str(vert.z) + " " + str(-vert.y) + "\n")
	x = 0
	while x < len(indices):
		f.write("f ")
		f.write(str(indices[x]) + " ")
		x += 1
		f.write(str(indices[x + 1]) + " ")
		x += 1
		f.write(str(indices[x - 1]) + "\n")
		x += 1
print("Done!")
