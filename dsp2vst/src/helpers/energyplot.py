import pandas as pd
import matplotlib.pyplot as plt

ir = pd.read_csv("ir_params.csv")
ir = ir.sort_values("delay")
plt.figure(figsize=(12,5))
plt.scatter(ir["delay"], ir["gain"], s=8)
plt.xlabel("Path Length (m)")
plt.ylabel("Gain")
plt.title("Reflection Distribution")
plt.grid(alpha=0.3)
plt.tight_layout()
plt.show()