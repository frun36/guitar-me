import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

interpolator = True

decimate_factor = 2
num_taps = 32

f_c = 1.0 / decimate_factor

plot = False
generate_c = True

if interpolator and num_taps % decimate_factor != 0:
    raise ValueError("For interpolator, num taps must be a multiple of decimate factor!")

coeffs = signal.firwin(num_taps, f_c, window="hamming")
if interpolator:
    coeffs = coeffs * decimate_factor

if plot:
    w, h = signal.freqz(coeffs, worN=8000)

    normalized_freq = w / np.pi
    magnitude_db = 20 * np.log10(np.abs(h) + 1e-12)

    plt.plot(normalized_freq, magnitude_db, "b-", label="Filter Response")

    plt.axvline(f_c, color="r", linestyle="--", label=f"Ideal Cutoff ({f_c} Nyquist)")
    plt.ylim(-100, 10)
    plt.xlim(0, 1)
    plt.grid(True, which="both", linestyle="-", alpha=0.3)

    plt.show()

if generate_c:
    q15_coeffs = np.clip(np.round(coeffs * 32768.0), -32768, 32767).astype(np.int16)
    print(f"const q15_t FIR_COEFFS[{num_taps}] = {{")
    for c in q15_coeffs:
        print(f"    {c},")
    print("};")
