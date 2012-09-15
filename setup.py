from distutils.core import setup, Extension

setup(
    name="resonate",
    ext_modules = [Extension("resonate", ["resonate.c"])],
    install_requires = ['scipy'],

    author = 'Andreas Jansson',
    author_email = 'andreas@jansson.me.uk',
    description = ('Python module for running a signal through an array of tuned resonators'),
    license = 'GPL v3',
    keywords = 'music audio dsp pitch',
    
    )
