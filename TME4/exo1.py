import random

import matplotlib.pyplot as plt
import networkx as nx

MAX = 400

class CommunityDetection:

    def __init__(self, graph, input_path):
        self.graph = graph
        self.input_path = input_path

    def simple_bechmark(self):
        G = nx.Graph()

        p = 0.8
        q = 0.002

        fichier = open(self.input_path, "w")

        for i in range(0, 400):
            for j in range(i + 1, 400):
                if i // 100 == j // 100:
                    if random.random() < p:
                        G.add_edge(i, j)
                        print(str(i) + " " + str(j))
                        fichier.write(str(i) + "\t" + str(j) + "\n")
                elif random.random() < q:
                    G.add_edge(i, j)
                    fichier.write(str(i) + "\t" + str(j) + "\n")

        nx.draw(G)
        plt.show()
