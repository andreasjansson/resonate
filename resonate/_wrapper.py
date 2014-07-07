from resonate import _resonate

def resonate(signal, sr, freqs, damping, rms_window, return_response=False):
    '''
    Args:
        signal          - 1D array used to drive the oscillator network
        sr              - sample rate
        freqs           - list of oscillator frequencies in Hz
        damping         - oscillator damping
        rms_window      - length of the rms window in samples
        return_response - whether to return the oscillator bank response. could be huge

    Returns:
        rms             - matrix of len(freqs) x (len(signal) / rms_window) rms values
                  * or *
        (rms, response) - if return_response=true, where response is a matrix
                          of size len(freqs) x len(signal)

        _if the input signal is a numpy array, the output will be numpy array(s) too_

    Example:

        import numpy as np
        import matplotlib.pyplot as plt
        from resonate import resonate

        # create a signal at 8Hz
        sr = 200
        signal = np.sin(2 * np.pi * 8 * np.arange(200) / sr)

        # resonate it
        rms, response = resonate(signal=signal,
                                sr=sr,
                                freqs=range(6, 10),
                                damping=0.001,
                                rms_window=20,
                                return_response=True)

        # see how the 3rd frequency resonates with the signal
        plt.plot(response.T)
    '''

    if type(signal).__module__ == 'numpy':
        import numpy as np
        return_numpy = True
        signal = signal.tolist()
        if type(freqs).__module__ == 'numpy':
            freqs = freqs.tolist()
    else:
        return_numpy = False

    out = _resonate.resonate(signal, sr, freqs, damping, rms_window, return_response)

    if return_numpy:
        if type(out) == tuple:
            rms, response = out
            return np.array(rms), np.array(response)
        else:
            return np.array(out)

    return out
