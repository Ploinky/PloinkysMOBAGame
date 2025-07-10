import bpy;
import sys;

argv = sys.argv
argv = argv[argv.index("--") + 1:]  # get all args after "--"

filepath = argv[0]

bpy.ops.export_scene.gltf(filepath=filepath, use_visible=True, export_apply=True)