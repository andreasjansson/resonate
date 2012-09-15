Resonate
========

Example usage:

    from resonate import resonate
    resonated_signals, rms, max_rms = resonate(audio, sample_rate, freqs, damping, springiness, rms_window_width)

`damping` should probably be somewhere around _8000.0_, `springiness` probably around _20.0_.

Here's one way of generating a list of frequencies corresponding to C3 - B4:

    import numpy as np
    c3 = 130.8128
    nfreqs = 12 * 2
    freqs = c3 * np.power(2, np.arange(nfreqs) / 12.0)
    freqs = freqs.tolist()
    
Normalisation
-------------

Lower frequencies tend to resonate stronger than high frequencies, so you'll probably want to normalise it by
multiplying the inputs with some function. I have yet to find a function that fits. The following function gets
somewhere along the way, but unfortunately far from linear:

    y = resonated_signal * math.pow((nfreqs - i) / float(nfreqs), 1.4)

Here is an RMS plot of the first 30 seconds of a synthesised version of Bach's Suite BWV 1006a:

![Example RMS plot](https://github.com/andreasjansson/resonate/raw/master/example_rms_plot.png)

Installation
------------

To install the module, run

    sudo python setup.py install
    
If you just want to build the module, run

    python setup.py build_ext -i
