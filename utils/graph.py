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
    expr = label.replace(" ", "")
    y_pos = expr.find("y")

    if y_pos <= 0:
        y_positive = True
    else:
        y_positive = expr[y_pos - 1] != "-"

    if "<=" in expr:
        if y_positive:
            le_constraints.append(y)
        else:
            ge_constraints.append(y)

    elif ">=" in expr:
        if y_positive:
            ge_constraints.append(y)
        else:
            le_constraints.append(y)

    k += 2

upper = np.minimum.reduce(le_constraints) if le_constraints else np.full_like(x, np.inf)
lower = np.maximum.reduce(ge_constraints) if ge_constraints else np.zeros_like(x)
lower = np.maximum(lower, 0)
mask = np.ones_like(x, dtype=bool)

for i in range(int(sys.argv[k])):
    k += 1
    px, py = sys.argv[k].split(',')

    plt.scatter(eval(px), eval(py), s=80)
    plt.text(eval(px) + 0.05, eval(py) + 0.05, f"({px}, {py})")

x_le = []
x_ge = []
k += 1

for i in range(int(sys.argv[k])):
    k += 1
    rhs = eval(sys.argv[k])
    k += 1
    label = sys.argv[k]
    plt.axvline(rhs, label=label)

    if "<=" in label:
        x_le.append(rhs)

    elif ">=" in label:
        x_ge.append(rhs)

if x_le:
    mask &= (x <= min(x_le))

if x_ge:
    mask &= (x >= max(x_ge))

plt.fill_between(x, lower, upper, where=(upper >= lower) & mask, alpha=0.25)

plt.xlabel("x axis")
plt.ylabel("y axis")
plt.grid(True)
plt.xlim(left=0)
plt.ylim(bottom=0)
plt.legend()
plt.savefig(sys.argv[1], dpi=300)
sys.exit(1 if np.isinf(upper).any() else 0)
