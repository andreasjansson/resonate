import scipy.io.wavfile as wavfile
import math
import numpy as np
import matplotlib.pyplot as plt
from resonate import resonate

c3 = 130.8128
npitches = 28
freqs = c3 * np.power(2, np.arange(npitches) / 12.0)

sr, audio = wavfile.read('test.wav')
audio = audio / float(max(audio))

print 'starting resonating'
resons, rms, max_rms = resonate(audio, sr, freqs, 10000.0, 20.0, 2000)
print 'done resonating'

note_names = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']

def plot_resonators(rms, max_rms):
    fig = plt.figure()
    fig.subplots_adjust(hspace=0)
    for i, resonator in enumerate(reversed(rms)):
        resonator = np.array(resonator)
        x = range(len(resonator))
        ax = fig.add_subplot(len(rms), 1, i + 1)
        ax.spines['right'].set_color('none')
        ax.spines['top'].set_color('none')
        ax.spines['left'].set_color('none')
        ax.spines['bottom'].set_color('none')
        ax.get_xaxis().set_ticks([])
        ax.get_yaxis().set_label_text(note_names[(len(rms) - i - 1) % 12])
        ax.get_yaxis().set_ticks([])
        ax.set_ylim(0, max_rms)
        ax.fill_between(x, 0, resonator)
    plt.show()

if __name__ == '__main__':
    plot_resonators(rms, max_rms)

