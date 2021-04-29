#include <vector>
#include <list>


#pragma once
class RTree
{

 public:
  class Node
  {
   public:
    std::vector<float> coordinates;
    std::vector<Node*> children;
    std::vector<float> dimensions;
    bool leaf;
    Node* parent;
    static Node*rootMake(bool isLeaf);

   public:
    Node(std::vector<float> *coordinates, std::vector<float> *dimensions, bool leaf);
    virtual ~Node();
  };

  class Entry : public Node
  {
   public:
    int entry;
    Entry(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry);
    virtual ~Entry();
  };

 private:
  RTree::Node* chooseLeaf(RTree::Node *node, RTree::Entry *entry);

  bool deleteData(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry);

  RTree::Node* findLeaf(RTree::Node *leafNode, std::vector<float> *coordinates, std::vector<float> *dimensions, int entry);

  float getArea(std::vector<float> *dimensions);

  void insert(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry);

  bool isOverlap(std::vector<float> *cordinatesNew, std::vector<float> *dimensionsNew, std::vector<float> *coordinates,
                 std::vector<float> *dimensions);

  RTree::Node *next(std::vector<RTree::Node *> *cc);

  std::vector<Node*> nodeSplitter(Node *node);

  float receiveNecessaryEnlargement(std::vector<float> *coordinates, std::vector<float> *dimensions, Node *node);

  void search(std::vector<float> *coordinates, std::vector<float> *dimensions, RTree::Node *n, std::vector<int> *results);

  std::vector<RTree::Node *> seedAssemble(std::vector<Node *> *newNode);

  void tighten(std::vector<RTree::Node*> nodes);

  void treeConfigure(RTree::Node *node, RTree::Node *nextNode);

  void treeDensify(RTree::Node *node);

  int maxEntries;
  int minEntries;
  int size;
  static int numberOfDims;

  Node *root;
  std::vector<float> pointDims;

 public:
  RTree();
  void insert(std::vector<float> *coordinates, int entry);
  std::vector<int> search(std::vector<float> *coordinates, std::vector<float> *dimensions);
  bool deleteData(std::vector<float> *coordinates, int entry);
  virtual ~RTree();
};