from setuptools import setup, Extension


VERSION_MAJOR = 1
VERSION_MINOR = 1
VERSION_PATCH = 10
VERSION = '.'.join(map(str, [VERSION_MAJOR,
                             VERSION_MINOR,
                             VERSION_PATCH]))


cvarda = Extension('cvarda',
                   sources = ['python_ext/wrapper.c',
                              'python_ext/helpers.c',
                              'src/ascii_trie.c',
                              'src/avl_tree.c',
                              'src/cov_table.c',
                              'src/itv_tree.c',
                              'src/mnv_table.c',
                              'src/mnv_tree.c',
                              'src/seq_table.c',
                              'src/snv_table.c',
                              'src/snv_tree.c',
                              'src/trie.c',
                              'src/utils.c'],
                   define_macros = [('VRD_VERSION_MAJOR', VERSION_MAJOR),
                                    ('VRD_VERSION_MINOR', VERSION_MINOR),
                                    ('VRD_VERSION_PATCH', VERSION_PATCH)],
                   extra_compile_args = ['-Wextra',
                                         '-Wpedantic',
                                         '-std=c99'])


setup(name = 'cvarda',
      version = VERSION,
      description = 'Varda2 Variant frequency database C library',
      ext_modules = [cvarda])
