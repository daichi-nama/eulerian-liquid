import argparse
import datetime
import subprocess
from pathlib import Path
from typing import List

import graph_creator
import paths
import render
import text
import tile
import yaml

import utils


def perform_experiment(experiments: list, output: Path,  is_server: bool):
    for e in experiments:
        command = e['command']
        command = str(paths.PROJECT_DIR) + '/' + command
        if is_server:
            command = 'xvfb-run --auto-servernum --server-args="-screen 0 1920x1080x24" ' + command
            # command = 'xvfb-run --auto-servernum --server-args="-screen 0 3840x2160x24" ' + command
        command += ' --full_path -o ' + str(output / e['output'])

        print(command)
        subprocess.run(command, shell=True, check=True)


def create_movie(obj: dict, output: Path, option: str):
    if 'text' in obj:
        print('Adding text ...')
        for e in obj['experiment']:
            if 'text' in e:
                for t in obj['text']:
                    text_in = e['text']
                    if option is not None:
                        text_in += '\r\n' + option
                    text.add_text(
                        output / e['output'] / (t['name']+'.mp4'),
                        output / e['output'] / (t['name'] + '_text.mp4'),
                        e['text'], yes=True)

    if 'movie' in obj:
        print('Creating movies ...')
        for m in obj['movie']:
            inputs = []
            for e in obj['experiment']:
                for i in m['inputs']:
                    inputs.append(output / e['output'] / (i['name'] + '.mp4'))

            file_name = m['output']
            if option is not None:
                file_name += '_' + option.replace('--', '').replace(' ', '_')
            tile.create_tile_movie(inputs,
                                   output / (file_name + '.mp4'), False, True)


def create_graph(obj: dict, output: Path, option: str):
    if 'graph' in obj:
        print('Creating graphs ...')
        for g in obj['graph']:
            inputs = []
            labels = []
            for e in obj['experiment']:
                for i in g['inputs']:
                    inputs.append(
                        output / e['output'] / 'data' / (i['name'] + '.csv'))

                labels.append(e['label'] if 'label' in e else e['output'])

            title = ''
            if 'title_confirmed' in g:
                title = g['title_confirmed']
            elif 'title' in g:
                title = g['title']
                if option is not None:
                    title += '\n' + option

            file_name = g['output']
            if option is not None:
                file_name += '_' + option.replace('--', '').replace(' ', '_')

            graph_creator.create(g['script'], inputs, labels, str(output / file_name),
                                 title=title,
                                 xlabel=g['xlabel'] if 'xlabel' in g else '',
                                 ylabel=g['ylabel'] if 'ylabel' in g else '')


def generate_rendered_movie(obj: dict, output: Path, option: str):
    if 'render' in obj:
        print('Rendering ...')
        for r in obj['render']:
            blend_name = r['blend']
            script_name = r['script']
            progressive = r['progressive'] if 'progressive' in r else 1080
            samples = r['samples'] if 'samples' in r else 16
            for e in obj['experiment']:
                indir = output / e['output'] / r['input']
                outdir = output / e['output']
                movie_name = r['output']
                render.render_movie(blend_name, script_name,
                                    indir, outdir, movie_name,
                                    progressive=progressive,
                                    samples=samples)


def has_duplicate_element(arr: list) -> bool:
    return len(arr) != len(set(arr))


def has_duplicate_experiment_name(obj: dict) -> bool:
    seq = [e['output'] for e in obj['experiment']]
    return has_duplicate_element(seq)


def has_duplicate_graph_name(obj: dict) -> bool:
    seq = [g['output'] for g in obj['graph']]
    return has_duplicate_element(seq)


def has_duplicate_rendered_movie_name(obj: dict) -> bool:
    seq = [r['output'] for r in obj['render']]
    return has_duplicate_element(seq)


def check_yml_is_valid(obj: dict):
    assert(has_duplicate_experiment_name(obj) == False)
    if 'graph' in obj:
        assert(has_duplicate_graph_name(obj) == False)
    if 'render' in obj:
        assert(has_duplicate_rendered_movie_name(obj) == False)


def get_yml_file_path(input: str, day: str = None) -> Path:
    input_file_name = Path(input).name
    if day is None:
        return paths.YML_DIR / (input_file_name + '.yml')
    else:
        return paths.YML_ARCHIVE_DIR / day / (input_file_name + '.yml')


def get_output_directory(input: str, day: str = None, name: str = None,
                         option: str = None, master: bool = False) -> Path:
    output = None
    if master:
        output = paths.OUT_DIR / 'master'
    elif day is not None:
        output = paths.ARCHIVE_DIR / day
    else:
        output = paths.ARCHIVE_DIR / datetime.date.today().strftime('%Y%m%d')

    if name is None:
        output /= Path(input).name
    else:
        output /= name

    if option is not None:
        option_name = option.replace('--', '').replace(' ', '_')
        output = Path(str(output) + '_' + option_name)

    return output


def prepare_directories(obj: dict, experiments: dict, output: Path, partial: bool,
                        render: bool = False, movie: bool = False, graph: bool = False, yes: bool = False):
    should_perform_experiment = (movie == False) and (
        graph == False) and (render == False)

    if should_perform_experiment:
        if partial:
            dirs = [output / str(e['output']) for e in experiments]
            utils.reset_directories_if_exist(dirs, yes)
        else:
            utils.reset_directory_if_exists(output, yes)
    else:
        if movie or graph:
            utils.overwrite_warning_if_exists(output, yes)
        if render:
            dirs = []
            for r in obj['render']:
                for e in obj['experiment']:
                    outdir = output / e['output'] / r['output']
                    dirs.append(outdir)
            utils.reset_directories_if_exist(dirs, yes)


def run(obj: dict, experiments: dict, output: Path, option: str = None, server: bool = False,
        render: bool = False, movie: bool = False, graph: bool = False):
    should_perform_experiment = (movie == False) and (
        graph == False) and (render == False)

    if should_perform_experiment:
        perform_experiment(experiments, output, server)
        generate_rendered_movie(obj, output, option)
        create_movie(obj, output, option)
        create_graph(obj, output, option)
    else:
        if render:
            generate_rendered_movie(obj, output, option)
        if movie:
            create_movie(obj, output, option)
        if graph:
            create_graph(obj, output, option)


def main():
    day = datetime.date.today().strftime('%Y%m%d')
    parser = argparse.ArgumentParser(description='Tester')
    parser.add_argument('input')
    parser.add_argument('-d', '--day')
    parser.add_argument('-o', '--output')

    parser.add_argument('--movie', action='store_true')
    parser.add_argument('--graph', action='store_true')
    parser.add_argument('--render', action='store_true')

    parser.add_argument('--server', action='store_true')
    parser.add_argument('--yes', action='store_true')

    parser.add_argument('--option')
    parser.add_argument('--indices', nargs='+', type=int)
    parser.add_argument('--master', action='store_true')

    args = parser.parse_args()
    partial = args.indices is not None
    if partial:
        assert(has_duplicate_element(args.indices) == False)

    with open(get_yml_file_path(args.input, args.day)) as file:
        obj = yaml.safe_load(file)

        check_yml_is_valid(obj)

        output = get_output_directory(
            args.input, args.day, args.output, args.option, args.master)

        experiments = obj['experiment']
        if args.option != None:
            for e in experiments:
                e['command'] += ' ' + args.option
        if partial:
            experiments = [experiments[i] for i in args.indices]

        prepare_directories(obj, experiments, output, partial=partial,
                            render=args.render, movie=args.movie, graph=args.graph, yes=args.yes)

        run(obj, experiments, output, option=args.option,
            server=args.server, render=args.render, movie=args.movie, graph=args.graph)


if __name__ == '__main__':
    main()
