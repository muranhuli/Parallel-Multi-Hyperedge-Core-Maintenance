#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>
using namespace std;

typedef pair<int, int> PII;
struct myCmp
{
    bool operator()(const PII &a, const PII &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

void getGraph(const string &filename, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    ifstream fin(filename, ios::in);
    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int tmp;
        vector<int> e;
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

void kcoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    set<pair<int, int>, myCmp> node_count;
    unordered_map<int, int> deg;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    for (auto &it : hyperNode)
    {
        deg[it.first] = it.second.size();
        node_count.insert(make_pair(it.first, deg[it.first]));
        visitNode[it.first] = false;
    }
    int K = 0;
    while (!node_count.empty())
    {
        pair<int, int> p = *node_count.begin();
        node_count.erase(node_count.begin());
        K = max(K, p.second);
        core[p.first] = K;
        visitNode[p.first] = true;
        for (auto &edge : hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (node == p.first)
                    continue;
                node_count.erase(make_pair(node, deg[node]));
                deg[node]--;
                node_count.insert(make_pair(node, deg[node]));
            }
        }
    }
}

void initialization(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode, const unordered_map<int, int> &core, unordered_map<int, unordered_set<int>> &nodeinfo, vector<PII> &edgeinfo)
{
    vector<PII>(int(hyperEdge.size())).swap(edgeinfo);
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        int mink = INT_MAX;
        for (auto node : hyperEdge[i])
        {
            mink = min(mink, core.at(node));
        }
        edgeinfo[i].first = mink;
    }
    for (auto &it : hyperNode)
    {
        for (auto edge : it.second)
        {
            if (edgeinfo[edge].first == core.at(it.first))
                nodeinfo[it.first].insert(edge);
        }
    }
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        int mink = INT_MAX;
        for (auto node : hyperEdge[i])
        {
            if (core.at(node) > edgeinfo[i].first)
                continue;
            mink = min(mink, int(nodeinfo[node].size()));
        }
        edgeinfo[i].second = mink;
    }
}

void insertEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, unordered_map<int, unordered_set<int>> &nodeinfo, vector<PII> &edgeinfo, const vector<vector<int>> &ee)
{
    vector<int> mink_e_node_set;
    vector<int> key_node;
    bool flag_isNewNode = false;
    int mink_e = INT_MAX;
    for (auto &e : ee)
    {
        int index = int(hyperEdge.size());
        hyperEdge.emplace_back(vector<int>());
        for (auto node : e)
        {
            hyperEdge[index].emplace_back(node);
            hyperNode[node].emplace_back(index);
        }
        int mink_ee = INT_MAX;
        vector<int> mink_e_node;
        for (auto node : e)
        {
            if (core[node] == 0)
            {
                core[node] = 1;
                flag_isNewNode = true;
            }
            if (mink_ee > core[node])
            {
                mink_ee = core[node];
                mink_e_node.clear();
                mink_e_node.emplace_back(node);
            }
            else if (mink_ee == core[node])
            {
                mink_e_node.emplace_back(node);
            }
        }
        for (auto node : mink_e_node)
        {
            mink_e_node_set.emplace_back(node);
            nodeinfo[node].insert(index);
        }
        int mink = INT_MAX;
        for (auto node : mink_e_node)
        {
            mink = min(mink, int(nodeinfo[node].size()));
        }
        edgeinfo.emplace_back(make_pair(mink_ee, mink));
        key_node.emplace_back(mink_e_node[0]);
        mink_e = mink_ee;
    }
    unordered_map<int, bool> visitedEdgeInProcess;
    for (auto node : mink_e_node_set)
    {
        for (auto edge : nodeinfo[node])
        {
            if (visitedEdgeInProcess[edge])
                continue;
            visitedEdgeInProcess[edge] = true;
            int mink = INT_MAX;
            for (auto node1 : hyperEdge[edge])
            {
                if (core.at(node1) == edgeinfo[edge].first)
                {
                    mink = min(mink, int(nodeinfo[node1].size()));
                }
            }
            edgeinfo[edge].second = mink;
        }
    }
    if (flag_isNewNode)
        return;
    queue<int> q;
    unordered_map<int, int> visitedNode;
    for (auto node : key_node)
    {
        q.push(node);
        visitedNode[node] = 1;
    }
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    vector<int> visitEdge1(hyperEdge.size(), 0);
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        int count = 0;
        for (auto edge : nodeinfo[v])
        {
            if (edgeinfo[edge].second > mink_e)
                ++count;
        }
        mcd.insert(make_pair(v, count));
        deg[v] = count;
        if (count <= mink_e)
            continue;
        for (auto edge : nodeinfo[v])
        {
            if (visitEdge1[edge] != 0)
                continue;
            visitEdge1[edge] = 1;
            if (edgeinfo[edge].second <= mink_e)
                continue;
            visitEdge1[edge] = 2;
            for (auto node : hyperEdge[edge])
            {
                if (core[node] == mink_e && visitedNode[node] != 1)
                {
                    q.push(node);
                    visitedNode[node] = 1;
                }
            }
        }
    }
    vector<bool> visitEdge(hyperEdge.size(), false);
    while (!mcd.empty() && (*mcd.begin()).second <= mink_e)
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        int v = p.first;
        deg.erase(v);
        for (auto edge : nodeinfo[v])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            if (visitEdge1[edge] != 2)
                continue;
            for (auto node : hyperEdge[edge])
            {
                if (deg.find(node) != deg.end())
                    if (mcd.erase(make_pair(node, deg[node])))
                    {
                        mcd.erase(make_pair(node, deg[node]));
                        deg[node]--;
                        mcd.insert(make_pair(node, deg[node]));
                    }
            }
        }
    }
    vector<int> tmp_node;
    while (!mcd.empty())
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        core[p.first]++;
        tmp_node.emplace_back(p.first);
    }
    unordered_set<int> set_node;
    for (auto node : tmp_node)
    {
        set_node.insert(node);
        vector<int> erase;
        for (auto edge : nodeinfo[node])
        {
            int mink = INT_MAX;
            for (auto node : hyperEdge[edge])
            {
                mink = min(mink, core.at(node));
            }
            edgeinfo[edge].first = mink;
            if (mink <= mink_e)
                erase.emplace_back(edge);
        }
        for (auto edge : erase)
        {
            nodeinfo[node].erase(edge);
        }
    }
    for (auto node : tmp_node)
    {
        for (auto edge : nodeinfo[node])
            for (auto node1 : hyperEdge[edge])
                if (core[node1] == edgeinfo[edge].first)
                {
                    if (nodeinfo[node1].find(edge) == nodeinfo[node1].end())
                    {
                        nodeinfo[node1].insert(edge);
                        set_node.insert(node1);
                    }
                }
    }
    vector<bool> visitEdge2(int(hyperEdge.size()), false);
    while (!set_node.empty())
    {
        int node = *set_node.begin();
        set_node.erase(set_node.begin());
        for (auto edge : hyperNode[node])
        {
            if (visitEdge2[edge])
                continue;
            visitEdge2[edge] = true;
            int mink = INT_MAX;
            for (auto node2 : hyperEdge[edge])
            {
                if (edgeinfo[edge].first < mink_e)
                    continue;
                if (core.at(node2) > edgeinfo[edge].first)
                    continue;
                mink = min(mink, int(nodeinfo[node2].size()));
            }
            edgeinfo[edge].second = mink;
        }
    }
}

const int percent = 30;
void dividDataSet(vector<vector<int>> &hyperEdge, vector<vector<int>> &InsertEdge, int n)
{
    random_device seed;
    default_random_engine gen(seed());

    for (int i = 0; i < n; i++)
    {
        InsertEdge.emplace_back(hyperEdge[i]);
    }
    for (int i = n; i < int(hyperEdge.size()); i++)
    {
        uniform_int_distribution<unsigned> u(0, i);
        int d = u(gen);
        if (d < n)
        {
            InsertEdge[d] = hyperEdge[i];
        }
    }
    for (int i = 0; i < int(InsertEdge.size()); i++)
    {
        auto pos = find(hyperEdge.begin(), hyperEdge.end(), InsertEdge[i]);
        hyperEdge.erase(pos);
    }
}
void initial(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    hyperNode.clear();
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        for (auto node : hyperEdge[i])
            hyperNode[node].emplace_back(i);
    }
}

// Select a parallel SPS set from the edge set
void divideEdge(const vector<vector<int>> &insertEdge, unordered_map<int, int> &core, unordered_map<int, vector<vector<int>>> &parallel_edges, vector<bool> &remove_edge)
{
    unordered_map<int, bool> min_node_set;
    parallel_edges.clear();
    for (int i = 0; i < int(insertEdge.size()); i++)
    {
        if (remove_edge[i])
            continue;
        int mink_e = INT_MAX;
        vector<int> mink_e_node;
        for (int j = 0; j < int(insertEdge[i].size()); j++)
        {
            int node = insertEdge[i][j];
            if (mink_e > core[node])
            {
                mink_e = core[node];
                mink_e_node.clear();
                mink_e_node.emplace_back(node);
            }
            else if (mink_e == core[node])
            {
                mink_e_node.emplace_back(node);
            }
        }
        bool flag = true;
        for (auto node : mink_e_node)
        {
            if (min_node_set[node])
            {
                flag = false;
                break;
            }
        }
        if (flag)
        {
            for (auto node : mink_e_node)
                min_node_set[node] = true;
            remove_edge[i] = true;
            parallel_edges[mink_e].emplace_back(insertEdge[i]);
        }
    }
}

int main()
{
    string file = "";
    string filepath = "";
    istringstream ss(file);
    string str;
    while (ss >> str)
    {
        string filename = filepath + str;
        cout << str << endl;
        ofstream fout("./result/break/" + str + "-insert.txt");
        for (int i = 1;; i++)
        {
            vector<vector<int>> hyperEdge;
            unordered_map<int, vector<int>> hyperNode;
            unordered_map<int, int> core;
            vector<vector<int>> InsertEdge;
            getGraph(filename, hyperEdge, hyperNode);
            int n = int(hyperEdge.size());
            int num = 50 * i;
            if (num >= 80000)
                break;
            dividDataSet(hyperEdge, InsertEdge, num);
            initial(hyperEdge, hyperNode);
            auto t6 = std::chrono::steady_clock::now();
            kcoreDecomp(hyperEdge, hyperNode, core);
            auto t7 = std::chrono::steady_clock::now();
            unordered_map<int, unordered_set<int>> nodeinfo;
            vector<PII> edgeinfo;
            initialization(hyperEdge, hyperNode, core, nodeinfo, edgeinfo);
            unordered_map<int, vector<vector<int>>> parallel_edges;
            vector<bool> remove_edge(int(InsertEdge.size()), false);
            auto t1 = std::chrono::steady_clock::now();
            while (true)
            {
                divideEdge(InsertEdge, core, parallel_edges, remove_edge);
                if (parallel_edges.empty())
                    break;
                for (const auto &it : parallel_edges)
                {
                    insertEdge(hyperEdge, hyperNode, core, nodeinfo, edgeinfo, it.second);
                }
            }
            auto t2 = std::chrono::steady_clock::now();
            double dr_ns1 = std::chrono::duration<double, std::nano>(t2 - t1).count();
            fout << dr_ns1 << " ";
            dr_ns1 = std::chrono::duration<double, std::nano>(t7 - t6).count();
            fout << dr_ns1 << endl;
        }
        fout.close();
    }
}