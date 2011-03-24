#!BPY

"""
Name: 'XMSH Exporter'
Blender: 249
Group: 'Export'
Tooltip: 'Toadlet XMSH exporter'
"""
import Blender
import bpy

def write(filename):
	if len(filename)<5 or filename[-5:-1]!='.xmsh':
		filename = filename.join('.xmsh')
	out = open(filename, "w")

	out.write('<XMSH Version="3">\n')

	scene = bpy.data.scenes.active
	# TODO: loop over selected objects instead of just using the active one
	objects = scene.objects.selected

	for ob in objects:
		if ob.type=='MESH': 
			mesh = ob.getData(mesh=True)
			out.write('\t<Mesh>\n')

			# Write out all vertexes in the mesh at once
			out.write('\t\t<Vertexes Count=\"%d\" ' % (len(mesh.verts)))
			# TODO: add TexCoord,Bone
			out.write('Type=\"Position,Normal\">\n') 
			for vert in mesh.verts:
				out.write('\t\t\t%f,%f,%f %f,%f,%f\n' % (vert.co.x,vert.co.y,vert.co.z,vert.no.x,vert.no.y,vert.no.z))
			out.write('\t\t</Vertexes>\n')

			# HACK: For now, we have one indexed submesh for the whole mesh.
			# Ultimately, with blender it appears materials are assigned by faces. So we would need
			# to loop through all of the faces that reference a material, and then form those faces into a submesh
			# which we can then assign materials to.
			out.write('\t\t<Submesh>\n')
			out.write('\t\t\t<Indexes Count=\"%d\">' % (len(mesh.verts)))
			for vert in mesh.verts:
				out.write('%f ' % (vert.index))
			out.write('</Indexes>\n')
			out.write('\t\t</Submesh>\n')
			out.write('</Mesh>\n')
	out.write('</XMSH>\n')
	out.close()

Blender.Window.FileSelector(write, "Export")

