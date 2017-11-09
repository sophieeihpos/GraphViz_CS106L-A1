#include <iostream>
#include "SimpleGraph.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>
#include <iterator>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <set>
#include <utility>
#include <map>
#include <ctime>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::iterator;
using std::vector;
using std::sin;
using std::cos;
using std::set;
using std::pair;
using std::map;
using std::time_t;

void Welcome();
void getFileName(string & fileName, int & runSecds);
int strToInt(string text);
void loadFile(string fileName, SimpleGraph & graph);
void initGraph(SimpleGraph & graph);
void marchNodes(SimpleGraph & graph);
bool findEdge(Edge edge,vector<Edge> & edges);

// Main method
int main() {
    Welcome();
    string fileName;
    int runSecds;
    string ifExit;
    while(true){


        getFileName(fileName,runSecds);
        SimpleGraph graph;
        loadFile(fileName,graph);
        initGraph(graph);
        InitGraphVisualizer(graph);
        DrawGraph(graph);

        time_t startT =time(NULL);
        double elapsed=0;
        while(elapsed<runSecds){
            marchNodes(graph);
            DrawGraph(graph);
            elapsed=std::difftime(time(NULL),startT);
        }

        while(true){
            cout << "Again? Y/N :";
            getline(cin,ifExit);
            if (ifExit.length()==1){
                char c=std::tolower(ifExit[0]);
                if (c=='y'){
                    break;
                }else if (c=='n'){
                   return 0;
                }
            }
        }
    }
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

void getFileName(string & fileName, int & runSecds){

    std::ifstream fileStream;
    cout << "Please enter file name of the graph: ";
    while(cin >> fileName){
        fileStream.open(fileName.c_str());
        if(fileStream.fail()){
            cout << "File is not found, please enter file name of the graph: ";
        }else{
            fileStream.close();
            break;
        }
    }

    cout << "Please enter number of seconds for running: ";

    runSecds=0;
    string text;
    while(true){
        getline(cin,text);
        try{
            runSecds=strToInt(text);
        }
        catch (std::invalid_argument & ia){
            cout << "Please enter a valid integer: ";
            continue;
        }
        if(runSecds!=0){
            break;
        }
    }
    return;
}

void loadFile(string fileName,SimpleGraph & graph){

    std::ifstream fileStream;
    fileStream.open(fileName.c_str());
    string currentLine;
    int numNodes=0;
    int currentNum;
    Edge edge;
    vector<Edge> edges;
    int count=0;
    if(fileStream.is_open()){
        getline(fileStream,currentLine);
        numNodes=strToInt(currentLine);
        while(true){
            getline(fileStream,currentLine,' ');
            if(fileStream.fail()){
                break;
            }else{
                currentNum=strToInt(currentLine);
                if(count<numNodes){
                    edge.start=currentNum;
                }
            }

            getline(fileStream,currentLine);
            if(fileStream.fail()){
                break;
            }else{
                currentNum=strToInt(currentLine);
                if(count<numNodes){
                    edge.end=currentNum;
                    edges.push_back(edge);
                }
            }
            count++;
        }
        fileStream.close();
        graph.edges=edges;
        graph.nodes=vector<Node>(numNodes);
    }
    return;
}
int strToInt(string text){
    size_t numDigits=std::count_if(text.begin(),text.end(),[]( char c ) { return std::isdigit( c ); } );
    if(numDigits==text.length()){
        return atoi(text.c_str());
    }else{
        throw std::invalid_argument("Input text is not a valid integer.");
    }
}

void initGraph(SimpleGraph & graph){
    const double PI=3.14159265358979323;
    const size_t SCALE=1;
    double deltaAngle;
    vector<Node>* nodes;
    nodes=& graph.nodes;
    deltaAngle=2*PI/nodes->size();
    double currentAngle=-PI;
    for (size_t i=0;i<graph.nodes.size();i++){
        (*nodes)[i].x = cos(currentAngle)*SCALE;
        (*nodes)[i].y=sin(currentAngle)*SCALE;
        currentAngle+=deltaAngle;
    }
    return;
}

void marchNodes(SimpleGraph & graph){
    const double K_REPEL=0.001;
    const double K_ATTRCT=0.001;
    vector<pair<double,double> > forces;
    Node* nodei;
    Node* nodej;
    double forceRepel,forceAttrct, theta;
    double deltX=0;
    double deltY=0;
    Edge edge;
    for(size_t i=0;i<graph.nodes.size();i++){
        nodei=& graph.nodes[i];
        for(size_t j=0;j<graph.nodes.size();j++){
            nodej=& graph.nodes[j];
            if(nodei==nodej){
                continue;
            }else{
                forceRepel=K_REPEL/sqrt(pow( (nodei->x-nodej->x), 2) + pow( (nodei->y-nodej->y), 2));
                theta=atan2( (nodej->y-nodei->y), (nodej->x-nodei->x) );
                edge.start=i;
                edge.end=j;
                deltX-= forceRepel*cos(theta);
                deltY-= forceRepel*sin(theta);
                if(findEdge(edge,graph.edges)){
                    forceAttrct=K_ATTRCT * ( pow(nodei->x-nodej->x,2) + pow ( nodei->y-nodej->y,2));
                    deltX+=forceAttrct*cos(theta);
                    deltY+=forceAttrct*sin(theta);
                }
            }
        }
        forces.push_back(std::make_pair(deltX,deltY));
        deltX=0;
        deltY=0;
    }

    for(size_t i=0;i<forces.size();i++){
        graph.nodes[i].x=graph.nodes[i].x+forces[i].first;
        graph.nodes[i].y=graph.nodes[i].y+forces[i].second;
    }

}

bool findEdge(Edge edge,vector<Edge> & edges){
    Edge currentEdge;
    for (size_t i=0;i<edges.size();i++){
        currentEdge=edges[i];
        if( (edge.start==currentEdge.start && edge.end==currentEdge.end) ||
                (edge.start==currentEdge.end && edge.end==currentEdge.start) ){
            return true;
        }
    }
    return false;
}


