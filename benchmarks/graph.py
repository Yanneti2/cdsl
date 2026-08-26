import matplotlib.pyplot
import matplotlib
import pandas
from sys import argv

is_log_x = False #flags do programa
is_log_y = False


colors = [
    "red",
    "blue",
    "green",
    "black",
    "purple",
    "yellow",
    "orange",
    "grey",
    "brown",
    ]

figura, eixo = matplotlib.pyplot.subplots()
for arg in argv[1:]:
    if arg == "--log-x":
        is_log_x = True
    if arg == "--log-y":
        is_log_y = True
    if arg.startswith("--"): #flags
        argv.remove(arg)


for i, csvfile in enumerate(argv[1:-1:2]):

    a = (pandas.read_csv(csvfile, sep = ";")) #recebe o caminho do csv como par (arg 1 no terminal)

    eixo.scatter(x = a.iloc[:, 0], y = a.iloc[:, 1], color = colors[i], label = argv[2*i + 2], s = 5) #colunas do arquivo csv

matplotlib.pyplot.xlabel(a.columns[0], fontsize = 10)
matplotlib.pyplot.ylabel(a.columns[1], fontsize = 10)

if is_log_x:
    eixo.set_xscale("log")
if is_log_y:
    eixo.set_yscale("log")

matplotlib.pyplot.legend()
matplotlib.pyplot.savefig(argv[-1], dpi = 500, bbox_inches = "tight") #argumento 2 na linha de cmd, combinamos de ser o nome do gráfico