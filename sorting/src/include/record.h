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

    // Constructs a Record object by reading data from a file.
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

    /**
     * @brief Assignment operator for Record objects.
     *
     * @param other The Record object to copy data from.
     * @return A reference to the current Record object after assignment.
     */
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

    /**
     * @brief Accesses the record data at the specified index.
     *
     * @param index The index of the data element to access.
     * @return A const reference to the data element at the specified index.
     */
    const char &operator[](size_t index) const
    {
        return m_chdata[index];
    }

    /**
     * @brief Returns a pointer to the raw data of the Record object.
     *
     * @return A const pointer to the raw data of the Record object.
     */
    const char *data() const
    {
        return m_chdata;
    }
};

/**
 * @brief Checks if two records are equal.
 *
 * This function compares the keys of two Record objects element-wise.
 * It returns true if all elements are equal, otherwise false.
 *
 * @param r1 The first Record object to compare.
 * @param r2 The second Record object to compare.
 * @return True if the records are equal, otherwise false.
 */
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

/**
 * @brief Compares two records lexicographically.
 *
 * This function compares the keys of two Record objects element-wise.
 * It returns true if the first record is less than the second record, otherwise false.
 *
 * @param r1 The first Record object to compare.
 * @param r2 The second Record object to compare.
 * @return True if the first record is less than the second record, otherwise false.
 */
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

/**
 * @brief Compares two records lexicographically.
 *
 * This function compares the keys of two Record objects element-wise.
 * It returns true if the first record is greater than the second record, otherwise false.
 *
 * @param r1 The first Record object to compare.
 * @param r2 The second Record object to compare.
 * @return True if the first record is greater than the second record, otherwise false.
 */
bool operator>(const Record &r1, const Record &r2)
{
    return !(r1 < r2 || r1 == r2);
};

#endif