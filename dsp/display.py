import matplotlib.pyplot as plt
import numpy as np
from abc import ABC, abstractmethod

SAMPLE_FREQ = 192000
DECIMATE_EXP = 2
F_S = SAMPLE_FREQ >> DECIMATE_EXP


class CMSISBiquad(ABC):
    def __init__(self, f_c, q, gain_db):
        self.f_s = F_S
        self.f_c = f_c
        self.q = q
        self.gain_db = gain_db

        # [b0, b1, b2, a1_neg, a2_neg]
        self.coeffs = np.zeros(5)
        self.update_coefficients()

    @abstractmethod
    def update_coefficients(self):
        pass

    def compute(self, f):
        w = 2 * np.pi * (f / self.f_s)

        cos_w = np.cos(w)
        sin_w = np.sin(w)

        cos_2w = np.cos(2 * w)
        sin_2w = np.sin(2 * w)

        b0, b1, b2, a1_neg, a2_neg = self.coeffs
        a1 = -a1_neg
        a2 = -a2_neg

        num_r = b0 + (b1 * cos_w) + (b2 * cos_2w)
        num_i = -((b1 * sin_w) + (b2 * sin_2w))

        den_r = 1.0 + a1 * cos_w + a2 * cos_2w
        den_i = -((a1 * sin_w) + (a2 * sin_2w))

        return 10 * np.log10(
            (num_r * num_r + num_i * num_i) / (den_r * den_r + den_i * den_i)
        )


class Peak(CMSISBiquad):
    def update_coefficients(self):
        A = 10 ** (self.gain_db / 40.0)
        w_0 = 2 * np.pi * self.f_c / self.f_s
        alpha = np.sin(w_0) / (2 * self.q)

        a_0 = 1.0 + alpha / A
        cos_w_0 = np.cos(w_0)

        self.coeffs[0] = (1.0 + alpha * A) / a_0
        self.coeffs[1] = (-2.0 * cos_w_0) / a_0
        self.coeffs[2] = (1.0 - alpha * A) / a_0

        self.coeffs[3] = -(-2.0 * cos_w_0) / a_0
        self.coeffs[4] = -(1.0 - alpha / A) / a_0


class LowShelf(CMSISBiquad):
    def update_coefficients(self):
        A = 10 ** (self.gain_db / 40.0)
        w_0 = 2 * np.pi * self.f_c / self.f_s
        alpha = np.sin(w_0) / (2 * self.q)

        cos_w_0 = np.cos(w_0)
        a_p_1 = A + 1.0
        a_m_1 = A - 1.0
        a_p_1_cos = a_p_1 * cos_w_0
        a_m_1_cos = a_m_1 * cos_w_0
        _2_sqrt_a_alpha = 2.0 * np.sqrt(A) * alpha

        a_0 = a_p_1 + a_m_1_cos + _2_sqrt_a_alpha

        self.coeffs[0] = (A * (a_p_1 - a_m_1_cos + _2_sqrt_a_alpha)) / a_0
        self.coeffs[1] = (2.0 * A * (a_m_1 - a_p_1_cos)) / a_0
        self.coeffs[2] = (A * (a_p_1 - a_m_1_cos - _2_sqrt_a_alpha)) / a_0

        self.coeffs[3] = -(-2.0 * (a_m_1 + a_p_1_cos)) / a_0
        self.coeffs[4] = -(a_p_1 + a_m_1_cos - _2_sqrt_a_alpha) / a_0


class HighShelf(CMSISBiquad):
    def update_coefficients(self):
        A = 10 ** (self.gain_db / 40.0)
        w_0 = 2 * np.pi * self.f_c / self.f_s
        alpha = np.sin(w_0) / (2 * self.q)

        cos_w_0 = np.cos(w_0)
        a_p_1 = A + 1.0
        a_m_1 = A - 1.0
        a_p_1_cos = a_p_1 * cos_w_0
        a_m_1_cos = a_m_1 * cos_w_0
        _2_sqrt_a_alpha = 2.0 * np.sqrt(A) * alpha

        a_0 = a_p_1 - a_m_1_cos + _2_sqrt_a_alpha

        self.coeffs[0] = (A * (a_p_1 + a_m_1_cos + _2_sqrt_a_alpha)) / a_0
        self.coeffs[1] = (-2.0 * A * (a_m_1 + a_p_1_cos)) / a_0
        self.coeffs[2] = (A * (a_p_1 + a_m_1_cos - _2_sqrt_a_alpha)) / a_0

        self.coeffs[3] = -(2.0 * (a_m_1 - a_p_1_cos)) / a_0
        self.coeffs[4] = -(a_p_1 - a_m_1_cos - _2_sqrt_a_alpha) / a_0


start = 20
end = 20_000
num_points = 128

filters = [LowShelf(100, 1, 3), Peak(800, 1, -3), HighShelf(2000, 1, 3)]

x = np.geomspace(start, end, num_points)
y = np.zeros(x.shape)
for f in filters:
    y += f.compute(x)

w_values = 2 * np.pi * (x / F_S)
# print("w")
# for w in w_values:
#     print(f"{w:.9f}f,")

print("const float32_t cos_w[OLED_WIDTH] = {")
for w in np.cos(w_values):
    print(f"    {w:.9f}f,")
print("};")
print()
print("const float32_t sin_w[OLED_WIDTH] = {")
for w in np.sin(w_values):
    print(f"    {w:.9f}f,")
print("};")
print()
print("const float32_t cos_2w[OLED_WIDTH] = {")
for w in np.cos(2 * w_values):
    print(f"    {w:.9f}f,")
print("};")
print()
print("const float32_t sin_2w[OLED_WIDTH] = {")
for w in np.sin(2 * w_values):
    print(f"    {w:.9f}f,")
print("};")


plt.plot(y)
plt.grid(True)
plt.ylim(-31, 32)
plt.show()
