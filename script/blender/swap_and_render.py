import argparse
import sys
from pathlib import Path

import bpy

DIGITS_NUM = 4


def set_smooth_shading(mesh: bpy.types.Mesh):
    for polygon in mesh.polygons:
        polygon.use_smooth = True


def set_output_properties(output_file_path: str,
                          progressive: int):
    bpy.context.scene.render.filepath = output_file_path
    bpy.context.scene.render.resolution_x = int(progressive*16/9)
    bpy.context.scene.render.resolution_y = progressive


def set_cycles_renderer(num_samples: int) -> None:
    bpy.context.scene.render.image_settings.file_format = 'PNG'
    bpy.context.scene.render.engine = 'CYCLES'
    bpy.context.scene.view_layers[0].cycles.use_denoising = True
    bpy.context.scene.cycles.samples = num_samples
    bpy.context.scene.cycles.device = "GPU"
    bpy.context.preferences.addons["cycles"].preferences.compute_device_type = "CUDA"
    bpy.context.preferences.addons["cycles"].preferences.get_devices()
    for d in bpy.context.preferences.addons["cycles"].preferences.devices:
        d["use"] = 1


def main():
    parser = argparse.ArgumentParser(description='Render')
    parser.add_argument('--blend')
    parser.add_argument('-i', '--input')
    parser.add_argument('-o', '--output')
    parser.add_argument('--progressive', type=int, default=1080)
    parser.add_argument('--samples', type=int, default=16)
    args = parser.parse_args(sys.argv[sys.argv.index('--') + 1:])

    bpy.ops.wm.open_mainfile(filepath=args.blend)

    output_file_path = args.output + '_' + '#'*DIGITS_NUM
    input_file_path = Path(args.input)

    sample_obj = bpy.data.objects["Sample"]
    # https://blender.stackexchange.com/questions/108110/getting-an-imported-objects-name
    old_objs = set(bpy.context.scene.objects)
    bpy.ops.import_scene.obj(filepath=str(input_file_path))
    imported_objs = set(bpy.context.scene.objects) - old_objs
    imported_obj_name = list(imported_objs)[0].data.name

    bpy.ops.transform.translate(value=sample_obj.location)
    bpy.data.objects[imported_obj_name].active_material = sample_obj.active_material.copy()
    set_smooth_shading(bpy.data.objects[imported_obj_name].data)
    bpy.data.objects.remove(sample_obj)

    set_output_properties(output_file_path, args.progressive)
    set_cycles_renderer(args.samples)


if __name__ == '__main__':
    main()
