#ifndef FILESORTER_H
#define FILESORTER_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <buffer.h>

using namespace std;

extern long SIZE_OF_REC;

template <typename Rec>
struct RecWithBlockIndex
{
    Rec value;
    size_t index;
};

template <typename Rec>
bool operator<(const RecWithBlockIndex<Rec> &r1, const RecWithBlockIndex<Rec> &r2)
{
    return r1.value < r2.value;
}

template <typename Rec>
bool operator>(const RecWithBlockIndex<Rec> &r1, const RecWithBlockIndex<Rec> &r2)
{
    return r1.value > r2.value;
}

template <typename Rec>
class FileSorter
{
    FILE *m_h_inpfile; // handle to input file
    FILE *m_h_outfile; // handle to output file
    long m_lnrecords;  // Number of records in file.
    int m_i_amt_of_mem;
    int m_sorting_order;

    long CountRecords(string &inFile);
    Rec ReadRecord(size_t index);
    void WriteRecord(size_t index, Rec value);
    size_t GetRecordIndex(size_t start_block, size_t end_block, vector<size_t> block_sizes, size_t start_record, size_t end_block_offset);
    RecWithBlockIndex<Rec> CreateRecWithBlockIndex(const Rec &value, size_t index);

public:
    FileSorter(string &inFile, string &outFile, int amt_of_mem, int sorting_order);
    ~FileSorter();

    int TwoPassMergeSort(long i, long j);
    int TwoPassMergeSort(size_t start_block, vector<size_t> block_sizes, size_t num_of_blocks_to_merge, size_t start_record, size_t end_record);
    size_t GetBufferSize();
    long GetNumRecords();

    void perror(int x);
};

/**
 * @brief Constructs a FileSorter object.
 *
 * This constructor initializes a FileSorter object with the specified input and output files,
 * amount of memory, and sorting order.
 *
 * @param inFile The input file name.
 * @param outFile The output file name.
 * @param amt_of_mem The amount of memory available for sorting.
 * @param sorting_order The sorting order (1 for ascending, 0 for descending).
 */
template <typename Rec>
FileSorter<Rec>::FileSorter(string &inFile, string &outFile, int amt_of_mem, int sorting_order)
{
    // Open input file
    m_h_inpfile = fopen(inFile.c_str(), "rb");
    if (!m_h_inpfile)
    {
        perror(-2); // File IO error
        fclose(m_h_inpfile);
        return;
    }

    // Open output file
    m_h_outfile = fopen(outFile.c_str(), "wb");
    if (!m_h_outfile)
    {
        perror(-2); // File IO error
        fclose(m_h_outfile);
        return;
    }

    // Set amount of memory
    m_i_amt_of_mem = amt_of_mem;

    // Set sortinrg order
    m_sorting_order = sorting_order;

    m_lnrecords = CountRecords(inFile);
}

/**
 * @brief Destructs the FileSorter object.
 *
 * This destructor closes the input and output files if they are open.
 */
template <typename Rec>
FileSorter<Rec>::~FileSorter()
{
    // Close input and output files if they are open
    if (m_h_inpfile)
        fclose(m_h_inpfile);
    if (m_h_outfile)
        fclose(m_h_outfile);
}

/**
 * @brief Counts the number of records in the input file.
 *
 * This function opens the input file and counts the number of records present in it.
 *
 * @param inFile The input file name.
 * @return The number of records in the input file, or -1 if an error occurs.
 */
template <typename Rec>
long FileSorter<Rec>::CountRecords(string &inFile)
{
    // Open input file
    ifstream inpFile_stream(inFile);
    if (!inpFile_stream.is_open())
    {
        perror(-2); // File IO error
        return -1;
    }

    // Count the number of records in the input file
    long num_of_records = 0;
    string line;
    while (getline(inpFile_stream, line))
    {
        ++num_of_records; // Increment record count for each line
    }
    inpFile_stream.close(); // Close the input file stream

    return num_of_records;
}

/**
 * @brief Reads a record from the input file at the specified index.
 *
 * This function seeks to the specified index in the input file and reads a record.
 *
 * @param index The index of the record to read.
 * @return The record read from the input file.
 */
template <typename Rec>
Rec FileSorter<Rec>::ReadRecord(size_t index)
{
    fseek(m_h_inpfile, index * SIZE_OF_REC, SEEK_SET);
    Rec record(m_h_inpfile);
    return record;
}

/**
 * @brief Writes a record to the output file at the specified index.
 *
 * This function seeks to the specified index in the output file and writes a record.
 *
 * @param index The index where the record should be written.
 * @param value The record to write to the output file.
 */
template <typename Rec>
void FileSorter<Rec>::WriteRecord(size_t index, Rec value)
{
    fseek(m_h_outfile, index * SIZE_OF_REC, SEEK_SET);
    fwrite(value.data(), SIZE_OF_REC, 1, m_h_outfile);
}

/**
 * @brief Calculates the index of a record in the file based on block sizes.
 *
 * This function calculates the absolute index of a record in the file given the indices of the starting and ending blocks,
 * the sizes of individual blocks, the index of the starting record within the starting block,
 * and the offset within the ending block.
 *
 * @param start_block The index of the starting block.
 * @param end_block The index of the ending block.
 * @param block_sizes Vector containing the sizes of individual blocks.
 * @param start_record The index of the starting record.
 * @param end_block_offset The offset within the ending block.
 * @return The index of the record in the file.
 */
template <typename Rec>
size_t FileSorter<Rec>::GetRecordIndex(size_t start_block, size_t end_block, vector<size_t> block_sizes, size_t start_record, size_t end_block_offset)
{
    size_t record_index = start_record;
    for (size_t i = start_block; i < end_block; i++)
    {
        record_index += block_sizes[i];
    }
    record_index += end_block_offset;
    return record_index;
}

/**
 * @brief Creates an instance RecordWithBlockIndex object.
 *
 * This function creates a RecordWithBlockIndex object containing a record and the index of block that it belongs to.
 *
 * @param value The record value.
 * @param index The index of the block.
 * @return A RecordWithBlockIndex object containing the record value and its index.
 */
template <typename Rec>
RecWithBlockIndex<Rec> FileSorter<Rec>::CreateRecWithBlockIndex(const Rec &value, size_t index)
{
    RecWithBlockIndex<Rec> r;
    r.value = value;
    r.index = index;
    return r;
}

/**
 * @brief Calculates the number of buffers available based on the amount of memory.
 *
 * This function computes the number of buffers available using the amount of memory allocated to the sorter.
 * The amount of memory available is provided in megabytes (MB), hence it's converted to bytes by multiplying by 1024*1024.
 * Then, it divides the available memory in bytes by the size of each record multiplied by 2 (assuming each buffer holds twice the size of a record)
 * to determine the number of buffers available.
 *
 * @return The number of buffers available based on the allocated memory.
 */
template <typename Rec>
size_t FileSorter<Rec>::GetBufferSize()
{
    return static_cast<size_t>(m_i_amt_of_mem) * 1024 * 1024 / (SIZE_OF_REC * 2);
}

/**
 * @brief Sorts records within a specified range.
 *
 * This method sorts records in the file from record index 'i' to 'j'.
 * It reads records into a buffer, sorts them either in ascending or descending order based on the sorting order,
 * and then writes the sorted records to the output file.
 *
 * @param i The starting index of the range of records to be sorted.
 * @param j The ending index of the range of records to be sorted.
 * @return An integer indicating the success of the sorting operation (1 for success).
 */
template <typename Rec>
int FileSorter<Rec>::TwoPassMergeSort(long i, long j)
{
    vector<Rec> buffer(GetBufferSize());

    long records_read = 0;
    for (long cur_record_idx = i; cur_record_idx <= j; cur_record_idx++)
    {
        Rec record = ReadRecord(cur_record_idx);
        buffer[records_read++] = record;
    }

    if (m_sorting_order == 1)
    {
        // sort in ascending order
        sort(buffer.begin(), buffer.begin() + records_read);
    }
    else
    {
        // sort in descending order
        sort(buffer.begin(), buffer.begin() + records_read, greater<Rec>());
    }

    for (long cur_record_idx = i; cur_record_idx <= j; cur_record_idx++)
    {
        WriteRecord(cur_record_idx, buffer[cur_record_idx - i]);
    }

    return 1;
}

/**
 * @brief Merges records within the specified range using the provided block sizes.
 * 
 * This method merges records within the specified range by reading them into a buffer, which employs a priority queue.
 * The buffer continuously pops the smallest or largest record (depending on the sorting order) and writes it to the output file
 * until all records within the range are merged.
 *
 * @param start_block The index of the starting block.
 * @param block_sizes Vector containing the sizes of individual blocks.
 * @param num_of_blocks_to_merge Number of blocks to merge.
 * @param start_record The index of the starting record.
 * @param end_record The index of the ending record.
 * @return An integer indicating the success of the merging operation (1 for success, -1 for failure).
 */
template <typename Rec>
int FileSorter<Rec>::TwoPassMergeSort(
    size_t start_block,
    vector<size_t> block_sizes,
    size_t num_of_blocks_to_merge,
    size_t start_record,
    size_t end_record)
{
    if (num_of_blocks_to_merge == 0)
    {
        return 1;
    }
    // If number of blocks need to be merged is 1
    else if (num_of_blocks_to_merge == 1)
    {
        // Writes all records from the block to the output file
        for (size_t i = start_record; i < end_record; i++)
        {
            Rec record = ReadRecord(i);
            WriteRecord(i, record);
        }
        return 1;
    }

    Buffer<RecWithBlockIndex<Rec>> buffer(num_of_blocks_to_merge, m_sorting_order);

    // Keeps track of the merged record index from each block
    vector<size_t> current_block_record_indices(block_sizes.size(), 0);
    size_t record_index = start_record;

    // Populates the buffer with the first record on each block
    for (size_t i = 0; i < num_of_blocks_to_merge; i++)
    {
        Rec record = ReadRecord(record_index);
        RecWithBlockIndex<Rec> record_with_block_index = CreateRecWithBlockIndex(record, i + start_block);
        current_block_record_indices[i + start_block]++;

        if (!buffer.push(record_with_block_index))
        {
            perror(-3);
            return -1;
        }
        record_index += block_sizes[i + start_block];
    }

    size_t current_record = start_record;

    // Merges all records within the specified range from the starting index to the end index
    while (current_record < end_record && !buffer.empty())
    {
        RecWithBlockIndex<Rec> r = buffer.top();
        WriteRecord(current_record, r.value);
        buffer.pop();
        current_record++;

        // Gets the block index where the popped record belongs
        size_t block_index = r.index;
        size_t current_block_record_index = current_block_record_indices[block_index];

        // If there are more records need to be merged from the block
        if (current_block_record_index < block_sizes[block_index])
        {
            // Reads the next record from the same block and adds it to the buffer
            size_t r_index = GetRecordIndex(start_block, block_index, block_sizes, start_record, current_block_record_index);
            Rec record = ReadRecord(r_index);
            RecWithBlockIndex<Rec> record_with_block_index = CreateRecWithBlockIndex(record, block_index);
            if (!buffer.push(record_with_block_index))
            {
                perror(-3);
                return -1;
            }
            current_block_record_indices[block_index]++;
        }
    }

    return 1;
}

/**
 * @brief Gets the total number of records in the file.
 *
 * @return The total number of records in the file.
 */
template <typename Rec>
long FileSorter<Rec>::GetNumRecords()
{
    return m_lnrecords;
}

/**
 * @brief Prints an error message based on the provided error code.
 *
 * This method prints a descriptive error message based on the error code passed as an argument.
 * The error code can represent various error conditions encountered during sorting.
 *
 * Error Codes:
 * -1: "No disk space left."
 * -2: "File IO error."
 * -3: "Buffer is full."
 * -4: "Sorting failed."
 * Default: "Unknown error code: x" (where 'x' is the provided error code)
 *
 * @param x The error code indicating the type of error.
 */
template <typename Rec>
void FileSorter<Rec>::perror(int x)
{
    // Print a verbose output of what went wrong when sorting was attempted
    switch (x)
    {
    case -1:
        cout << "No disk space left." << endl;
        break;
    case -2:
        cout << "File IO error." << endl;
        break;
    case -3:
        cout << "Buffer is full." << endl;
        break;
    case -4:
        cout << "Sorting failed." << endl;
        break;
    default:
        cout << "Unknown error code: " << x << endl;
    }
}

#endif