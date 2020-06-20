import matplotlib.pyplot as plt


def genColor():
    for c in ('r', 'g', 'b', 'c', 'm', 'u'):
        yield c


class Graphic:

    info = dict()
    plot = None
    colorGenerator = genColor()

    @staticmethod
    def parse_file(stat_file):
        with open(stat_file) as f:
            Graphic.info[stat_file] = (list(), list())
            for line in f:
                line = line.strip().split()
                nodes = int(line[0])
                time = float(line[1])
                Graphic.info[stat_file][0].append(nodes)
                Graphic.info[stat_file][1].append(time)

    @staticmethod
    def clear_info():
        Graphic.info = dict()

    def save_plot(self, name):
        self.plot.savefig(name)

    def show_plot(self):
        self.plot.show()

    def make_plot(self):
        
        for calc_type in self.info.keys():
            x = Graphic.info[calc_type][0]
            y = Graphic.info[calc_type][1]
            plt.plot(x, y, c=next(Graphic.colorGenerator), label=calc_type.split('/')[1])

        plt.xlabel('number of nodes')
        plt.ylabel('time, s')
        plt.title('dijkstra openmp: ')
        plt.legend()

        return plt

    def run(self):
        self.plot = self.make_plot()
        self.show_plot()


if __name__ == '__main__':
    prefix = "stat lab 4/"
    files = (prefix + "not paralleled", prefix + "paralleled", prefix + "paralleled with reduction")

    g = Graphic()
    for file in files:
        g.parse_file(file)
    g.run()
