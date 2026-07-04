import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
ir = pd.read_csv("../../ir_params.csv")
ir = ir.sort_values("delay")
delays = ir["delay"]
gains = ir["gain"]

# Create figure
fig, ax = plt.subplots(figsize=(12, 5))

# Stem plot
markerline, stemlines, baseline = ax.stem(
    delays,
    gains,
    linefmt='C0-',
    markerfmt='C0o',
    basefmt='k-'
)

plt.setp(markerline, markersize=4)
plt.setp(stemlines, linewidth=1)

ax.set_title("Sparse Room Impulse Response")
ax.set_xlabel("Path Length (m)")
ax.set_ylabel("Gain")
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()