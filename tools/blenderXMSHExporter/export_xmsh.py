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
* Faces without a material will be assigned a dummy one.
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
import os,sys,subprocess


class XMSHVertex:	
	def __init__(self,index,co,no):
		self.index=index
		self.co=co
		self.no=no

		# A list of 2-element lists
		self.bones=[]
		
	
def export(filename):
	# Exporter specific variables
	mBoneIndicies={}
	mBoneCounter=0

	# Try to find the tmshoptimizer executable
	tmshoptimizer=None
	path=os.environ['PATH']
	paths=path.split(os.pathsep)
	if sys.platform=='win32':
		exe="tmshoptimizer.exe"
	else:
		exe="tmshoptimizer"
	if os.path.isfile(os.path.join(os.curdir,exe)):
		tmshoptimizer=os.path.join(os.curdir,exe)
	else:
		for p in paths:
			f=os.path.join(p,exe)
			if os.path.isfile(f):
				tmshoptimizer=f
				break

	# If available, give the user the option to optimize the xmsh
	if tmshoptimizer:
		runOptimizer=Blender.Draw.Create(1)
		options=[]
		options.append(("Optimize XMSH",runOptimizer,"If tmshOptimizer is present, optimize the output xmsh"))
		if not Blender.Draw.PupBlock("Toadlet XMSH Exporter", options):
			return

	# Write out the xmsh file
	out = open(filename, "w")
	out.write('<XMSH Version="3">\n')

	scene = bpy.data.scenes.active
	objects = scene.objects.selected

	Blender.Window.WaitCursor(1)

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
			obMatrix=ob.matrix.copy()
			mesh.transform(obMatrix,True)

			# Create our initial xmshVerts by referencing the existing mesh verts
			xmshVerts=[]
			for vert in mesh.verts:
				xmshv=XMSHVertex(vert.index,vert.co,vert.no)

				# Assign bones if a skeleton is present
				if len(mBoneIndicies)>0:
					bonePairs=mesh.getVertexInfluences(vert.index)
					for bone in bonePairs:
						# Only store the bone if it is present in one of the exported armatures
						if bone[0] in mBoneIndicies:
							xmshv.bones.append([mBoneIndicies[bone[0]],bone[1]])

				# Add the new xmshVert
				xmshVerts.append(xmshv)

	
			# Create a list of materials and face vertex indices by material; adding a dummy material if none exists
			xmshMaterials=mesh.materials
			if len(xmshMaterials)==0:
				xmshMaterials.append(Blender.Material.New(''))
			xmshMatFaceIndicies=[[] for i in range(len(xmshMaterials))]

			# Loop through all mesh faces and fill up our vertex texture coordinates and our matFaceIndicies arrays
			# XMSH does not handle multiple UV coords per vertex
			# Instead, we create additional verticies to handle this case, and write out
			# additional mesh vertices and face indices to handle them.
			# The xmshVertUVs dictionary helps us tell which vertices are duplicates
			xmshVertUVs={} 
			for face in mesh.faces:
				for i in range(len(face.verts)):
					vert=face.verts[i];
					if mesh.faceUV:
						# FaceUVs means check for a new vertex requirement
						if vert.index in xmshVertUVs and (
							xmshVertUVs[vert.index].x!=face.uv[i].x or xmshVertUVs[vert.index].y!=face.uv[i].y):

							# Found a vertex with multiple UV coords
							# Create a new vertex, give it these UV coords, and bump it's index
							xmshv=XMSHVertex(vert.index,vert.co,vert.no)
							xmshv.bones=xmshVerts[vert.index].bones
							xmshv.uv=face.uv[i]
							xmshv.index=len(xmshVerts)

							# This new vertex goes into our materialFaceIndex 
							xmshMatFaceIndicies[face.mat].append(xmshv.index)
							
							# Store 
							xmshVerts.append(xmshv)
						else:
							# Just a new vertex
							# Assign it's UV coords to an existing xmshVert
							xmshVerts[vert.index].uv=face.uv[i]

							# Store the UVs for this vertex index and continue
							xmshVertUVs[vert.index]=face.uv[i]
							xmshMatFaceIndicies[face.mat].append(vert.index)
					else:
						# No UVs mean no worrying about new vertices
						xmshMatFaceIndicies[face.mat].append(vert.index)

			# Write out all xmsh verticies at once
			out.write('\t<Mesh>\n')
			out.write('\t\t<Vertexes Count=\"%d\" ' % (len(xmshVerts)))
			out.write('Type=\"Position,Normal') 
			if mesh.faceUV:
				out.write(',TexCoord')
			if len(mBoneIndicies)>0:
				out.write(',Bone')
			out.write('\">\n')
			for vert in xmshVerts:
				out.write('\t\t\t%f,%f,%f %f,%f,%f' % (vert.co.x,vert.co.y,vert.co.z,vert.no.x,vert.no.y,vert.no.z))
				if mesh.faceUV:
					out.write(' %f,%f' % (vert.uv.x, vert.uv.y))
				first=True
				for bone in vert.bones:
					if first:
						out.write(' ')
					else:
						out.write(',')
					out.write('%d,%f' % (bone[0], bone[1]))
					first=False
				out.write('\n')
			out.write('\t\t</Vertexes>\n')

			# Write out one submesh per mesh material
			for i in range(len(xmshMatFaceIndicies)):
				submesh=xmshMatFaceIndicies[i]
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
					out.write('\t\t\t\t\t<Shininess>%f</Shininess>\n' % (mat.getHardness()))
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
			mesh.transform(obMatrix.invert(),True)

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

				# XMSH skeletons require each bone matrix to be in the space of its parent.
				# Hints on blender bone matricies:
				#	http://www.devmaster.net/forums/showthread.php?t=15178
				#	http://www.blender.org/development/release-logs/blender-240/how-armatures-work/
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

	# Call the mesh optimizer if requested
	if tmshoptimizer and runOptimizer==1:
		if not Blender.Draw.PupBlock("Export Done",["Begin Optimization?"]):
			return
		args=[tmshoptimizer,filename]
		text=subprocess.Popen(args,stdout=subprocess.PIPE).communicate()[0]
		texts=text.split("\n")
		if not Blender.Draw.PupBlock("Optimizer Done",texts):
			return

	Blender.Window.WaitCursor(0)

Blender.Window.FileSelector(export, "Export toadlet XMSH", Blender.sys.makename(ext='.xmsh'))

