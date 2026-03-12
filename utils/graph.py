import sys

try:
    import numpy as np
    import matplotlib.pyplot as plt

except ModuleNotFoundError:
    from subprocess import check_call

    check_call([sys.executable, "-m", "pip", "install", "numpy", "matplotlib"])
    import numpy as np
    import matplotlib.pyplot as plt

x = np.array(list(eval(sys.argv[2])))
le_constraints = []
ge_constraints = []
k = 4

for i in range(int(sys.argv[3])):
    y = np.array(list(eval(sys.argv[k])))
    label = sys.argv[k + 1]
    plt.plot(x, y, label=label)

    if "<=" in label:
        le_constraints.append(y)

    elif ">=" in label:
        ge_constraints.append(y)

    k += 2

upper = (np.minimum.reduce(le_constraints) if le_constraints else np.full_like(x, np.inf))
lower = (np.maximum.reduce(ge_constraints) if ge_constraints else np.zeros_like(x))
lower = np.maximum(lower, 0)
plt.fill_between(x, lower, upper, where=(upper >= lower), alpha=0.25)

for i in range(int(sys.argv[k])):
    k += 1
    x, y = eval(sys.argv[k])
    plt.scatter(x, y, s=80)
    plt.text(x + 0.05, y + 0.05, f"({x}, {y})")

plt.xlabel("X values")
plt.ylabel("Y values")
plt.grid(True)
plt.xlim(left=0)
plt.ylim(bottom=0)
plt.legend()
plt.savefig(sys.argv[1], dpi=300)
