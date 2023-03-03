import argparse
from pathlib import Path
from typing import List

import iogrid
import matplotlib.pyplot as plt


def visualize(input: Path, outputs: List[str],
              x_min: float, x_max: float, y_min: float, y_max: float,
              width: float, height: float):
    x, values = iogrid.load_grid_1d(input)
    fig = plt.figure(figsize=(width, height))
    ax = fig.add_subplot(111)
    ax.set_aspect('equal')
    plt.plot(x, values)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.xlim(x_min, x_max)
    plt.ylim(y_min, y_max)
    plt.tight_layout()
    for o in outputs:
        plt.savefig(o)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', type=Path)
    parser.add_argument('-o', '--outputs', nargs='+', type=Path)
    parser.add_argument('--x_min', type=float)
    parser.add_argument('--x_max', type=float)
    parser.add_argument('--y_min', type=float)
    parser.add_argument('--y_max', type=float)
    parser.add_argument('--width', type=float)
    parser.add_argument('--height', type=float)
    args = parser.parse_args()
    visualize(args.input, args.outputs,  args.x_min, args.x_max,
              args.y_min, args.y_max, args.width, args.height)


if __name__ == '__main__':
    main()
