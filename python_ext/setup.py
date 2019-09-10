from setuptools import setup, Extension

VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = '.'.join(map(str, [VERSION_MAJOR,
                             VERSION_MINOR,
                             VERSION_PATCH]))

cvarda = Extension('cvarda',
                   sources = ['wrapper.c'],
                   define_macros = [('VERSION_MAJOR', VERSION_MAJOR),
                                    ('VERSION_MINOR', VERSION_MINOR),
                                    ('VERION_PATCH', VERSION_PATCH)],
                   extra_compile_args = ['-Wextra',
                                         '-Wpedantic',
                                         '-std=c99'])

setup(name = 'cvarda',
      version = VERSION,
      description = 'Variant frequency database',
      ext_modules = [cvarda])
