import bpy

mesh_names = []

for object in bpy.data.objects:
	if object.type == 'MESH':
		mesh_names.append(object.data.name)

for name in mesh_names:
	mesh = bpy.data.meshes[name]
	for vertex in mesh.vertices:
		print(vertex.co.x)
