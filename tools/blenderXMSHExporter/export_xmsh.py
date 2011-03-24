#!BPY

"""
Name: 'XMSH Exporter'
Blender: 249
Group: 'Export'
Tooltip: 'Toadlet XMSH exporter'
"""

__author__ = "Andrew Fischer"
__url__ = ['http://code.google.com/p/toadlet','www.blender.org']
__version__ = "1.0"

__bpydoc__ = """\
This script exports a toadlet XMSH file.

Usage:

Select the objects you wish to export and run this script fromt he "File->Export" menu.
"""

#********** Copyright header - do not remove **********
#*
#* The Toadlet Engine
#*
#* Copyright 2009, Lightning Toads Productions, LLC
#*
#* Author(s): Alan Fischer, Andrew Fischer
#*
#* This file is part of The Toadlet Engine.
#*
#* The Toadlet Engine is free software: you can redistribute it and/or modify
#* it under the terms of the GNU Lesser General Public License as published by
#* the Free Software Foundation, either version 3 of the License, or
#* (at your option) any later version.
#*
#* The Toadlet Engine is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU Lesser General Public License for more details.
#*
#* You should have received a copy of the GNU Lesser General Public License
#* along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
#*
#********** Copyright header - do not remove **********

import Blender
import bpy

def write(filename):
	out = open(filename, "w")
	out.write('<XMSH Version="3">\n')

	scene = bpy.data.scenes.active
	objects = scene.objects.selected

	for ob in objects:
		if ob.type=='Mesh': 
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
			
			# compute the total number of indices
			indicies=0
			for face in mesh.faces:
				indicies+=len(face.verts)
			out.write('\t\t\t<Indexes Count=\"%d\">' % (indicies))

			# export all vertices from each face
			for face in mesh.faces:
				for vert in face.verts:
					out.write('%d '% (vert.index))
			out.write('</Indexes>\n')
			out.write('\t\t</Submesh>\n')
			out.write('\t</Mesh>\n')
	out.write('</XMSH>\n')
	out.close()

Blender.Window.FileSelector(write, "Export toadlet XMSH", Blender.sys.makename(ext='.xmsh'))

