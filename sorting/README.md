### External Merge Sort

This README provides instructions for running the external merge sort algorithm on an input file.

#### Instructions:

1. **Compilation:**
   - Run the `make` command to compile the source code. This will produce an executable file named `extsort`:

     ```
     make
     ```

2. **Execution:**
   - After compilation, execute the external merge sort algorithm with the following command:

     ```
     ./extsort input.dat output.dat 100 8 32 1
     ```

#### Parameters Explanation:

- `input.dat`: Name of the input file. You can replace it with your specific input file name.
- `output.dat`: Name of the output file where sorted data will be written. You can specify any desired output file name.
- `100`: Size of each record in bytes. Adjust this value according to your input file's record size.
- `8`: Size of the key in bytes. Modify this value to match the key size of your input data.
- `32`: Memory limit in megabytes (MB). Set this value according to the available memory resources.
- `1`: Indication of the sorting order. Use `1` for ascending order or `0` for descending order.
