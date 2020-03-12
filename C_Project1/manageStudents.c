/**
 * @file manageStudents.c
 * @author  Guy Kornblit <guy.kornblit@mail.huji.ac.il>
 * @version 1.0
 * @date 12 Nov 2019
 * @brief This file holds the code for manageStudents program.
 * the program allows recording student details, manipulating and analysing the data.
 * namely, the program supports 2 features:
 * a. finding the best student out of the students registered.
 * b. sorting students by specified keys.
 */
// ------------------------------ includes ------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// -------------------------- const definitions -------------------------
#define MAIN_ARGC 2 // file name and feature name.
#define ARGV_MAX_LEN 6
#define BEST_FEATURE_ARGUMENT "best"
#define MERGE_FEATURE_ARGUMENT "merge"
#define QUICK_FEATURE_ARGUMENT "quick"
#define USAGE_MSG "USAGE: type .\\manageStudents followed by 'best', 'merge', or 'quick'"
#define MAX_ARG_LEN 41 // 40 + null char
#define REQUIRED_ARGS  6
#define MAX_INPUT_LINE_LEN 151 // 150 assumed + null char
#define MAX_STUDENTS_ENTERED 5000
#define EXIT_KEY_WIN "q\r\n"
#define EXIT_KEY_LINUX "q\n"
#define N_ARGS_ERR_MSG "ERROR: Wrong number of arguments, expected: 6 args divided by tabs.\n"
#define ID_ERR_MSG "ERROR: ID have to be 10 numbers, while the first digit is not zero.\n"
#define NAME_ERR_MSG "ERROR: name only contain alphabetic characters, spaces or \"-\".\n"
#define GRADE_ERR_MSG "ERROR: grade can only contain number between 0-100.\n"
#define AGE_ERR_MSG "ERROR: age can only contain number between 18-120.\n"
#define COUNTRY_ERR_MSG "ERROR: country can only contain alphabetic chars or \"-\".\n"
#define CITY_ERR_MSG "ERROR: city can only contain alphabetic chars or \"-\".\n"
#define LINE_INDICATOR_MSG "in line %d\n"

// ------------------------------ functions declaration --------------------------

/**
 * @brief This enum holds error types when handling the user input argument.
 */
typedef enum
{
    NO_PROBLEM,
    ID_ERROR,
    NAME_ERROR,
    GRADE_ERROR,
    AGE_ERROR,
    COUNTRY_ERROR,
    CITY_ERROR,
    N_ARGS_ERROR,
    PARSE_ERROR
} ErrorType;

/**
 * @brief A struct representing a student details.
 */
typedef struct StudentInfo
{
    char id[MAX_ARG_LEN];
    char name[MAX_ARG_LEN];
    int grade;
    int age;
    char country[MAX_ARG_LEN];
    char city[MAX_ARG_LEN];

} StudentInfo;

/**
 * @brief check if the id provided by user valid - thus all chars are digits, and the first digit
 * is not zero.
 * @return 0 if check ran successfully, and ID_ERROR otherwise.
 */
int checkId();

/**
 * @brief this function manages the validity of the user argument as provided in free text in
 * stdin, according to various guidelines specified in every validation function.
 * besides nArgs, each argument is global so there is no need in other params.
 * @param nArgs - int representing number of arguments provided (or that were interpreted
 * successfully).
 * @return - a relevant ErrorType; NO_PROBLEM if validation passed succesfully.
 */
ErrorType checkArgsLegality(int nArgs);

/**
 ** @brief checks if a given int is within a given range.
  * @param toInt - int to check.
  * @param minVal - minimal result value.
  * @param maxVal - maximal result value.
  * @param dest - pointer to the result int.
  * @return ErrorType NO_PROBLEM if successfully, PARSE_ERROR otherwise. saves the parsed correct
  * data if the process was successful.
  */
ErrorType parseRangedInt(const char *toInt, int *dest, int minVal, int maxVal);

/**
 * @brief prints informative error to stdout, and the line number where the error occurred.
 * and resets the error flag to NO_PROBLEM.
 * @param msg - an informative msg about the problem.
 * @param lineNum int value.
 */
void printError(const char msg[], int lineNum);

/**
 * @brief This function checks if the given string contains standard alphabetic characters, or
 * one of the valid characters provided.
 * @param toCheck - string to validate.
 * @param validChars - array of valid characters to include in the given string.
 * @return 0 when success, NAME_ERROR otherwise.
 */
int checkNameArg(const char toCheck[], const char validChars[]);

/**
 * @brief This function registers a students with valid details as provided and parsed by user, and
 * saves it by it's order of insertion to the global students array.
 * @param studentCount - number of valid students provided and recorded.
 */
void registerStudent(int studentCount);

/**
 * @brief this function manages the output for the best student from those recorded, and proceed
 * only if
 * the students array isn't empty. If it does, an informative msg is printed to stdout.
 */
void outputBestStudent();

/**
 * @brief When called, this function calculate among the students recorded which one have the
 * highest grade/age ratio.
 * @return StudentInfo representing the best student.
 */
StudentInfo chooseBest();

/**
 * @brief This function used to reset to zeroes all of the user arguments (saved globally), in
 * order to better represent and avoid bugs during iterations (including user mistakes).
 */
void resetArgs();

/**
* @brief The main function that implements QuickSort.
* @param arr Array to be sorted.
* @param low Starting index
* @param high Ending index
*/
void quickSort(StudentInfo arr[], int low, int high);

/**
 *  @brief This function takes last element as pivot, places the pivot element at its correct
 *  position in sorted array, and places all smaller (smaller than pivot) to left of pivot and
 *  all greater elements to right of pivot.
 * @param arr array to order by pivot.
 * @param low starting index
 * @param high ending index
 * @return sorted index of pivot.
 */
int partition(StudentInfo arr[], int low, int high);

/**
 * @brief A utility function to swap two elements inplace.
 * @param a student info pointer
 * @param b student info pointer
 */
void swap(StudentInfo *a, StudentInfo *b);

/**
* @brief Main function to run merge sort on the students array.
* @param arr
* @param l  left index of the sub-array of arr to be sorted
* @param r right index of the sub-array of arr to be sorted
*/
void mergeSort(StudentInfo arr[], int l, int r);

/**
 * @brief Merges two sub arrays
 * of arr[].
 * First subarray is arr[l..m], Second subarray is arr[m+1..r]
 * @param arr arr to be split and merged sorted
 * @param l left index of the subarray
 * @param m middle index of the subarray
 * @param r right index of the subarray.
 */
void merge(StudentInfo arr[], int l, int m, int r);


// Constants
const int ID_NUM_DIGITS = 10;
const int MAX_AGE = 120;
const int MIN_AGE = 18;
const int MIN_GRADE = 0;
const int MAX_GRADE = 100;
const char LOCATION_VALID_CHARS[1] = "-";
const char NAME_VALID_CHARS[2] = " -";

// Global variables.
char gId[MAX_ARG_LEN], gGrade[MAX_ARG_LEN], gAge[MAX_ARG_LEN];
char gName[MAX_ARG_LEN], gCountry[MAX_ARG_LEN], gCity[MAX_ARG_LEN];
int gNumRegistered = 0;
int gParsedGrade;
int gParsedAge;
StudentInfo gStudents[MAX_STUDENTS_ENTERED];
ErrorType gInputError = NO_PROBLEM;

// Implementation.
void readInput()
{
    int inputLine = 0;
    char userInput[MAX_INPUT_LINE_LEN] = "\0";
    while (gInputError == NO_PROBLEM) // indicator that all problem handled.
    {
        printf("Enter student info. To exit press q, then enter\n");
        // reset previous input variables.
        resetArgs();
        memset(userInput, '\0', MAX_INPUT_LINE_LEN);

        fgets(userInput, MAX_INPUT_LINE_LEN, stdin);
        if (!strcmp(userInput, EXIT_KEY_WIN) || !strcmp(userInput, EXIT_KEY_LINUX))
        {
            return;
        }
        else
        {
            int nArgs = sscanf(
                    userInput,
                    "%[^\t\n]\t%[^\t\n]\t%[^\t\n]\t%[^\t\n]\t%[^\t\n]\t%[^\t\n]",
                    gId, gName, gGrade, gAge, gCountry, gCity);
            gInputError = checkArgsLegality(nArgs);
            switch (gInputError)
            {
                case N_ARGS_ERROR:
                    printError(N_ARGS_ERR_MSG, inputLine++);
                    continue;
                case ID_ERROR:
                    printError(ID_ERR_MSG, inputLine++);
                    continue;
                case NAME_ERROR:
                    printError(NAME_ERR_MSG, inputLine++);
                    continue;
                case GRADE_ERROR:
                    printError(GRADE_ERR_MSG, inputLine++);
                    continue;
                case AGE_ERROR:
                    printError(AGE_ERR_MSG, inputLine++);
                    continue;
                case COUNTRY_ERROR:
                    printError(COUNTRY_ERR_MSG, inputLine++);
                    continue;
                case CITY_ERROR:
                    printError(CITY_ERR_MSG, inputLine++);
                    continue;
                default:
                    registerStudent(gNumRegistered++);
                    ++inputLine;
                    continue;
            }
        }
    }
}

void outputBestStudent()
{
    if (gNumRegistered != 0)
    {
        StudentInfo best = chooseBest();
        printf("best student info is: %s\t%s\t%d\t%d\t%s\t%s\t\n", best.id, best.name,
               best.grade, best.age, best.country, best.city);
    }
}

StudentInfo chooseBest()
{
    float maxScore = 0.f;
    int bestStudent = 0;
    for (int i = 0; i < gNumRegistered; i++)
    {
        float ratio = ((float) gStudents[i].grade / (float) gStudents[i].age);
        if (ratio > maxScore)
        {
            maxScore = ratio;
            bestStudent = i;
        }
    }
    return gStudents[bestStudent];
}

void resetArgs()
{
    memset(gId, '\0', MAX_ARG_LEN);
    memset(gName, '\0', MAX_ARG_LEN);
    memset(gGrade, '\0', MAX_ARG_LEN);
    memset(gAge, '\0', MAX_ARG_LEN);
    memset(gCountry, '\0', MAX_ARG_LEN);
    memset(gCity, '\0', MAX_ARG_LEN);
}

void printError(const char msg[], const int lineNum)
{
    printf("%s", msg);
    printf(LINE_INDICATOR_MSG, lineNum);
    gInputError = NO_PROBLEM;
}

ErrorType checkArgsLegality(const int nArgs)
{
    if (nArgs != REQUIRED_ARGS)
    {
        return N_ARGS_ERROR;
    }
    else if (checkId())
    {
        return ID_ERROR;
    }
    else if (checkNameArg(gName, NAME_VALID_CHARS))
    {
        return NAME_ERROR;
    }
    else if (parseRangedInt(gGrade, &gParsedGrade, MIN_GRADE, MAX_GRADE) == PARSE_ERROR)
    {
        return GRADE_ERROR;
    }
    else if (parseRangedInt(gAge, &gParsedAge, MIN_AGE, MAX_AGE) == PARSE_ERROR)
    {
        return AGE_ERROR;
    }
    else if (checkNameArg(gCountry, LOCATION_VALID_CHARS))
    {
        return COUNTRY_ERROR;
    }
    else if (checkNameArg(gCity, LOCATION_VALID_CHARS))
    {
        return CITY_ERROR;
    }
    else
    {
        return NO_PROBLEM;
    }
}

void registerStudent(const int studentCount)
{
    StudentInfo student = {.grade = gParsedGrade, .age = gParsedAge};
    strcpy(student.id, gId);
    strcpy(student.name, gName);
    strcpy(student.country, gCountry);
    strcpy(student.city, gCity);
    gStudents[studentCount] = student;
}

int checkId()
{
    if (((int) strlen(gId)) == ID_NUM_DIGITS && gId[0] != '0')
    {
        for (int i = 0; i < ID_NUM_DIGITS; i++)
        {
            if (isdigit(gId[i]))
            {
                continue;
            }
            else
            {
                return ID_ERROR;
            }
        }
        return 0;
    }
    return ID_ERROR;
}

ErrorType parseRangedInt(const char *toInt, int *dest, const int minVal, const int maxVal)
{
    char *end = NULL;
    int parsed = (int) strtol(toInt, &end, 10);
    if (*end == '\0')
    {
        if (parsed <= maxVal && parsed >= minVal)
        {
            *dest = parsed;
            return NO_PROBLEM;
        }
    }
    return PARSE_ERROR;
}

int checkNameArg(const char toCheck[], const char validChars[])
{
    for (int i = 0; i < (int) strlen(toCheck); i++)
    { // Assuming valid input size per arg
        // (therefore int casting should work fine).
        if (!isalpha(toCheck[i]))
        { // if there's a non letter char, check if it's within valid
            // chars.
            if (strchr(validChars, toCheck[i]) == NULL)
            {
                return NAME_ERROR;
            }
        }
    }
    return 0;
}

void merge(StudentInfo arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // create temp arrays
    StudentInfo L[MAX_STUDENTS_ENTERED], R[MAX_STUDENTS_ENTERED];

    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
    {
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++)
    {
        R[j] = arr[m + 1 + j];
    }
    // Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i].grade <= R[j].grade)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any.
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(StudentInfo arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void swap(StudentInfo *a, StudentInfo *b)
{
    StudentInfo temp = *a;
    *a = *b;
    *b = temp;
}

int partition(StudentInfo arr[], int low, int high)
{
    StudentInfo pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (strcmp(arr[j].name, pivot.name) < 0)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(StudentInfo arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/**
 * @brief this function loops through an array of students info and prints it by order.
 * @param res - pointer to a sorted student info array.
 */
void outputSorted(StudentInfo res[])
{
    if (gNumRegistered != 0)
    {
        for (int i = 0; i < gNumRegistered; i++)
        {
            printf("%s\t%s\t%d\t%d\t%s\t%s\t\n", res[i].id, res[i].name,
                   res[i].grade, res[i].age, res[i].country, res[i].city);
        }
    }
}

/**
 * @brief Main function, validate the program arguments, and manages by input which feature to
 * activate.
 * @param argc number of args
 * @param argv arguments provided in CLI
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int main(int argc, char *argv[])
{
    if (argc == MAIN_ARGC && strlen(argv[1]) <= ARGV_MAX_LEN)
    {
        if (!strcmp(argv[1], BEST_FEATURE_ARGUMENT))
        {
            readInput();
            // if the user requested to exit "best" and the output prompts successfully
            outputBestStudent();
            return EXIT_SUCCESS;

        }
        else if (!strcmp(argv[1], MERGE_FEATURE_ARGUMENT))
        {
            readInput();
            mergeSort(gStudents, 0, gNumRegistered - 1);
            outputSorted(gStudents);
            return EXIT_SUCCESS;

        }
        else if (!strcmp(argv[1], QUICK_FEATURE_ARGUMENT))
        {
            readInput();
            quickSort(gStudents, 0, gNumRegistered - 1);
            outputSorted(gStudents);
            return EXIT_SUCCESS;
        }
    }
    else
    {
        printf(USAGE_MSG);
        return EXIT_FAILURE;
    }
}