import numpy as np
from general import GENERATED_MATRIX_DIR
from random import randint


class MatrixGenerator:

    def __init__(self, name_template, dimensions, part=None, boundaries=None):
        self.nameTemplate = GENERATED_MATRIX_DIR + name_template
        self.dimensions = dimensions
        self.slice = part
        if not part or len(part) != 2:
            self.slice = (0, len(dimensions))
        self.boundaries = boundaries
        if not boundaries or len(boundaries) != 2:
            self.boundaries = (1, 1000)

        self.generated_matrices = []

    def get_matrices(self):
        return self.generated_matrices

    def generate_name(self, name_suffix):
        return self.nameTemplate + name_suffix

    def generate_matrix(self, dim):
        matrix = np.random.randint(*self.boundaries, dim[0] * dim[1]).reshape(dim[0], dim[1])
        self.generated_matrices.append(matrix)

    def generate_matrix(self, dim):
        matrix = np.random.randint(*self.boundaries, dim[0] * dim[1]).reshape(dim[0], dim[1])
        for i, line in enumerate(matrix):
            mx = max(line)
            matrix[i][i] = mx + randint(1, mx)
        self.generated_matrices.append(matrix)

    def print_matrix(self, name, index=None):
        if not index:
            index = -1
        with open(name, 'w') as file:
            arr = self.generated_matrices[index]
            dim = (len(arr), len(arr[0]))
            arrStr = '\n'.join(' '.join(map(str, arr[i])) for i in range(len(arr)))
            print(*dim, file=file)
            print(arrStr, file=file)

    def generate_and_print_matrix(self, name, dim):
        self.generate_matrix(dim)
        self.print_matrix(name)
        self.generated_matrices.pop()

    def run(self):
        for i in range(self.slice[0], self.slice[1]):
            dim = self.dimensions[i]
            name = self.generate_name(str(i))
            self.generate_and_print_matrix(name, dim)


class PairMatrixGenerator(MatrixGenerator):

    def __init__(self, name_template, dimensions, part=None):
        super().__init__(name_template, dimensions, part)

    def run(self):
        for i in range(self.slice[0], self.slice[1]):
            for part, dim in enumerate(self.dimensions[i]):
                name = self.generate_name(str(i) + '.' + str(part))
                self.generate_and_print_matrix(name, dim)


class VectorGenerator:

    def __init__(self, name_template, dimension, part=None, boundaries=None, transposed=False):
        self.nameTemplate = GENERATED_MATRIX_DIR + name_template
        self.dimensions = dimension
        self.slice = part
        self.transposed = transposed
        if not part or len(part) != 2:
            self.slice = (0, len(dimension))
        self.boundaries = boundaries
        if not boundaries or len(boundaries) != 2:
            self.boundaries = (1, 1000)

        self.generated_vectors = []

    def get_matrices(self):
        return self.generated_vectors

    def generate_name(self, name_suffix):
        return self.nameTemplate + name_suffix

    def generate_vector(self, dim):
        vector = np.random.randint(*self.boundaries, dim)
        self.generated_vectors.append(vector)

    def print_vector(self, name, index=None):
        if not index:
            index = -1
        with open(name, 'w') as file:
            arr = self.generated_vectors[index]
            dim = len(arr)
            toJoin = ' '
            if self.transposed:
                toJoin = '\n'
            arrStr = toJoin.join(map(str, arr))
            print(dim, file=file)
            print(arrStr, file=file)

    def generate_and_print_vector(self, name, dim):
        self.generate_vector(dim)
        self.print_vector(name)
        self.generated_vectors.pop()

    def run(self):
        for i in range(self.slice[0], self.slice[1]):
            dim = self.dimensions[i]
            name = self.generate_name(str(i))
            self.generate_and_print_vector(name, dim)


if __name__ == '__main__':

    name = "vec"
    dims = (10, 20, 40, 100, 1000, 10000)
    g = VectorGenerator(name, dims, boundaries=(0, 1000))
    g.run()

    for i in range(len(dims)):
        with open(GENERATED_MATRIX_DIR+name+str(i)) as file:
            for line in file:
                d = sorted(map(int, line.strip().split()))

        with open(GENERATED_MATRIX_DIR+name+str(i)+'o', 'w') as file:
            print(len(d), file=file)
            for e in d:
                print(e, file=file, end=" ")
