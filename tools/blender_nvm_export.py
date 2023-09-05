import bpy
import struct
import bmesh
import math

def export_omp(context, filepath):
    print("Running omp export...")
    
    with open(filepath, 'w') as f:
        # magic and version information
        f.write("omp 1\n")
                
        objects = bpy.context.selected_objects    
        for obj in objects:
            verts = []
            indices = []
            
            me = obj.data
            me.calc_loop_triangles()
            me.calc_normals_split()
            uvmap = me.uv_layers.active
            
            for tri in me.loop_triangles:
                for i in range(3):
                    vert_index = tri.vertices[i]
                    loop_index = tri.loops[i]

                    pos = me.vertices[vert_index].co
                    # normals
                    # norm = me.loops[loop_index].normal
                    uv = uvmap.data[loop_index].uv
                    
                    foundOne = False
                    vertI = 0
                    while vertI < len(verts):
                        # normals
                        # if verts[vertI] == (pos, norm, uv):
                        if verts[vertI] == (pos, uv):
                            foundOne = True
                            indices.append(vertI)
                            break
                        vertI += 1
                    
                    if foundOne == False:
                        # normals
                        # verts.append((pos, norm, uv))
                        verts.append((pos, uv))
                        indices.append(len(verts) - 1)
            for vert in verts:
                co = vert[0]
                uv = vert[1]
                f.write("v " + str(co.x) +  " " + str(co.z) + " " + str(co.y) + " " + str(uv.x) + " " + str(1 - uv.y) + "\n")
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

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export.omp"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export map to omp"

    # ExportHelper mixin class uses this
    filename_ext = ".nvm"

    filter_glob: StringProperty(
        default="*.nvm",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return export_omp(context, self.filepath)

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
    bpy.ops.export.omp('INVOKE_DEFAULT')