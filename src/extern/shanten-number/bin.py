
import argparse

parser = argparse.ArgumentParser(description='Convert txt to binary.')
parser.add_argument('input', type=str, help='input file')
args = parser.parse_args()

input_path = args.input
output_path = input_path.replace('.txt', '.bin')

with open(input_path, 'r') as f:
    fstr = f.read()
    fstr = bytes([int(x) for x in fstr.split()])
    
with open(output_path, 'wb') as f:
    f.write(fstr)
        