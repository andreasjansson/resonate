import unittest2 as unittest
import numpy as np
from resonate import resonate

class TestResonate(unittest.TestCase):

    def test_zeros(self):
        signal = [0] * 6
        rms, response = resonate(signal, sr=5, freqs=[10], damping=0, rms_window=2,
                                          return_response=True)
        self.assertEquals(rms, [[0] * 3])
        self.assertEquals(response, [[0] * 6])

    def test_numpy_array(self):
        signal = np.zeros(6)
        rms, response = resonate(signal, sr=5, freqs=[10], damping=0, rms_window=2,
                                          return_response=True)
        self.assertEquals(rms.tolist(), [[0] * 3])
        self.assertEquals(response.tolist(), [[0] * 6])

    def test_resonance(self):
        sr = 200
        signal = np.sin(2 * np.pi * 8 * np.arange(200) / sr)
        rms = resonate(signal=signal,
                       sr=sr,
                       freqs=range(1, 20),
                       damping=0.001,
                       rms_window=20)

        self.assertEquals(np.argmax(np.max(rms, 1)), 7)

if __name__ == '__main__':
    unittest.main()
