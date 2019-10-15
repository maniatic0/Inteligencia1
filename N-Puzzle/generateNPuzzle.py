import sys

from psvnSerializer import psvnSerializer


def generateMovementState(linearStartPosition, linearEndPosition, linearSize):
    """Generate A Move from linearStartPosition to linearEndPosition in an N puzzle of linearSize"""
    fromState = ["-" for _ in range(linearSize)]

    fromState[linearStartPosition] = 0
    fromState[linearEndPosition] = "X"

    toState = ["-" for _ in range(linearSize)]

    toState[linearStartPosition] = "X"
    toState[linearEndPosition] = 0

    return fromState, toState


def generateMovementsRules(position, height, width):
    """Generate valid movement rules from a 2D position (tuple) of a N-Puzzle of height and width"""
    validMovements = []
    size = height * width
    linearStartPos = position[0] * width + position[1]

    # Down (Down is inverted in the matrix representation)
    if position[0] + 1 < height:
        linearEndPos = (position[0] + 1) * width + position[1]
        fromState, toState = generateMovementState(linearStartPos, linearEndPos, size)
        validMovements.append((fromState, toState, "down"))

    # Up (Up is inverted in the matrix representation)
    if position[0] - 1 >= 0:
        linearEndPos = (position[0] - 1) * width + position[1]
        fromState, toState = generateMovementState(linearStartPos, linearEndPos, size)
        validMovements.append((fromState, toState, "up"))

    # Right
    if position[1] + 1 < width:
        linearEndPos = position[0] * width + (position[1] + 1)
        fromState, toState = generateMovementState(linearStartPos, linearEndPos, size)
        validMovements.append((fromState, toState, "right"))

    # Left
    if position[1] - 1 >= 0:
        linearEndPos = position[0] * width + (position[1] - 1)
        fromState, toState = generateMovementState(linearStartPos, linearEndPos, size)
        validMovements.append((fromState, toState, "left"))

    return validMovements

def generateFunc(width, N):
    format = f"""
#include <cstdlib>
long long unsigned heuristic(const state_t *from) {{
    long long unsigned res = 0;
    int x, y, x_r, y_r;
    for(int i = 0; i <{ N}; ++i){{
        if (from->vars[i] == 0) {{
            continue;
        }}
        x = from->vars[i] % {width};
        y = from->vars[i] / {width};
        x_r = i % {width};
        y_r = i / {width};
        res += std::abs(x - x_r) + std::abs(y - y_r);
    }}
    return res;
}}
    """
    return format

def main(height, width, filename, filename_header):

    N = height * witdh

    with open(filename, "w") as f:

        serializer = psvnSerializer(f)

        serializer.AddNumberOfStateVariables(N, "The {}-Puzzle".format(N - 1))

        serializer.AddStateComposition(
            [N for _ in range(N)], "We have {} positions".format(N)
        )

        serializer.AddComment("")
        serializer.AddComment("This is the Row Distribution")
        rowDist = serializer.STATE_SEP.join(
            [
                serializer.STATE_SEP.join([str(i) for _ in range(width)])
                for i in range(height)
            ]
        )
        serializer.AddComment(
            "{} {} {}".format(rowDist, serializer.OPERATOR_MARKER, rowDist)
        )
        serializer.AddComment("")

        for i in range(height):
            for j in range(width):
                pos = (i, j)
                serializer.AddComment("Valid Movements for position {}".format(pos))
                validMovements = generateMovementsRules(pos, height, width)
                for fromState, toState, label in validMovements:
                    serializer.AddRule(fromState, toState, label)

        serializer.AddGoal([i for i in range(N)])

        f.close()
    with open(filename_header, "w") as f:
        print(generateFunc(width, width * height), file = f)
        f.close()
        


if __name__ == "__main__":
    help = "How to use:\ngenerateNPuzzle.py height width <path>\
        \nWhere height and width are the respective height and width of the rectangle to play\
        \nAnd path is the optional path used to save the file. \
        \nIf not provided, it will output to a file called N_puzzle.psvn where N is height * width"

    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print("Not enough arguments", file=sys.stderr)
        print(help, file=sys.stderr)
        exit(-1)

    height = -1
    witdh = -1

    try:
        height = int(sys.argv[1])
    except Exception as ex:
        print("Error reading the height", file=sys.stderr)
        print(ex, file=sys.stderr)
        exit(-1)

    if height < 1:
        print("Invalid height. It must be a positive integer", file=sys.stderr)
        print(ex, file=sys.stderr)
        exit(-1)

    try:
        witdh = int(sys.argv[2])
    except Exception as ex:
        print("Error reading the width", file=sys.stderr)
        print(ex, file=sys.stderr)
        exit(-1)

    if witdh < 1:
        print("Invalid witdh. It must be a positive integer", file=sys.stderr)
        print(ex, file=sys.stderr)
        exit(-1)

    filename = str(height * witdh - 1) + "_puzzle.psvn"
    filename_header = str(height * witdh - 1) + "_puzzle.heuristic.hpp"

    if len(sys.argv) == 4:
        filename = sys.argv[3]

    try:
        main(height, witdh, filename, filename_header)
        exit(0)
    except Exception as ex:
        print("Error while processing", file=sys.stderr)
        print(ex, file=sys.stderr)
        exit(-1)
