#pragma once

#include "Matrix4x4.h"
#include <string>
#include <vector>

struct aiNode;
class Node
{
public:

    Node() = default;
    ~Node() = default;

    void ReadNode(const aiNode* _node);

    Matrix4x4 GetLocalMatrix() const { return localMatrix_; }
    std::string name_ = {};
private:
    Matrix4x4 localMatrix_;
    std::vector<Node> children_ = {};

};