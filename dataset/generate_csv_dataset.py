import argparse
import random

DATAS = [100, 500, 1000, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 750000, 1000000, 2500000, 5000000]


def parse_args():
    parser = argparse.ArgumentParser(description="Generator")

    parser.add_argument('output',
                        type=str,
                        help='data/output.csv')
    return parser.parse_args()


if __name__ == '__main__':
    args = parse_args()

    if args.samples < 0:
        raise ValueError('Samples != 0!')

    for j in range(len(DATAS)):
        with open(args.output + "/" + str(DATAS[j]) + ".csv", 'w') as file:
            for i in range(DATAS[j]):
                x_coord = random.randint(0, 100)
                y_coord = random.randint(0, 100)

                file.write(str(x_coord) + "," + str(y_coord) + "\n")
