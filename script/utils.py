import shutil
import sys
from pathlib import Path
from typing import List


def reset_directory_if_exists(dir: Path, yes: bool = False):
    if dir.exists():
        if yes:
            shutil.rmtree(dir)
            dir.mkdir(exist_ok=False)
            return
        while True:
            choice = input('Overwrite "' +
                           str(dir) + '"? [y/n]: ').lower()
            if choice in ['y']:
                shutil.rmtree(dir)
                dir.mkdir(exist_ok=False)
                break
            elif choice in ['n']:
                sys.exit()


def reset_directories_if_exist(dirs: List[Path], yes: bool = False):
    existing_dirs = []
    for d in dirs:
        if d.exists():
            existing_dirs.append(d)
    if len(existing_dirs) == 0:
        return

    if yes:
        for d in existing_dirs:
            shutil.rmtree(d)
            return
    while True:
        print('Overwrite the following directories?')
        for d in existing_dirs:
            print(f'- {d}')
        choice = input('[y/n]: ').lower()
        if choice in ['y']:
            for d in existing_dirs:
                shutil.rmtree(d)
            break
        elif choice in ['n']:
            sys.exit()


def overwrite_warning_if_exists(dir: Path, yes: bool = False):
    if dir.exists():
        if yes:
            return
        while True:
            choice = input('Overwrite "contents" in "' +
                           str(dir) + '"? [y/n]: ').lower()
            if choice in ['y']:
                break
            elif choice in ['n']:
                sys.exit()
