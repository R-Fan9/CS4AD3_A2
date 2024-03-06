#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <cstring>

using namespace std;

extern long KEY_SIZE;
extern long SIZE_OF_REC;

class Record
{
private:
    char *m_chdata;

public:
    // Constructor
    Record() : m_chdata(new char[SIZE_OF_REC])
    {
        // Initialize m_chdata with zeros
        memset(m_chdata, 0, SIZE_OF_REC);
    }

    Record(FILE *file) : m_chdata(new char[SIZE_OF_REC])
    {
        fread(m_chdata, 1, SIZE_OF_REC, file);
    }

    // Destructor
    ~Record()
    {
        delete[] m_chdata;
    }

    // Copy constructor
    Record(const Record &other) : m_chdata(new char[SIZE_OF_REC])
    {
        // Copy data from other object
        memcpy(m_chdata, other.m_chdata, SIZE_OF_REC);
    }

    // Assignment operator
    Record &operator=(const Record &other)
    {
        // Check for self-assignment
        if (this != &other)
        {
            delete[] m_chdata;
            m_chdata = new char[SIZE_OF_REC];
            memcpy(m_chdata, other.m_chdata, SIZE_OF_REC);
        }
        return *this;
    }

    // Operator[] for accessing data
    const char &operator[](size_t index) const
    {
        return m_chdata[index];
    }

    const char *data() const
    {
        return m_chdata;
    }
};

bool operator==(const Record &r1, const Record &r2)
{
    for (long i = 0; i < KEY_SIZE; ++i)
    {
        if (r1[i] != r2[i])
        {
            return false;
        }
    }
    return true;
}

bool operator<(const Record &r1, const Record &r2)
{
    // Compare the keys of the two records lexicographically
    for (long i = 0; i < KEY_SIZE; i++)
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

bool operator>(const Record &r1, const Record &r2)
{
    return !(r1 < r2 || r1 == r2);
};

#endif