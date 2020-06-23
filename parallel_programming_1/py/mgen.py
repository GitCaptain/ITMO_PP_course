import numpy as np


class MatrixGenerator:

    def __init__(self, name_template, dimensions, part=None, boundaries=None):
        self.nameTemplate = "../inputs/" + name_template
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

    def print_matrix(self, name, index=None):
        if not index:
            index = -1
        with open(name, 'w') as file:
            arr = self.generated_matrices[index]
            dim = (len(arr), len(arr[0]))
            arrStr = '\n'.join(' '.join(map(str, arr[i])) + " " for i in range(len(arr)))
            print(*dim, file=file)
            print(arrStr, file=file)

    def generate_and_print_matrix(self, name, dim):
        self.generate_matrix(dim)
        self.print_matrix(name)
        self.generated_matrices.pop()

    def run(self):
        for i in range(self.slice[0], self.slice[1]):
            dim = self.dimensions[i]
            name = self.generate_name([str(i)])
            self.generate_and_print_matrix(name, dim)


class PairMatrixGenerator(MatrixGenerator):

    def __init__(self, name_template, dimensions, part=None):
        super().__init__(name_template, dimensions, part)

    def run(self):
        for i in range(self.slice[0], self.slice[1]):
            for part, dim in enumerate(self.dimensions[i]):
                name = self.generate_name(str(i) + '.' + str(part))
                self.generate_and_print_matrix(name, dim)


if __name__ == '__main__':
    dims = [
        ((2000, 4000), (4000, 800)),
        ((1, 10000), (10000, 15)),
        ((10000, 2), (2, 3000)),
        ((4000, 4000), (4000, 4000)),
        ((1500, 5000), (5000, 400))
            ]

    # dims = [
    #     ((2, 4), (4, 8)),
    #     ((1, 10), (10, 15)),
    #     ((1, 2), (2, 3)),
    #     ((40, 40), (40, 40)),
    #     ((15, 5), (5, 4))
    # ]

    g = PairMatrixGenerator("in", dims)
    g.run()
