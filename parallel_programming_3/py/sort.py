from sys import argv

if __name__ == '__main__':
    file_in = argv[1]
    file_out = argv[2]
    with open(file_in) as file:
        sz = file.readline()
        arr = []
        for line in file:
            arr.extend(map(int, line.strip().split()))

    with open(file_out, 'w') as file:
        print(len(arr), file=file)
        print(*sorted(arr), '', file=file)
