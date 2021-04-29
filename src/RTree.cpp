#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <float.h>
#include <RTree.h>

int RTree::numDims;


// NODE STRUCTURE
RTree::Node::Node(std::vector<float> coords, std::vector<float> dimensions, bool leaf)
{
  this->coords = coords;
  this->dimensions = dimensions;
  this->leaf = leaf;
}


RTree::Node *RTree::Node::buildRoot(bool asLeaf)
{
  std::vector<float> coordsInit(static_cast<unsigned long>(numDims));
  std::vector<float> dimensionsInit(static_cast<unsigned long>(RTree::numDims));

  for (int i = 0; i < 2; i++)
  {
    coordsInit[i] = static_cast<float>(std::sqrt(std::numeric_limits<float>::max()));
    dimensionsInit[i] = -2.0f * static_cast<float>(std::sqrt(std::numeric_limits<float>::max()));
  }

  return new RTree::Node(coordsInit, dimensionsInit, asLeaf);
}


RTree::Entry::Entry(std::vector<float> coords, std::vector<float> dimensions, int entry)
    : Node(coords, dimensions, true)
{
  this->entry = entry;
}



RTree::RTree()
{
  this->maxEntries = 3;
  this->minEntries = 2;
  this->numDims = 2;
  this->pointDims = {0, 0};
  this->root = Node::buildRoot(true);
}




void RTree::insert(std::vector<float> coords, std::vector<float> dimensions, int entry)
{
  assert(coords.size() == numDims);
  assert(dimensions.size() == numDims);

  Entry *e = new Entry(coords, dimensions, entry);
  RTree::Node l = chooseLeaf(*root, *e);
  l.children.push_back(e);
  size++;
  e->parent = &l;

  if (l.children.size() > maxEntries)
  {
    std::vector<Node*> splits = splitNode(&l);
    adjustTree(splits[0], splits[1]);
  }
  else
  {
    adjustTree(&l, nullptr);
  }
}


bool RTree::deleting(std::vector<float> &coords, std::vector<float> &dimensions, int entry)
{
  assert(coords.size() == numDims);
  assert(dimensions.size() == numDims);

  RTree::Node *fl = findLeaf(root, &coords, &dimensions, entry);

  if(fl == nullptr)
  {
    std::wcout << "Node is not founded" << std::endl;
    findLeaf(root, &coords, &dimensions, entry);
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
    condenseTree(fl);
    size--;
  }

  if (size == 0)
  {
    root = Node::buildRoot(true);
  }

  return (removed != 0);
}


bool RTree::deleting(std::vector<float> coords, int entry)
{
  return deleting(coords, pointDims, entry);
}


void RTree::search(std::vector<float> *coords, std::vector<float> *dimensions, RTree::Node *n, std::vector<int> *results)
{
  if (n->leaf)
  {
    for (Node *e : n->children)
    {
      if (isOverlap(*coords, *dimensions, e->coords, e->dimensions))
      {
        results->push_back(((Entry *) e)->entry);
      }
    }
  }
  else
  {
    for (Node *c : n->children)
    {
      if (isOverlap(*coords, *dimensions, c->coords, c->dimensions))
      {
        search(coords, dimensions, c, results);
      }
    }
  }
}




float RTree::getRequiredExpansion(std::vector<float> coords, std::vector<float> dimensions, RTree::Node e)
{
  float area = getArea(dimensions);
  auto *deltas = new std::vector<float>(dimensions.size());

  for (unsigned long i = 0; i < deltas->size(); i++)
  {
    if ((coords.at(i) + dimensions.at(i)) < (e.coords.at(i) + e.dimensions.at(i)))
    {
      deltas->at(i) = e.coords.at(i) + e.dimensions.at(i) - coords.at(i) - dimensions.at(i);
    } else if (coords.at(i) + dimensions.at(i) > (e.coords.at(i) + e.dimensions.at(i)))
    {
      deltas->at(i) = coords.at(i) - e.coords.at(i);
    }
  }

  float expanded = 1.0f;

  for (unsigned long i = 0; i < dimensions.size(); i++)
  {
    expanded *= dimensions.at(i) + deltas->at(i);
  }

  return (expanded - area);
}


RTree::Node RTree::chooseLeaf(RTree::Node n, RTree::Entry e)
{
  if (n.leaf)
  {
    return n;
  }

  float minInc = std::numeric_limits<float>::max();
  Node *next = nullptr;

  for (RTree::Node *c : n.children)
  {
    float inc = getRequiredExpansion(c->coords, c->dimensions, e);

    if (inc < minInc)
    {
      minInc = inc;
      next = c;
    }
    else if (inc == minInc)
    {
      float curArea = 1.0f;
      float thisArea = 1.0f;

      for (int i = 0; i < c->dimensions.size(); i++)
      {
        curArea *= next->dimensions.at(static_cast<unsigned long>(i));
        thisArea *= c->dimensions.at(static_cast<unsigned long>(i));
      }

      if (thisArea < curArea)
      {
        next = c;
      }
    }
  }
  return chooseLeaf(*next, e);
}


float RTree::getArea(std::vector<float> dimensions)
{
  float area = 1.0f;

  for (unsigned long i = 0; i < dimensions.size(); i++)
  {
    area *= dimensions.at(i);
  }

  return area;
}




void RTree::tighten(std::vector<RTree::Node *> nodes)
{
  assert(((nodes.size() >= 1), L"Pass some nodes to tighten"));

  for(RTree::Node *n: nodes)
  {
    std::cout << n->children.size() << std::endl;
    assert(n->children.size() > 0);
    std::cout << "Passed" << std::endl;

    std::vector<float> minCoords{0, 0};

    std::vector<float> maxCoords{0, 0};

    for (unsigned long long int i = 0; i < numDims; i++)
    {
      minCoords.at(i) = std::numeric_limits<float>::max();
      maxCoords.at(i) = 0.000000;

      for (Node *c : n->children)
      {
        c->parent = n;
        if (c->coords[i] < minCoords.at(i))
        {
          minCoords.at(i) = c->coords[i];
        }

        if ((c->coords[i] + c->dimensions[i]) > maxCoords.at(i))
        {
          maxCoords.at(i) = (c->coords[i] + c->dimensions[i]);
        }
      }
    }

    for (int i = 0; i < numDims; i++)
    {
      maxCoords.at(i) -= minCoords.at(i);
    }

  }
}


void RTree::adjustTree(RTree::Node *n, RTree::Node *nn)
{
  if(n == root)
  {
    if(nn != nullptr)
    {
      root = Node::buildRoot(false);
      root->children.push_back(n);
      n->parent = root;
      root->children.push_back(nn);
      nn->parent = root;
    }

    std::vector<RTree::Node *> value{root};
    tighten(value);

    return;
  }

  std::vector<RTree::Node *> value{n};
  tighten(value);

  if(nn != nullptr)
  {
    std::vector<RTree::Node *> value{nn};
    tighten(value);

    if(n->parent->children.size() > minEntries)
    {
      std::vector<Node*> splits = splitNode(n->parent);
      adjustTree(splits[0], splits[1]);
    }
  }

  if(n->parent != nullptr)
  {
    adjustTree(n->parent, nullptr);
  }
}

std::vector<RTree::Node *> RTree::pickSeeds(std::vector<Node *> *nn)
{
  std::vector<RTree::Node*> bestPair(2);
  bool foundBestPair = false;
  float bestSep = 0.0f;

  for(int i = 0; i < numDims; i++)
  {
    float dimLb = FLT_MAX,
        dimMinUb = FLT_MAX;
    float dimUb = -1.0f * FLT_MAX,
        dimMaxLb = -1.0f * FLT_MAX;
    Node *nMinUb = nullptr;
    Node *nMaxLb = nullptr;

    for (Node *n : *nn)
    {
      if (n->coords[i] < dimLb)
      {
        dimLb = n->coords[i];
      }

      if (n->dimensions[i] + n->coords[i] > dimUb)
      {
        dimUb = n->dimensions[i] + n->coords[i];
      }

      if (n->coords[i] > dimMaxLb)
      {
        dimMaxLb = n->coords[i];
        nMaxLb = n;
      }

      if (n->dimensions[i] + n->coords[i] < dimMinUb)
      {
        dimMinUb = n->dimensions[i] + n->coords[i];
        nMinUb = n;
      }
    }

    float separate = (nMaxLb == nMinUb) ? -1.0F : static_cast<float>(std::abs((dimMinUb - dimMaxLb) / (dimUb - dimLb)));

    if (separate >= bestSep)
    {
      bestPair[0] = nMaxLb;
      bestPair[1] = nMinUb;
      bestSep = separate;
      foundBestPair = true;
    }
  }

  if (!foundBestPair)
  {
    bestPair.clear();
    bestPair.push_back(nn->front());
    nn->erase(nn->begin() + 1);
    bestPair.push_back(nn->front());
  }

  int firstIndex = 0;
  int secondIndex = 0;

  for(int i = 0; i < nn->size(); i++)
  {
    if(nn->at(i) == bestPair[0]){
      firstIndex = i + 1;
    }

    if(nn->at(i) == bestPair[1])
    {
      secondIndex = i;
    }

  }

  nn->erase(nn->begin() + firstIndex);
  nn->erase(nn->begin() + secondIndex);

  return bestPair;
}


RTree::Node *RTree::pickNext(std::vector<RTree::Node *> &cc)
{
  return cc[0];
}


std::vector<int> RTree::search(std::vector<float> coords, std::vector<float> dimensions)
{
  assert(coords.size() == numDims);
  assert(dimensions.size() == numDims);
  std::vector<int> *results{};
  search(&coords, &dimensions, root, results);

  return *results;
}


bool RTree::isOverlap(std::vector<float> coords2, std::vector<float> dimm2, std::vector<float> coords,
                      std::vector<float> dimensions)
{

  const float YAY_FACTOR =1.001f;

  for (int i = 0; i < coords2.size(); i++)
  {
    bool overlapInThisDim = false;

    if (coords2[i] == coords[i])
    {
      overlapInThisDim = true;
    }
    else if (coords2[i] < coords[i])
    {
      if (coords2[i] + (YAY_FACTOR * dimm2[i]) >= coords[i])
      {
        overlapInThisDim = true;
      }
    }
    else if (coords2[i] > coords[i])
    {
      if (coords[i] + (YAY_FACTOR * dimensions[i]) >= coords2[i])
      {
        overlapInThisDim = true;
      }
    }
    if (!overlapInThisDim)
    {
      return false;
    }
  }

  return true;
}



RTree::Node* RTree::findLeaf(RTree::Node *n, std::vector<float> *coords, std::vector<float> *dimensions, int entry)
{
  if (n->leaf)
  {
    for (Node *c : n->children)
    {
      if (((Entry *) c)->entry == entry)
      {
        return n;
      }
    }

    return nullptr;
  }
  else
  {

    for (Node *c : n->children)
    {
      if (isOverlap(c->coords, c->dimensions, *coords, *dimensions))
      {
        Node *result = findLeaf(c, coords, dimensions, entry);
        if (result != nullptr)
        {
          return result;
        }
      }
    }
    return nullptr;
  }
}


void RTree::condenseTree(RTree::Node *n)
{
  std::vector<RTree::Node> q = {};

  while (n != root){
    if (n->leaf && (n->children.size() < minEntries))
    {
      for(auto elem : n->children)
      {
        q.push_back(*elem);
      }

      int index = 0;
      for(int i = 0; i < n->parent->children.size(); i++)
      {
        if(n->parent->children.at(i) == n)
        {
          index = i + 1;
        }
      }

      n->parent->children.erase(n->parent->children.begin() + index);
    }
    else if (!n->leaf && (n->children.size() < minEntries))
    {
      std::list<Node> leafToVis = {};

      while (leafToVis.size() > 0)
      {
        Node c = leafToVis.back();
        if (c.leaf){
          for(auto elem : c.children)
          {
            q.push_back(*elem);
          }
        }
        else
        {
          for(auto elem : c.children)
          {
            leafToVis.push_back(*elem);
          }
        }
      }

      int index = 0;
      for(int i = 0; i < n->parent->children.size(); i++)
      {
        if(n->parent->children.at(i) == n)
        {
          index = i + 1;
        }
      }

      n->parent->children.erase(n->parent->children.begin() + index);
    }
    else
    {
      std::vector<Node *> argument;
      argument.push_back(n);
      tighten(argument);
    }

    n = n->parent;
  }

  if (root->children.size() == 0)
  {
    root = Node::buildRoot(true);
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

  for (Node ne : q)
  {
    Entry *e = (Entry*) &ne;
    insert(e->coords, e->dimensions, e->entry);
  }
  size -= q.size();
}



std::vector<RTree::Node *> RTree::splitNode(RTree::Node *n)
{
  std::vector<Node *> nn = std::vector<RTree::Node *>{n, new Node(n->coords, n->dimensions, n->leaf)};
  nn.at(1)->parent = n->parent;

  if (nn.at(1) != nullptr)
  {
    nn.at(1)->parent->children.push_back(nn.at(1));
  }

  std::vector<Node *> cc = n->children;
  n->children.clear();

  std::vector<Node *> ss = pickSeeds(&cc);

  nn.at(0)->children.push_back(ss.at(0));
  nn.at(1)->children.push_back(ss.at(1));

  tighten(nn);

  while (cc.size() != 0)
  {
    if ((nn[0]->children.size() >= static_cast<unsigned long>(minEntries))
        && (nn.at(1)->children.size() + cc.size() == static_cast<unsigned long>(minEntries)))
    {
      std::fill(cc.begin(), cc.end(), nn[1]);
      cc.clear();
      tighten(nn);

      return nn;
    }
    else if ((nn[1]->children.size() >= static_cast<unsigned long>(minEntries))
             && (nn.at(0)->children.size() + cc.size() == static_cast<unsigned long>(minEntries)))
    {
      std::fill(cc.begin(), cc.end(), nn[0]);
      cc.clear();
      tighten(nn);

      return nn;
    }

    RTree::Node *c = pickNext(cc);
    RTree::Node *preferred = nullptr;

    float e0 = getRequiredExpansion(nn.at(0)->coords, nn.at(0)->dimensions, *c);
    float e1 = getRequiredExpansion(nn.at(1)->coords, nn.at(1)->dimensions, *c);

    if (e0 < e1)
    {
      preferred = nn.at(0);

    } else if (e0 > e1)
    {
      preferred = nn.at(1);

    }
    else
    {
      float a0 = getArea(nn.at(0)->dimensions);
      float a1 = getArea(nn.at(1)->dimensions);

      if (a0 < a1)
      {
        preferred = nn.at(0);

      } else if (e0 > a1)
      {
        preferred = nn.at(1);
      }
      else
      {
        if (nn.at(0)->children.size() < nn.at(1)->children.size())
        {
          preferred = nn.at(0);

        } else if (nn.at(0)->children.size() > nn.at(1)->children.size())
        {
          preferred = nn.at(1);

        }
        else
        {
          preferred = nn.at(static_cast<unsigned long>(rand()));
        }
      }
    }
    preferred->children.push_back(c);
    tighten(std::vector<RTree::Node *>{preferred});

  }
  return nn;
}
