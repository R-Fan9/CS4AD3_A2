#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <cstring>

using namespace std;

extern long key_size;

template <unsigned SizeOfRec>
class Record
{
private:
    char m_chdata[SizeOfRec]; // Data for record contained here

public:
    // Constructor
    Record()
    {
        // Initialize m_chdata with zeros
        memset(m_chdata, 0, SizeOfRec);
    }

    Record(FILE *file)
    {
        fread(m_chdata, 1, SizeOfRec, file);
    }

    // Destructor
    ~Record() {}

    // Copy constructor
    Record(const Record &other)
    {
        // Copy data from other object
        memcpy(m_chdata, other.m_chdata, SizeOfRec);
    }

    // Assignment operator
    Record &operator=(const Record &other)
    {
        // Check for self-assignment
        if (this != &other)
        {
            // Copy data from other object
            memcpy(m_chdata, other.m_chdata, SizeOfRec);
        }
        return *this;
    }

    // Size function
    long size()
    {
        return SizeOfRec;
    }

    // Operator[] for accessing data
    const char &operator[](size_t index) const
    {
        return m_chdata[index];
    }
};

template <unsigned SizeOfRec>
bool operator==(const Record<SizeOfRec> &r1, const Record<SizeOfRec> &r2)
{
    for (long i = 0; i < key_size; ++i)
    {
        if (r1[i] != r2[i])
        {
            return false;
        }
    }
    return true;
}

template <unsigned SizeOfRec>
bool operator<(const Record<SizeOfRec> &r1, const Record<SizeOfRec> &r2)
{
    // Compare the keys of the two records lexicographically
    for (long i = 0; i < key_size; i++)
    {
        if (r1[i] < r2[i])
        {
            return true;
        }
        else if (r1[i] > r2[i])
        {
            return false;
        }
    }

    // Two records are equal
    return false;
};

template <unsigned SizeOfRec>
bool operator>(const Record<SizeOfRec> &r1, const Record<SizeOfRec> &r2)
{
    return !(r1 < r2 || r1 == r2);
};

#endif