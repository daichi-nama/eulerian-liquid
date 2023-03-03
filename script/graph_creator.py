import argparse
from typing import List

from graph import *


def create(script_name: str, inputs: List[str], labels: List[str],
           output: str, title: str, xlabel: str, ylabel: str):
    eval(script_name+'.create')(inputs, labels, output,
                                title=title, xlabel=xlabel, ylabel=ylabel)


def main():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-s', '--script_name')
    parser.add_argument('-i', '--inputs', nargs='+')
    parser.add_argument('-l', '--labels', nargs='+')
    parser.add_argument('-o', '--output', default='graph')  # no extension
    parser.add_argument('--title')
    parser.add_argument('--xlabel')
    parser.add_argument('--ylabel')
    args = parser.parse_args()

    create(args.inputs, args.labels, args.output,
           args.title, args.xlabel, args.ylabel)


if __name__ == '__main__':
    main()
