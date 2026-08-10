import matplotlib.pyplot as plt


with open("/tmp/path.csv") as f:
    line = f.read().strip()
    prices = [float(x) for x in line.split(",")]

plt.plot(prices)
plt.xlabel("Time step")
plt.ylabel("Price")
plt.title("GBM Simulated Path")
plt.show()