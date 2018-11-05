import pylab as pl

def read_points(filename):
    """Lee un archivo con un input válido del problema Lumberjack y retorna un par de vectores con las coordenadas x e y de los árboles."""
    xs, ys = [], []
    with open(filename, 'r') as f:
        t, n, k = f.readline().split()
        for i in range(int(k)):
            x, y, _, _, _, _ = f.readline().split()
            xs.append(int(x))
            ys.append(int(y))
    return xs, ys

xs, ys = read_points('input.txt')
fig = pl.figure()
ax = fig.add_subplot(111)
ax.set_aspect('equal', 'box')
pl.plot(xs, ys, 'o', color='green')
pl.xlim(xmin=-1)
pl.ylim(ymin=-1)
pl.grid(True)

delta_x = {'up': 0, 'left': -1, 'down': 0, 'right': 1}
delta_y = {'up': 1, 'left': 0, 'down': -1, 'right': 0}
x, y = 0, 0

with open('output.txt', 'r') as output:
    for line in output:
        action, direction = line.split()
        if action == 'move':
            pl.arrow(x, y, delta_x[direction], delta_y[direction], color='red', width=0.025)
            x += delta_x[direction]
            y += delta_y[direction]
        elif action == 'cut':
            pl.plot(x, y, 'x')
            pl.arrow(x, y, 0.5*delta_x[direction], 0.5*delta_y[direction], width=0.05)

pl.show()
