"""
 * =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 *
"""

import os
import subprocess
import tarfile


def get_previous_regression_files():
    return None


def get_source_files():
    nitf_home = os.environ.get('JENKINS_NITF_LOCATION', None)
    if nitf_home is None:
        raise EnvironmentError(
            'JENKINS_NITF_LOCATION environment variable not set')

    nitfs = []
    for root, dirs, files in os.walk(nitf_home):
        nitfs.extend([os.path.join(root, nitf)
                      for nitf in files if nitf.endswith('.nitf')])
    return [os.path.join(nitf_home, nitf) for nitf in nitfs]


def run_program(progname, nitf):
    program = os.path.join('install', 'bin', progname)
    output_pathname = os.path.basename(nitf)
    output_pathname += "_" + os.path.basename(progname) + ".dump"
    with open(output_pathname, 'w') as f:
        subprocess.call([program, nitf], stdout=f)
    return output_pathname


def show_nitf(nitf, output_files):
    output_files.append(run_program('show_nitf', nitf))


def show_nitf_plus_plus(nitf, output_files):
    output_files.append(run_program('show_nitf++', nitf))


def run_regressions(nitfs):
    output_files = []
    for nitf in nitfs:
        show_nitf(nitf, output_files)
        show_nitf_plus_plus(nitf, output_files)
    return output_files


def tar_files(files, tarname):
    with tarfile.open(tarname, 'w:gz') as tar:
        for name in files:
            tar.add(name)


def remove_files(files):
    for item in files:
        os.remove(item)


def main():
    regression_files = get_previous_regression_files()
    nitfs = get_source_files()
    output_files = run_regressions(nitfs)
    if regression_files:
        # TODO: Run regressions
        raise NotImplementedError('Not implemented yet!')
    else:
        tar_files(output_files, 'nitro_gold.tar.gz')
        remove_files(output_files)


if __name__ == '__main__':
    main()
