#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cmath>
#include <list>

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
                swap(pQueue[0], pQueue[pQueue.size() - 1]);
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
            copy(pQueue.begin(), pQueue.end(), ostream_iterator<T>(cout, "\n"));
            cout << endl;
        }

        auto begin() {
            return pQueue.begin();
        }

        auto end() {
            return pQueue.end();
        }

        void heapUp(int pos, ComparerFunc comparer) {
            int child = pos - 1;
            int parent = (child - 1) / 2;
            while (parent >= 0 && comparer(pQueue[child], pQueue[parent]) < 0) {
                
                swap(pQueue[child], pQueue[parent]);
                child = parent;
                parent = (child - 1) / 2;
                
            }
        }

        void heapDown(int pos, ComparerFunc comparer) {
            int parent = 0;
            int child = parent * 2 + 1;

            while (child < pos) {
                if (child + 1 < pos && comparer(pQueue[child], pQueue[child+1]) > 0) {
                    child = child + 1;
                }
                if (comparer(pQueue[child], pQueue[parent]) < 0) {
                    swap(pQueue[child], pQueue[parent]);
                }
                parent = child;
                child = parent * 2 + 1;
            }
        }
    private:
        vector<T> pQueue;
        
};

class Node {
    public:
        Node( int x, int y, int g, int h) : parent(nullptr), xPos(x), yPos(y), gValue(g), hValue(h) {
        }
        shared_ptr<Node> parent = nullptr;
        int xPos, yPos;
        int gValue, hValue;
        static function<int(const Node&, const Node&)> nodeComparer;
        friend bool operator==(const Node a, const Node b);
        friend ostream& operator<<(ostream& os, const Node& node);
};

bool operator==(const Node a, const Node b) {
    return a.xPos == b.xPos && a.yPos == b.yPos;
}

ostream& operator<<(ostream& os, const Node& node) {
    os << "Node :" << node.xPos << ',' << node.yPos /* << "parent (" << node.parent->xPos << ' ' << node.parent->yPos */ << "\t";
    os << "Weight: " << node.gValue << ',' << node.hValue << "\t";
    if ( node.parent != nullptr) {
        os << "parent (" << node.parent->xPos << ' ' << node.parent->yPos << ")\t";
    } else {
        os << "parent (" << node.parent << ")\t";
    }
    return os;
}

decltype(Node::nodeComparer) Node::nodeComparer = [](const Node& a, const Node& b) {
                return a.gValue + a.hValue - b.gValue - b.hValue;
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

int dist(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

template<typename T = Node>
bool process(T parent, T&& current, const vector<vector<int>>& matrix, Heap<T>& heap, const vector<T>& closedList, const pair<int, int> end) {
    int sz = matrix.size();

    if (current.xPos < 0 || current.xPos >= sz || current.yPos < 0 || current.yPos >= sz) {
        return false;
    }
    auto cit = find(closedList.begin(), closedList.end(), current);
    if ( cit != closedList.end() || matrix[current.xPos][current.yPos] == 0) {
        return false;
    }

    auto it = find(heap.begin(), heap.end(), forward<T>(current));
    if (it != heap.end()) {
        if (it->gValue > current.gValue) {
            it->gValue = current.gValue;
            it->parent = make_shared<T>(parent);
            heap.heapUp(it - heap.begin(), T::nodeComparer);
        }
    } else {
        current.parent = make_shared<T>(parent);
        heap.addToHeap(current, T::nodeComparer);
    }
    if (current.xPos == end.first && current.yPos == end.second) {
        return true;
    }
    return false;
}

list<pair<int, int>> astar(const vector<vector<int>>& matrix, pair<int, int> start, pair<int, int> end) {
    Node startNode = Node(start.first, start.second, 0, dist(end.first, end.second, start.first, start.second));
    Node endNode = Node(end.first, end.second, -1, 0);
    
    Heap<Node> heapOfNodes;
    vector<Node> closedList;
    vector<pair<int, int>> neighbours {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    heapOfNodes.addToHeap(startNode, Node::nodeComparer);
    while(1) {
        auto&& evicted = heapOfNodes.top();
        closedList.push_back(evicted);

        auto ret = false;
        for (auto coods : neighbours) {
            ret = process(evicted, Node( evicted.xPos + coods.first, evicted.yPos + coods.second, 
                                         evicted.gValue + 1, dist(end.first, end.second, evicted.xPos + coods.first, evicted.yPos + coods.second)), 
                                         matrix, heapOfNodes, closedList, end);
            if (ret)
                break;
        }

        if (heapOfNodes.isEmpty() || ret)
            break;

        heapOfNodes.removeFromHeap(Node::nodeComparer);
        // heapOfNodes.toString();
    }
    list<pair<int, int>> path;
    if (!heapOfNodes.isEmpty()) {
        shared_ptr<Node> iter = make_shared<Node>(heapOfNodes.top());

        while (iter->parent != nullptr) {
            path.push_front(make_pair(iter->xPos, iter->yPos));
            iter = iter->parent;
        }
    }
    return path;
}

int main() {
    // vector<pair<int,int>> edges{{1,3},{2,3},{1,4},{1,5},{2,5},{4,4},{5,6},{4,7},{3,5},{4,5},{4,6},{2,7},{3,6}};
    vector<vector<int>> matrix {
        
        { 1, 0, 1, 1, 1, 1, 0, 1, 1 }, 
        { 1, 1, 1, 0, 1, 1, 1, 0, 1 }, 
        { 1, 1, 1, 0, 1, 1, 0, 1, 1 }, 
        { 0, 0, 1, 0, 1, 1, 0, 1, 0 }, 
        { 1, 1, 1, 0, 1, 1, 1, 1, 1 }, 
        { 1, 0, 1, 1, 1, 1, 0, 1, 1 }, 
        { 1, 0, 0, 0, 0, 1, 0, 0, 0 }, 
        { 1, 0, 1, 1, 1, 1, 0, 1, 1 }, 
        { 1, 1, 1, 0, 0, 0, 1, 0, 0 } 

    };
    /* vector<vector<int>> matrix {
        
        { 1, 0, 1 }, 
        { 0, 1, 0 }, 
        { 1, 1, 1 }

    }; */
    for (auto x:matrix) {
        for (auto y:x) {
            cout << y << " ";
        }
        cout << endl;
    }
    pair<int, int> start {8, 0};
    pair<int, int> end {0, 8};
    list<pair<int, int>> path = astar(matrix, start, end);
    if (!path.empty()) {
        cout << start.first << "," << start.second << "->";
        for(auto p : path) {
            cout << p.first << "," << p.second << "->";
        }
        cout << end.first << "," << end.second << "->";
    } else {
        cout << "no path found" << endl;
    }
    cout << endl;
}
