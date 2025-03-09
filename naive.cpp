#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

int total_arestas;

class Graph {
public:
    int V;
    vector<vector<int>> adj;
    vector<int> parent, rank;
    unordered_set<long long> edges;

    Graph(int V) : V(V), adj(V), parent(V), rank(V, 1) {
        for (int i = 0; i < V; i++)
            parent[i] = i;
    }

    int find(int u) {
        return (parent[u] == u) ? u : (parent[u] = find(parent[u]));
    }

    void unite(int u, int v) {
        int rootU = find(u), rootV = find(v);
        if (rootU != rootV) {
            if (rank[rootU] > rank[rootV])
                parent[rootV] = rootU;
            else
                parent[rootU] = rootV;
            if (rank[rootU] == rank[rootV])
                rank[rootV]++;
        }
    }

    bool addEdge(int u, int v) {
        long long code = ((long long)u << 32) | v;
        if (u == v || edges.count(code))
            return false;
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.insert(code);
        edges.insert(((long long)v << 32) | u);
        unite(u, v);
        total_arestas++;
        return true;
    }

    void generateRandomConnectedGraph(unsigned int seed) {
        srand(seed);
        vector<int> nodes(V);
        for (int i = 0; i < V; i++)
            nodes[i] = i;
        random_shuffle(nodes.begin(), nodes.end());

        for (int i = 1; i < V; i++) {
            int u = nodes[i];
            int v = nodes[rand() % i];
            addEdge(u, v);
        }

        for (int i = 0; i < V / 2; i++) {
            int u = rand() % V, v = rand() % V;
            addEdge(u, v);
        }
    }

    void makeEulerian() {
        vector<int> oddVertices;
        for (int i = 0; i < V; i++)
            if (adj[i].size() % 2)
                oddVertices.push_back(i);
        for (size_t i = 0; i + 1 < oddVertices.size(); i += 2)
            addEdge(oddVertices[i], oddVertices[i + 1]);
    }

    bool hasOddDegreeVertices() {
        for (const auto &neighbors : adj)
            if (neighbors.size() % 2)
                return true;
        return false;
    }

    void removeEdge(int u, int v) {
        adj[u].erase(remove(adj[u].begin(), adj[u].end(), v), adj[u].end());
        adj[v].erase(remove(adj[v].begin(), adj[v].end(), u), adj[v].end());
    }

    bool checkConnectivityAfterRemove(int u, int v) {
        
        vector<int> backupParent = parent;
        vector<int> backupRank = rank;
        
        removeEdge(u, v);
        
        parent = backupParent;
        rank = backupRank;
        
        for (int i = 0; i < V; i++) {
            for (int j : adj[i]) {
                if (find(i) != find(j)) {
                    unite(i, j);
                }
            }
        }

        bool isConnected = (find(u) == find(v));
        
        parent = backupParent;
        rank = backupRank;
        addEdge(u, v);
        
        return !isConnected;
    }
    pair<int, int> getFirstNonBridgeEdge(int u) {
    for (int v : adj[u]) {
        if (!checkConnectivityAfterRemove(u, v)) {
            return make_pair(u, v);  // Retorna a primeira aresta que não é ponte
        }
    }
    // Se todas as arestas são pontes, retorna a primeira aresta.
    return make_pair(u, adj[u].front());
}


    vector<pair<int, int>> findBridges() {
        vector<pair<int, int>> bridges;
        
        for (int u = 0; u < V; u++) {
            for (int v : adj[u]) {
                if (checkConnectivityAfterRemove(u, v)) {
                    bridges.push_back({u, v});
                }
            }
        }
        return bridges;
    }

void fleury(int start) {
    vector<int> path;
    int u = start;
    
    // Enquanto houverem arestas no grafo
    while (!adj[u].empty()) {
        path.push_back(u);
        
        // Pega a primeira aresta que não é ponte
        pair<int, int> edge = getFirstNonBridgeEdge(u);
        int next = edge.second;
        
        // Remove a aresta selecionada
        removeEdge(u, next);
        u = next;
    }        cout << "Caminho Euleriano: ";
        //   for (int v : path)
        //     cout << v << " -> ";
        //   cout << "FIM" << endl;
    }
};

int main() {
    time_t start;
    time(&start);
    int V = 10000;
    unsigned int seed = time(0);
    Graph g(V);
    int i = 0;
    do {
        g = Graph(V);
        total_arestas = 0;
        g.generateRandomConnectedGraph(seed);
        g.makeEulerian();
        seed = rand();
        i++;
    } while (g.hasOddDegreeVertices());

    cout << "Tentadas " << i << " vezes" << total_arestas << endl;
    cout << "Grafo Euleriano gerado com " << V << " vértices." << endl;

    g.fleury(0);
    time_t end;
    time(&end);
    cout << "demorou " << end - start << " segundos" << endl;
    return 0;
}

