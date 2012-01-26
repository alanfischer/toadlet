# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 3
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8 compliant>

bl_info = {
    "name": "Toadlet XMSH Exporter",
    "author": "Andrew Fischer",
    "version": (1, 0),
    "blender": (2, 5, 7),
    "api": 35622,
    "location": "File > Import-Export",
    "description": "Export selected meshes, materials and armatures to toadlet XMSH version 4",
    "warning": "Only triangle meshes are supported, convert any quads.",
    "wiki_url": "http://wiki.blender.org/index.php/Extensions:2.6/Py/Scripts/Import-Export/ToadletXMSH_Exporter",
    "tracker_url": "http://code.google.com/p/toadlet",
    "category": "Import-Export"}

import os
import time
import mathutils
import bpy
from bpy.props import BoolProperty
from bpy_extras.io_utils import ExportHelper


class XMSHVertex:
    def __init__(self, index, co, no):
        self.index = index
        self.co = co
        self.no = no

        # A list of 2-element lists
        self.bones = []


def export(context, props, filepath):
    mBoneIndicies = {}
    mBoneCounter = 0

    objects = context.selected_objects
    if len(objects) == 0:
        print("No objects selected for export.")
        return False

    out = open(filepath, "w")
    out.write('<XMSH Version="4">\n')

    # The XMSH format references bones by index, not by name as blender does.
    # Find all armatures to create a name,index dictionary for all bones.
    for ob in objects:
        if ob.type == 'ARMATURE':
            armature = ob.data
            for bone in armature.bones:
                if bone.name in mBoneIndicies:
                    # TODO: This should be part of some larger check to ensure the objects being
                    # exported are suitable; then popup and warn the user if not and what the reason is
                    print('Warning! Duplicate bone names detected, exported skeletons will be incorrect')
                mBoneIndicies[bone.name] = mBoneCounter
                mBoneCounter += 1

    # Export all selected meshes
    for ob in objects:
        if ob.type == 'MESH':
            mesh = ob.to_mesh(context.scene, True, 'PREVIEW')
            if len(mesh.vertices) == 0:
                continue

            # Transform the mesh coordinates into worldspace, as suggested by the blender documentation:
            # http://www.blender.org/documentation/249PythonDoc/NMesh.NMesh-class.html#transform
            # Note: Updated for 2.5x
            obMatrix = ob.matrix_world.copy()
            mesh.transform(obMatrix)

            # All blender vertices are packed into our own list of XMSHVertexs
            xmshVerts = []
            for vert in mesh.vertices:
                xmshv = XMSHVertex(vert.index, vert.co, vert.normal)

                if len(mBoneIndicies) > 0:
                    # NOTE: Blender matches verts to bones for influences by having the VertexGroup named the same thing as the bone
                    # The VertexGroup name is only accessible from object.vertex_groups, so loop through and match IDs
                    for obGroup in ob.vertex_groups:
                        for vertGroup in vert.groups:
                            if obGroup.index == vertGroup.group:
                                if obGroup.name in mBoneIndicies:
                                    xmshv.bones.append([mBoneIndicies[obGroup.name], vertGroup.weight])

                xmshVerts.append(xmshv)

            # Create a list of materials and face vertex indices by material; adding a dummy material if none exists
            xmshMaterials = mesh.materials
            if len(xmshMaterials) == 0:
                xmshMaterials.append(bpy.data.materials.new(''))
            xmshMatFaceIndicies = [[] for i in range(len(xmshMaterials))]

            # Loop through all mesh faces and fill up our vertex texture coordinates and our matFaceIndicies arrays
            # XMSH does not handle multiple UV coords per vertex
            # Instead, we create additional vertices to handle this case,
            # and write out additional mesh vertices and face indices to handle them.
            # The xmshVertUVs dictionary helps us tell which vertices are duplicates
            xmshVertUVs = {}
            for face in mesh.faces:
                for i in range(len(face.vertices)):
                    if i == 3:
                        print("Only tri faces are supported. Convert Quads to Tris before exporting to xmsh")
                        return False
                    vert = mesh.vertices[face.vertices[i]]

                    if len(mesh.uv_textures) > 0:
                        uvLayer = mesh.uv_textures.active.data
                        uv = mathutils.Vector(uvLayer[face.index].uv[i])

                        if vert.index in xmshVertUVs and (xmshVertUVs[vert.index].x != uv.x or xmshVertUVs[vert.index].y != uv.y):
                            # A blender vertex with multiple UV coords requires a new xmsh vertex
                            xmshv = XMSHVertex(vert.index, vert.co, vert.normal)
                            xmshv.bones = xmshVerts[vert.index].bones
                            xmshv.uv = uv
                            xmshv.index = len(xmshVerts)
                            xmshMatFaceIndicies[face.material_index].append(xmshv.index)
                            xmshVerts.append(xmshv)
                        else:
                            # Otherwise these are uv coords for a vertex in our XMSHVertex array
                            xmshVerts[vert.index].uv = uv

                            # Store the UVs and append to our face indices
                            xmshVertUVs[vert.index] = uv
                            xmshMatFaceIndicies[face.material_index].append(vert.index)
                    else:
                        xmshMatFaceIndicies[face.material_index].append(vert.index)

            # Start writing out the mesh
            out.write('\t<Mesh>\n')
            out.write('\t\t<Vertexes Count=\"%d\" ' % (len(xmshVerts)))
            out.write('Type=\"Position,Normal')
            if len(mesh.uv_textures) > 0:
                out.write(',TexCoord')
            if len(mBoneIndicies) > 0:
                out.write(',Bone')
            out.write('\">\n')

            for vert in xmshVerts:
                out.write('\t\t\t%f,%f,%f %f,%f,%f' % (vert.co.x, vert.co.y, vert.co.z, vert.no.x, vert.no.y, vert.no.z))
                if len(mesh.uv_textures) > 0:
                    out.write(' %f,%f' % (vert.uv.x, vert.uv.y))
                first = True
                for bone in vert.bones:
                    if first:
                        out.write(' ')
                    else:
                        out.write(',')
                    out.write('%d,%f' % (bone[0], bone[1]))
                    first = False
                out.write('\n')

            out.write('\t\t</Vertexes>\n')

            # Write out one submesh per mesh material
            for i in range(len(xmshMatFaceIndicies)):
                submesh = xmshMatFaceIndicies[i]
                if len(submesh) == 0:
                    continue

                out.write('\t\t<SubMesh>\n')
                out.write('\t\t\t<Indexes Count=\"%d\">' % (len(submesh)))
                for index in submesh:
                    out.write('%d ' % (index))
                out.write('</Indexes>\n')

                mat = xmshMaterials[i]
                if mat:
                    out.write('\t\t\t<Material Name=\"%s\">\n' % (mat.name))
                    out.write('\t\t\t\t<Ambient>%f,%f,%f,%f</Ambient>\n' % (mat.ambient, mat.ambient, mat.ambient, mat.alpha))
                    out.write('\t\t\t\t<Diffuse>%f,%f,%f,%f</Diffuse>\n' % (mat.diffuse_color[0], mat.diffuse_color[1], mat.diffuse_color[2], mat.alpha))
                    out.write('\t\t\t\t<Specular>%f,%f,%f,%f</Specular>\n' % (mat.specular_color[0], mat.specular_color[1], mat.specular_color[2], mat.alpha))
                    out.write('\t\t\t\t<Shininess>%f</Shininess>\n' % (mat.specular_intensity))
                    out.write('\t\t\t\t<Emmissive>%f,%f,%f,%f</Emmissive>\n' % (mat.emit, mat.emit, mat.emit, mat.alpha))

                    # Shadeless materials in blender are unaffected by light
                    if mat.use_shadeless:
                        out.write('\t\t\t\t<Light>false</Light>\n')
                    else:
                        out.write('\t\t\t\t<Light>true</Light>\n')

                    # Blender doesn't cull, specifically. Rather it either lights a mesh's faces doublesided or it does not.
                    # If double sided is not set, we assume backface culling.
                    if mesh.show_double_sided:
                        out.write('\t\t\t\t<Cull>none</Cull>\n')
                    else:
                        out.write('\t\t\t\t<Cull>back</Cull>\n')

                    # Export all texture images associated with this material, reversed makes topmost the highest priority
                    for mtex in reversed(mat.texture_slots):
                        if mtex and mtex.texture.type == 'IMAGE':
                            out.write('\t\t\t\t<TextureStage>\n')
                            if mtex.texture.image and mtex.texture.image.filepath:
                                out.write('\t\t\t\t\t<Texture File=\"%s\"/>\n' % (mtex.texture.image.filepath))
                            out.write('\t\t\t\t</TextureStage>\n')

                    out.write('\t\t\t</Material>\n')

                out.write('\t\t</SubMesh>\n')

            out.write('\t</Mesh>\n')

            # Clean up
            bpy.data.meshes.remove(mesh)

    # Export all selected armatures
    for ob in objects:
        if ob.type == 'ARMATURE':
            armature = ob.data
            out.write('\t<Skeleton>\n')

            for bone in armature.bones:
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
                #    http://www.devmaster.net/forums/showthread.php?t=15178
                #    http://www.blender.org/development/release-logs/blender-240/how-armatures-work/
                if bone.parent:
                    parentMat = bone.parent.matrix_local.copy()
                    parentMat.invert()
                else:
                    parentMat = ob.matrix_world.copy()
                boneMat = bone.matrix_local * parentMat

                out.write('\t\t\t<Translate>%f,%f,%f</Translate>\n' % (boneMat.to_translation().x, boneMat.to_translation().y, boneMat.to_translation().z))
                out.write('\t\t\t<Rotate>%f,%f,%f,%f</Rotate>\n' % (boneMat.to_quaternion().x, boneMat.to_quaternion().y, boneMat.to_quaternion().z, boneMat.to_quaternion().w))
                out.write('\t\t\t<Scale>%f,%f,%f</Scale>\n' % (boneMat.to_scale().x, boneMat.to_scale().y, boneMat.to_scale().z))
                out.write('\t\t</Bone>\n')

            out.write('\t</Skeleton>\n')

    out.write('</XMSH>\n')
    out.close()

    return True


class ExportXMSH(bpy.types.Operator, ExportHelper):
    '''Export a mesh and armature to an XMSH file format (.xmsh)'''
    bl_idname = "export.xmsh"
    bl_label = "Export Toadlet XMSH (.xmsh)"

    filename_ext = ".xmsh"

    def execute(self, context):
        start_time = time.time()
        print('\n*** Starting XMSH Export ***')
        props = self.properties
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)

        exported = export(context, props, filepath)

        if exported:
            print('*** Export finished in %s seconds ***' % ((time.time() - start_time)))
            print(filepath)

        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager

        # This function calls self.execute()
        wm.fileselect_add(self)
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
