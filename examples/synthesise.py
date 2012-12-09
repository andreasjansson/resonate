import numpy as np
from resonate import resonate
import scipy.io.wavfile as wavfile
import math
import sys
import gc

def square_wave():
    return [1] * 1000 + [-1] * 1000

def sawtooth_wave():
    return list(np.arange(-1, 1, .001))

def oscil(fq, sr, wave_table, winlen, phase):
    phase_incr = fq / sr
    samples = np.array([0] * winlen, dtype = np.float)
    for i in range(0, winlen):
        low_index = int(len(wave_table) * phase)
        low = wave_table[low_index]
        high_index = low_index % len(wave_table)
        high = wave_table[high_index]
        mid = (phase % 1) * (high - low) + low
        samples[i] = mid
        phase += phase_incr
        if phase >= 1:
            phase -= 1
    return samples, phase

def synthesise(wav_filename, wave_table = sawtooth_wave(), sr = 22050, winlen = 1000):
    print 'Reading wav file'
    sr, audio = wavfile.read(wav_filename)
    audio = list(audio / float(max(audio)))
    
    a0 = 27.5
    nfreqs = 12 * 5
    freqs = a0 * np.power(2, np.arange(0, nfreqs) / 12.0)

    print 'Resonating'
    rms, max_rms = resonate(audio, sr, freqs, 10000.0, 19.0, winlen, False)
    audio = None
    gc.collect()

    threshold = max_rms / 2.4
    out = np.array([0] * (len(rms[0]) * winlen))

    for i, fq in enumerate(freqs):
        sys.stdout.write('\rSynthesising %3d%%' % (100 * i / float(len(freqs))))
        sys.stdout.flush()
        phase = 0
        for t, x in enumerate(rms[i]):
            if x > threshold:
                samples, phase = oscil(fq, sr, wave_table, winlen, phase)
                samples *= x
                out[t * winlen : (t + 1) * winlen] += samples
            else:
                phase = 0

    print '\nNormalising'
    out = np.array((2 ** 15 - 1) * (out / float(max(np.abs(out)))), dtype = np.int16)
    return out

if __name__ == '__main__':
    out = synthesise('test.wav')
    print 'Writing wave file'
    wavfile.write('synthesised.wav', 22050, out)
