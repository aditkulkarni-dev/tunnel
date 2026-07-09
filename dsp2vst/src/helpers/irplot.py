import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV
ir = pd.read_csv("../../../ir_params.csv")
ir = ir.sort_values("delay")
delays = ir["delay"]
gains = ir["gain"]
print(f"sum of gains : {gains.sum()}")
print(f"number of rows : {len(gains)}")
energy = np.sum(gains ** 2)
print("IR energy:", energy)

# Create figure
plt.figure(figsize=(12,5))
plt.loglog(delays, gains, '.')
plt.xlabel("Path length (m) (log)")
plt.ylabel("Gain (log scale)")
plt.title("Sparse IR Energy Decay")
plt.grid(True, which="both", alpha=0.3)
plt.show()