#ifndef UTILTYPES_H
#define UTILTYPES_H

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>

using namespace std;

class SSGParams{
public:
    SSGParams(int tau_n,
              float tau_c,
              int tau_f,
              float tau_d,
              float tau_p,
              float coeff_node_disappear1,
              float coeff_node_disappear2,
              float coeff_node_appear,
              float coeff_coh_exp_base,
              float coeff_coh_appear_thres);
    int tau_n;
    float tau_c;
    int tau_f;
    float tau_d;
    float tau_p;
    float coeff_node_disappear1;
    float coeff_node_disappear2;
    float coeff_node_appear;
    float coeff_coh_exp_base;
    float coeff_coh_appear_thres;
};

class RecognitionParams
{
public:
    RecognitionParams(float tau_r,
                      int plot_h,
                      int plot_w,
                      int ssg_h,
                      int ssg_w)
    {
        this->tau_r = tau_r;
        this->plot_h = plot_h;
        this->plot_w = plot_w;
        this->ssg_h = ssg_h;
        this->ssg_w = ssg_w;
    }
    float tau_r;
    int plot_h;
    int plot_w;
    int ssg_h;
    int ssg_w;
};

class GraphMatchParams
{
public:
    GraphMatchParams(float pos_weight,
                     float color_weight,
                     float area_weight);
    float pos_weight;
    float color_weight;
    float area_weight;
};

class SegmentTrackParams{
public:
    SegmentTrackParams(int tau_w, float tau_m);
    int tau_w;
    float tau_m;
};

typedef struct
{
    int pixelsSize;
    float avgHue;
    float avgSat;
    float avgVal;
    float avgR;
    float avgG;
    float avgB;
    float centerX;
    float centerY;
    std::vector<cv::Point> pixels;
}BlobStats;

class NodeSig
{
    public:
    int id;
    cv::Point center;
    float colorR;
    float colorG;
    float colorB;
    int   area;
    std::vector<std::pair<int,float> > edges;
    NodeSig()
    {
        colorR = 0;
        colorG = 0;
        colorB = 0;
        area = 0;
        center = cv::Point(0,0);
        id = 0;
    }

    static void write2File(NodeSig node, std::string filename)
    {
        std::ofstream file;
        filename.append("_graph.txt");
        file.open(filename.c_str(), std::ios::app);
        file << node.id << endl;
        file << node.center.x << endl;
        file << node.center.y << endl;
        file << node.colorR << endl;
        file << node.colorG << endl;
        file << node.colorB << endl;
        file << node.area << endl;
        file << node.edges.size() << endl;
        for (unsigned int i = 0; i < node.edges.size(); i++)
        {
            file << node.edges[i].first << " " << node.edges[i].second << endl;
        }
        file.close();
    }
    static void resetGraphFile(int nrNodes, std::string filename)
    {
        ofstream file;
        filename.append("_graph.txt");
        file.open(filename.c_str());
        file << nrNodes << endl;
        file.close();
    }
};

class SSG
{
    int id;
    int start_frame;
    int end_frame;
    int sample_frame;
public:
    SSG(int id){this->id = id;}
    SSG(int id, vector<NodeSig> nodes)
    {
        this->id = id;
        for(int i = 0; i < nodes.size(); i++)
            this->nodes.push_back(make_pair(nodes[i],1));
    }
    int getId(){return id;}
    void setId(int id){this->id = id;}
    void setStartFrame(int frame){start_frame = frame;}
    void setEndFrame(int frame){end_frame = frame;}
    int getStartFrame(){return start_frame;}
    int getEndFrame(){return end_frame;}
    void setSampleFrame(int frame){this->sample_frame = frame;}
    int getSampleFrame(){return sample_frame;}
    vector<pair<NodeSig, int> > nodes;
};

class RAG_m
{
public:
    RAG_m(){}
    RAG_m(vector<NodeSig>& nodes){this->nodes = nodes;}
    vector<NodeSig> nodes;
};


///////////////////////////////
//BasePoint Class Declaration//
///////////////////////////////

class BasePointSSG
{
    int number;
    string filename;
    vector<NodeSig> rag;
public:
    BasePointSSG(int number, string filename, vector<NodeSig>& rag)
    {
        this->number = number;
        this->filename = filename;
        this->rag = rag;
    }
    int getNumber(){return number;}
    string getFilename(){return filename;}
    vector<NodeSig>& getRAG(){return rag;}
};

///////////////////////////
//Place Class Declaration//
///////////////////////////

class PlaceSSG
{
    int id;
    vector<SSG> members;
public:
    PlaceSSG(int id, SSG member)
    {
        this->id = id;
        this->members.push_back(member);
    }
    void addMember(SSG member){this->members.push_back(member);}
    SSG& getMember(int member_id)
    {
        return members[member_id];
    }
    int getCount() {return members.size();}
    int getId() {return id;}
};

//////////////////////////////
//TreeNode Class Declaration//
//////////////////////////////

class TreeNode
{
    int label;
    double val;
    double x_pos;
    TreeNode* parent;
    vector<TreeNode*> children;
    PlaceSSG* descriptor;

public:
    TreeNode();
    TreeNode(int label, double val);
    void setLabel(int label){this->label = label;}
    int getLabel(){return label;}
    void setVal(double val){this->val = val;}
    double getVal(){return val;}
    void setXPos(double x_pos){this->x_pos = x_pos;}
    double getXPos(){return x_pos;}
    void setParent(TreeNode* parent){this->parent = parent;}
    TreeNode* getParent(){return parent;}
    void addChild(TreeNode* node);
    vector<TreeNode*>& getChildren();
    void setDescriptor(PlaceSSG* descriptor){this->descriptor = descriptor;}
    PlaceSSG* getDescriptor(){return descriptor;}
    bool isTerminal(){return children.size() == 0;}
    bool isRoot(){return parent == NULL;}

};

#endif // UTILTYPES_H
