#include <iostream>
#include <cstdlib>
#include <queue>
#include <chrono>

#include "csp.cpp"

using namespace std;

class CSPMAC;

bool AC3(CSPMAC *csp);
bool revise(CSPMAC *csp, int xi, int xj);

class CSPMAC : public CSP {
public:
    CSPMAC(int colors, Graph graph);

    int selectUnassignmentVariable();
    vector<int> orderDomainValues(int var);

protected:
    bool inference(int var, int color);
private:
    bool selectFirstVariable;
};

CSPMAC::CSPMAC(int c, Graph g) : CSP(c, g), selectFirstVariable(true) {}

int CSPMAC::selectUnassignmentVariable() {
    if(selectFirstVariable) {
        selectFirstVariable = false;
        return degreeHeuristic(assignment, graph);
    }
    return minimumRemainingValue(assignment, domain);
}

vector<int> CSPMAC::orderDomainValues(int var) {
    if(var < 0 || var >= graph.adjList.size()) {
        cout<<"Exit from CSPMAC::orderDomainValues"<<endl;
        exit(EXIT_FAILURE);
    }
    return leastConstrainingValue(var, domain, graph);
}

bool CSPMAC::inference(int var, int color) {

    /* Maintain Arc Consistency (AC-3) */
    if(!AC3(this))
        return false;
    /* End */

    // If any variable has only one value in domain then assign it
    for(int i=0; i<domain.size(); i++) {
        if(domain[i].size() == 1)
            addAssignment(i, domain[i][0], false);
        else if(domain[i].size() == 0)
            return false;
    }
    return true;
}

bool AC3(CSPMAC *csp) {
    queue< pair<int, int> > q;

    for(int i=0; i<csp->graph.adjList.size(); i++)
        for(int j=0; j<csp->graph.adjList[i].size(); j++)
            if(csp->graph.adjList[i][j])
                q.push({i, j});

    while(!q.empty()) {
        pair<int, int> qF = q.front();
        int xi = qF.first;
        int xj = qF.second;
        q.pop();
        if(revise(csp, xi, xj)) {
            if(csp->domain[xi].size() == 0)
                return false;
            for(int xk: csp->graph.adjList[xi])
                if(xk != xj)
                    q.push({xk, xi});
        }
    }
    return true;
}

bool revise(CSPMAC *csp, int xi, int xj) {
    bool revised = false;
    for(int x=0; x<csp->domain[xi].size(); x++) {
        bool satisfies = false;
        for(int y=0; y<csp->domain[xj].size(); y++) {
            if(csp->domain[xi][x] != csp->domain[xj][y]) {
                satisfies = true;
                break;
            }
        }
        if(!satisfies) {
            csp->domain[xi].erase(csp->domain[xi].begin()+x);
            revised = true;
        }
    }
    return revised;
}

int main() {
//    vector< vector<int> > adjList(7);
//    adjList[0].push_back(1);
//    adjList[0].push_back(2);
//    adjList[1].push_back(0);
//    adjList[1].push_back(2);
//    adjList[1].push_back(3);
//    adjList[2].push_back(0);
//    adjList[2].push_back(1);
//    adjList[2].push_back(3);
//    adjList[2].push_back(4);
//    adjList[2].push_back(5);
//    adjList[3].push_back(1);
//    adjList[3].push_back(2);
//    adjList[3].push_back(4);
//    adjList[4].push_back(2);
//    adjList[4].push_back(3);
//    adjList[4].push_back(5);
//    adjList[5].push_back(2);
//    adjList[5].push_back(4);

    Graph graph("adjMatrix.txt");

    CSPMAC csp(3, graph);

    auto start = std::chrono::high_resolution_clock::now();
    Assignment assignment = backtrack(csp);
    auto finish = std::chrono::high_resolution_clock::now();
    float exeTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count(); // in ns

    csp.assignment = assignment;

    if(csp.isAssignmentSolution()) {
        cout<<"Node\t->\tColor"<<endl;
        cout<<"---------------------"<<endl;
        for(int i=0; i<assignment.size(); i++) {
            cout<<(i+1)<<"\t->\t"<<assignment[i]<<endl;
        }
    } else {
        cout<<"Solution does not exist."<<endl;
    }

    cout<<"\nExecution time: "<<exeTime / 1000000<<" ms.\n";

	return 0;
}


