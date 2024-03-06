#ifndef FILESORTER_H
#define FILESORTER_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <buffer.h>

using namespace std;

extern int sorting_order;

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

    long CountRecords(string &inFile);
    Rec ReadRecord(size_t index);
    void WriteRecord(size_t index, Rec value);
    size_t GetRecordIndex(size_t start_block, size_t end_block, vector<size_t> block_sizes, size_t start_record, size_t end_block_offset);
    RecWithBlockIndex<Rec> CreateRecWithBlockIndex(const Rec &value, size_t index);

public:
    FileSorter(string &inFile, string &outFile, int amt_of_mem);
    ~FileSorter();

    int TwoPassMergeSort(long i, long j);
    int TwoPassMergeSort(size_t start_block, vector<size_t> block_sizes, size_t num_of_blocks_to_merge, size_t start_record, size_t end_record);
    long GetNumRecords();
    size_t GetBufferSize();

    void perror(int x);
};

template <typename Rec>
FileSorter<Rec>::FileSorter(string &inFile, string &outFile, int amt_of_mem)
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

    m_lnrecords = CountRecords(inFile);
}

template <typename Rec>
FileSorter<Rec>::~FileSorter()
{
    // Close input and output files if they are open
    if (m_h_inpfile)
        fclose(m_h_inpfile);
    if (m_h_outfile)
        fclose(m_h_outfile);
}

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

template <typename Rec>
Rec FileSorter<Rec>::ReadRecord(size_t index)
{
    fseek(m_h_inpfile, index * sizeof(Rec), SEEK_SET);
    Rec record(m_h_inpfile);
    return record;
}

template <typename Rec>
void FileSorter<Rec>::WriteRecord(size_t index, Rec value)
{
    fseek(m_h_outfile, index * sizeof(Rec), SEEK_SET);
    fwrite(&value, sizeof(Rec), 1, m_h_outfile);
}

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

template <typename Rec>
RecWithBlockIndex<Rec> FileSorter<Rec>::CreateRecWithBlockIndex(const Rec &value, size_t index)
{
    RecWithBlockIndex<Rec> r;
    r.value = value;
    r.index = index;
    return r;
}

template <typename Rec>
size_t FileSorter<Rec>::GetBufferSize()
{
    return static_cast<size_t>(m_i_amt_of_mem) * 1024 * 1024 / sizeof(Rec);
}

template <typename Rec>
int FileSorter<Rec>::TwoPassMergeSort(long i, long j)
{
    vector<Rec> buffer(GetBufferSize());

    fseek(m_h_inpfile, i * sizeof(Rec), SEEK_SET);

    long num_of_records = j - i + 1;

    long records_read = fread(&buffer[0], sizeof(Rec), num_of_records, m_h_inpfile);
    if (records_read <= 0)
    {
        perror(-2);
        return -1;
    }

    if (sorting_order == 1)
    {
        // sort in ascending order
        sort(buffer.begin(), buffer.begin() + records_read);
    }
    else
    {
        // sort in descending order
        sort(buffer.begin(), buffer.begin() + records_read, greater<Rec>());
    }

    fseek(m_h_outfile, i * sizeof(Rec), SEEK_SET);
    fwrite(&buffer[0], sizeof(Rec), records_read, m_h_outfile);

    return 1;
}

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
    else if (num_of_blocks_to_merge == 1)
    {
        for (size_t i = start_record; i < end_record; i++)
        {
            Rec record = ReadRecord(i);
            WriteRecord(i, record);
        }
        return 1;
    }

    Buffer<RecWithBlockIndex<Rec>> buffer(num_of_blocks_to_merge, sorting_order);

    size_t current_block_indices[block_sizes.size()] = {0};
    size_t record_index = start_record;

    // Populate the buffer with the first record on each block
    for (size_t i = 0; i < num_of_blocks_to_merge; i++)
    {
        Rec record = ReadRecord(record_index);
        RecWithBlockIndex<Rec> record_with_block_index = CreateRecWithBlockIndex(record, i + start_block);
        current_block_indices[i + start_block]++;

        if (!buffer.push(record_with_block_index))
        {
            perror(-3);
            return -1;
        }
        record_index += block_sizes[i + start_block];
    }

    size_t current_record = start_record;
    while (current_record < end_record && !buffer.empty())
    {
        RecWithBlockIndex<Rec> r = buffer.top();
        WriteRecord(current_record, r.value);
        buffer.pop();
        current_record++;

        size_t block_index = r.index;
        size_t current_block_index = current_block_indices[block_index];

        if (current_block_index < block_sizes[block_index])
        {
            size_t r_index = GetRecordIndex(start_block, block_index, block_sizes, start_record, current_block_index);
            Rec record = ReadRecord(r_index);
            RecWithBlockIndex<Rec> record_with_block_index = CreateRecWithBlockIndex(record, block_index);
            if (!buffer.push(record_with_block_index))
            {
                perror(-3);
                return -1;
            }
            current_block_indices[block_index]++;
        }
    }

    return 1;
}

template <typename Rec>
long FileSorter<Rec>::GetNumRecords()
{
    return m_lnrecords;
}

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
    default:
        cout << "Unknown error code: " << x << endl;
    }
}

#endif