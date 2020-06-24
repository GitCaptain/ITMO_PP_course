import numpy as np


class VectorGenerator:
    def __init__(self, name_template, dimensions, part=None, boundaries=None):
        self.nameTemplate = "../inputs/" + name_template
        self.dimensions = dimensions
        self.slice = part
        if not part or len(part) != 2:
            self.slice = (0, len(dimensions))
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
            dim = (len(arr))
            arrStr = ' '.join(map(str, arr))
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

    dims = (100, 1000, 10000, 100000, 1000000, 10000000, 100000000//2)
    g = VectorGenerator("vec", dims)
    g.run()
