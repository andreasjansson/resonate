import numpy as np
from resonate import resonate
import scipy.io.wavfile as wavfile
import math
import gc

def square_wave():
    return [1] * 1000 + [-1] * 1000

def sawtooth_wave():
    return list(np.arange(-1, 1, .001))

def oscil(fq, sr, wave_table, winlen, phase):
    phase_incr = fq / sr
    samples = np.array([0] * winlen)
    for i in range(0, winlen):
        phase += phase_incr
        low = math.floor(len(wave_table) * phase)
        high = math.ceil(len(wave_table) * phase)
        mid = (phase % 1) * (high - low) + low
        samples[i] = mid
        if phase >= 1:
            phase -= 1
    return samples, phase

def synthesise(wav_filename, wave_table = square_wave(), sr = 22050, winlen = 1000):
    print 'Reading wav file'
    sr, audio = wavfile.read(wav_filename)
    audio = audio / float(max(audio))
    
    a0 = 27.5
    nfreqs = 12 * 5
    freqs = a0 * np.power(2, np.arange(0, nfreqs) / 12.0)

    print 'Resonating'
    _, rms, max_rms = resonate(audio, sr, freqs, 10000.0, 20.0, winlen)
    _ = None # free memory
    gc.collect()
    print 'Done resonating'

    threshold = max_rms / 2.3
    out = np.array([0] * (len(rms[0]) * winlen))

    print 'Synthesising'
    for i, fq in enumerate(freqs):
        phase = 0
        for t, x in enumerate(rms[i]):
            if x > threshold:
                samples, phase = oscil(fq, sr, wave_table, winlen, phase)
                out[t * winlen : (t + 1) * winlen] += samples
            else:
                phase = 0

    print 'Normalising'
    out = np.array((2 ** 15) * (out / float(max(out))), dtype = np.int16)
    return out

if __name__ == '__main__':
    out = synthesise('test.wav')
    print 'Writing wave file'
    wavfile.write('synthesised.wav', 22050, out)
