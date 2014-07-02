from setuptools import setup, Extension

# run the setup
setup(
    name='resonate',
    ext_modules=[
        Extension('resonate._resonate',
                  sources=['src/resonate.c']),
    ],
)
