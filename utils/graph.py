import sys

try:
    import numpy as np
    import matplotlib.pyplot as plt

except ModuleNotFoundError:
    from subprocess import check_call

    check_call([sys.executable, "-m", "pip", "install", "numpy", "matplotlib"])
    import numpy as np
    import matplotlib.pyplot as plt


def plot(
        file_name: str, x_vals: list[float], y_curves: list[tuple[list[float], str]], points: list[tuple[str, str]],
        vertical_lines: list[tuple[float, str]]
) -> int:
    fig, ax = plt.subplots()
    x = np.array(x_vals)
    le_constraints, ge_constraints = [], []

    for y_vals, label in y_curves:
        y = np.array(y_vals)
        ax.plot(x, y, label=label)
        expr = label.replace(" ", "")
        y_pos = expr.find("y")
        y_positive = y_pos <= 0 or expr[y_pos - 1] != "-"

        if "<=" in expr:
            (le_constraints if y_positive else ge_constraints).append(y)

        elif ">=" in expr:
            (ge_constraints if y_positive else le_constraints).append(y)

    upper = np.minimum.reduce(le_constraints) if le_constraints else np.full_like(x, np.inf)
    lower = np.maximum.reduce(ge_constraints) if ge_constraints else np.zeros_like(x)
    lower = np.maximum(lower, 0)
    mask = np.ones_like(x, dtype=bool)
    x_le, x_ge = [], []

    for px, py in points:
        ax.scatter(eval(px), eval(py), s=80)
        ax.text(eval(px) + 0.05, eval(py) + 0.05, f"({px}, {py})")

    for rhs, label in vertical_lines:
        ax.axvline(rhs, label=label)

        if "<=" in label:
            x_le.append(rhs)

        elif ">=" in label:
            x_ge.append(rhs)

    if x_le:
        mask &= (x <= min(x_le))

    if x_ge:
        mask &= (x >= max(x_ge))

    ax.fill_between(x, lower, upper, where=(upper >= lower) & mask, alpha=0.25)

    ax.set_xlabel("x axis")
    ax.set_ylabel("y axis")
    ax.grid(True)
    ax.set_xlim(left=0)
    ax.set_ylim(bottom=0)
    ax.legend()
    fig.savefig(file_name, dpi=300)
    plt.close(fig)
    return 1 if np.isinf(upper).any() else 0