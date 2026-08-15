
import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf

ir, sr = sf.read("C:/Program Files/Image-Line/FL Studio 2025/Data/Patches/Impulses/ir_test_inverse_decay.wav"
)
fft = np.abs(np.fft.rfft(ir))

freq = np.fft.rfftfreq(len(ir), 1/sr)

plt.semilogx(freq, 20*np.log10(fft / np.max(fft)))
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude (dB)")
plt.grid()
plt.show()