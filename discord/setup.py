from subprocess import run
from os.path import split, join, abspath
from os import chdir, listdir
from distutils.core import setup, Extension
from shutil import copy

BUILD_DIR = '../build-release'


def build_c():
    chdir(BUILD_DIR)
    run(['cmake', '..', '-DCMAKE_BUILD_TYPE=Release',
        '-DBUILD_BENCH=OFF', '-DBUILD_TESTS=ON', '-GNinja'])
    run(['ninja'])
    if run(['ctest', '--output-on-failure']).returncode != 0:
        raise Exception('Tests failed.')
    chdir(split(abspath(__file__))[0])


def build_ext(name, version, sources, **kwargs):
    ext = Extension(name, sources, extra_compile_args=['-std=c++20'],
                    include_dirs=['../src'],
                    library_dirs=[BUILD_DIR], **kwargs)
    setup(name=name, version=version, ext_modules=[ext])


def post():
    lib_path = None
    for f in listdir('build'):
        if f.startswith('lib'):
            lib_path = join('build', f)
    if not lib_path:
        raise SystemError('Could not find lib directory for cc modules.')
    for f in listdir(lib_path):
        if f.endswith('.so'):
            copy(join(lib_path, f), f)


build_c()
build_ext('test', '1.0', ['cc/test.cc'], language='c++',
          libraries=['Mahjong'])
post()
