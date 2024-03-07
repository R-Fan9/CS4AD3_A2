#include <iostream>
#include <cstdio>
#include <cstdio>
#include <cmath>
#include <vector>
#include <record.h>
#include <fileSorter.h>

using namespace std;

// GLOBALS
long KEY_SIZE;
int SORTING_ORDER;
long SIZE_OF_REC;

/**
 * @brief Calculates the number of blocks required for processing entities with given buffers.
 *
 * @param num_of_entities Total number of entities to process.
 * @param num_of_buffers Number of available buffers.
 * @return Number of blocks needed.
 */
size_t get_num_blocks(long num_of_entities, size_t num_of_buffers)
{
    double n = static_cast<double>(num_of_entities) / num_of_buffers;
    return static_cast<size_t>(ceil(n));
}

/**
 * @brief Calculates the number of passes needed for external merge sort.
 *
 * @param num_of_records Total number of records.
 * @param num_of_buffers Number of available buffers.
 * @return Number of passes needed.
 */
int get_num_passes(long num_of_records, size_t num_of_buffers)
{
    double records_per_buffer = ceil(static_cast<double>(num_of_records) / num_of_buffers);
    double log_base = num_of_buffers;
    double log_result = log(records_per_buffer) / log(log_base);
    double num_of_passes = ceil(log_result);
    return static_cast<int>(num_of_passes);
}

/**
 * @brief Pass 0 of the external merge sort algorithm.
 *
 * This function represents the initial pass of the external merge sort algorithm.
 * It breaks down the records from the input file into blocks, sorts each block individually,
 * and returns the sizes of the blocks generated.
 *
 * @param in_file The input file containing the unsorted records.
 * @param out_file The output file to store the sorted blocks of records.
 * @param amt_of_mem The amount of memory available for sorting.
 * @param num_of_buffers Number of available buffers for sorting.
 * @param num_of_records Total number of records in the input file.
 * @return A vector containing the sizes of the blocks generated.
 */
vector<size_t> pass0(string in_file, string out_file, int amt_of_mem, size_t &num_of_buffers, long &num_of_records)
{
    FileSorter<Record> sorter(in_file, out_file, amt_of_mem, SORTING_ORDER);
    num_of_records = sorter.GetNumRecords();
    num_of_buffers = sorter.GetBufferSize();
    long num_of_blocks = get_num_blocks(num_of_records, num_of_buffers);
    vector<size_t> block_sizes(num_of_blocks);

    for (long i = 0; i < num_of_blocks - 1; i++)
    {
        block_sizes[i] = num_of_buffers;
    }
    block_sizes[num_of_blocks - 1] = num_of_records % num_of_buffers;

    long start_record = 0;
    for (long i = 0; i < num_of_blocks; i++)
    {
        size_t block_size = block_sizes[i];
        int sorted = sorter.TwoPassMergeSort(start_record, start_record + block_size - 1);
        if (!sorted)
        {
            sorter.perror(-4);
        }

        start_record += block_size;
    }

    return block_sizes;
}

/**
 * @brief Merge a specified number of sorted blocks into a single sorted block.
 *
 * This function merges a specified number of sorted blocks into one sorted block.
 * It calculates the index of the start and end records for the blocks to be merged,
 * then performs a merge sort operation on them.
 *
 * @param sorter A reference to the FileSorter object used for sorting.
 * @param block_sizes Vector containing the sizes of individual blocks.
 * @param start_block Index of the first block to merge.
 * @param num_of_blocks_to_merge Number of blocks to merge.
 * @return The size of the resulting merged block.
 */
size_t merge_blocks(FileSorter<Record> &sorter, vector<size_t> block_sizes, size_t start_block, size_t num_of_blocks_to_merge)
{
    size_t start_record = 0;
    for (size_t i = 0; i < start_block; i++)
    {
        start_record += block_sizes[i];
    }

    size_t end_record = start_record;
    for (size_t i = start_block; i < start_block + num_of_blocks_to_merge; i++)
    {
        end_record += block_sizes[i];
    }

    int sorted = sorter.TwoPassMergeSort(start_block, block_sizes, num_of_blocks_to_merge, start_record, end_record);
    if (!sorted)
    {
        sorter.perror(-4);
    }

    return end_record - start_record;
}

/**
 * @brief Perform a pass of the external merge sort algorithm.
 *
 * This function represents a pass (1, 2, ... n) of the external merge sort algorithm.
 * In each pass, it merges subsets of sorted blocks into larger sorted blocks.
 * The number of blocks in a subset to be merged depends on the number of available buffers.
 *
 * @param in_file The input file containing the sorted blocks of records.
 * @param out_file The output file to store the larger sorted blocks of records.
 * @param amt_of_mem The amount of memory available for sorting.
 * @param block_sizes Vector containing the sizes of individual blocks.
 * @return A vector containing the sizes of the merged blocks.
 */
vector<size_t> pass(string in_file, string out_file, int amt_of_mem, vector<size_t> block_sizes)
{
    FileSorter<Record> sorter(in_file, out_file, amt_of_mem, SORTING_ORDER);
    size_t num_of_blocks = block_sizes.size();

    size_t num_of_buffers = sorter.GetBufferSize();
    size_t num_of_new_blocks = get_num_blocks(num_of_blocks, num_of_buffers);
    vector<size_t> new_block_sizes(num_of_new_blocks);

    size_t start_block = 0;
    for (size_t i = 0; i < num_of_new_blocks; i++)
    {
        size_t n = min(num_of_blocks, num_of_buffers);
        new_block_sizes[i] = merge_blocks(sorter, block_sizes, start_block, n);
        num_of_blocks -= n;
        start_block += n;
    }

    return new_block_sizes;
}

int main(int argc, char **argv)
{
    string in_file_name;
    string out_file_name;
    int amt_of_mem = 0; // amount of available memory in MB

    argv++;
    in_file_name = argv[0];

    argv++;
    out_file_name = argv[0];

    argv++;
    SIZE_OF_REC = atol(argv[0]);

    argv++;
    KEY_SIZE = atol(argv[0]);

    argv++;
    amt_of_mem = atoi(argv[0]);

    argv++;
    SORTING_ORDER = atoi(argv[0]);

    size_t num_of_buffers;
    long num_of_records;

    string tmp_file_name = "pass0.dat";
    vector<size_t> block_sizes = pass0(in_file_name, tmp_file_name, amt_of_mem, num_of_buffers, num_of_records);
    int num_of_passes = get_num_passes(num_of_records, num_of_buffers);

    for (int i = 0; i < num_of_passes - 1; i++)
    {
        string tmp_outfile_name = "pass" + to_string(i + 1) + ".dat";
        block_sizes = pass(tmp_file_name, tmp_outfile_name, amt_of_mem, block_sizes);
        remove(tmp_file_name.c_str());
        tmp_file_name = tmp_outfile_name;
    }

    pass(tmp_file_name, out_file_name, amt_of_mem, block_sizes);
    remove(tmp_file_name.c_str());

    return 0;
}