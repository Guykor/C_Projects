
#include "RBTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**
 * enum classifying the problems and violations can be made when inserting a new node to an RBTree.
 */
typedef enum
{
    ROOT_DECLARATION,
    NO_VIOLATION,
    R_PARENT_R_UNCLE,
    R_PARENT_B_UNCLE
} RBTreeCorruption;


RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree *newTree = (RBTree *) malloc(sizeof(RBTree));
    if (newTree == NULL)
    {
        return NULL;
    }
    newTree->root = NULL;
    newTree->compFunc = compFunc;
    newTree->freeFunc = freeFunc;
    newTree->size = 0;
    return newTree;
}

/**
 * constructor to a new Node in the heap, initialized with  color RED and assigned with data
 * pointer to data that the user allocated in the heap.
 * @param data - pointer to unknown type of data allocated in the heap.
 * @return pointer to a new node in heap, NULL if fails.
 */
Node *createNewNode(void *data)
{
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode != NULL)
    {
        newNode->color = RED;
        newNode->data = data;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->parent = NULL;
        return newNode;
    }
    return NULL;
}

/**
 * This function only adds the node to the binary tree by value,
 * this function will be called only if the node data isn't already in tree.
 * @param tree - existing tree
 * @param node - node to insert
 * @return 1 on success, 0 if the data already appears.
 */
int addNodeByValue(RBTree *tree, Node *node)
{
    assert(tree != NULL && node != NULL);
    if (tree->root == NULL)
    {
        tree->root = node;
        return 1;
    }
    Node *p = tree->root;
    while (p != NULL)
    {

        int comp = tree->compFunc(p->data, node->data);
        if (comp < 0) // p->data < node->data
        {
            if (p->right == NULL)
            {
                p->right = node;
                node->parent = p;
                break;
            }
            else
            {
                p = p->right;
            }
        }
        else  // p->data > node->data
        {
            if (p->left == NULL)
            {
                p->left = node;
                node->parent = p;
                break;
            }
            else
            {
                p = p->left;
            }
        }
    }
    return 1;
}

/**
 * gets a node with a parent, finds and returns the node uncle in the tree.
 * @return uncle node pointer.
 */
Node *findUncle(const Node *node)
{
    if (node->parent != NULL && node->parent->parent != NULL)
    {
        Node *grandFather = node->parent->parent;
        if (grandFather->right == node->parent)
        {
            return grandFather->left;
        }
        else
        {
            return grandFather->right;
        }
    }
    return NULL;
}

/**
 * finds and returns the tree corruption status after inserting a new node to the tree.
 * treats NULL Uncle as Black!!
 * @param node - the new node inserted to the tree.
 * @return  RBTreeCorruption type of corruption for the RBTree.
 */
RBTreeCorruption findCorruption(const Node *node, const Node *uncle)
{
    if (node->parent == NULL)
    {
        return ROOT_DECLARATION;
    }
    else if (node->parent->color == BLACK)
    {
        return NO_VIOLATION;
    }
    else //node->parent->color == RED
    {
        if (uncle != NULL && uncle->color == RED)
        {
            return R_PARENT_R_UNCLE;
        }
        else //uncle == NULL || uncle->color == BLACK
        {
            return R_PARENT_B_UNCLE;
        }
    }
}

void rotateLL(Node *node)
{
    Node *tmp = node->right;
    node->right = node->parent;
    node->parent = node->parent->parent;
    if (node->parent != NULL)
    {
        if (node->right == node->parent->right) // if the G was a left right chile
        {
            node->parent->right = node;
        }
        else
        {
            node->parent->left = node;
        }
    }
    node->right->parent = node;
    node->right->left = tmp; //node right "child".
    if (tmp != NULL)
    {
        tmp->parent = node->right;
    }
}

void rotateRR(Node *node)
{
    Node *tmp = node->left;
    node->left = node->parent;
    node->parent = node->parent->parent;
    if (node->parent != NULL)
    {
        if (node->left == node->parent->right) // if the G was a left right chile
        {
            node->parent->right = node;
        }
        else
        {
            node->parent->left = node;
        }

    }

    node->left->parent = node;
    node->left->right = tmp; //node right "child".
    if (tmp != NULL)
    {
        tmp->parent = node->left;
    }
}

void rotateLR(Node *node)
{
    Node *tmp = node->right;
    node->right = node->parent;
    node->parent = node->parent->parent;
    if (node->parent != NULL) // if now the node is tree root
    {
        node->parent->right = node;
    }
    node->right->parent = node;
    node->right->left = tmp;
    if (tmp != NULL)
    {
        tmp->parent = node->right;
    }
    rotateRR(node);
}

/**
 * looking from bottom to top.
 * @param node
 */
void rotateRL(Node *node)
{
    Node *tmp = node->left;
    node->left = node->parent;
    node->parent = node->parent->parent;
    if (node->parent != NULL) // if now the node is tree root
    {
        node->parent->left = node;
    }
    node->left->parent = node;
    node->left->right = tmp;
    if (tmp != NULL)
    {
        tmp->parent = node->left;
    }
    rotateLL(node);
}


/**
 * preforms rotation on a subtree that the node grandFather is it's root.
 * @param node violating node in tree.
 * @return Top Node after rotation in the subtree that the node grandfather was it's root.
 */
Node *rotate(Node *node)
{
    // if node is right child of a left child
    if (node == node->parent->right && node->parent == node->parent->parent->left)
    {
        rotateRL(node);
        node->color = BLACK;
        node->right->color = RED;
        return node;
    }
        // node is left child of a right child
    else if (node == node->parent->left && node->parent == node->parent->parent->right)
    {
        rotateLR(node);
        node->color = BLACK;
        node->left->color = RED;
        return node;
    }
    else
    {
        // node is left child of a left child.
        if (node == node->parent->left && node->parent == node->parent->parent->left)
        {
            rotateLL(node->parent);
            node->parent->right->color = RED;
        }
        else // node is right child of a right child.
        {
            rotateRR(node->parent);
            node->parent->left->color = RED;
        }
        node->parent->color = BLACK;
        return node->parent;
    }
}

/**
 * balance and fix tree violations after inserting a new node.
 * this function choose different operations based on the type of corruption
 * the insertion caused.
 * @param tree - a valid RB tree.
 * @param node - a valid Node, pointer to the node inserted.
 */
void balanceTree(RBTree *tree, Node *node)
{
    Node *nodeUncle = findUncle(node); // can be null if no uncle exists.

    switch (findCorruption(node, nodeUncle))
    {
        case NO_VIOLATION:
            break;
        case ROOT_DECLARATION:
            node->color = BLACK;
            break;
        case R_PARENT_R_UNCLE:
            node->parent->color = BLACK;
            nodeUncle->color = BLACK;
            node->parent->parent->color = RED;
            balanceTree(tree, node->parent->parent);
            break;
        case R_PARENT_B_UNCLE:
        {
            Node *subTreeRoot = rotate(node);
            tree->root = (subTreeRoot->parent == NULL) ? subTreeRoot : tree->root;
            break;
        }
    }
}

int addToRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || containsRBTree(tree, data) == 1)
    {
        return 0;
    }

    Node *newNode = createNewNode(data);
    if (newNode == NULL)
    {
        return 0;
    }
    int status = addNodeByValue(tree, newNode);
    if (status == 0) // if item is already in tree.
    {
        return 0;
    }
    balanceTree(tree, newNode);
    ++tree->size;
    return 1;
}

int containsRBTree(RBTree *tree, void *data)
{
    Node *p = tree->root;
    while (p != NULL)
    {
        int cmp = tree->compFunc(p->data, data);
        if (cmp == 0)
        {
            return 1;
        }
        else if (cmp > 0)
        {
            p = p->left;
        }
        else
        {
            p = p->right;
        }
    }
    return 0;
}

/**
 * gets a node that represent a subtree root and returns the minimal node in tree.
 * @param root - pointer to a Node
 * @return pointer to the minimal Node in subTree.
 */
Node *getSubTreeMinNode(Node *root)
{
    Node *p = root;
    while (p != NULL && p->left != NULL)
    {
        p = p->left;
    }
    return p;
}

/**
 * gets a node and returns a pointer to it's successor in the tree.
 * @param node - pointer to a node.
 * @return pointer to the node Successor.
 */
Node *getSuccessor(const Node *node)
{
    if (node->right != NULL)
    {
        return getSubTreeMinNode(node->right);
    }
    Node *parent = node->parent;
    while (parent != NULL && node == parent->right)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

int forEachRBTree(RBTree *tree, forEachFunc func, void *args)
{
    Node *p = getSubTreeMinNode(tree->root);
    int i = 0;
    while (p != NULL && i <= tree->size)
    {
        if (func(p->data, args) == 0)
        {
            return 0;
        }
        p = getSuccessor(p);
        ++i;
    }
    return 1;
}

/**
 * recursively go over each node in the tree from the root and free data member with a
 * relevant free function, and than free the node itself.
 * @param node - root of a subtree.
 * @param freeData function of type FreeFunc.
 */
void freeNodes(Node *node, FreeFunc freeData)
{
    if (node == NULL)
    {
        return;
    }
    freeNodes(node->right, freeData);
    freeNodes(node->left, freeData);
    freeData(node->data);
    free(node);
    node = NULL;
}

void freeRBTree(RBTree *tree)
{
    if (tree != NULL && tree->root != NULL)
    {
        freeNodes(tree->root, tree->freeFunc);
        free(tree);
        tree = NULL;
    }
}