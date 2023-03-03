import argparse
import subprocess
from pathlib import Path

import movie_generator
import paths

import utils

DIGITS_NUM = 4


def render(blend_file_path: Path, script_path: Path, frame: int,
           input_mesh_path: Path, output_image_path: Path,
           progressive: int = 1080, samples: int = 16):
    command = 'blender --background -noaudio --python '
    command += str(script_path) + ' '
    command += '--log-level 0 '
    command += '--render-frame ' + str(frame) + ' -- '
    command += '--blend ' + str(blend_file_path) + ' '
    command += '--input ' + str(input_mesh_path) + ' '
    command += '--output ' + str(output_image_path) + ' '
    command += '--progressive ' + str(progressive) + ' '
    command += '--samples ' + str(samples)
    print(command)
    subprocess.run(command, shell=True, check=True)


def render_movie(blend_file_name: str, script_name: str, input_mesh_dir: Path, outdir: Path, name: str,
                 progressive: float = 1080, samples: int = 16, yes: bool = False):
    image_output_dir = outdir / name
    utils.reset_directory_if_exists(image_output_dir, yes)

    image_output_dir.mkdir()
    meshes = sorted(input_mesh_dir.glob('*.obj'))
    if len(meshes) == 0:
        print('No mesh')
    for frame, mesh_name in enumerate(meshes):
        render(paths.BLENDER_DIR/(blend_file_name+'.blend'),
               paths.BLENDER_DIR/(script_name+'.py'),
               frame, mesh_name, image_output_dir/name,
               progressive=progressive, samples=samples)

    movie_generator.generate_movie(
        image_output_dir, name, DIGITS_NUM, outdir / (name+'.mp4'))


def main():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-b', '--blend_name')
    parser.add_argument('-s', '--script_name')
    parser.add_argument('-i', '--indir', type=Path)
    parser.add_argument('-o', '--outdir', type=Path)
    parser.add_argument('--name')
    parser.add_argument('--yes', action='store_true')
    args = parser.parse_args()

    render_movie(args.blend_name, args.script_name,
                 args.indir, args.outdir, args.name, yes=args.yes)


if __name__ == '__main__':
    main()
