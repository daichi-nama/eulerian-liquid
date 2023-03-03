from pathlib import Path

import numpy as np


def load_grid_1d(input: Path):
    x = np.loadtxt(input, delimiter=',', skiprows=0, max_rows=1)
    values = np.loadtxt(input, delimiter=',', skiprows=1)
    return x, values


def load_grid_2d(input: Path):
    X = np.loadtxt(input, delimiter=',', skiprows=0, max_rows=1)
    Y = np.loadtxt(input, delimiter=',', skiprows=1, max_rows=1)
    x, y = np.meshgrid(X, Y)
    values = np.loadtxt(input, delimiter=',', skiprows=2)
    return x, y, values
