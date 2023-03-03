import argparse
import os
import subprocess
from pathlib import Path

import paths
import text


def movie2_h(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "hstack" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie2_v(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "vstack" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie3_h(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v]hstack=inputs=3[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie3_v(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v]vstack=inputs=3[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie4(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v]xstack=inputs=4:layout=0_0|w0_0|0_h0|w0_h0[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie4_h(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v]hstack=inputs=4[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie5_h(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-i ' + str(input[4]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v][4:v]hstack=inputs=5[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie6(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-i ' + str(input[4]) + ' '
               '-i ' + str(input[5]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v]\
               xstack=inputs=6:layout=0_0|w0_0|w0+w3_0|0_h0|w0_h0|w0+w3_h0[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie8(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-i ' + str(input[4]) + ' '
               '-i ' + str(input[5]) + ' '
               '-i ' + str(input[6]) + ' '
               '-i ' + str(input[7]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v]\
               xstack=inputs=8:layout=0_0|w0_0|w0+w1_0|w0+w1+w2_0|0_h0|w0_h0|w0+w1_h0|w0+w1+w2_h0[v]" '
               # xstack=inputs=8:layout=0_0|0_h0|w0_0|w0_h0|w0+w1_0|w0+w1_h0|w0+w1+w2_0|w0+w1+w2_h0[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie9(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 '
               '-i ' + str(input[0]) + ' '
               '-i ' + str(input[1]) + ' '
               '-i ' + str(input[2]) + ' '
               '-i ' + str(input[3]) + ' '
               '-i ' + str(input[4]) + ' '
               '-i ' + str(input[5]) + ' '
               '-i ' + str(input[6]) + ' '
               '-i ' + str(input[7]) + ' '
               '-i ' + str(input[8]) + ' '
               '-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
               '-filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v][8:v]\
               xstack=inputs=9:layout=0_0|w0_0|w0+w3_0|0_h0|w0_h0|w0+w3_h0|0_h0+h3|w0_h0+h3|w0+w3_h0+h3[v]" '
               '-map "[v]" '
               + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def movie16(input, output_path, yes):
    command = ('ffmpeg -loglevel warning -r 60 ')
    for num in range(16):
        command += '-i ' + str(input[num]) + ' '

    command += ('-vcodec libx264 -pix_fmt yuv420p -b:v 12000k '
                '-filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v][8:v][9:v][10:v][11:v][12:v][13:v][14:v][15:v]\
               xstack=inputs=16:layout=0_0|w0_0|w0+w1_0|w0+w1+w2_0|0_h0|w0_h0|w0+w1_h0|w0+w1+w2_h0|0_h0+h4|w0_h0+h4|w0+w1_h0+h4|w0+w1+w2_h0+h4|0_h0+h4+h8|w0_h0+h4+h8|w0+w1_h0+h4+h8|w0+w1+w2_h0+h4+h8[v]" '
                '-map "[v]" '
                + str(output_path))
    if yes:
        command += ' -y'
    subprocess.run(command, shell=True, check=True)


def create_tile_movie(inputs, output_path, vertical, yes):
    num = len(inputs)
    if num == 2:
        if vertical:
            movie2_v(inputs, output_path, yes)
        else:
            movie2_h(inputs, output_path, yes)
    elif num == 3:
        if vertical:
            movie3_v(inputs, output_path, yes)
        else:
            movie3_h(inputs, output_path, yes)
    elif num == 4:
        # movie4_h(inputs, output_path, yes)
        movie4(inputs, output_path, yes)
    elif num == 6:
        movie6(inputs, output_path, yes)
    elif num == 8:
        movie8(inputs, output_path, yes)
    elif num == 9:
        movie9(inputs, output_path, yes)
    elif num == 16:
        movie16(inputs, output_path, yes)
    else:
        print('No video was created.  Input number:', num)


def main():
    parser = argparse.ArgumentParser(description='Tile movie maker')

    parser.add_argument('--full_path', action='store_true')
    parser.add_argument('-d', '--day')
    parser.add_argument('--indir')
    parser.add_argument('--movie_dir', action='store_true')
    parser.add_argument('-i', '--inputs', nargs='+')
    parser.add_argument('-o', '--output', default='tile')
    parser.add_argument('--vertical', action='store_true')
    parser.add_argument('--yes', action='store_true')
    parser.add_argument('-t', '--text')
    parser.add_argument('-x', type=int, default='10')
    parser.add_argument('-y', type=int, default='10')
    parser.add_argument('--fontcolor', default='white')
    parser.add_argument('--fontsize', type=int, default='20')

    args = parser.parse_args()
    indir = args.indir
    inputs = args.inputs
    output_path = args.output

    if args.full_path == False:
        outdir = None
        basedir = None
        if args.movie_dir:
            basedir = paths.MOVIE_DIR
        else:
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

        for i in range(len(inputs)):
            inputs[i] = indir / Path(inputs[i] + '.mp4')

        if args.output == 'tile':
            output_path = outdir / \
                (args.output + str(len(args.inputs)) + '.mp4')
        else:
            output_path = outdir / (args.output + '.mp4')

    create_tile_movie(inputs, output_path, args.vertical, args.yes)

    if args.text is not None:
        file_name = output_path.stem
        file_name += '_text.mp4'
        text.add_text(output_path, outdir / file_name,
                      args.text, fontcolor=args.fontcolor, x=args.x, y=args.y,
                      fontsize=args.fontsize, yes=args.yes)


if __name__ == '__main__':
    main()
