Resonate
========

A Python module that drives an array of resonators with an input signal and
returns the resulting displacements, their RMS amplitude over an arbitrary window,
and the overall peak RMS amplitude.

Example usage:

    from resonate import resonate
    resonated_signals, rms, max_rms = resonate(audio, sample_rate, freqs, damping, spring_constant, rms_window_width)

`audio` and `freq` can be standard Python lists, or NumPy arrays.
`audio` needs to be a single (mono) signal.
`damping` should probably be somewhere around _8000.0_, `spring_constant` probably around _20.0_.
`rms_window_width` is specified in number of samples.

Here's one way of generating a list of frequencies corresponding to C3 - B4:

    import numpy as np
    c3 = 130.8128
    nfreqs = 12 * 2
    freqs = c3 * np.power(2, np.arange(nfreqs) / 12.0)
    freqs = freqs.tolist()
    
Here is an RMS plot of a 30 seconds, 2 octave extract of a synthesised version of Bach's Suite BWV 1006a:

![Example RMS plot](https://github.com/andreasjansson/resonate/raw/master/example_rms_plot.png)

Installation
------------

To install the module, run

    sudo python setup.py install
    
If you just want to build the module, run

    python setup.py build_ext -i
