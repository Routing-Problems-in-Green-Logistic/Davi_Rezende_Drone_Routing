from operator import index
import sys
import pandas as pd
import matplotlib.pyplot as plt
import random as random

#LENDO O ARQUIVO DE ENTRADA DA INSTANCIA
instanceFileName = "inst11000_cli300_dfc974_dc172.txt"
coord = pd.read_csv(instanceFileName, header=None, names=['StringId', 'type', 'x', 'y'], delim_whitespace=True);
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

#LENDO O ARQUIVO DE ENTRADA DOS CLUSTERS
clusterInstanceFileName = "inst11000_cli300_dfc974_dc172_CLUSTER_PLOT_FILE.txt"
coord = pd.read_csv(clusterInstanceFileName, header=None, names=['Cluster', 'Point', 'CentroidX', 'CentroidY'], delim_whitespace=True);
del coord['Cluster'][0]
clusters = coord['Cluster']
del coord['Point'][0]
points = coord['Point']
del coord['CentroidX'][0]
centroidX = coord['CentroidX']
del coord['CentroidY'][0]
centroidY = coord['CentroidY']

#IMPRIMINDO A LEITURA DO ARQUIVO DE ENTRADA DOS CLUSTERS
print("LEITURA DO ARQUIVO DE ENTRADA DOS CLUSTERS:")
print("\nCLUSTERS:")
print(clusters.values)
print("\nPOINTS:")
print(points.values)
print("\nCOORDENADAS X DO CENTROIDE:")
print(centroidX.values)
print("\nCOORDENADAS Y DO CENTROIDE:")
print(centroidY.values)

#PLOTANDO A DISTRIBUIÇÃO DOS PONTOS SEM CLUSTERIZAÇÃO
plt.figure()
plt.grid(visible=True, alpha=0.3)
plt.xlabel("EIXO X")
plt.ylabel("EIXO Y")

for i in range(len(identifiers)):
    if types[i+1] == 'd':
        marker = '^'
        color = '#000000'
    else:
        marker = 'o'
        color = '#0000ff'

    plt.plot(int(coordX[i+1]), int(coordY[i+1]), c = color, marker = marker)

plt.savefig('instance_' + instanceFileName + '.png')

# #PLOTANDO A DISTRIBUIÇÃO DOS PONTOS COM CLUSTERIZAÇÃO
# plt.figure()
# plt.grid(visible=True, alpha=0.3)
# plt.xlabel("EIXO X")
# plt.ylabel("EIXO Y")

# #PLOTANDO OS DEPÓSITOS
# for i in range(len(identifiers)):
#     if types[i+1] == 'd':
#         plt.plot(float(coordX[i+1]), float(coordY[i+1]), c = '#000000', marker = '^')

# # #PLOTANDO OS PARKING SPOTS
# # for i in range(len(centroidX)):
# #     plt.plot(float(centroidX[i+1]), float(centroidY[i+1]), c = '#000000', marker = 's')

# #PLOTANDO OS CLIENTES COM SEUS RESPECTIVOS CLUSTERS
# clusterAux = clusters[1]
# # calculando cor aleatória
# r = random.random()
# g = random.random()  
# b = random.random() 
# color = (r, g, b)

# for i in range(len(points)):

#     #encontrando esse ponto no vetor identifiers, para capturar o indice e conseguir encontrar as coordenadas
#     #nos vetores coordX e coordY
#     for y in range(len(identifiers)):
#         if(identifiers[y+1] == points[i+1]):
#             indice = y
#             break

#     if(clusterAux != clusters[i+1]):
#         clusterAux = clusters[i+1]
#         # calculando cor aleatória
#         r = random.random()
#         g = random.random()  
#         b = random.random() 
#         color = (r, g, b) 

#     plt.plot(float(coordX[indice+1]), float(coordY[indice+1]), c = color, marker = 'o')
#     plt.plot(float(centroidX[i+1]), float(centroidY[i+1]), c = color, marker = 's')

# plt.savefig('instance_' + clusterInstanceFileName + '.png')