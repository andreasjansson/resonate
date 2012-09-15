Resonate
========

Example usage:

    from resonate import resonate
    resonated_signals, rms, max_rms = resonate(audio, sample_rate, freqs, damping, springiness, rms_window_width)

`damping` should probably be somewhere around _8000.0_, `springiness` probably around _20.0_.

Here's one way of generating a list of frequencies corresponding to C3 - B4:

    import numpy as np
    c3 = 130.8128
    npitches = 12 * 2
    freqs = c0 * np.power(2, np.arange(npitches) / 12.0)
    freqs = freqs.tolist()

Lower frequencies tend to resonate stronger than high frequencies, so you'll probably want to normalise it by
multiplying the inputs with some function. I have yet to find a function that fits.

