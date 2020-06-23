import numpy as np


if __name__ == '__main__':

    fnum = "0"
    names = [f"../inputs/in{fnum}.0", f"../inputs/in{fnum}.1"]
    matrices = []

    for name in names:
        with open(name) as file:
            matrix = []
            dims = file.readline()
            for line in file:
                matrix.append(list(map(int, line.split())))
            matrices.append(np.array(matrix))

    result_name = f"../outputs/res{fnum}"
    with open(result_name, 'w') as file:
        resMatrix = np.matmul(matrices[0], matrices[1])
        resMatrixStr = '\n'.join(' '.join(map(str, resMatrix[i])) + " " for i in range(len(resMatrix)))
        print(len(resMatrix), len(resMatrix[0]), file=file)
        print(resMatrixStr, file=file)
