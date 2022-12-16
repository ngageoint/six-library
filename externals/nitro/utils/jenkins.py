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

import filecmp
import os
import shutil
import subprocess
import sys
import tarfile

"""
Jenkins script for NITRO.
The known good results are in nitro_gold.tar.gz.
If this file does not exist (Jenkins should handle this),
this script will create it. If the tarball does exist,
this script will run show_nitf on a collection of NITFs
(and possibly other tasks in the future) and compare the output.
"""


GOLD = 'nitro_gold.tar.gz'
NITF_VARNAME = 'JENKINS_NITF_LOCATION'
GOLD_DIR = 'expected'


def get_previous_regression_files():
    if os.path.exists(GOLD):
        with tarfile.open(GOLD) as tar:
            nitfs = tar.getnames()
            def is_within_directory(directory, target):
                
                abs_directory = os.path.abspath(directory)
                abs_target = os.path.abspath(target)
            
                prefix = os.path.commonprefix([abs_directory, abs_target])
                
                return prefix == abs_directory
            
            def safe_extract(tar, path=".", members=None, *, numeric_owner=False):
            
                for member in tar.getmembers():
                    member_path = os.path.join(path, member.name)
                    if not is_within_directory(path, member_path):
                        raise Exception("Attempted Path Traversal in Tar File")
            
                tar.extractall(path, members, numeric_owner=numeric_owner) 
                
            
            safe_extract(tar)
        os.mkdir(GOLD_DIR)
        for nitf in nitfs:
            shutil.move(nitf, GOLD_DIR)
        return [os.path.join(GOLD_DIR, nitf) for nitf in nitfs]
    else:
        # No known good files, so we'll generate them later for this job
        return []


def get_source_files():
    nitf_home = os.environ.get(NITF_VARNAME, None)
    if nitf_home is None:
        raise EnvironmentError(
            '{} environment variable not set'.format(NITF_VARNAME))

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


def compare_output(expected, actual):
    success = True
    for expected_nitf in expected:
        basename = os.path.basename(expected_nitf)
        if basename not in actual:
            raise EnvironmentError(
                'Cannot find generated file corresponding to {}'.format(expected_nitf))
        print('Comparing {} and {}'.format(expected_nitf, basename))
        if not filecmp.cmp(expected_nitf, basename):
            print('Files {} and {} differ'.format(expected_nitf, basename))
            success = False

    if success:
        print('Test passed')
    else:
        print('Test failed')
    return success


def main():
    return_code = 0
    regression_files = get_previous_regression_files()
    nitfs = get_source_files()
    output_files = run_regressions(nitfs)
    if regression_files:
        if not compare_output(regression_files, output_files):
            return_code = -1
    else:
        # First run: generate initial files
        tar_files(output_files, GOLD)

    if os.path.isdir(GOLD_DIR):
        shutil.rmtree(GOLD_DIR)
    remove_files(output_files)
    sys.exit(return_code)


if __name__ == '__main__':
    main()
