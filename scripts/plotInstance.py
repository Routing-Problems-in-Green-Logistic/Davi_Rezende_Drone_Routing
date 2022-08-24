import sys
import pandas as pd
import matplotlib.pyplot as plt

#LENDO O ARQUIVO DE ENTRADA
coord = pd.read_csv("tam11000_cli300_config3.txt", header=None, names=['StringId', 'type', 'x', 'y'], delim_whitespace=True);
del coord['StringId'][0]
del coord['StringId'][len(coord['StringId'])]
del coord['StringId'][len(coord['StringId'])]
del coord['StringId'][len(coord['StringId'])]
identifiers = coord['StringId']
del coord['type'][0]
del coord['type'][len(coord['type'])]
del coord['type'][len(coord['type'])]
del coord['type'][len(coord['type'])]
types = coord['type']
del coord['x'][0]
del coord['x'][len(coord['x'])]
del coord['x'][len(coord['x'])]
del coord['x'][len(coord['x'])]
coordX = coord['x']
del coord['y'][0]
del coord['y'][len(coord['y'])]
del coord['y'][len(coord['y'])]
del coord['y'][len(coord['y'])]
coordY = coord['y']

#IMPRIMINDO A LEITURA DO ARQUIVO DE ENTRADA
print("LEITURA DO ARQUIVO DE ENTRADA:")
print("\nSTRING ID:")
print(identifiers.values)
print("\nTYPE:")
print(types.values)
print("\nCOORDENADAS X:")
print(coordX.values)
print("\nCOORDENADAS Y:")
print(coordY.values)

#PLOTANDO A DISTRIBUIÇÃO DOS PONTOS
plt.figure()
plt.grid(visible=True, alpha=0.3)
plt.xlabel("EIXO X")
plt.ylabel("EIXO Y")

for i in range(len(identifiers)):
    if types[i+1] == 'd':
        color = '#000000'
    else:
        color = '#0000ff'

    plt.plot(int(coordX[i+1]), int(coordY[i+1]), c = color, marker = 'o')

plt.savefig('instance' + ''+ '.png')