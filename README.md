Resonate
========

A Python module that drives an array of resonators with an input signal and
returns the resulting displacements, their RMS amplitude over an arbitrary window,
and the overall peak RMS amplitude. The effect is wavelet-like, but faster since the phase is adaptive.

Example usage:

    from resonate import resonate
    rms, max_rms = resonate(audio, sample_rate, freqs, damping, rms_window_width)

`audio` and `freq` can be standard Python lists, or NumPy arrays.
`audio` needs to be a single (mono) signal.
`damping` should probably be somewhere around _0.002_.
`rms_window_width` is specified in number of samples.

If you want the displacements back, pass True as a final argument:

    displacements, rms, max_rms = resonate(audio, sample_rate, freqs, damping, rms_window_width, True)

Doing this will take lots of memory.

Here is an RMS plot of a 30 seconds, 2 octave extract of a synthesised version of Bach's Suite BWV 1006a:

![Example RMS plot](https://github.com/andreasjansson/resonate/raw/master/example_rms_plot.png)

Installation
------------

To install the module, run

    sudo python setup.py install
    
If you just want to build the module, run

    python setup.py build_ext -i

References
----------

 * https://instruct1.cit.cornell.edu/courses/bionb441/FinalProjects/f2006/knr9/Final%20Project/KevinRohmannBioNB441FinalProject.htm
 * http://jp.physoc.org/content/312/1/377.full.pdf
 * http://www.cs.otago.ac.nz/research/publications/oucs-2008-03.pdf
