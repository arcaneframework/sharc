import argparse

parser = argparse.ArgumentParser()
parser.add_argument("config_file", help="File configuration")
parser.add_argument("verbose", nargs='?', type=int, default=0, help="verbose mode")
args = parser.parse_args()

verbose = args.verbose
config_file = args.config_file
