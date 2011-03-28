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

Notes:

Only selected objects will be exported.
Only sticky UV texture coordinates are supported; per-face UVs will be ignored.
Faces without a material will be assigned a dummy one.
No operations or transformations are performed by this script, objects are exported exactly as is.

Usage:

Select the objects you wish to export and run this script from the "File->Export" menu.
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

			# Don't bother writing out empty meshes
			if len(mesh.verts)==0:
				continue;

			# Write out all vertexes in the mesh at once
			out.write('\t<Mesh>\n')
			out.write('\t\t<Vertexes Count=\"%d\" ' % (len(mesh.verts)))
			# TODO: add Bones, and therefore skeletons
			out.write('Type=\"Position,Normal') 
			if mesh.vertexUV:
				out.write(',TexCoord')
			out.write('\">\n')
			for vert in mesh.verts:
				out.write('\t\t\t%f,%f,%f %f,%f,%f' % (vert.co.x,vert.co.y,vert.co.z,vert.no.x,vert.no.y,vert.no.z))
				if mesh.vertexUV:
					out.write(' %f,%f' % (vert.uvco.x,vert.uvco.y))
				out.write('\n')
			out.write('\t\t</Vertexes>\n')

			# Generate a list of face vertex indices by material; adding a dummy material if none exists
			xmshMaterials=mesh.materials
			if len(xmshMaterials)==0:
				xmshMaterials.append(Blender.Material.New(''))
			matFaceIndicies=[[] for i in range(len(xmshMaterials))]
			for face in mesh.faces:
				for vert in face.verts:
					matFaceIndicies[face.mat].append(vert.index)

			# Write out one submesh per mesh material
			for i in range(len(matFaceIndicies)):
				submesh=matFaceIndicies[i]

				# Don't bother writing out empty submeshes
				if len(submesh)==0:
					continue

				out.write('\t\t<SubMesh>\n')
				out.write('\t\t\t<Indexes Count=\"%d\">' % (len(submesh)))
				for index in submesh:
					out.write('%d ' % (index))
				out.write('</Indexes>\n')

				mat=xmshMaterials[i]
				if mat:
					out.write('\t\t\t<Material Name=\"%s\">\n' % (mat.name))
					
					# TODO: Right now the mapping of blender material lightning effects to toadlet XMSH is unclear
					out.write('\t\t\t\t<LightEffect>\n')
					out.write('\t\t\t\t\t<Ambient>%f,%f,%f,%f</Ambient>\n' % (mat.getRGBCol()[0],mat.getRGBCol()[1],mat.getRGBCol()[2],mat.getAlpha()))
					out.write('\t\t\t\t\t<Diffuse>%f,%f,%f,%f</Diffuse>\n' % (mat.getRGBCol()[0],mat.getRGBCol()[1],mat.getRGBCol()[2],mat.getAlpha()))
					out.write('\t\t\t\t\t<Specular>%f,%f,%f,%f</Specular>\n' % (mat.getSpecCol()[0],mat.getSpecCol()[1],mat.getSpecCol()[2],mat.getAlpha()))
					out.write('\t\t\t\t\t<Shininess>%f</Shininess>\n' % (mat.getSpec()))
					out.write('\t\t\t\t\t<Emmissive>%f,%f,%f,%f</Emmissive>\n' % (mat.getEmit(),mat.getEmit(),mat.getEmit(),mat.getAlpha()))
					out.write('\t\t\t\t</LightEffect>\n')

					# TODO: Right now if a material is set to SHADELESS it is unlit, otherwise light affects it
					if mat.getMode() & Blender.Material.Modes['SHADELESS']:
						out.write('\t\t\t\t<Lighting>false</Lighting>\n')
					else:
						out.write('\t\t\t\t<Lighting>true</Lighting>\n')

					# TODO: No idea with this one; how does blender determine culling?
					out.write('\t\t\t\t<FaceCulling>back</FaceCulling>\n')
					out.write('\t\t\t</Material>\n')
				out.write('\t\t</SubMesh>\n')

			out.write('\t</Mesh>\n')

	out.write('</XMSH>\n')
	out.close()

Blender.Window.FileSelector(write, "Export toadlet XMSH", Blender.sys.makename(ext='.xmsh'))

