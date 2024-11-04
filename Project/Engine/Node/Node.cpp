#include "Node.h"

#include <assimp/scene.h>

void Node::ReadNode(const aiNode* _node)
{
    aiMatrix4x4 aiLocalMatrix = _node->mTransformation;
    aiLocalMatrix.Transpose();//転置

    // メンバ変数にコピー
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            localMatrix_.m[i][j] = aiLocalMatrix[i][j];

    name_ = _node->mName.C_Str();
    children_.resize(_node->mNumChildren);
    for (uint32_t i = 0; i < _node->mNumChildren; ++i)
    {
        children_[i].ReadNode(_node->mChildren[i]);
    }
}

