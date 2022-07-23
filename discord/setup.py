from subprocess import run
from os.path import join
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
        raise AssertionError('Tests failed.')
    chdir('../discord')


def build_ext(name, version, sources, **kwargs):
    ext = Extension(name, sources, extra_compile_args=['-std=c++20'],
                    include_dirs=['../src'],
                    library_dirs=[BUILD_DIR], **kwargs)
    setup(name=name, version=version, ext_modules=[ext])

def build_all(version, **kwargs):
    for name in listdir('cc'):
        if name.endswith('.cc'):
            build_ext(name[:-3], version, [join('cc', name)], **kwargs)


def post():
    lib_path = None
    for f in listdir('build'):
        if f.startswith('lib'):
            lib_path = join('build', f)
    if not lib_path:
        raise FileNotFoundError('Could not find lib directory for cc modules.')
    for f in listdir(lib_path):
        if f.endswith('.so'):
            copy(join(lib_path, f), f)


build_c()
build_all('1.0', language='c++', libraries=['Mahjong'])
post()
