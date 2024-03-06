// All File IO must be performed using stdio.h

using namespace std;

template < unsigned SizeOfRec >
class Record {
  private:
       char m_chdata[SizeOfRec];  // Data for record contained here

  public:
       Record();
       ~Record();
       long size();
/* 
   Other comments for the class
   ----------------------------
   The class must implement a copy constructor.
   The class must implement a operator[] for the data in the class
   The class must implement an assignment operator.

*/
};

template < unsigned SizeOfRec >
bool operator<(const Record &  r1,const Record &  r2){
  //  Put your comparison function here.
  return true;
};


// Total amount of hard drive space used should not exceed twice the
// size of the input file.

// Total amount of memory used should not exceed the amount input by
// the user. An error should occur if you cannot sort in the amount
// of memory the user inputs.

template < typename Rec > 
class FileSorter {
       FILE *m_h_inpfile;   // handle to input file
       FILE *m_h_outfile;   // handle to output file
       long m_lnrecords;    // Number of records in file.
       int  m_i_amt_of_mem;

  public:
       FileSorter(
          string& inFile,   // Name of unsorted heapfile.
          string& outFile,  // Name of sorted heapfile.
          int amt_of_mem    // Amount of memory in MB available for sorting
                            // Your program should not allocate a total of more than amt_of_mem MB memory at any
                            // given time. In testing, the input file size will always exceed amt_of_mem MB.
       );
       ~FileSorter();       // destructor 

       int TwoPassMergeSort(long i, long j); // Only sorts records i to j.
       int TwoPassMergeSort();               // Sorts the whole file.
       // Return value for sorters should correspond to :
       // Status of call: Should return 1 if sorted, -x for an error. You should say what -x is, when you code.
       // For instance -1 = No disk space left
       //              -2 = File IO error
       //              ...

       // The following function should print a verbose output of what
       // went wrong when sorting was attempted
       void perror(int x);

};


// Main function: should be short!
int main(int argc, char **argv){

  return 0;
}

