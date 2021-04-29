#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <RTree.h>

int RTree::numberOfDims;


bool RTree::deleteData(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry)
{
  assert(coordinates->size() == numberOfDims);
  assert(dimensions->size() == numberOfDims);

  RTree::Node *fl = findLeaf(root, coordinates, dimensions, entry);

  if(fl == nullptr)
  {
    std::wcout << "No such node" << std::endl;
    findLeaf(root, coordinates, dimensions, entry);
  }

  assert(fl != nullptr);
  assert(fl->leaf);

  auto li = fl -> children.begin();
  auto removed = 0;

  while (li != fl->children.end())
  {
    Entry *e = (Entry *) *li;
    if (e->entry == entry)
    {
      removed = e->entry;
      int index = 0;
      for(int i = 0; i < fl->children.size(); i++)
      {
        if(fl->children.at(i) == *li)
        {
          index = i + 1;
        }
      }

      fl->children.erase(fl->children.begin() + index);
      break;
    }
    li++;
  }

  if (removed != 0)
  {
    treeDensify(fl);
    size--;
  }

  if (size == 0)
  {
    root = Node::rootMake(true);
  }

  return (removed != 0);
}


bool RTree::deleteData(std::vector<float> *coordinates, int entry)
{
  return deleteData(coordinates, &pointDims, entry);
}


void RTree::search(std::vector<float> *coordinates, std::vector<float> *dimensions, RTree::Node *n, std::vector<int> *results)
{
  if (n->leaf)
  {
    for (Node *e : n->children)
    {
      if (isOverlap(coordinates, dimensions, &(e->coordinates), &(e->dimensions)))
      {
        results->push_back(((Entry *) e)->entry);
      }
    }
  }
  else
  {
    for (Node *c : n->children)
    {
      if (isOverlap(coordinates, dimensions, &(c->coordinates), &(c->dimensions)))
      {
        search(coordinates, dimensions, c, results);
      }
    }
  }
}




float RTree::receiveNecessaryEnlargement(std::vector<float> *coordinates, std::vector<float> *dimensions, RTree::Node *node)
{
  float area = getArea(dimensions);
  auto *pVector = new std::vector<float>(dimensions->size());

  for (unsigned long i = 0; i < pVector->size(); i++)
  {
    if ((coordinates->at(i) + dimensions->at(i)) < (node->coordinates.at(i) + node->dimensions.at(i)))
    {
      pVector->at(i) = node->coordinates.at(i) + node->dimensions.at(i) - coordinates->at(i) - dimensions->at(i);
    }
    else if (coordinates->at(i) + dimensions->at(i) > (node->coordinates.at(i) + node->dimensions.at(i)))
    {
      pVector->at(i) = coordinates->at(i) - node->coordinates.at(i);
    }
  }

  float toPlus = 1.0f;

  for (unsigned long i = 0; i < dimensions->size(); i++)
  {
    toPlus *= dimensions->at(i) + pVector->at(i);
  }
  return (toPlus - area);
}


RTree::RTree()
{
  this->maxEntries = 4;
  this->minEntries = 2;
  this->numberOfDims = 2;
  this->pointDims = {0, 0};
  this->root = Node::rootMake(true);
}



RTree::Node* RTree::chooseLeaf(RTree::Node *node, RTree::Entry *entry)
{
  if (node-> leaf)
  {
    return node;
  }

  float d = std::numeric_limits<float>::max();
  Node *nextNode = nullptr;

  for (RTree::Node *child : node->children)
  {
    float inc = receiveNecessaryEnlargement(&(child->coordinates), &(child->dimensions), entry);

    if (inc < d)
    {
      d = inc;
      nextNode = child;
    }
    else if (inc == d)
    {
      float currentSquare = 1.0F;
      float existingSquare = 1.0F;

      for (int i = 0; i < child-> dimensions.size(); i++)
      {
        currentSquare *= nextNode->dimensions.at(static_cast<unsigned long>(i));
        existingSquare *= child->dimensions.at(static_cast<unsigned long>(i));
      }

      if (existingSquare < currentSquare)
      {
        nextNode = child;
      }
    }
  }
  return chooseLeaf(&*nextNode, entry);
}


float RTree::getArea(std::vector<float> *dimensions)
{

  float square = 1.0F;
  for (unsigned long i = 0; i < dimensions->size(); i++)
  {
    square *= dimensions->at(i);
  }

  return square;
}




void RTree::tighten(std::vector<RTree::Node *> nodes)
{
  assert(((nodes.size() >= 1)));

  for(RTree::Node *n: nodes)
  {
    assert(n->children.size() > 0);

    std::vector<float> minCoords{0, 0};

    std::vector<float> maxCoords{0, 0};

    for (unsigned long long int i = 0; i < numberOfDims; i++)
    {
      minCoords.at(i) = std::numeric_limits<float>::max();
      maxCoords.at(i) = 0.000000;

      for (Node *pNode : n->children)
      {
        pNode->parent = n;
        if (pNode->coordinates[i] < minCoords.at(i))
        {
          minCoords.at(i) = pNode->coordinates[i];
        }

        if ((pNode->coordinates[i] + pNode->dimensions[i]) > maxCoords.at(i))
        {
          maxCoords.at(i) = (pNode->coordinates[i] + pNode->dimensions[i]);
        }
      }
    }

    for (int i = 0; i < numberOfDims; i++)
    {
      maxCoords.at(i) -= minCoords.at(i);
    }

  }
}


void RTree::treeConfigure(RTree::Node *node, RTree::Node *nextNode)
{
  if(node == root)
  {
    if(nextNode != nullptr)
    {
      root = Node::rootMake(false);
      root->children.push_back(node);
      node->parent = root;
      root->children.push_back(nextNode);
      nextNode->parent = root;
    }

    std::vector<RTree::Node *> value{root};
    tighten(value);

    return;
  }

  std::vector<RTree::Node *> value{node};
  tighten(value);

  if(nextNode != nullptr)
  {
    std::vector<RTree::Node *> value{nextNode};
    tighten(value);

    if(node->parent->children.size() > minEntries)
    {
      std::vector<Node*> splits = nodeSplitter(node->parent);
      treeConfigure(splits[0], splits[1]);
    }
  }

  if(node->parent != nullptr)
  {
    treeConfigure(node->parent, nullptr);
  }
}

std::vector<RTree::Node *> RTree::seedAssemble(std::vector<Node *> *newNode)
{
  std::vector<RTree::Node*> bestPair(2);
  bool foundBestPair = false;
  float separateBest = 0.0f;

  for(int i = 0; i < numberOfDims; i++)
  {
    float dimmension = std::numeric_limits<float>::max(),
        dimmensionMin = std::numeric_limits<float>::max();
    float dimmension2 = -1.0f * std::numeric_limits<float>::max(),
        dimmensionMax2 = -1.0f * std::numeric_limits<float>::max();
    Node *nodeMin = nullptr;
    Node *nodeMax = nullptr;

    for (Node *n : *newNode)
    {
      if (n->coordinates[i] < dimmension)
      {
        dimmension = n->coordinates[i];
      }

      if (n->dimensions[i] + n->coordinates[i] > dimmension2)
      {
        dimmension2 = n->dimensions[i] + n->coordinates[i];
      }

      if (n->coordinates[i] > dimmensionMax2)
      {
        dimmensionMax2 = n->coordinates[i];
        nodeMax = n;
      }

      if (n->dimensions[i] + n->coordinates[i] < dimmensionMin)
      {
        dimmensionMin = n->dimensions[i] + n->coordinates[i];
        nodeMin = n;
      }
    }

    float separate = (nodeMax == nodeMin) ? -1.0F : static_cast<float>(std::abs((dimmensionMin - dimmensionMax2) / (dimmension2 - dimmension)));

    if (separate >= separateBest)
    {
      bestPair[0] = nodeMax;
      bestPair[1] = nodeMin;
      separateBest = separate;
      foundBestPair = true;
    }
  }

  if (!foundBestPair)
  {
    bestPair.clear();
    bestPair.push_back(newNode->front());
    newNode->erase(newNode->begin() + 1);
    bestPair.push_back(newNode->front());
  }

  int firstIndex = 0;
  int secondIndex = 0;

  for(int i = 0; i < newNode->size(); i++)
  {
    if(newNode->at(i) == bestPair[0]){
      firstIndex = i + 1;
    }

    if(newNode->at(i) == bestPair[1])
    {
      secondIndex = i;
    }

  }

  newNode->erase(newNode->begin() + firstIndex);
  newNode->erase(newNode->begin() + secondIndex);

  return bestPair;
}


RTree::Node *RTree::next(std::vector<RTree::Node *> *cc)
{
  return cc->at(0);
}


std::vector<int> RTree::search(std::vector<float> *coordinates, std::vector<float> *dimensions)
{
  assert(coordinates->size() == numberOfDims);
  assert(dimensions->size() == numberOfDims);
  std::vector<int> results = {};
  search(coordinates, dimensions, root, &results);

  return results;
}


bool RTree::isOverlap(std::vector<float> *cordinatesNew, std::vector<float> *dimensionsNew, std::vector<float> *coordinates,
                      std::vector<float> *dimensions)
{

  const float CONST =1.001F;

  for (int i = 0; i < cordinatesNew->size(); i++)
  {
    bool overlapInThisDimension = false;

    if (cordinatesNew[i] == coordinates[i])
    {
      overlapInThisDimension = true;
    }
    else if (cordinatesNew[i] < coordinates[i])
    {
      if (cordinatesNew->at(i) + (CONST * dimensionsNew->at(i)) >= coordinates->at(i))
      {
        overlapInThisDimension = true;
      }
    }
    else if (cordinatesNew[i] > coordinates[i])
    {
      if (coordinates->at(i) + (CONST * dimensions->at(i)) >= cordinatesNew->at(i))
      {
        overlapInThisDimension = true;
      }
    }
    if (!overlapInThisDimension)
    {
      return false;
    }
  }

  return true;
}



RTree::Node* RTree::findLeaf(RTree::Node *leafNode, std::vector<float> *coordinates, std::vector<float> *dimensions, int entry)
{
  if (leafNode->leaf)
  {
    for (Node *childrenLeafNode : leafNode->children)
    {
      if (((Entry *) childrenLeafNode)->entry == entry)
      {
        return leafNode;
      }
    }

    return nullptr;
  }
  else
  {

    for (Node *childrenLeafNode : leafNode->children)
    {
      if (isOverlap(&(childrenLeafNode->coordinates), &(childrenLeafNode->dimensions), coordinates, dimensions))
      {
        Node *result = findLeaf(childrenLeafNode, coordinates, dimensions, entry);
        if (result != nullptr)
        {
          return result;
        }
      }
    }
    return nullptr;
  }
}

RTree::Node::Node(std::vector<float> *coordinates, std::vector<float> *dimensions, bool leaf)
{
  this->coordinates = *coordinates;
  this->dimensions = *dimensions;
  this->leaf = leaf;
}



void RTree::treeDensify(RTree::Node *node)
{
  std::vector<RTree::Node> vNode = {};

  while (node != root){
    if (node->leaf && (node->children.size() < minEntries))
    {
      for(auto elem : node->children)
      {
        vNode.push_back(*elem);
      }

      int index = 0;
      for(int i = 0; i < node -> parent -> children.size(); i++)
      {
        if(node->parent->children.at(i) == node)
        {
          index = i + 1;
        }
      }

      node->parent->children.erase(node->parent->children.begin() + index);
    }
    else if (!node->leaf && (node->children.size() < minEntries))
    {
      std::list<Node> leafForVisit = {};

      while (leafForVisit.size() > 0)
      {
        Node backNode = leafForVisit.back();
        if (backNode.leaf){
          for(auto elem : backNode.children)
          {
            vNode.push_back(*elem);
          }
        }
        else
        {
          for(auto elem : backNode.children)
          {
            leafForVisit.push_back(*elem);
          }
        }
      }

      int index = 0;
      for(int i = 0; i < node->parent->children.size(); i++)
      {
        if(node->parent->children.at(i) == node)
        {
          index = i + 1;
        }
      }

      node->parent->children.erase(node->parent->children.begin() + index);
    }
    else
    {
      std::vector<Node *> argument;
      argument.push_back(node);
      tighten(argument);
    }

    node = node->parent;
  }

  if (root->children.size() == 0)
  {
    root = Node::rootMake(true);
  } else if ((root->children.size() == 1) && (!root->leaf))
  {
    root = root->children.front();
    root->parent = nullptr;
  }
  else
  {
    std::vector<Node *> argument;
    argument.push_back(root);
    tighten(argument);
  }

  for (Node ne : vNode)
  {
    Entry *e = (Entry*) &ne;
    insert(&(e->coordinates), &(e->dimensions), e->entry);
  }
  size -= vNode.size();
}

RTree::Node *RTree::Node::rootMake(bool isLeaf)
{
  std::vector<float> coordsInit(static_cast<unsigned long>(numberOfDims));
  std::vector<float> dimensionsInit(static_cast<unsigned long>(RTree::numberOfDims));

  for (int i = 0; i < 2; i++)
  {
    coordsInit[i] = static_cast<float>(std::sqrt(std::numeric_limits<float>::max()));
    dimensionsInit[i] = -2.0F * static_cast<float>(std::sqrt(std::numeric_limits<float>::max()));
  }

  return new RTree::Node(&coordsInit, &dimensionsInit, isLeaf);
}


std::vector<RTree::Node *> RTree::nodeSplitter(RTree::Node *node)
{
  std::vector<Node *> nodes = std::vector<RTree::Node *>{node, new Node(&(node->coordinates), &(node->dimensions), node->leaf)};
  nodes.at(1)->parent = node->parent;

  if (nodes.at(1) != nullptr)
  {
    nodes.at(1)->parent->children.push_back(nodes.at(1));
  }

  std::vector<Node *> childrenNode = node->children;
  node->children.clear();

  std::vector<Node *> ss = seedAssemble(&childrenNode);

  nodes.at(0)->children.push_back(ss.at(0));
  nodes.at(1)->children.push_back(ss.at(1));

  tighten(nodes);

  while (childrenNode.size() != 0)
  {
    if ((nodes[0]->children.size() >= static_cast<unsigned long>(minEntries))
        && (nodes.at(1)->children.size() + childrenNode.size() == static_cast<unsigned long>(minEntries)))
    {
      std::fill(childrenNode.begin(), childrenNode.end(), nodes[1]);
      childrenNode.clear();
      tighten(nodes);

      return nodes;
    }
    else if ((nodes[1]->children.size() >= static_cast<unsigned long>(minEntries))
             && (nodes.at(0)->children.size() + childrenNode.size() == static_cast<unsigned long>(minEntries)))
    {
      std::fill(childrenNode.begin(), childrenNode.end(), nodes[0]);
      childrenNode.clear();
      tighten(nodes);

      return nodes;
    }

    RTree::Node *cNode = next(&childrenNode);
    RTree::Node *desirable = nullptr;

    float abiba = receiveNecessaryEnlargement(&(nodes.at(0)->coordinates), &(nodes.at(0)->dimensions), cNode);
    float aboba = receiveNecessaryEnlargement(&(nodes.at(1)->coordinates), &(nodes.at(1)->dimensions), cNode);

    if (abiba < aboba)
    {
      desirable = nodes.at(0);

    } else if (abiba > aboba)
    {
      desirable = nodes.at(1);

    }
    else
    {
      float booba1 = getArea(&(nodes.at(0)->dimensions));
      float booba2 = getArea(&(nodes.at(1)->dimensions));

      if (booba1 < booba2)
      {
        desirable = nodes.at(0);

      } else if (abiba > booba2)
      {
        desirable = nodes.at(1);
      }
      else
      {
        if (nodes.at(0)->children.size() < nodes.at(1)->children.size())
        {
          desirable = nodes.at(0);

        } else if (nodes.at(0)->children.size() > nodes.at(1)->children.size())
        {
          desirable = nodes.at(1);
        }
        else
        {
          desirable = nodes.at(static_cast<unsigned long>(rand()));
        }
      }
    }
    desirable-> children.push_back(cNode);
    tighten(std::vector<RTree::Node *>{desirable});

  }
  return nodes;
}

void RTree::insert(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry)
{
  assert(coordinates->size() == numberOfDims);
  assert(dimensions->size() == numberOfDims);

  Entry *pEntry = new Entry(coordinates, dimensions, entry);
  RTree::Node *leaf = chooseLeaf(this -> root, pEntry);
  leaf->children.push_back(pEntry);
  size++;
  pEntry->parent = leaf;

  if (leaf->children.size() > maxEntries)
  {
    std::vector<Node*> splits = nodeSplitter(leaf);
    treeConfigure(splits[0], splits[1]);
  } else
  {
    treeConfigure(leaf, nullptr);
  }
}

RTree::Entry::Entry(std::vector<float> *coordinates, std::vector<float> *dimensions, int entry)
    :Node(coordinates, dimensions, true)
{
  this->entry = entry;
}

