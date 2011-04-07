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

* Only selected armatures and meshes will be exported.
* Only triangle meshes are supported, convert any quads before exporting.
* Only sticky UV texture coordinates are supported; per-face UVs will be ignored.
* Faces without a material will be assigned a dummy one.
* No operations or transformations are performed by this script, objects are exported exactly as is.
* Bones with idential names are not supported, and will result in badly exported skeletons

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
	# Exporter specific variables
	mBoneIndicies={}
	mBoneCounter=0

	out = open(filename, "w")
	out.write('<XMSH Version="3">\n')

	scene = bpy.data.scenes.active
	objects = scene.objects.selected

	# The XMSH format references bones by index, not by name as blender does.
	# Find all armatures to create a name,index dictionary for all bones.
	for ob in objects:
		if ob.type=='Armature':
			armature = ob.getData()

			for name in armature.bones.keys():
				if name in mBoneIndicies:
					# TODO: Actually warn the user with a popup or something useful
					# TODO: Perhaps this should be part of some larger check to ensure the objects being
					# exported are suitable; then popup and warn the user if not and what the reason is
					print 'Warning! Duplicate bone names detected, exported skeletons will be incorrect'
				mBoneIndicies[name]=mBoneCounter
				mBoneCounter+=1

	# Export all selected meshes
	for ob in objects:
		if ob.type=='Mesh': 
			mesh = ob.getData(mesh=True)
			if len(mesh.verts)==0:
				continue;

			# Transform the mesh coordinates into worldspace, as suggested by the blender documentation:
			# http://www.blender.org/documentation/249PythonDoc/NMesh.NMesh-class.html#transform
			mesh.transform(ob.matrix,True)

			# Write out all vertexes in the mesh at once
			out.write('\t<Mesh>\n')
			out.write('\t\t<Vertexes Count=\"%d\" ' % (len(mesh.verts)))
			out.write('Type=\"Position,Normal') 
			if mesh.vertexUV:
				out.write(',TexCoord')
			if len(mBoneIndicies)>0:
				out.write(',Bone')
			out.write('\">\n')
			for vert in mesh.verts:
				out.write('\t\t\t%f,%f,%f %f,%f,%f' % (vert.co.x,vert.co.y,vert.co.z,vert.no.x,vert.no.y,vert.no.z))
				if mesh.vertexUV:
					out.write(' %f,%f' % (vert.uvco.x,vert.uvco.y))
				if len(mBoneIndicies)>0:
					bonePairs=mesh.getVertexInfluences(vert.index)
					for bone in bonePairs:
						if bone[0] in mBoneIndicies:
							# Only write out the bone if it is present in the exported armatures
							out.write(' %d,%f' % (mBoneIndicies[bone[0]],bone[1]))
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

					# Export all texture images associated with this material
					for mtex in mat.textures:
						if mtex and mtex.tex:
							out.write('\t\t\t\t<TextureStage>\n')
							if mtex.tex.image and mtex.tex.image.filename:
								out.write('\t\t\t\t\t<Texture File=\"%s\"/>\n' % (mtex.tex.image.filename))
							out.write('\t\t\t\t</TextureStage>\n')

					out.write('\t\t\t</Material>\n')

				out.write('\t\t</SubMesh>\n')


			# undo our transforms, to avoid screwing with blender
			tmpmat=ob.matrix.copy()
			mesh.transform(tmpmat.invert(),True)

			out.write('\t</Mesh>\n')

	# Export all selected armatures
	for ob in objects:
		if ob.type=='Armature':
			armature = ob.getData()

			out.write('\t<Skeleton>\n')

			for bone in armature.bones.values():
				out.write('\t\t<Bone')
				out.write(' Index=\"%d\"' % (mBoneIndicies[bone.name]))
				if bone.parent:
					out.write(' Parent=\"%d\"' % (mBoneIndicies[bone.parent.name]))
				else:
					out.write(' Parent=\"-1\"')
				out.write(' Name=\"%s\"' % (bone.name))
				out.write('>\n')

				# TODO: boneMat=bone.matrix['BONESPACE'] <- This gives a 3x3 of rotations only. Does it have any value in this export?
				# Hints on blender bone matricies:
				#	http://www.devmaster.net/forums/showthread.php?t=15178
				#	http://www.blender.org/development/release-logs/blender-240/how-armatures-work/
				# XMSH skeletons require each bone matrix to be in the space of its parent.
				# Try out the transforms suggested in the above links to achieve this:
				if bone.parent:
					parentMat=bone.parent.matrix['ARMATURESPACE'].copy()
					parentMat.invert()
				else:
					parentMat=ob.matrixWorld
				boneMat=bone.matrix['ARMATURESPACE']*parentMat

				out.write('\t\t\t<Translate>%f,%f,%f</Translate>\n' % (boneMat.translationPart().x, boneMat.translationPart().y, boneMat.translationPart().z))
				out.write('\t\t\t<Rotate>%f,%f,%f,%f</Rotate>\n' % (boneMat.toQuat().x, boneMat.toQuat().y, boneMat.toQuat().z, boneMat.toQuat().w))
				out.write('\t\t\t<Scale>%f,%f,%f</Scale>\n' % (boneMat.scalePart().x, boneMat.scalePart().y, boneMat.scalePart().z))

				# TODO: apparently these are depreciated. Double check to be sure.
				#out.write('\t\t\t<WorldToBoneTranslate>%f,%f,%f</WorldToBoneTranslate>\n' % ())
				#out.write('\t\t\t<WorldToBoneRotate>%f,%f,%f</WorldToBoneRotate>\n' % ())
			
				out.write('\t\t</Bone>\n')

			out.write('\t</Skeleton>\n')

	out.write('</XMSH>\n')
	out.close()

Blender.Window.FileSelector(write, "Export toadlet XMSH", Blender.sys.makename(ext='.xmsh'))

