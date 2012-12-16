import scipy.io.wavfile as wavfile
import math
import numpy as np
import matplotlib.pyplot as plt
from resonate import resonate

def plot_resonators():

    c2 = 65.4064 * 4
    npitches = 22
    freqs = c2 * np.power(2, np.arange(npitches) / 12.0)

    sr, audio = wavfile.read('bwv1080_5_midi.wav')
    audio = audio / float(max(audio))
    audio = audio[0 : 30 * 22050]

    print 'starting resonating'
    rms, max_rms = resonate(audio, sr, freqs, .002, 1000, False)
    print 'done resonating'

    note_names = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']
    
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
        ax.fill_between(x, 0, resonator, color = 'black')
    plt.show()

if __name__ == '__main__':
    plot_resonators()

