import bpy
import struct
import bmesh
import math
from bpy_extras.io_utils import (axis_conversion)

from math import radians

import bpy
from mathutils import *

def getMesh():
    for obj in bpy.context.selected_objects:
        if obj.type == "MESH":
            return obj
    return 0
    
def getArmature():
    for obj in bpy.context.selected_objects:
        if obj.type == "ARMATURE":
            return obj
    return 0

def getAnimations():
    anims = []
    for action in bpy.data.actions:
        anims.append(action)
    return anims

def export_p3d(context, filepath):
    print("Running p3d export...")
    
    my_mesh = getMesh()
    my_armature = getArmature()
    my_animations = getAnimations()
    
    verts = []
    indices = []

    my_armature.data.pose_position = "REST"
    my_armature.data.update_tag()
    bpy.context.scene.frame_set(bpy.context.scene.frame_current)

    me = my_mesh.data

    me.calc_loop_triangles()
    me.calc_normals_split()
    uvmap = me.uv_layers.active
        
    for tri in me.loop_triangles:
        for i in range(2, -1, -1):
            vert_index = tri.vertices[i]
            loop_index = tri.loops[i]

            pos = me.vertices[vert_index].co
            norm = me.loops[loop_index].normal
            uv = uvmap.data[loop_index].uv
            bone_indices = [0, 0, 0, 0]
            bone_weights = [0, 0, 0, 0]
            
            if my_armature:
                for jointBindingIndex, group in enumerate(me.vertices[vert_index].groups):
                    if jointBindingIndex > 3:
                        break
                    group_index = group.group
                    bone_name = my_mesh.vertex_groups[group_index].name
                    bone_indices[jointBindingIndex] = my_armature.data.bones.find(bone_name)
                    bone_weights[jointBindingIndex] = group.weight
            
            foundOne = False
            vertI = 0
            while vertI < len(verts):
                if verts[vertI] == (pos, norm, uv, bone_indices, bone_weights):
                    foundOne = True
                    indices.append(vertI)
                    break
                vertI += 1
            
            if foundOne == False:
                verts.append((pos, norm, uv, bone_indices, bone_weights))
                indices.append(len(verts) - 1)
            
    with open(filepath, 'wb') as f:
        f.write("p3d".encode())
        f.write(struct.pack('i', 1))
        
        f.write(struct.pack('i', len(verts)))
        
        for v in verts:
            # x, y, z
            f.write(struct.pack('f', -v[0][0]))
            f.write(struct.pack('f', v[0][2]))
            f.write(struct.pack('f', -v[0][1]))
            
            # nx, ny, ny            
            f.write(struct.pack('f', v[1][0]))
            f.write(struct.pack('f', v[1][1]))
            f.write(struct.pack('f', v[1][2]))

            # u, v
            f.write(struct.pack('f', v[2][0]))
            f.write(struct.pack('f', 1-v[2][1]))

            #bones
            f.write(struct.pack('f', v[3][0]))
            f.write(struct.pack('f', v[3][1]))
            f.write(struct.pack('f', v[3][2]))
            f.write(struct.pack('f', v[3][3]))

            #weights
            f.write(struct.pack('f', v[4][0]))
            f.write(struct.pack('f', v[4][1]))
            f.write(struct.pack('f', v[4][2]))
            f.write(struct.pack('f', v[4][3]))
            print(v[3])
            print(v[4])

        
        f.write(struct.pack('i', len(indices)))
        
        for i in indices:
            f.write(struct.pack('i', i))
        
        f.write(struct.pack('i', len(my_animations))

        for a in my_animations:
            f.write(struct.pack('i', len(a.name)))
            f.write(a.name.encode())
        
                
    with open(filepath + "_skn", 'wb') as f:
        f.write("p3d".encode())
        f.write(struct.pack('i', 1))
        
        f.write(struct.pack('i', len(my_armature.data.bones)))
        for b in my_armature.data.bones:
            f.write(struct.pack('i', len(b.name)))
            f.write(b.name.encode())

            if b.parent and b.parent.name:
                f.write(struct.pack('i', my_armature.data.bones.find(b.parent.name)))
            else:
                f.write(struct.pack('i', 0))

            mat = b.matrix_local
            if b.parent:
                mat = b.parent.matrix_local.inverted() @ mat

            translation = mat.to_translation()
            rotation = mat.to_quaternion()

            f.write(struct.pack('f', -rotation.x))
            f.write(struct.pack('f', rotation.z))
            f.write(struct.pack('f', -rotation.y))
            f.write(struct.pack('f', -rotation.w))
            f.write(struct.pack('f', -translation.x))
            f.write(struct.pack('f', translation.z))
            f.write(struct.pack('f', -translation.y))

    my_armature.data.pose_position = "POSE"
    my_armature.data.update_tag()
    bpy.context.scene.frame_set(bpy.context.scene.frame_current)

    for a in my_animations:
        my_armature.animation_data.action = a
        with open(filepath.replace(".p3d", "") + "_" + a.name + ".p3d_anim", "wb") as f:
            f.write("p3d".encode())
            f.write(struct.pack('i', 1))
            
            f.write(struct.pack('i', int(a.frame_range.y)))
            f.write(struct.pack('f', (a.frame_range.y - 1) / 30))
            f.write(struct.pack('i', len(my_armature.data.bones)))
                
            for frame_index in range(int(a.frame_range.x), int(a.frame_range.y)):
                bpy.context.scene.frame_set(frame_index)
            
                for bone in my_armature.pose.bones:

                    mat = bone.matrix
                    
                    if bone.parent:
                        mat = bone.parent.matrix.inverted() @ mat
                    print(mat.to_quaternion())

                    translation = mat.to_translation()
                    rotation = mat.to_quaternion()

                    f.write(struct.pack('f', -rotation.x))
                    f.write(struct.pack('f', rotation.z))
                    f.write(struct.pack('f', -rotation.y))
                    f.write(struct.pack('f', -rotation.w))
                    f.write(struct.pack('f', -translation.x))
                    f.write(struct.pack('f', translation.z))
                    f.write(struct.pack('f', -translation.y))


    print("Done!")

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export.p3d"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export model to p3d"

    # ExportHelper mixin class uses this
    filename_ext = ".p3d"

    filter_glob: StringProperty(
        default="*.p3d",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return export_p3d(context, self.filepath)

# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Text Export Operator")


def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
    bpy.ops.export.p3d('INVOKE_DEFAULT')