import matplotlib.pyplot as plt

paths = []
with open("/tmp/WienerSimulationPaths.csv") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        prices = [float(x) for x in line.split(",")]
        paths.append(prices)

for prices in paths:
    plt.plot(prices, linewidth=0.8, alpha=0.7)

plt.xlabel("Time step")
plt.ylabel("Price")
plt.title(f"GBM Simulated Paths (n={len(paths)})")
plt.show()