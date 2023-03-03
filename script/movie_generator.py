import argparse
import subprocess
from pathlib import Path


def generate_movie(indir: Path, name: str, digits: int, output: Path):
    input = Path(indir) / name
    # command = ('ffmpeg -loglevel warning -nostdin -y -r 60 -i ' + str(input) +
    #            '_%0' + str(digits) + 'd.png '
    #            '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
    #            + str(output))
    command = ('ffmpeg -loglevel warning -nostdin -y -r 60 -i ' + str(input) +
               '_%0' + str(digits) + 'd.png '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" '
               + str(output))
    subprocess.run(command, shell=True, check=True)


def main():
    parser = argparse.ArgumentParser(
        description='Movie maker from serial number images')
    parser.add_argument('--indir')
    parser.add_argument('-n', '--name')
    parser.add_argument('-d', '--digits', default=3)
    parser.add_argument('-o', '--output')
    args = parser.parse_args()

    generate_movie(args.indir, args.name, args.digits, args.output)


if __name__ == '__main__':
    main()
