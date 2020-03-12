#########################################
############## TEST - EX1 ###############
#########################################
# If you found a bug in the test send email to:
# amit.david@mail.huji.ac.il
#
# HOW TO RUN
# 3) Make sure you has gcc and python 3.x installed
# 1) Place manageStudents.c in the same folder as ex1_check.py
# 2) Run in pycharm or terminal (USE python 3.x)
# 3) Results will show in terminal
#
# All .cmp file generated from school solution
#########################################
import difflib
import subprocess

#########################################
PROGRAM_NAME = "manageStudents"

BEST = "best"
MERGE = "merge"
QUICK = "quick"

#########################################
INVALID_ARGS = "in/invalidArgs.in"
INVALID_EXIT = "in/invalidExit.in"
VALID_DATA = "in/validData.in"
MIXED_DATA = "in/mixedData.in"
LARGE_DATA = "in/5kData.in"
NO_DATA = "in/noData.in"
PRE_1_DATA = "in/1.in"
PRE_2_DATA = "in/2.in"
PRE_12_DATA = "in/12.in"
PRE_13_DATA = "in/13.in"
PRE_17_DATA = "in/17.in"

#########################################
ARGS_TEST = "args"
EXIT_TEST = "exit"
BEST_TEST_VALID = "bestValid"
BEST_TEST_MIXED = "bestMixed"
BEST_TEST_NO_DATA = "bestNoData"
LARGE_TEST = "bestFrom5k"
MERGE_TEST_VALID = "mergeValid"
MERGE_TEST_MIXED = "mergeMixed"
MERGE_TEST_NO_DATA = "mergeNoData"
QUICK_TEST_VALID = "quickValid"
QUICK_TEST_MIXED = "quickMixed"
QUICK_TEST_NO_DATA = "quickNoData"

PRE_1_TEST = "1"
PRE_2_TEST = "2"
PRE_12_TEST = "12"
PRE_13_TEST = "13"
PRE_17_TEST = "17"


#########################################
def cmp_file_name(name):
    return f"cmp/{name}Results.cmp"


def out_file_name(name):
    return f"out/{name}Results.out"


#########################################
def cmpFiles(test_name, cmp_file, out_file):
    with open(cmp_file, 'r') as fCmp:
        with open(out_file, 'r') as fOut:
            diff = difflib.ndiff(fOut.readlines(), fCmp.readlines())

            maybe_error = False

            for i, line in enumerate(diff):
                if maybe_error:
                    if line.startswith("+ ERROR: "):
                        maybe_error = False
                    else:
                        print(test_name + "\nTest - FAIL")
                        print("Difference found in '" + out_file + "' at line " + str(i) + "\n")
                        return

                elif line.startswith("- "):
                    if line.startswith("- ERROR: "):
                        maybe_error = True
                    else:
                        print(test_name + "\nTest - FAIL")
                        print("Difference found in '" + out_file + "' at line " + str(i) + "\n")
                        return

                elif line.startswith("+ "):
                    print(test_name + "\nTest - FAIL")
                    print("Difference found in '" + out_file + "' at line " + str(i) + "\n")
                    return

            print(test_name + " test - PASS")


def test(test_name, arg, in_file, cmp_file, out_file):
    with open(out_file, 'w') as fOut:
        with open(in_file, 'r') as fIn:
            try:
                # Use this line to get results from school solution to out/X.out files
                # p = subprocess.run("~labcc/www/c_ex1/manageStudents " + arg, stdin=fIn, stdout=fOut,
                #                      shell=True, timeout=10)

                # Use this line to test your code
                p = subprocess.run(PROGRAM_NAME + " " + arg, stdin=fIn, stdout=fOut, shell=True, timeout=10)
            except subprocess.TimeoutExpired:
                print(test_name + "\nTest - FAIL")
                print("TIMEOUT Reached")
                return

    cmpFiles(test_name, cmp_file, out_file)


#########################################
if __name__ == "__main__":
    print("Compiling file...")

    compiled = subprocess.run('gcc -Wextra -Wall -Wvla -std=c99 -lm manageStudents.c -o '
                              'manageStudents', capture_output=True, text=True, shell=True)

    if compiled.returncode != 0:
        print("Program failed compiling")
    else:
        print("Program compiled successfully\n\n")

        # testExit
        test("Exit", BEST, INVALID_EXIT, cmp_file_name(EXIT_TEST), out_file_name(
            EXIT_TEST))

        print()

        # testBest
        test("Best - valid data", BEST, VALID_DATA, cmp_file_name(BEST_TEST_VALID), out_file_name(
            BEST_TEST_VALID))
        test("Best - mixed data", BEST, MIXED_DATA, cmp_file_name(BEST_TEST_MIXED), out_file_name(
            BEST_TEST_MIXED))
        test("Best - no data", BEST, NO_DATA, cmp_file_name(BEST_TEST_NO_DATA), out_file_name(
            BEST_TEST_NO_DATA))

        print()

        # test5k
        test("5k", BEST, LARGE_DATA, cmp_file_name(LARGE_TEST), out_file_name(LARGE_TEST))

        print()

        # testMerge
        test("Merge - valid data", MERGE, VALID_DATA, cmp_file_name(MERGE_TEST_VALID),
             out_file_name(MERGE_TEST_VALID))
        test("Merge - mixed data", MERGE, MIXED_DATA, cmp_file_name(MERGE_TEST_MIXED),
             out_file_name(MERGE_TEST_MIXED))
        test("Merge - no data", BEST, NO_DATA, cmp_file_name(MERGE_TEST_NO_DATA), out_file_name(
            MERGE_TEST_NO_DATA))

        print()

        # testQuick
        test("Quick - valid data", QUICK, VALID_DATA, cmp_file_name(QUICK_TEST_VALID),
             out_file_name(QUICK_TEST_VALID))
        test("Quick - mixed data", QUICK, MIXED_DATA, cmp_file_name(QUICK_TEST_MIXED),
             out_file_name(QUICK_TEST_MIXED))
        test("Quick - no data", BEST, NO_DATA, cmp_file_name(QUICK_TEST_NO_DATA), out_file_name(
            QUICK_TEST_NO_DATA))

        print()

        # testPreSubmit
        test("Pre-submit 1", BEST, PRE_1_DATA, cmp_file_name(PRE_1_TEST), out_file_name(
            PRE_1_TEST))
        test("Pre-submit 2", BEST, PRE_2_DATA, cmp_file_name(PRE_2_TEST), out_file_name(
            PRE_2_TEST))

        test("Pre-submit 12", MERGE, PRE_12_DATA, cmp_file_name(PRE_12_TEST), out_file_name(
            PRE_12_TEST))
        test("Pre-submit 13", MERGE, PRE_13_DATA, cmp_file_name(PRE_13_TEST), out_file_name(
            PRE_13_TEST))

        test("Pre-submit 17", QUICK, PRE_17_DATA, cmp_file_name(PRE_17_TEST), out_file_name(
            PRE_17_TEST))
