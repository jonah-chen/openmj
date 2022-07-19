
from distutils.command.build import build
from subprocess import run
from os.path import split
from os import chdir
from distutils.core import setup, Extension

BUILD_DIR = '../build-release'

def build_c():
    chdir(BUILD_DIR)
    run(['cmake', '..', '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_BENCH=OFF', '-DBUILD_TESTS=ON', '-GNinja'])
    run(['ninja'])
    if run(['ctest', '--output-on-failure']).returncode != 0:
        raise Exception('Tests failed.')
    chdir(split(__file__)[0])

def build_ext(name, version, sources, **kwargs):
    ext = Extension(name, sources, extra_compile_args=['-std=c++20'],
                    include_dirs=['../src'],
                    library_dirs=[BUILD_DIR], **kwargs)
    setup(name='test2', version=version, ext_modules=[ext])

build_c()
build_ext('test', '1.0', ['cc/test.cc'], language='c++',
          libraries=['Mahjong'])
