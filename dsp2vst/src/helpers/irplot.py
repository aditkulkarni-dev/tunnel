import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
ir = pd.read_csv("../../ir_params.csv")
ir = ir.sort_values("delay")
delays = ir["delay"]
gains = ir["gain"]

# Create figure
plt.figure(figsize=(12,5))
plt.loglog(delays, gains, '.')
plt.xlabel("Path length (m) (log)")
plt.ylabel("Gain (log scale)")
plt.title("Sparse IR Energy Decay")
plt.grid(True, which="both", alpha=0.3)
plt.show()