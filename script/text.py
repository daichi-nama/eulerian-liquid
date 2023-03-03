import argparse
import os
import subprocess
from pathlib import Path

import paths


def add_text(input: Path, output: Path, text: str,
             fontcolor: str = 'white', x: int = 10, y: int = 10,
             fontsize: int = 20, yes: bool = False):
    command = ('ffmpeg -loglevel warning '
               '-i ' + str(input) + ' '
               '-vf drawtext="fontfile=/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc:'
               'text=\'' + text + '\':'
               'fontsize=' + str(fontsize) + ':'
               'fontcolor=white:'
               'x=' + str(x) + ':y=' + str(y) + '" '
               + str(output))
    if yes:
        command += ' -y'
    # print(command)
    subprocess.run(command, shell=True, check=True)


def main():
    parser = argparse.ArgumentParser(description='Text adder')

    parser.add_argument('--full_path', action='store_true')
    parser.add_argument('-d', '--day')
    parser.add_argument('--indir')
    parser.add_argument('-i', '--input')
    parser.add_argument('-o', '--output')
    parser.add_argument('-t', '--text')
    parser.add_argument('-x', '--x', type=int, default='10')
    parser.add_argument('-y', '--y', type=int, default='10')
    parser.add_argument('--fontcolor',  default='white')
    parser.add_argument('--fontsize', type=int, default='20')
    parser.add_argument('--yes', action='store_true')

    args = parser.parse_args()
    indir = args.indir
    input = args.input
    output = args.output
    output_path = args.output

    if input is None:
        raise ValueError('Exception! No input')

    if args.text is None:
        raise ValueError('Exception! No text')

    if output is None:
        output = Path(input).name + '_text'

    if args.full_path == False:
        outdir = None
        basedir = paths.OUT_DIR
        if args.day is not None:
            basedir = paths.ARCHIVE_DIR
            basedir /= args.day

        if indir is None:
            indir = basedir
            outdir = paths.MOVIE_DIR
            outdir.mkdir(parents=True, exist_ok=True)
        else:
            indir = basedir / indir
            outdir = indir

        input = indir / Path(input + '.mp4')
        output_path = outdir / (output + '.mp4')

    add_text(input, output_path, args.text, fontcolor=args.fontcolor, x=args.x,
             y=args.y, fontsize=args.fontsize, yes=args.yes)


if __name__ == '__main__':
    main()
