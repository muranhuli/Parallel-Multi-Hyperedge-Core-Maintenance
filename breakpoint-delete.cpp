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

void eraseEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, unordered_map<int, unordered_set<int>> &nodeinfo, vector<PII> &edgeinfo, const vector<int> &ee)
{
    vector<int> mink_e_node_set;
    bool flag_isNewNode = false;
    int mink_e = INT_MAX;
    for (auto index : ee)
    {
        vector<int> e(hyperEdge[index]);
        hyperEdge[index].clear();
        for (auto node : e)
        {
            hyperNode[node].erase(find(hyperNode[node].begin(), hyperNode[node].end(), index));
        }
        int mink_ee = INT_MAX;
        vector<int> mink_e_node;
        bool flag_isNewNode = false;
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
            nodeinfo[node].erase(index);
        }
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
                if (core.at(node1) > edgeinfo[edge].first)
                    continue;
                mink = min(mink, int(nodeinfo[node1].size()));
            }
            edgeinfo[edge].second = mink;
        }
    }
    if (flag_isNewNode)
        return;
    queue<int> q;
    for (auto node : mink_e_node_set)
    {
        q.push(node);
    }
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    vector<int> visitEdge1(hyperEdge.size(), 0);
    unordered_set<int> node_set;
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        int count = 0;
        for (auto edge : nodeinfo[v])
        {
            if (!visitEdge1[edge] && edgeinfo[edge].second >= mink_e)
                ++count;
        }
        if (count >= mink_e)
            continue;
        node_set.insert(v);
        for (auto edge : nodeinfo[v])
        {
            if (visitEdge1[edge] != 0)
                continue;
            visitEdge1[edge] = 1;
            for (auto node : hyperEdge[edge])
            {
                if (core[node] == mink_e && node_set.find(node) == node_set.end())
                {
                    q.push(node);
                }
            }
        }
    }
    for (auto node : node_set)
    {
        core[node]--;
        if (core[node] == 0)
            core.erase(node);
        nodeinfo.erase(node);
    }
    for (auto node : node_set)
    {
        for (auto edge : hyperNode[node])
        {
            if (edgeinfo[edge].first < mink_e - 1)
                continue;
            int mink = INT_MAX;
            for (auto node : hyperEdge[edge])
            {
                mink = min(mink, core.at(node));
            }
            edgeinfo[edge].first = mink;
            nodeinfo[node].insert(edge);
        }
    }
    unordered_set<int> set_node;
    for (auto node : node_set)
    {
        set_node.insert(node);
        for (auto edge : nodeinfo[node])
            for (auto node1 : hyperEdge[edge])
                if (core[node1] == edgeinfo[edge].first + 1)
                {
                    nodeinfo[node1].erase(edge);
                    set_node.insert(node1);
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
                if (edgeinfo[edge].first < mink_e - 1)
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
void readfile(const string &filename, vector<vector<int>> &hyperEdge, vector<vector<int>> &InsertEdge)
{
    random_device seed;
    default_random_engine gen(seed());
    uniform_int_distribution<unsigned> u(1, 100);
    ifstream fin(filename, ios::in);
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
        if (u(gen) <= percent)
            InsertEdge.emplace_back(e);
        else
            hyperEdge.emplace_back(e);
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

void initialerase(vector<int> &eraseEdgeSet, int len, int n)
{
    for (int i = len - 1; i >= len - n; i--)
    {
        eraseEdgeSet.push_back(i);
    }
}

// Select a parallel SPS set from the edge set
void divideEdge(vector<vector<int>> &hyperEdge, const vector<int> &eraseEdge, unordered_map<int, int> &core, unordered_map<int, vector<int>> &parallel_edges, vector<bool> &remove_edge)
{
    unordered_map<int, bool> min_node_set;
    parallel_edges.clear();
    for (int i = 0; i < int(eraseEdge.size()); i++)
    {
        if (remove_edge[i])
            continue;
        int mink_e = INT_MAX;
        vector<int> mink_e_node;
        for (int j = 0; j < int(hyperEdge[eraseEdge[i]].size()); j++)
        {
            int node = hyperEdge[eraseEdge[i]][j];
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
            parallel_edges[mink_e].emplace_back(eraseEdge[i]);
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
        ofstream fout("./result/break/" + str + ".txt");
        for (int i = 1;; i++)
        {
            vector<vector<int>> hyperEdge;
            unordered_map<int, vector<int>> hyperNode;
            unordered_map<int, int> core;
            vector<int> eraseEdgeSet;
            getGraph(filename, hyperEdge, hyperNode);
            int n = int(hyperEdge.size());
            int num = 10 * i;
            if (num >= 300)
                break;
            kcoreDecomp(hyperEdge, hyperNode, core);
            unordered_map<int, unordered_set<int>> nodeinfo;
            vector<PII> edgeinfo;
            initialization(hyperEdge, hyperNode, core, nodeinfo, edgeinfo);
            initialerase(eraseEdgeSet, int(hyperEdge.size()), num);
            unordered_map<int, vector<int>> parallel_edges;
            vector<bool> remove_edge(int(eraseEdgeSet.size()), false);
            auto t1 = std::chrono::steady_clock::now();
            while (true)
            {
                divideEdge(hyperEdge, eraseEdgeSet, core, parallel_edges, remove_edge);
                if (parallel_edges.empty())
                    break;
                for (auto &it : parallel_edges)
                {
                    eraseEdge(hyperEdge, hyperNode, core, nodeinfo, edgeinfo, it.second);
                }
            }
            auto t2 = std::chrono::steady_clock::now();
            double dr_ns1 = std::chrono::duration<double, std::nano>(t2 - t1).count();
            fout << dr_ns1 << " ";
            for (int i = 0; i < num; i++)
                hyperEdge.pop_back();
            initial(hyperEdge, hyperNode);
            t1 = std::chrono::steady_clock::now();
            kcoreDecomp(hyperEdge, hyperNode, core);
            t2 = std::chrono::steady_clock::now();
            dr_ns1 = std::chrono::duration<double, std::nano>(t2 - t1).count();
            fout << dr_ns1 << endl;
        }
        fout.close();
    }
}