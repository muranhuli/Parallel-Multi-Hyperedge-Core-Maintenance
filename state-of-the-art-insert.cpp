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

struct myCmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b)
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

void findHypergraphSubCore(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode, const unordered_map<int, int> &core, int u,
                           unordered_map<int, vector<int>> &preEdge, unordered_map<int, int> &cd)
{
    unordered_map<int, bool> visitNode;
    queue<int> Q;
    int k = core.at(u);
    Q.push(u);
    visitNode[u] = true;
    vector<int> visitEdge(hyperEdge.size(), false);
    while (!Q.empty())
    {
        int v = Q.front();
        Q.pop();

        for (auto &edge : hyperNode.at(v))
        {
            int minK = INT_MAX;
            for (auto &w : hyperEdge[edge])
                minK = min(minK, core.at(w));
            if (minK == k)
            {
                cd[v]++;
                preEdge[v].push_back(edge);
                if (visitEdge[edge])
                    continue;
                visitEdge[edge] = true;
                for (auto &w : hyperEdge[edge])
                {
                    if (minK == core.at(w) && visitNode.find(w) == visitNode.end())
                    {
                        Q.push(w);
                        visitNode[w] = true;
                    }
                }
            }
        }
    }
}

void insertEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, const vector<int> &e, int &vertices_num)
{

    int index = hyperEdge.size();
    hyperEdge.emplace_back();
    for (auto &it : e)
    {
        hyperEdge[index].push_back(it);
        hyperNode[it].push_back(index);
    }

    int r = e[0];
    bool flag = false;
    for (int i = 0; i < e.size(); i++)
    {
        if (core.find(e[i]) == core.end())
        {
            core[e[i]] = 1;
            flag = true;
        }
        if (core[r] > core[e[i]])
            r = e[i];
    }
    if (flag)
    {
        return;
    }
    unordered_map<int, int> cd;
    unordered_map<int, vector<int>> preEdge;
    findHypergraphSubCore(hyperEdge, hyperNode, core, r, preEdge, cd);
    vertices_num += int(cd.size());
    int k = core.at(r);
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    for (auto &node : cd)
    {
        if (core[node.first] == k)
        {
            mcd.insert(make_pair(node.first, node.second));
            deg[node.first] = node.second;
        }
    }
    vector<bool> visitEdge(hyperEdge.size(), false);
    while (!mcd.empty() && (*mcd.begin()).second <= k)
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        int v = p.first;
        for (auto &edge : preEdge[v])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (deg.find(node) == deg.end() || deg.at(node) <= deg.at(v))
                    continue;
                mcd.erase(make_pair(node, deg[node]));
                deg[node]--;
                mcd.insert(make_pair(node, deg[node]));
            }
        }
        deg.erase(v);
    }
    while (!mcd.empty())
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        core[p.first]++;
    }
}

void kcoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
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
                if (visitNode[node])
                    continue;
                node_count.erase(make_pair(node, deg[node]));
                deg[node]--;
                node_count.insert(make_pair(node, deg[node]));
            }
        }
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
void dividDataSet(vector<vector<int>> &hyperEdge, vector<vector<int>> &InsertEdge, int n)
{
    for (int i = 0; i < n; i++)
    {
        InsertEdge.emplace_back(hyperEdge[i]);
    }
    for (int i = 0; i < int(InsertEdge.size()); i++)
    {
        auto pos = find(hyperEdge.begin(), hyperEdge.end(), InsertEdge[i]);
        hyperEdge.erase(pos);
    }
}

int main()
{
    string file = "";
    string filepath = "";
    istringstream ss(file);
    string str;
    ofstream fout("");
    while (ss >> str)
    {

        cout << str << endl;
        string filename = filepath + str;
        vector<vector<int>> hyperEdge;
        unordered_map<int, vector<int>> hyperNode;
        unordered_map<int, int> core;
        vector<vector<int>> InsertEdge;
        getGraph(filename, hyperEdge, hyperNode);
        int n = int(hyperEdge.size());
        int num = 30;

        dividDataSet(hyperEdge, InsertEdge, num);
        initial(hyperEdge, hyperNode);
        kcoreDecomp(hyperEdge, hyperNode, core);
        int vertices_num = 0;
        auto t5 = std::chrono::steady_clock::now();
        for (int i = 0; i < InsertEdge.size(); i++)
        {
            insertEdge(hyperEdge, hyperNode, core, InsertEdge[i], vertices_num);
        }
        auto t6 = std::chrono::steady_clock::now();
        auto dr_ns3 = std::chrono::duration<double, std::nano>(t6 - t5).count();
        fout << str << " " << dr_ns3 / num << ' ' << vertices_num << endl;
    }
    fout.close();
}