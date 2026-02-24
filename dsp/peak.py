import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

f_s = 96000.0 / 2.0

g = 6
q = 10
f_c = 800

a = np.exp(g * np.log(10) / 40)
w_0 = 2 * np.pi * f_c / f_s
alpha = np.sin(w_0) / (2 * q)

# Audio EQ cookbook

b_0 = 1 + alpha * a
b_1 = -2 * np.cos(w_0)
b_2 = 1 - alpha * a

a_0 = 1 + alpha / a
a_1 = -2 * np.cos(w_0)
a_2 = 1 - alpha / a

plot = True
generate_c = True

b = np.array([b_0, b_1, b_2]) / a_0
a = np.array([a_0, a_1, a_2]) / a_0

print(b, a)

if plot:
    w, h = signal.freqz(b, a, worN=8000)

    normalized_freq = w / np.pi * f_s / 2.0
    magnitude_db = 20 * np.log10(np.abs(h) + 1e-12)

    plt.plot(normalized_freq, magnitude_db, "b-", label="Filter Response")

    plt.axvline(f_c, color="r", linestyle="--", label=f"Ideal Cutoff ({f_c} Nyquist)")
    plt.grid(True, which="both", linestyle="-", alpha=0.3)

    plt.ylim((-20., 20.))
    plt.xlim((20, 20000))
    plt.xscale("log")

    plt.show()

if generate_c:
    bit_shift = 1
    b /= 1 << bit_shift
    a /= 1 << bit_shift

    q15_b = np.clip(np.round(b * 32768.0), -32768, 32767).astype(np.int16)
    q15_a = np.clip(np.round(a * 32768.0), -32768, 32767).astype(np.int16)

    print(f"{q15_b[0]}, 0, {q15_b[1]}, {q15_b[2]}, {-q15_a[1]}, {-q15_a[2]}")
