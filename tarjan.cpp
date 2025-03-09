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

  void bridgeUtil(int u, vector<bool> &visited, vector<int> &disc,
                  vector<int> &low, vector<int> &parent,
                  vector<pair<int, int>> &bridges, int &time) {
    visited[u] = true;
    disc[u] = low[u] = ++time;

    for (int v : adj[u]) {
      if (!visited[v]) {
        parent[v] = u;
        bridgeUtil(v, visited, disc, low, parent, bridges, time);
        low[u] = min(low[u], low[v]);
        if (low[v] > disc[u])
          bridges.push_back({u, v});
      } else if (v != parent[u]) {
        low[u] = min(low[u], disc[v]);
      }
    }
  }

  vector<pair<int, int>> findBridges() {
    vector<bool> visited(V, false);
    vector<int> disc(V, -1), low(V, -1), parent(V, -1);
    vector<pair<int, int>> bridges;
    int time = 0;
    for (int i = 0; i < V; i++)
      if (!visited[i])
        bridgeUtil(i, visited, disc, low, parent, bridges, time);
    return bridges;
  }

  void removeEdge(int u, int v) {
    adj[u].erase(remove(adj[u].begin(), adj[u].end(), v), adj[u].end());
    adj[v].erase(remove(adj[v].begin(), adj[v].end(), u), adj[v].end());
  }

  void fleury(int start) {
    vector<pair<int, int>> bridges;
    vector<int> path;
    int u = start;
    while (true) {
      path.push_back(u);
      if (adj[u].empty())
        break;
      bridges = findBridges();
      int next = -1;
      for (int v : adj[u]) {
        if (::find(bridges.begin(), bridges.end(), make_pair(u, v)) ==
                bridges.end() &&
            ::find(bridges.begin(), bridges.end(), make_pair(v, u)) ==
                bridges.end()) {
          next = v;
          break;
        }
      }
      if (next == -1)
        next = adj[u].front();
      removeEdge(u, next);
      u = next;
    }
    // cout << "Caminho Euleriano: ";
    //   for (int v : path)
    //     cout << v << " -> ";
    //   cout << "FIM" << endl;
  }
};

int main() {
  time_t start;
  time(&start);
  time_t total = 0;
  int V = 10000;
  for (int j = 0; j < 10; j++) {
    time(&start);
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

    cout << "Tentadas " << i << " vezes, total de arestas: " << total_arestas
         << endl;
    cout << "Grafo Euleriano gerado com " << V << " vértices." << endl;

    g.fleury(0);
    time_t end;
    time(&end);
    cout << "demorou " << end - start << "segundos" << endl;
    time_t tmp;
    tmp = end - start;
    total += tmp;
  }
  cout << "Media = " << total / 10 << "segundos";
  return 0;
}
