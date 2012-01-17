#!BPY

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

bl_info={
	"name": "Toadlet XMSH Exporter",
	"author": "Andrew Fischer",
	"blender": (2, 5, 7),
	"api": 35622,
	"location": "File > Import-Export",
	"description": "Export toadlet meshes and materials",
	"warning": "Only triangle meshes supported, convert any quads. "
		"Bones with identical names not supported.",
	"wiki_url": "http://code.google.com/p/toadlet",
	"tracker_url": "",
	"support": 'COMMUNITY',
	"category": "Import-Export"}


import os,sys,subprocess,time
import bpy
from bpy.props import BoolProperty
from bpy_extras.io_utils import ExportHelper


class XMSHVertex:	
	def __init__(self,index,co,no):
		self.index=index
		self.co=co
		self.no=no

		# A list of 2-element lists
		self.bones=[]
		
	
def doExport(context,props,filepath):
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

	# Write out the xmsh file
	out = open(filepath, "w")
	out.write('<XMSH Version="3">\n')

	#objects = bpy.data.scenes.active.objects.selected
	objects = context.selected_objects

	# The XMSH format references bones by index, not by name as blender does.
	# Find all armatures to create a name,index dictionary for all bones.
	for ob in objects:
		if ob.type=='ARMATURE':
			armature = ob.getData()
			for name in armature.bones.keys():
				if name in mBoneIndicies:
					# TODO: This should be part of some larger check to ensure the objects being
					# exported are suitable; then popup and warn the user if not and what the reason is
					print('Warning! Duplicate bone names detected, exported skeletons will be incorrect')
				mBoneIndicies[name]=mBoneCounter
				mBoneCounter+=1

	# Export all selected meshes
	for ob in objects:
		if ob.type=='MESH': 
			mesh = ob.to_mesh(context.scene,True,'PREVIEW')
			if len(mesh.vertices)==0:
				continue;

			# Transform the mesh coordinates into worldspace, as suggested by the blender documentation:
			# http://www.blender.org/documentation/249PythonDoc/NMesh.NMesh-class.html#transform
			# Note: Updated for 2.5x
			obMatrix=ob.matrix_world.copy()
			mesh.transform(obMatrix)

			# Create our initial xmshVerts by referencing the existing mesh verts
			xmshVerts=[]
			for vert in mesh.vertices:
				xmshv=XMSHVertex(vert.index,vert.co,vert.normal)

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
				xmshMaterials.append(bpy.data.Material.New(''))
			xmshMatFaceIndicies=[[] for i in range(len(xmshMaterials))]

			# Loop through all mesh faces and fill up our vertex texture coordinates and our matFaceIndicies arrays
			# XMSH does not handle multiple UV coords per vertex
			# Instead, we create additional vertices to handle this case, 
			# and write out additional mesh vertices and face indices to handle them.
			# The xmshVertUVs dictionary helps us tell which vertices are duplicates
			xmshVertUVs={} 
			for face in mesh.faces:
				for i in range(len(face.vertices)):
					vert=mesh.vertices[face.vertices[i]];
					if mesh.uv_textures:
						uvLayer=mesh.uv_textures.active.data
						# FaceUVs means check for a new vertex requirement
						if vert.index in xmshVertUVs and (
							xmshVertUVs[vert.index].x!=face.uv[i].x or xmshVertUVs[vert.index].y!=face.uv[i].y):

							# Found a vertex with multiple UV coords
							# Create a new vertex, give it these UV coords, and bump it's index
							xmshv=XMSHVertex(vert.index,vert.co,vert.normal)
							xmshv.bones=xmshVerts[vert.index].bones
							xmshv.uv=uvLayer[face.index].uv
							xmshv.index=len(xmshVerts)

							# This new vertex goes into our materialFaceIndex 
							xmshMatFaceIndicies[face.material_index].append(xmshv.index)
							
							# Store 
							xmshVerts.append(xmshv)
						else:
							# Just a new vertex
							# Assign it's UV coords to an existing xmshVert
							xmshVerts[vert.index].uv=uvLayer[face.index].uv

							# Store the UVs for this vertex index and continue
							xmshVertUVs[vert.index]=uvLayer[face.index].uv
							xmshMatFaceIndicies[face.material_index].append(vert.index)
					else:
						# No UVs mean no worrying about new vertices
						xmshMatFaceIndicies[face.material_index].append(vert.index)

			# Write out all xmsh vertices at once
			out.write('\t<Mesh>\n')
			out.write('\t\t<Vertexes Count=\"%d\" ' % (len(xmshVerts)))
			out.write('Type=\"Position,Normal') 
			if mesh.uv_textures:
				out.write(',TexCoord')
			if len(mBoneIndicies)>0:
				out.write(',Bone')
			out.write('\">\n')			
			for vert in xmshVerts:
				out.write('\t\t\t%f,%f,%f %f,%f,%f' % (vert.co.x,vert.co.y,vert.co.z,vert.no.x,vert.no.y,vert.no.z))
				if mesh.uv_textures:
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
					out.write('\t\t\t\t<LightEffect>\n')
					out.write('\t\t\t\t\t<Ambient>%f,%f,%f,%f</Ambient>\n' % (mat.ambient,mat.ambient,mat.ambient,mat.alpha))
					out.write('\t\t\t\t\t<Diffuse>%f,%f,%f,%f</Diffuse>\n' % (mat.diffuse_color[0],mat.diffuse_color[1],mat.diffuse_color[2],mat.alpha))
					out.write('\t\t\t\t\t<Specular>%f,%f,%f,%f</Specular>\n' % (mat.specular_color[0],mat.specular_color[1],mat.specular_color[2],mat.alpha))
					out.write('\t\t\t\t\t<Shininess>%f</Shininess>\n' % (mat.specular_intensity))
					out.write('\t\t\t\t\t<Emmissive>%f,%f,%f,%f</Emmissive>\n' % (mat.emit,mat.emit,mat.emit,mat.alpha))
					out.write('\t\t\t\t</LightEffect>\n')

					# TODO: Right now if a material is set to SHADELESS it is unlit, otherwise light affects it
					if mat.use_shadeless:
						out.write('\t\t\t\t<Lighting>false</Lighting>\n')
					else:
						out.write('\t\t\t\t<Lighting>true</Lighting>\n')

					# TODO: No idea with this one; how does blender determine culling?
					out.write('\t\t\t\t<FaceCulling>back</FaceCulling>\n')

					# Export all texture images associated with this material, reversed makes topmost the highest priority
					for mtex in reversed(mat.texture_slots):
						if mtex and mtex.texture.type=='IMAGE':
							out.write('\t\t\t\t<TextureStage>\n')
							if mtex.texture.image and mtex.texture.image.filepath:
								out.write('\t\t\t\t\t<Texture File=\"%s\"/>\n' % (mtex.texture.image.filepath))
							out.write('\t\t\t\t</TextureStage>\n')

					out.write('\t\t\t</Material>\n')

				out.write('\t\t</SubMesh>\n')
				
			out.write('\t</Mesh>\n')

			# Clean up
			bpy.data.meshes.remove(mesh)
			# Undo the worldspace transforms, otherwise the model will remain transformed in the application
			#mesh.transform(obMatrix.invert(),True)

	# Export all selected armatures
	for ob in objects:
		if ob.type=='ARMATURE':
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
				out.write('\t\t</Bone>\n')
				
			out.write('\t</Skeleton>\n')
			
	out.write('</XMSH>\n')
	out.close()


class ExportXMSH(bpy.types.Operator, ExportHelper):
	'''Export to an XMSH file format (.xmsh)'''
	bl_idname = "export_shape.xmsh"
	bl_label = "Export Toadlet XMSH (.xmsh)"

	filename_ext = ".xmsh"

		# Call the mesh optimizer if requested
	    # if tmshoptimizer and runOptimizer==1:
		# TODO: This needs to be an option box available in the export dialog
		#args=[tmshoptimizer,filename]
		#text=subprocess.Popen(args,stdout=subprocess.PIPE).communicate()[0]
		#texts=text.split("\n")
		#Blender.Draw.PupBlock("Optimizer Done",texts)
		# If available, give the user the option to optimize the xmsh
	    # if tmshoptimizer:
		# TODO: This needs to be an option box available in the export dialog
		#runOptimizer=Blender.Draw.Create(1)
		#options=[("Optimize XMSH",runOptimizer,"If tmshOptimizer is present, optimize the output xmsh")]
		#Blender.Draw.PupBlock("Toadlet XMSH Exporter", options)

	xmsh_optimize = BoolProperty(name="Optimize Mesh",
		description="Run the tmshOptimizer tool, if found, on the exported mesh.",
		default=False,)
    
	def execute(self, context):
		start_time = time.time()
		print('\n*** Starting XMSH Export ***')
		props = self.properties
		filepath= self.filepath
		filepath= bpy.path.ensure_ext(filepath, self.filename_ext)

		exported = doExport(context, props, filepath)
		
		if exported:
			print('Export finished in %s seconds' %((time.time() - start_time)))
			print(filepath)
			
		return {'FINISHED'}

	def invoke(self, context, event):
		wm = context.window_manager
		wm.fileselect_add(self) # will run self.execute()
		return {'RUNNING_MODAL'}


def menu_func_export(self, context):
	self.layout.operator(ExportXMSH.bl_idname, text="Toadlet (.xmsh)")


def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func_export)

	
if __name__ == "__main__":
	register()

