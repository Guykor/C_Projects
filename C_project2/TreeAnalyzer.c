#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "queue.h"

// Constants //
#define TXT_FILE_LINE_LEN_LIMIT 1025 //(1024 + null val)
#define ARGC_ERROR_MSG "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define INVALID_INPUT_MSG "Invalid input\n"
#define EXPECTED_ARGS_NUM 3
#define TXT_FILE_MIN_LINES 2 // for n nodes specification and the first node (cause n=0 is illegal).
#define NODE_KEY(LINE_NUM) (LINE_NUM - 2)
#define LIN_LEAF_SYMBOL_EOL "-\n"
#define WIN_LEAF_SYMBOL_EOL "-\r\n"
#define NODE_CHILDREN_TXT_DELIMITER " \r\n"
#define VALID_N_EDGES_IN_TREE(N) (N - 1)
#define MEMORY_ERROR_MSG "Memory problem occurred, exiting..."
#define FIRST_NODE_ROW_IN_FILE 2
#define UNINITIALIZED_TREE_VAL -1
#define LEAF_SYMBOL '-'
#define VALID_LINE_NUM_IN_FILE(N) (N + 1)

// Declarations //

/**
 * represents Tree Node data structure.
 */
typedef struct Node
{
    int *children;
    int parentNodeKey;
    int key;
    int isLeaf;
    int nChildren;
    //BFS utils
    int previousKey;
    int dist;
} Node;

/**
 * represent a Tree data structure.
 */
typedef struct Tree
{
    Node *root;
    Node *nodes; // array of Nodes in the heap.
    int nVertices;
    int nEdges;
} Tree;

/**
 * enum to determine which error occurred during run.
 */
typedef enum Error
{
    FAIL,
    SUCCESS,
    MEM_ERR
} ErrStatus;

/**
 * manages pre processing the tree data and analyze the tree created.
 * @param argv  - arguments from user
 * @return - ErrStatus - success of failure, and informative msg.
 */
int analyze(char *const *argv);

/**
  * Parses the given tree in a txt file to a data structure allowing manipulations.
  * concretely, parse each node by his key and his set of children, and places an array of nodes
  * in the tree structure.
  * @param filePath - valid path to txt file specifying the tree.
  * @param firstNode - starting node finding shortest route to the second one.
  * @param secondNode - ending node for the shortest route from the first one.
  * @param tree - pointer to a tree, will hold every vertex in tree, and will be freed outside
  *                this scope.
  * @param firstNodeKey - key to a node in the tree (!) who is key was given by user (firstNode)
  * @param secondNodeKey - key to a node in the tree (!) who is key was given by user (secondNode)
  * @return ErrStatus SUCCESS, otherwise FAIL while printing a msg to stderr. raises MEM_ERR if
  * memory allocation failed.
  */
ErrStatus buildTree(const char *filePath, char *firstNode, char *secondNode, Tree **outTree,
                    int *firstNodeKey, int *secondNodeKey);

/**
 * sets multiple variables used building the tree:
 * this function loads the file into the given pointer, parse and read the first row in file
 * specifying the number of nodes into a variable, and checks the two supplied nodes key.

 * @param filePath - valid path for the graph txt file.
 * @param fp FILE pointer, will be used to parse the whole file outside this scope.
 * @param firstNode - first node string argument given.
 * @param secondNode - second node string argument given.
 * @param inputNumVertices - number of vertices to be in the tree specified in the txt file.
 * @param uKey - parsed first node value, will be updated if function will succeed.
 * @param vKey - parsed second node value.
 * @return the given pointer will be updated and SUCCESS will return if no problems found or
 * occurred with one of the arguments. otherwise FAILURE. MEM_ERR if malloc have failed.
 */
ErrStatus initTools(const char *filePath, FILE **fp, char *firstNode, char *secondNode,
                    int *inputNumVertices, int *uKey, int *vKey);

/**
 * Helper function to load txt file given by the user. this function will update a FILE pointer
 * with the pointer to the file, and will register in another pointer the size of the file
 * (numOfLines).
 * the function also checks if the number of lines is below the minimal required for valid input.
 * @param filePath - path provided by user.
 * @param fp - FILE pointer, will be updated with the opened file.
 * @param fSize - int pointer, will be updated with the correct length of file.
 * @return ErrStatus SUCCESS or FAILURE if there was a problem reading the file or with
 * it's length. MEM_ERR if a malloc call have failed.
 */
ErrStatus loadFile(const char *filePath, FILE **fp, int *fSize);

/**
 * checks and parses the  number of vertices in the tree, given in the first row of the txt file.
 * n should be a int number s.t n != 0, this function check if n == numOfRows in file, but this
 * doesn't cover the case when the number of line is ok but those lines are empty.
 * @param fp - FILE pointer to the txt file.
 * @param fileLength numOfLines in file
 * @param numVertices - pointer to the output value.
 * @return ErrStatus SUCCESS or FAIL.
 */
ErrStatus setVerticesNumber(FILE *fp, int fileLength, int *numVertices);

/**
 * Helper function to parse strings to integers.
 * @param num - string to convert.
 * @param out - int pointer, will be updated with the parsed value.
 * @return ErrStatus SUCCESS or FAILURE.
 */
ErrStatus parseNum(char *num, int *out);

/**
 * basic test for input nodes validity as a fail-fast mechanism.
 * tries to parse the nodes keys to numbers, and check if they within range of valid nodes [0,|V|]
 * @param first - key for the first node - string from user
 * @param second - key for second node.
 * @param firstNodeKey - pointer to int, will updated if the input is valid.
 * @param secondNodeKey - pointer to int, will be updated.
 * @param verticesNum - number of vertices given by the user.
 * @return if no problem, will update the valid keys of the nodes as integers and return
 * SUCCESS, otherwise FAILURE.
 */
ErrStatus checkInputNodes(char first[], char second[], int *firstNodeKey, int *secondNodeKey,
                          int verticesNum);

/**
 * adds vertices to the tree given, parsing them from the tree specification provided in the txt
 * file, while checking for problems with input.
 * the function will update existing nodes array in the tree, and fill their data.
 *
 * Memory Note: for every node, an array of ints is allocated to register his children, if
 * something will went wrong with the problem, the method freeTree will handle all memory
 * allocated, including this one.
 *
 * @param fp - FILE pointer to the txt file.
 * @param inputNumVertices - number of vertices supplied in the first row in the file.
 * @param tree - the tree to build, tree->nodes assumed to be initialized before call.
 * @return EXIT_SUCCESS, otherwise EXIT_FAILURE.
 */
ErrStatus addTreeVertices(FILE *fp, int inputNumVertices, Tree *tree);

/**
* links vertex sons specified in the line in a list of numbers separated with spaces.
* valid input is string of numbers of type int s.t 0 =< x =< |V| else iff whole line is "-"
* means V is a leaf.
* the function receives all nodes in tree array, and update for each children it's parent (based
* on assumption that the key is the location in the tree array).
* input assumptions: every value in the numbers list is separated by one space;
* @param line - line in txt file, containing data about children of node.
* @param parent - pointer to the parent node.
* @param tree - pointer to the tree.
* @param childCounter will be updated with number of children parsed for the parent.
* @param numVertices - number of given vertices by input.
* @return - SUCCESS, FAIL or MEM_ERR if a malloc call have failed.
 */
ErrStatus parseChildren(char *line, Node *parent, Tree *tree, int *childCounter, int numVertices);

/**
 * this function iterates over the built nodes and choose the node with no parent to be the tree
 * root. if more than one root was picked or none, it means that the tree contains a circle, and
 * the function will return FAIL.
 * @param tree - pointer to a tree with all the nodes updated.
 * @return FAIL if tree have circle, SUCCESS otherwise.
 */
ErrStatus setRoot(Tree *tree);

/**
 * this function prints the following data about a given tree, using helper functions:
 * 1. value of root vertex (finding root should take O(n)).
 * 2. number of vertices and edges in tree (each one in separate line) (should take O(1)).
 * 3. minimal and maximal height in the tree in separated lines.
 * 4. diameter of the tree - length of the longest simple route.
 * 5. the shortest route between the two given vertices, separated with spaces (should take O(n^2)).
 * @param tree - tree to investigate.
 * @param pathStartNodeKey - key of a node in the tree.
 * @param pathEndNodeKey  - key of a node in the tree.
 */
void printTreeDetails(const Tree *tree, int pathStartNodeKey, int pathEndNodeKey);

/**
 * free heap memory allocated for the tree - array of children for each node,
 * and the array of nodes in the tree. and the nodes themselves.
 * @param tree - pointer to the tree.
 */
void freeTree(Tree *tree);

/**
 * BFS implementation for the tree.
 * assumptions - 1. the tree nodes can be accessed in the tree's nodes array by their key.
 *               2. the dist and prev fields updated during run are ints and not pointers, so
 *               infinity is represented by the invalid value -1.
 * Running time - O(|V|+|E|) = O(2n-1) = O(n).
 * @param tree pointer to the graph (tree in our case).
 * @param start - starting node (of type Node).
 */
void BFS(const Tree *tree, int startNodeKey);

/**
 * Finds the minimal and maximal heights exists in the tree, e.g the longest and shortest routes
 * from the tree root to a leaf.
 * the function calls BFS to set the shortest routes from the root of the tree, and than iterate
 * over leaves and update the longest and shortest routs.
 * @param tree pointer to a Tree
 * @param maxHeight pointer to int result val
 * @param minHeight pointer to int result val
 * @param deepestLeafKey int Key of to the most far leaf from root in the tree.
 */
void calcMinimalAndMaximalHeight(const Tree *tree, int *maxHeight, int *minHeight, int
                                 *deepestLeafKey);

/**
 * find the delimiter of the tree, e.g the longest simple route in the tree.
 * we can assume that the diameter route will start and end in a leaf, otherwise we could go deeper
 * and achieve more distance. Thus, using the most far leaf from the root, we can find the
 * diameter finding the longest route from it to another leaf.
 * running time - max num of leaves is
 * @param tree - pointer to a tree.
 * @param deepestLeaf - address to a leaf in the tree that is in the end of the longest route
 * from root.
 * @param diameter - the int result value.
 */
void calcTreeDiameter(const Tree *tree, int deepestLeafKey, int *diameter);

/**
 * finds shortest route between two given nodes and output the nodes key in the route.
 * the path is u->v, but the function will start from the end and reverse order to result in
 * correct output.
 * will result in an array allocated in the heap! need to free after use.
 * @param tree a pointer to the tree
 * @param startNodeKey  key of a valid node in the tree to start path from
 * @param endNode - key of a valid node in the tree to end the path.
 * @param nodesInRoute - pointer to an array of the nodes keys in path - the result.
 */
void findDistance(const Tree *tree, int startNodeKey, int endNodeKey,
                  int **nodesInRoute, int *pathLen);

/**
 * used to print the nodes key in the route between two nodes.
 * @param nodesInRoute
 * @param pathLength
 * @param pathStartNodeKey
 * @param pathEndNodeKey
 */
void printRoute(int *nodesInRoute, int pathLength, int pathStartNodeKey, int pathEndNodeKey);

/**
 * argv[1] - graphFilePath, assuming there's no need in conversion to absolute and
 * the file exists.
 * arg[2] = firstVertex
 * argv[3] = second Vertex
 * @return EXIT_SUCCESS EXIT_FAILURE.
 */
int main(int argc, char **argv);

// Implementation //

ErrStatus loadFile(const char *filePath, FILE **fp, int *fSize)
{
    // we assume the file exists
    *fp = fopen(filePath, "r");
    if (*fp != NULL)
    {
        int lineCounter = 0;
        char line[TXT_FILE_LINE_LEN_LIMIT] = {'\0'};
        while (fgets(line, sizeof(line), *fp))
        {
            if (strcmp(line, "\n") != 0) //if line not empty
            {
                lineCounter++;
                memset(line, '\0', sizeof(line));
            }
        }

        *fSize = lineCounter;
        if (*fSize >= TXT_FILE_MIN_LINES)
        {
            fseek(*fp, 0, SEEK_SET);
            return SUCCESS;
        }
    }
    return FAIL;
}

ErrStatus parseNum(char *num, int *out)
{
    char *end = NULL;
    *out = (int) strtol(num, &end, 10);
    if (*end != '\0' && *end != '\n')
    {
        return FAIL;
    }
    else
    {
        return SUCCESS;
    }
}

ErrStatus setVerticesNumber(FILE *fp, const int fileLength, int *numVertices)
{
    char line[TXT_FILE_LINE_LEN_LIMIT];
    fgets(line, sizeof(line), fp);
    if (parseNum(line, numVertices) == SUCCESS)
    {
        if (*numVertices > 0 && (fileLength == VALID_LINE_NUM_IN_FILE(*numVertices)))
        {
            // check if file length sufficient the lower bound for given n (cause we ignore empty
            // lines in the end of the file.)
            return SUCCESS;
        }
    }
    return FAIL;
}


ErrStatus checkInputNodes(char first[], char second[], int *firstNodeKey, int *secondNodeKey,
                          const int verticesNum)
{
    int highestKey = verticesNum - 1; //starting from zero
    int lowestKey = 0;
    if (parseNum(first, firstNodeKey) == SUCCESS &&
        parseNum(second, secondNodeKey) == SUCCESS)
    {
        if ((lowestKey <= *firstNodeKey && *firstNodeKey <= highestKey) &&
            (lowestKey <= *secondNodeKey && *secondNodeKey <= highestKey))
        {
            return SUCCESS;
        }
    }
    return FAIL;
}

ErrStatus parseChildren(char *line, Node *const parent, Tree *tree,
                        int *childCounter, const int numVertices)
{
    *childCounter = 0; // reset counter (fool safe).
    if (line[0] == LEAF_SYMBOL || strcmp(line, LIN_LEAF_SYMBOL_EOL) == 0 ||
        strcmp(line, WIN_LEAF_SYMBOL_EOL) == 0)
    {
        return SUCCESS;
    }

    int maxKey = numVertices - 1; //starting from zero.
    int minKey = 0;
    int childKey;
    // spilt by spaces, or EOF (just to get rid of garbage in the last child key
    char *token = strtok(line, NODE_CHILDREN_TXT_DELIMITER);
    if (token == NULL) // line doesn't contain number of leaf_symbol.
    {
        return FAIL;
    }
    while (token != NULL)
    {
        if (parseNum(token, &childKey) == SUCCESS)
        {
            // for every child check if within range and if tree isn't recursive.
            if ((minKey <= childKey && childKey <= maxKey) && childKey != parent->key)
            {
                parent->children[*childCounter] = childKey; //add to the parent children arr.
                (*childCounter)++;
                // update the parent field for the children in tree nodes array
                tree->nodes[childKey].parentNodeKey = parent->key;

                token = strtok(NULL, NODE_CHILDREN_TXT_DELIMITER);
                continue;
            }
        }
        return FAIL;
    }
    // make sure childCount per Node in tree < numVertices specified in file.
    if (*childCounter > numVertices)
    {
        return FAIL;
    }

    return SUCCESS;
}

ErrStatus setRoot(Tree *const tree)
{
    int rootNum = 0;
    for (int i = 0; i < tree->nVertices; i++)
    {
        if (tree->nodes[i].parentNodeKey == UNINITIALIZED_TREE_VAL)
        {
            rootNum++;
            tree->root = &tree->nodes[i];
        }
    }
    if (rootNum == 0 || rootNum > 1)
    {
        return FAIL; //tree have circle! within it's n-1 edges
    }
    return SUCCESS;
}

ErrStatus addTreeVertices(FILE *fp, const int inputNumVertices, Tree *tree)
{
    fseek(fp, 0, SEEK_SET); // make sure to read file from the start.
    char line[TXT_FILE_LINE_LEN_LIMIT] = "\0";
    int lineCounter = 1;
    tree->nVertices = 0;
    tree->nEdges = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (lineCounter < FIRST_NODE_ROW_IN_FILE)
        { // first row (nVertices) handled in another method.
            lineCounter++;
            continue;
        }
        // build new Node in the tree.
        Node *v = &tree->nodes[tree->nVertices];
        v->key = NODE_KEY(lineCounter);
        tree->nVertices += 1;

        lineCounter++;
        int childCount = 0;

        v->children = (int *) malloc(inputNumVertices * sizeof(int));

        if (v->children == NULL)
        {
            return MEM_ERR;
        }
        // update the variables defined before.
        if (parseChildren(line, v, tree, &childCount, inputNumVertices) ==
            SUCCESS)
        {
            memset(line, '\0', sizeof(line)); //reset line values.

            if (childCount == 0)
            {
                v->isLeaf = 1;
                v->nChildren = 0;
                free(v->children);
                v->children = NULL;
            }
            else
            {
                v->children = (int *) realloc(v->children, childCount * sizeof(int));
                if (v->children == NULL)
                {
                    return MEM_ERR;
                }
                v->nChildren = childCount;
                v->isLeaf = 0;
                tree->nEdges += childCount;
            }
        }
        else //if parsing node children returned failure
        {
            return FAIL;
        }
    }
    return SUCCESS;
}


ErrStatus initTools(const char *filePath, FILE **fp, char *firstNode, char *secondNode,
                    int *inputNumVertices, int *uKey, int *vKey)
{
    int fSize = 0;
    if (loadFile(filePath, fp, &fSize) == SUCCESS &&
        setVerticesNumber(*fp, fSize, inputNumVertices) == SUCCESS &&
        checkInputNodes(firstNode, secondNode, uKey, vKey, *inputNumVertices) == SUCCESS)
    {
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}


ErrStatus buildTree(const char *filePath, char *firstNode, char *secondNode, Tree **outTree,
                    int *firstNodeKey, int *secondNodeKey)
{
    FILE *fp = NULL;
    int inputNumVertices;
    int uKey, vKey;
    ErrStatus utilsStatus = initTools(filePath, &fp, firstNode, secondNode, &inputNumVertices,
                                      &uKey, &vKey);
    if (utilsStatus == SUCCESS)
    {
        Tree *tree = (Tree *) malloc(sizeof(Tree));
        *outTree = tree; // points the outside scope tree to this one (the out pointer will free
        // the memory outside this func).

        if (tree == NULL)
        {
            fclose(fp);
            return MEM_ERR;
        }
        tree->root = NULL;
        tree->nodes = (Node *) malloc(inputNumVertices * sizeof(Node));
        memset(tree->nodes, UNINITIALIZED_TREE_VAL, inputNumVertices * sizeof(Node));

        if (tree->nodes == NULL)
        {
            fclose(fp);
            return MEM_ERR;
        }

        ErrStatus verticesAdded = addTreeVertices(fp, inputNumVertices, tree);
        if (verticesAdded == SUCCESS)
        {
            if (tree->nVertices == inputNumVertices &&
                tree->nEdges == VALID_N_EDGES_IN_TREE(tree->nVertices) &&
                setRoot(tree) == SUCCESS)
            {
                *firstNodeKey = uKey;
                *secondNodeKey = vKey;
                fclose(fp);
                return SUCCESS;
            }
        }
        if (verticesAdded == MEM_ERR)
        {
            fclose(fp);
            return MEM_ERR;
        }
    }
    if (fp != NULL)
    {
        fclose(fp);
    }
    return FAIL;
}

void freeTree(Tree *tree)
{
    if (tree != NULL)
    {

        for (int i = 0; i < tree->nVertices; i++)
        {
            if (tree->nodes != NULL && tree->nodes[i].children != NULL)
            {
                free(tree->nodes[i].children);
            }
        }
        if (tree->nodes != NULL)
        {
            free(tree->nodes);
        }
        free(tree);
    }
}

void BFS(const Tree *tree, const int startNodeKey)
{
    for (int i = 0; i < tree->nVertices; i++)
    {
        tree->nodes[i].dist = UNINITIALIZED_TREE_VAL;
        tree->nodes[i].previousKey = UNINITIALIZED_TREE_VAL;
    }
    Node *start = &tree->nodes[startNodeKey];
    start->dist = 0;
    Queue *Q = allocQueue();
    enqueue(Q, start->key);
    while (queueIsEmpty(Q) == 0) //not empty
    {
        int u = (int) dequeue(Q);
        for (int j = 0; j < tree->nodes[u].nChildren; j++)
        {
            int childKey = tree->nodes[u].children[j];
            Node *child = &tree->nodes[childKey];
            if (child->dist == UNINITIALIZED_TREE_VAL)
            {
                enqueue(Q, childKey);
                child->previousKey = u;
                child->dist = tree->nodes[u].dist + 1;
            }
        }
        int pKey = tree->nodes[u].parentNodeKey;
        if (pKey != UNINITIALIZED_TREE_VAL) // if have parent
        {
            Node *parent = &tree->nodes[pKey];
            if (parent != NULL && parent->dist == UNINITIALIZED_TREE_VAL)
            {
                enqueue(Q, parent->key);
                parent->previousKey = u;
                parent->dist = tree->nodes[u].dist + 1;
            }
        }
    }
    freeQueue(&Q);
}


void calcMinimalAndMaximalHeight(const Tree *tree, int *maxHeight, int *minHeight, int
                                 *deepestLeafKey)
{
    //reset
    *maxHeight = 0;
    *minHeight = VALID_N_EDGES_IN_TREE(tree->nVertices); //longest possible route length.

    BFS(tree, tree->root->key);
    for (int i = 0; i < tree->nVertices; i++)
    {
        Node *v = &tree->nodes[i];
        if (v->isLeaf == 1)
        {
            if (v->dist > *maxHeight)
            {
                *maxHeight = v->dist;
                *deepestLeafKey = v->key;
            }
            if (v->dist < *minHeight)
            {
                *minHeight = v->dist;
            }
        }
    }
}


void calcTreeDiameter(const Tree *tree, const int deepestLeafKey, int *const diameter)
{
    if (tree->nVertices == 1)
    {
        *diameter = 0;
    }
    else if (tree->nVertices == 2)
    {
        *diameter = 1;
    }
    else
    {
        *diameter = 0; //reset
        BFS(tree, deepestLeafKey);

        // flag the root as a leaf (in a graph perspective)
        tree->root->isLeaf = 1;

        for (int i = 0; i < tree->nVertices; i++)
        {
            Node *v = &tree->nodes[i];
            if (v->isLeaf == 1)
            {
                if (v->dist > *diameter)
                {
                    *diameter = v->dist;
                }
            }
        }
    }
}


void findDistance(const Tree *tree, const int startNodeKey, const int endNodeKey,
                  int **nodesInRoute, int *pathLen)
{
    BFS(tree, endNodeKey);
    int pathLength = tree->nodes[startNodeKey].dist;
    *pathLen = ++pathLength; // add the last node in path.

    if (pathLength != 0)
    {
        *nodesInRoute = (int *) malloc(sizeof(int) * pathLength);
        memset(*nodesInRoute, 0, sizeof(int) * pathLength);
        int nextKeyInPath = startNodeKey;
        for (int i = 0; i < pathLength; i++)
        {
            (*nodesInRoute)[i] = nextKeyInPath;
            nextKeyInPath = tree->nodes[nextKeyInPath].previousKey;
        }
    }

}

void printRoute(int *const nodesInRoute, const int pathLength, const int pathStartNodeKey, const
                int pathEndNodeKey)
{
    if (pathLength != 0)// if path  is not empty.
    {
        printf("Shortest Path Between %d and %d: ", pathStartNodeKey, pathEndNodeKey);
        for (int i = 0; i < pathLength - 1; i++)
        {
            printf("%d ", nodesInRoute[i]);
        }
        printf("%d\n", nodesInRoute[pathLength - 1]);
    }
    if (nodesInRoute != NULL)
    {
        free(nodesInRoute);
    }
}

void printTreeDetails(const Tree *tree, const int pathStartNodeKey, const int pathEndNodeKey)
{
    int maxHeight, minHeight;
    int deepestLeafKey;
    int diameter;
    calcMinimalAndMaximalHeight(tree, &maxHeight, &minHeight, &deepestLeafKey);
    calcTreeDiameter(tree, deepestLeafKey, &diameter);

    int *nodesInRoute = NULL;
    int pathLength = 0;

    findDistance(tree, pathStartNodeKey, pathEndNodeKey, &nodesInRoute, &pathLength);

    printf("Root Vertex: %d\n", tree->root->key);
    printf("Vertices Count: %d\n", tree->nVertices);
    printf("Edges Count: %d\n", tree->nEdges);
    printf("Length of Minimal Branch: %d\n", minHeight);
    printf("Length of Maximal Branch: %d\n", maxHeight);
    printf("Diameter Length: %d\n", diameter);
    printRoute(nodesInRoute, pathLength, pathStartNodeKey, pathEndNodeKey);
}


int analyze(char *const *argv)
{
    Tree *tree = NULL;

    // will hold keys of valid nodes in the tree by the keys provided by user.
    int pathStartNodeKey, pathEndNodeKey;
    ErrStatus errType = buildTree(argv[1], argv[2], argv[3], &tree, &pathStartNodeKey,
                                  &pathEndNodeKey);
    if (errType == SUCCESS)
    {
        printTreeDetails(tree, pathStartNodeKey, pathEndNodeKey);
        freeTree(tree);
        return EXIT_SUCCESS;
    }
    else if (errType == FAIL)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        freeTree(tree);
        return EXIT_FAILURE;
    }
    else
    {
        fprintf(stderr, MEMORY_ERROR_MSG);
        freeTree(tree);
        return EXIT_FAILURE;
    }
}

int main(int argc, char **argv)
{
    if (argc - 1 != EXPECTED_ARGS_NUM) //without the name of the program.
    {
        fprintf(stderr, ARGC_ERROR_MSG);
        return EXIT_FAILURE;
    }
    else
    {
        return analyze(argv);
    }
}

