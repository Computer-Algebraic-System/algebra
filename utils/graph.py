import sys

try:
    import matplotlib.pyplot as plt

except ModuleNotFoundError:
    from subprocess import check_call

    check_call([sys.executable, "-m", "pip", "install", "matplotlib"])
    import matplotlib.pyplot as plt

x = list(eval(sys.argv[2]))
ys = [list(eval(arg)) for arg in sys.argv[3:]]

for y in ys:
    plt.plot(x, y)

plt.xlabel("X values")
plt.ylabel("Y values")
plt.title("Multiple Line Graph")
plt.grid(True)
plt.savefig(sys.argv[1], dpi=300)
