import bpy
import struct

def export_omp(context, filepath):
    print("Running omp export...")
    
    verts = []
    indices = []
    
    objects = bpy.context.scene.objects
    for obj in objects:
        mesh = obj.data
        
        for face in mesh.polygons:
            indices.append(face.vertices[0])
            indices.append(face.vertices[2])
            indices.append(face.vertices[1])
            
        for vertex in mesh.vertices:
            pos = [vertex.co[0], vertex.co[2], vertex.co[1]]
            verts.append(pos)
                
    with open(filepath, 'wb') as f:
        f.write("omp".encode())
        f.write(struct.pack('i', 1))
        
        f.write(struct.pack('i', len(verts)))
        
        for v in verts:
            f.write(struct.pack('f', v[0]))
            f.write(struct.pack('f', v[1]))
            f.write(struct.pack('f', v[2]))
        
        f.write(struct.pack('i', len(indices)))
        
        for i in indices:
            f.write(struct.pack('i', i))
                
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
    filename_ext = ".omp"

    filter_glob: StringProperty(
        default="*.omp",
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