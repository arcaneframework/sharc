import config
from generate_tests import main as generate_tests
from run_tests import main as run_tests
from process_results import main as process_results

def main():
    print 'Generating test cases'
    generate_tests()
    print 'Running tests'
    run_tests()
    print 'Processing results'
    process_results()

if __name__ == "__main__":
    main()
