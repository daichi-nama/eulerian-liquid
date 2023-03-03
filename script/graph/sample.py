import argparse
from typing import List

import matplotlib.pyplot as plt
import numpy as np


def create(inputs: List[float], labels: List[str],  output_name: str, title: str = '', xlabel: str = '', ylabel: str = ''):
    plt.figure()
    for i in range(0, len(inputs)):
        data = np.loadtxt(inputs[i])
        plt.plot(data, label=labels[i], marker='.', markersize=10)

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend(bbox_to_anchor=(1, 1), loc='upper right')
    plt.grid()
    plt.tight_layout()
    plt.savefig(output_name + '.pdf', bbox_inches="tight")
    plt.savefig(output_name + '.png', bbox_inches="tight")
    plt.clf()
