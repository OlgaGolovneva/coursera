#include <algorithm>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <queue>

// Build suffix tree of the string text given its suffix array suffix_array
// and LCP array lcp_array. Return the tree as a mapping from a node ID
// to the vector of all outgoing edges of the corresponding node. The edges in the
// vector must be sorted in the ascending order by the first character of the edge label.
// Root must have node ID = 0, and all other node IDs must be different
// nonnegative integers.
//
// For example, if text = "ACACAA$", an edge with label "$" from root to a node with ID 1
// must be represented by Edge(1, 6, 7). This edge must be present in the vector tree[0]
// (corresponding to the root node), and it should be the first edge in the vector
// (because it has the smallest first character of all edges outgoing from the root).

using std::queue;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;



// Data structure to store edges of a suffix tree.
struct Edge {
    // The ending node of this edge.
    int node;
    // Starting position of the substring of the text
    // corresponding to the label of this edge.
    int start;
    // Position right after the end of the substring of the text
    // corresponding to the label of this edge.
    int end;
    
    Edge(int node_, int start_, int end_) : node(node_), start(start_), end(end_) {}
    Edge(const Edge& e) : node(e.node), start(e.start), end(e.end) {}
};

// Data structure to store Nodes
struct SuffixTreeNode{
    SuffixTreeNode* parent;
    
    std::map<char,SuffixTreeNode*> children;
    
    //number of letters from root to current node
    int stringDepth;
    int edgeStart;
    int edgeEnd;
    int id;
    
    SuffixTreeNode(SuffixTreeNode* parent, std::map<char,SuffixTreeNode*> children,int stringDepth,int edgeStart,int edgeEnd): parent(parent),children(children),stringDepth(stringDepth),edgeStart(edgeStart),edgeEnd(edgeEnd) {}
};

void CreateNewLeaf(SuffixTreeNode*& node,const string& text, int& suffix){
    std::map<char,SuffixTreeNode*> empty;
    SuffixTreeNode* leaf=new SuffixTreeNode(node,empty,(int)text.size()-suffix,suffix+node->stringDepth,(int)text.size()-1);
    node->children[text[leaf->edgeStart]]=leaf;
}

SuffixTreeNode* BreakEdge(SuffixTreeNode*& node,const string& s,const int& start, const int& offset){
    SuffixTreeNode* midNode;
    
    std::map<char,SuffixTreeNode*> empty;
    
    char startChar=s[start];
    char midChar=s[start+offset];
    
    //midNode=new SuffixTreeNode(node,empty,node->stringDepth+offset,start+offset,node->children[startChar]->edgeEnd);
    midNode=new SuffixTreeNode(node,empty,node->stringDepth+offset,start,start+offset-1);
    
    midNode->children[midChar]=node->children[startChar];
    node->children[startChar]->parent=midNode;
    node->children[startChar]->edgeStart+=offset;
    node->children[startChar]=midNode;
    
    return midNode;
}

const char alphabet[] = {'$','A','C','G','T'};
const int alphabetSize = 5;

map<int, vector<Edge> > CreateTree(SuffixTreeNode*& root){
    map<int, vector<Edge> > result;
    queue<SuffixTreeNode*> q;
    q.push(root);
    root->id=0;
    int id=1;
    while (!q.empty())
    {
        SuffixTreeNode* node = q.front();
        q.pop();
        vector<Edge> kids;
        for (int i = 0; i < alphabetSize;i++)
        {
            char c = alphabet[i];
            if (node->children.find(c)!=node->children.end())
            {
                node->children[c]->id=id;
                Edge kid = Edge(node->children[c]->id,node->children[c]->edgeStart,node->children[c]->edgeEnd);
                kids.push_back(kid);
                q.push(node->children[c]);
                id++;
            }
        }
        result[node->id]=kids;
    }
    return result;
}
map<int, vector<Edge> > SuffixTreeFromSuffixArray(const vector<int>& suffix_array,
                                                  const vector<int>& lcp_array,
                                                  const string& text) {

    std::map<char,SuffixTreeNode*> empty;
    
    SuffixTreeNode* root=new SuffixTreeNode(NULL,empty,0,-1,-1);
    
    int lcpPrev=0;
    int suffix,edgeStart,offset;
    
    SuffixTreeNode* curNode=root;
    SuffixTreeNode* midNode;
    
    for (int i=0;i<text.size();i++){
        suffix=suffix_array[i];
        while (curNode->stringDepth>lcpPrev) curNode=curNode->parent;
        if (curNode->stringDepth == lcpPrev) {
            CreateNewLeaf(curNode,text,suffix);
            curNode=curNode->children[text[suffix+curNode->stringDepth]];
        }
        else{
            edgeStart=suffix_array[i-1]+curNode->stringDepth;
            offset=lcpPrev-curNode->stringDepth;
            midNode=BreakEdge(curNode,text,edgeStart,offset);
            CreateNewLeaf(midNode,text,suffix);
            curNode=midNode->children[text[suffix+midNode->stringDepth]];
        }
        //std::map<char,SuffixTreeNode*> children;
        
        if (i<text.size()-1) lcpPrev=lcp_array[i];
    }
    
    map<int, vector<Edge> > tree = CreateTree(root);
    
    return tree;
}


int main() {
    char buffer[200001];
    scanf("%s", buffer);
    string text = buffer;
    vector<int> suffix_array(text.length());
    for (int i = 0; i < text.length(); ++i) {
        scanf("%d", &suffix_array[i]);
    }
    vector<int> lcp_array(text.length() - 1);
    for (int i = 0; i + 1 < text.length(); ++i) {
        scanf("%d", &lcp_array[i]);
    }
    // Build the suffix tree and get a mapping from
    // suffix tree node ID to the list of outgoing Edges.
    map<int, vector<Edge> > tree = SuffixTreeFromSuffixArray(suffix_array, lcp_array, text);
    printf("%s\n", buffer);
    // Output the edges of the suffix tree in the required order.
    // Note that we use here the contract that the root of the tree
    // will have node ID = 0 and that each vector of outgoing edges
    // will be sorted by the first character of the corresponding edge label.
    //
    // The following code avoids recursion to avoid stack overflow issues.
    // It uses a stack to convert recursive function to a while loop.
    // The stack stores pairs (node, edge_index).
    // This code is an equivalent of
    //
    //    OutputEdges(tree, 0);
    //
    // for the following _recursive_ function OutputEdges:
    //
    // void OutputEdges(map<int, vector<Edge> > tree, int node_id) {
    //   const vector<Edge>& edges = tree[node_id];
    //   for (int edge_index = 0; edge_index < edges.size(); ++edge_index) {
    //     printf("%d %d\n", edges[edge_index].start, edges[edge_index].end);
    //     OutputEdges(tree, edges[edge_index].node);
    //   }
    // }
    //
    vector<pair<int, int> > stack(1, make_pair(0, 0));
    while (!stack.empty()) {
        pair<int, int> p = stack.back();
        stack.pop_back();
        int node = p.first;
        int edge_index = p.second;
        if (!tree.count(node)) {
            continue;
        }
        const vector<Edge>& edges = tree[node];
        if (edge_index + 1 < edges.size()) {
            stack.push_back(make_pair(node, edge_index + 1));
        }
        if (edges.size()>0){
        printf("%d %d\n", edges[edge_index].start, edges[edge_index].end+1);
            stack.push_back(make_pair(edges[edge_index].node, 0));}
    }
    return 0;
}