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

for y_vals, label in zip(sys.argv[3::2], sys.argv[4::2]):
    y = np.array(list(eval(y_vals)))
    plt.plot(x, y, label=label)

    if "<=" in label:
        le_constraints.append(y)

    elif ">=" in label:
        ge_constraints.append(y)


upper = (
    np.minimum.reduce(le_constraints)
    if le_constraints
    else np.full_like(x, np.inf)
)

lower = (
    np.maximum.reduce(ge_constraints)
    if ge_constraints
    else np.zeros_like(x)
)

lower = np.maximum(lower, 0)

plt.fill_between(
    x,
    lower,
    upper,
    where=(upper >= lower),
    alpha=0.25
)

plt.xlabel("X values")
plt.ylabel("Y values")
plt.grid(True)
plt.xlim(left=0)
plt.ylim(bottom=0)
plt.legend()
plt.savefig(sys.argv[1], dpi=300)