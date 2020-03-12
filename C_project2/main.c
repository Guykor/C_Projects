#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "string.h"

#define TXT_FILE_LINE_LEN_LIMIT 1025 //(1024 + null val)
#define ARGC_ERROR_MSG "USAGE: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define INVALID_INPUT_MSG "Invalid input\n"
#define EXPECTED_ARGS_NUM 3
#define TXT_FILE_MIN_LINES 2 // for n nodes specification and the first node (cause n=0 is illegal).
#define NODE_KEY(LINE_NUM) (LINE_NUM - 2)
#define LEAF_SYMBOL "-"
#define NODE_CHILDREN_TXT_DELIMITER " \r\n"

#define MEMORY_ERROR_MSG "Memory problem occurred, exiting..."
#define FIRST_NODE_ROW_IN_FILE 2
// todo: conver size_t to ints.
//todo: go over the 6 rules.
/**
 * represents Tree Node data structure.
 */
typedef struct Node
{
    int *children;
    int key;
    int isLeaf;
    int nChildren;
} Node;

/**
 * represent a Tree data structure.
 */
typedef struct Tree
{
    Node root;
    Node *nodes; // array of Nodes in the heap.
    int nVertices;
    int nEdges;
    int minimalRouteLen;
    int maximalRouteLen;
    int treeDelimiter;
} Tree;

/**
 * Helper function to load txt file given by the user. this function will update a FILE pointer
 * with the pointer to the file, and will register in another pointer the size of the file
 * (numOfLines).
 * the function also checks if the number of lines is below the minimal required for valid input.
 * @param filePath - path provided by user.
 * @param fp - FILE pointer, will be updated with the opened file.
 * @param fSize - int pointer, will be updated with the correct length of file.
 * @return EXIT_SUCCESS or EXIT_FAILURE if there was a problem reading the file or with it's length.
 */
int loadFile(const char *filePath, FILE *fp, long fSize)
{
    // we assume the file exists
    fp = fopen(filePath, "r");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        fSize = ftell(fp);
        if (fSize >= TXT_FILE_MIN_LINES)
        {
            fseek(fp, 0, SEEK_SET);
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr, INVALID_INPUT_MSG);
    return EXIT_FAILURE;
}

/**
 * Helper function to parse strings to integers.
 * @param num - string to convert.
 * @param out - int pointer, will be updated with the parsed value.
 * @return EXIT_SUCCESS or EXIT_FAILRUE.
 */
int parseNum(char *num, int *out)
{
    char *end = NULL;
    *out = (int) strtol(num, &end, 10);
    if (*end != '\0')
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}

/**
 * checks and parses the  number of vertices in the tree, given in the first row of the txt file.
 * n should be a int number s.t n != 0, this function check if n == numOfRows in file, but this
 * doesn't cover the case when the number of line is ok but those lines are empty.
 * @param fp - FILE pointer to the txt file.
 * @param fileLength numOfLines in file
 * @param numVertices - pointer to the output value.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int setVerticesNumber(FILE *fp, const size_t fileLength, int *numVertices)
{
    char line[TXT_FILE_LINE_LEN_LIMIT];
    fgets(line, sizeof(line), fp);
    if (parseNum(line, numVertices) == EXIT_SUCCESS)
    {
        if (numVertices > 0 && (*numVertices == fileLength - 1))
        {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}


/**
 * basic test for input nodes validity as a fail-fast mechanism.
 * tries to parse the nodes keys to numbers, and check if they within range of valid nodes [0,|V|]
 * @param first - key for the first node - string from user
 * @param second - key for second node.
 * @param firstNodeKey - pointer to int, will updated if the input is valid.
 * @param secondNodeKey - pointer to int, will be updated.
 * @param verticesNum - number of vertices given by the user.
 * @return if no problem, will update the valid keys of the nodes as integers and return
 * EXIT_SUCCESS, otherwise EXIT_FAILURE.
 */
int checkInputNodes(char first[], char second[], int *firstNodeKey, int *secondNodeKey,
                    const int verticesNum)
{
    int highestKey = verticesNum;
    int lowestKey = 0;
    if (parseNum(first, firstNodeKey) == EXIT_SUCCESS &&
        parseNum(second, secondNodeKey) == EXIT_SUCCESS)
    {
        if ((lowestKey <= *firstNodeKey <= highestKey) &&
            (lowestKey <= *secondNodeKey <= highestKey))
        {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

/**
 * links vertex sons specified in the line in a list of numbers separated with spaces.
 * valid input is string of numbers of type int s.t 0 =< x =< |V| else iff whole line is "-"
 * means V is a leaf.
 * input assumptions: every value in the numbers list is separated by one space;
 * @param line
 * @param parentKey
 * @param childrenKeys
 * @param childCounter
 * @param numVertices
 * @return
 */
int parseChildren(char *line, const int parentKey, int *childrenKeys, int *childCounter,
                  const int numVertices)
{
    *childCounter = 0; // reset counter (fool safe).
    if (strcmp(line, LEAF_SYMBOL) == 0 && strlen(line) == 1)
    {
        return EXIT_SUCCESS;
    }

    int maxKey = numVertices;
    int minKey = 0;
    int childKey;
    // spilt by spaces, or EOF (just to get rid of garbage in the last child key
    char *token = strtok(line, NODE_CHILDREN_TXT_DELIMITER);
    while (token != NULL)
    {
        if (parseNum(token, &childKey) == EXIT_SUCCESS)
        {
            // for every child check if within range and if tree isn't recursive.
            if (minKey <= childKey <= maxKey && childKey != parentKey)
            {
                childrenKeys[*childCounter] = childKey;
                (*childCounter)++;
                token = strtok(NULL, NODE_CHILDREN_TXT_DELIMITER);
                continue;
            }
        }
        return EXIT_FAILURE;
    }
    // make sure childCount per Node in tree < numVertices specified in file.
    if (*childCounter > numVertices)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * adds vertices to the tree given, parsing them from the tree specification provided in the txt
 * file, while checking for problems with input.
 * the function will create new nodes, fill their data and update the tree nodes list and number
 * of actual nodes.
 *
 * Memory Note: for every node, an array of ints is allocated to register his children, if
 * something will went wrong with the problem, the method freeTree will handle all memory
 * allocated, including this one.
 *
 * @param fp - FILE pointer to the txt file.
 * @param inputNumVertices - number of vertices supplied in the first row in the file.
 * @param tree - the tree to build.
 * @return EXIT_SUCCESS, otherwise EXIT_FAILURE.
 */
int addTreeVertices(FILE *fp, const int inputNumVertices, Tree *tree)
{
    fseek(fp, 0, SEEK_SET); // make sure to read file from the start.
    char line[TXT_FILE_LINE_LEN_LIMIT];
    int lineCounter = 1;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (lineCounter < FIRST_NODE_ROW_IN_FILE)
        { // first row (nVertices) handled in other method.
            lineCounter++;
            continue;
        }
        // build new Node in the tree.
        Node v = tree->nodes[tree->nVertices];
        tree->nVertices += 1;

        // use shorter pointer to the Node.
        v.key = NODE_KEY(lineCounter);
        int childCount = 0;

        v.children = (int *) malloc(inputNumVertices * sizeof(int));
        if (v.children == NULL)
        {
            fprintf(stderr, MEMORY_ERROR_MSG);
            return EXIT_FAILURE;
        }
        // update the variables defined before.
        if (parseChildren(line, v.key, v.children, &childCount, inputNumVertices) == EXIT_SUCCESS)
        {
            memset(line, '\0', sizeof(line)); //reset line values.

            if (childCount == 0)
            {
                v.isLeaf = 1;
                free(v.children);
                v.children = NULL;
            }
            else
            {
                v.children = (int *) realloc(v.children, childCount * sizeof(int));
                v.nChildren = childCount;

                if (v.children == NULL)
                {
                    fprintf(stderr, MEMORY_ERROR_MSG);
                    return EXIT_FAILURE;
                }
            }
        }
        else //if parsing node children returned failure
        {
            fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * sets multiple variables used building the tree:
 * this function loads the file into the given pointer, parse and read the first row in file
 * specifying the number of nodes into a variable, and checks the two supplied nodes key.
 *
 * @param filePath valid path for the grpah txt file.
 * @param fp FILE pointer, will be used to parse the whole file outside this scope.
 * @param firstNode - first node string argument given.
 * @param secondNode - second node string argument given.
 * @param inputNumVertices - number of vertices to be in the tree specified in the txt file.
 * @param uKey - parsed first node value, will be updated if function will succeed.
 * @param vKey - parsed second node value.
 * @return the given pointer will be updated and EXIT_SUCCESS will return if no problems found or
 * occurred with one of the arguments. otherwise EXIT_FAILURE.
 */
int initTools(const char *filePath, FILE *fp, char *firstNode, char *secondNode,
                int *inputNumVertices, int *uKey, int *vKey)
{
    long fSize = 0;
    if (loadFile(filePath, fp, fSize) == EXIT_FAILURE ||
        setVerticesNumber(fp, fSize, inputNumVertices) == EXIT_FAILURE
        || checkInputNodes(firstNode, secondNode, uKey, vKey, *inputNumVertices) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}

/**
  * manager program to parse the given tree in a txt file to a data structure allowing
  * manipulations.
  * concretely, parse each node by his key and his list of children, and places an array of nodes
  * in the tree structure.
  * @param filePath valid path to txt file specifying the tree.
  * @param firstNode - starting node finding shortest route to the second one.
  * @param secondNode - ending node for the shortest route from the first one.
  * @param tree - pointer to a tree, will hold every vertex in tree, and will be freed outside
                  this scope.
  * @return EXIT SUCCESS, otherwise EXIT_FAILURE while printing a msg to stderr.
  */
int buildTree(const char *filePath, char *firstNode, char *secondNode, Tree *tree, Node *u, Node *v)
{
    FILE *fp = NULL;
    int inputNumVertices;
    int uKey, vKey;
    if (initTools(filePath, fp, firstNode, secondNode, &inputNumVertices, &uKey, &vKey) ==
        EXIT_SUCCESS)
    {
        tree->nVertices = 0;
        tree->nodes = (Node *) malloc(inputNumVertices * sizeof(Node));
        if (tree->nodes != NULL)
        {
            if (addTreeVertices(fp, inputNumVertices, tree) == EXIT_SUCCESS)
            {
                if (tree->nVertices == inputNumVertices)
                {
                    *u = tree->nodes[uKey];
                    *v = tree->nodes[vKey];
                    fclose(fp);
                    return EXIT_SUCCESS;
                }
            }
        }
    }
    fclose(fp);
    return EXIT_FAILURE;
}

/**
 * free heap memory allocated for the tree - array of children for each node,
 * and the array of nodes in the tree. and the nodes themselves.
 * @param tree - pointer to the tree.
 */
void freeTree(Tree *tree)
{
    for (int i = 0; i < (int) tree->nVertices; i++)
    {
        free(tree->nodes[i].children);
    }
    free(tree->nodes);
    free(tree);
}

int BFS(int nodeKey1, int nodeKey2, Node *routeNodes)
{
    // free memory afterwards!
    // the algorithm will have to explore the parent as well (didnt understand yet).
    // using queue
    // O(n^2)
    return 0;
}

void printTreeDetails(Tree *tree)
{
    for (int i = 0; tree->nVertices; i++)
    {
        printf("for node %d:\n", tree->nodes[i].key);
        printf("children are ");
        for (int j = 0; j < tree->nodes[i].nChildren; j++)
        {
            printf("%d ", tree->nodes[i].children[j]);
        }
    }
    // value of root vertex finding root should take O(n)
    // number of vertices and edges in tree (each one in separate line) O(1)
    // minimal and maximal height in the tree in spearated lines.
    // O(n^2)
    // koter of the tree - length of the longest simple route., if n=1 D(T)=0.
    // the shortest route between the two given vertices - separated with spaces.
}

/**
 * argv[1] - graphFilePath, assuming ther'es no need in conversion to absolute and
 * the file exists.
 * arg[2] = firstVertex
 * argv[3] = second Vertex
 * @return
 */
int main(int argc, char **argv)
{
    if (argc - 1 != EXPECTED_ARGS_NUM) //without the name of the program.
    {
        fprintf(stderr, ARGC_ERROR_MSG);
        return EXIT_FAILURE;
    }
    else
    {
        Tree tree;
        Node u, v;
        if (buildTree(argv[0], argv[1], argv[2], &tree, &u, &v) == EXIT_FAILURE)
        {
            fprintf(stderr, INVALID_INPUT_MSG);
            freeTree(&tree);
            return EXIT_FAILURE;
        }

        printTreeDetails(&tree);
    }
    return EXIT_SUCCESS;
}