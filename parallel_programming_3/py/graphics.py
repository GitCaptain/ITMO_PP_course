import matplotlib.pyplot as plt


class Graphic:

    info = dict()
    plot = None

    colors = {
        1: 'r',
        2: 'g',
        4: 'b',
        8: 'y'
    }
    markers = {
        1: 'o',
        2: 'P',
        4: 'D',
        8: 'x'
    }

    @staticmethod
    def parse_file(stat_file):
        with open(stat_file) as f:
            for line in f:
                line = line.strip().split()
                vector = line[0]
                processes = int(line[1])
                time = float(line[2])
                if not Graphic.info.get(processes):
                    Graphic.info[processes] = (list(), list())
                Graphic.info[processes][0].append(vector)
                Graphic.info[processes][1].append(time)

    @staticmethod
    def clear_info():
        Graphic.info = dict()

    def save_plot(self, name):
        self.plot.savefig(name)

    def show_plot(self):
        self.plot.show()

    def make_plot(self):

        for process, values in self.info.items():
            plt.plot(values[0], values[1], c=self.colors[process], label="number of processes: " + str(process))

        plt.xlabel('array size')
        plt.ylabel('time, s')
        plt.title('Quick Sort + MPI: ')
        plt.legend()

        return plt

    def run(self):
        self.plot = self.make_plot()
        self.show_plot()


if __name__ == '__main__':
    file = "../outputs/timelog"
    g = Graphic()
    g.parse_file(file)
    g.run()
