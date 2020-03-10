#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <functional>

using namespace std;

template<typename T>
class Heap {
    using ComparerFunc = function<int(const T& first, const T& second)>;
    public:
        Heap() {
            pQueue.reserve(50);
        }
        void addToHeap(T val, ComparerFunc comparer) {
            pQueue.push_back(val);
            heapUp(pQueue.size(), comparer);
        }

        void removeFromHeap(ComparerFunc comparer) {
            if (!isEmpty()) {
                pQueue.pop_back();
                heapDown(pQueue.size(), comparer);
            }
        }

        bool isEmpty() {
            return pQueue.size() == 0;
        }

        T top() {
            return pQueue[0];
        }

        void toString() {
            copy(pQueue.begin(), pQueue.end(), ostream_iterator<T>(cout, " "));
        }
    private:
        vector<T> pQueue;
        void heapUp(int pos, ComparerFunc comparer) {
            int child = pos - 1;
            int parent = (child - 1) / 2;
            while (parent >= 0 && comparer(pQueue[child], pQueue[parent]) < 0) {
                swap(pQueue[child], pQueue[parent]);
                child = parent;
                parent = (child - 1) / 2;
            }
        }

        void heapDown(T val, int pos, ComparerFunc comparer) {
            int parent = 0;
            int child = parent * 2 + 1;

            while (child < pos) {
                if (child + 1 < pos && comparer(pQueue[child], pQueue[child+1]) > 0) {
                    child = child + 1;
                }
                if (comparer(pQueue[child], pQueue[parent] < 0)) {
                    swap(pQueue[child], pQueue[parent]);
                }
                parent = child;
                child = parent * 2 + 1;
            }
        }
};

class Node {
    public:
        Node(shared_ptr<Node> p) { 
            parent = make_shared<Node>(p); 
            nodeComparer = [](const Node& a, const Node& b) {
                return a.gValue + a.hValue - b.gValue - b.hValue;
            };
        }
        shared_ptr<Node> parent;
        int xPos, yPos;
        int gValue, hValue;
        function<int(const Node&, const Node&)> nodeComparer;
};

vector<vector<int>> makeGraph(vector<pair<int,int>>& edges, int nVertices) {
    vector<vector<int>> graph(nVertices,vector<int>(nVertices, 0));
    for(auto& i:edges) {
        graph[i.first-1][i.second-1] = 1;
        graph[i.second-1][i.first-1] = 1;
    }
    for (int i = 0; i < graph.size(); ++i) {
      for (int j = 0; j < graph[i].size(); ++j) {
        cout << graph[i][j] << '\t';
      }
      cout << endl;
    }
    return graph;
}

void astar();

/* auto intComparer = [](int x, int y) {
  return x - y;
}; */

int main() {
    vector<pair<int,int>> edges{{1,3},{2,3},{1,4},{1,5},{2,5},{4,4},{5,6},{4,7},{3,5},{4,5},{4,6},{2,7},{3,6}};
    // vector<vector<int>> mat{{1,1,1,0,0,1,0}, {1,0,0,1,0,0,1}, {1,0,0,1,0,1,1}, {0,1,1,0,1,1,1}, {0,1,0,0,0,1,1}, {1,0,0,0,0,1,1}, {0,1,1,0,0,1,1}};
    // vector<vector<int>> graph = makeGraph(edges, 7);
    /* vector<int> num{14, 19, 11, 10, 6, 13, 17, 12, 18, 15, 9, 8, 16};
    Heap<int> heapInt;
    for_each(num.begin(), num.end(), [&heapInt](int n) { 
        heapInt.addToHeap(n, intComparer);
    });

    heapInt.toString(); */
}
