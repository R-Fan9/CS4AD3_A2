


/*-----------------------------------------------------------------------------------*\
For testing purposes only. If you want to modify, read the entire code before you do.
\*-----------------------------------------------------------------------------------*/
#define KEYLEN 8 
#define RECLEN 100


/***************************************************************************
 ***************************************************************************

    Creator: Tom Barclay MSR

    Date: 1/17/96

    Copyright (c) 1995, 1996, 1998 Microsoft Corporation, All Right Reserved

****************************************************************************
****************************************************************************/

/*
    SortGen.EXE :  Sort Benchmark data generator

    This is a console application.
    It generates 100 byte records.
        The first 10 bytes is a random key of 10 printable characters.
        The next 10 bytes is the record number,0,1,... with leading blanks.
         The remaining bytes contain varying fields of
            "AAAAAAAAAA", "BBBBBBBBBB", ... "ZZZZZZZZZZ".
            The series repeats until it fills the record.
            The next record picks up where we left off on the previous record.

    This is the command syntax:

        SortGen.exe NumberOfRecordsToGenerate  OutputFileName

    Example generating 100mb

        SortGen.exe 1000000 sort100mb.dat

    Modification History:
    JNG         04/29/98    Modified Barclay's code to use stdio.h.
    Peng Liu    12/07/02    Modified Barclay's code to generate 100-bytes records instead of 101-bytes records.

    LIMITATIONS:  Program works up to 2^31 records (about 200GB),
       before 31 bit record counter overflows.
       We hope this will be a problem someday.
*/

//
//  INCLUDE FILES
//

# include   <stdio.h>
# include   <stdlib.h>
# include   <time.h>

#define  VERSION    "V1.1-4"



#define TXTLEN (RECLEN-KEYLEN-10)

/*************************************************************************
`* format of record: 10 bytes of key,
 *                   10 bytes of serial number, 
 *                   80 bytes of data  
 *************************************************************************/
struct record
{ 
    char    sortkey[KEYLEN] ;
    char    recnum[10];
    char    txtfld[TXTLEN];
};
//
// GLOBALS
//
const   int RecordSize          = (sizeof(struct record));  // 100 bytes
const   int RecordsPerBuffer    = 1<<16;            // 64 K records per buffer
const   int BufferSize          = RECLEN * (1<<16);    // about 512KB of IO at a time.

int     MaxRecord       =   1;                  // # records to generate
char    *OutFileName    =   "input.dat";        // Name of file to create
 
/*************************************************************************
 *                                                                       *
 * My random number generator (I trust it).                              *
 *                                                                       *
 *************************************************************************/
unsigned int my_rand(void)
    {
      // static unsigned int     randx;
    unsigned int x = rand();
    // return (randx = randx * ((unsigned int)3141592621) + ((unsigned int)663896637));
    return x;
    }

/*************************************************************************
 *                                                                       *
 * Generate a random key of 10 printable characters                      *
 *                                                                       *
 *************************************************************************/
void rand_key(char key[KEYLEN])
{
    unsigned    temp;

    // generate a random key consisting of 95 ascii values, ' ' to '~'
    temp = my_rand();
    temp /= 52;                             // filter out lower order bits
    key[3] = ' ' + (temp % 95);
    temp /= 95;
    key[2] = ' ' + (temp % 95);       
    temp /= 95;
    key[1] = ' ' + (temp % 95);
    temp /= 95;
    key[0] = ' ' + (temp % 95);
    
    temp = my_rand();
    temp /= 52;                             // filter out lower order bits 
    key[7] = ' ' + (temp % 95);
    temp /= 95;
    key[6] = ' ' + (temp % 95);
    temp /= 95;
    key[5] = ' ' + (temp % 95);
    temp /= 95;
    key[4] = ' ' + (temp % 95);
    
}

/*************************************************************************
 *                                                                       *
 * Generate a random record.                                             *
 *                                                                       *
 *************************************************************************/
void gen_rec(struct record *rp)
{
    static      int current = 0;
    char        *sptr;
    static char nxtchar = 'A';
    int         i;
    
    rand_key(rp->sortkey) ;                 // Get 10 byte sort key     
    sprintf(rp->recnum, "%10d", current++);
    sptr = rp->txtfld ;
    for( i=0; i < TXTLEN; i++)
       *sptr++ = nxtchar ;
        nxtchar++;
        if( nxtchar > 'Z' ) nxtchar = 'A';

    sptr[-2] = '\r';
    sptr[-1] = '\n';
}

/*************************************************************************
 *                                                                       *
 * Fill the output buffer with records                                   *
 * Buffer holds an integer number of whole records                       *
 *  (buffer size is a multiple of record size)                           *
 *                                                                       *
 *************************************************************************/
int FillBuffer(char *Buffer, int BufferSize, long Record, long MaxRecord)
    {   
    int RecordCount = 0;
    do  {
        gen_rec((struct record *) &Buffer[RecordCount*RecordSize]);
        Record++;
        RecordCount++;
        }while ((RecordCount < RecordsPerBuffer) && (Record < MaxRecord));

    return (RecordCount);
}
 
/*************************************************************************
 *                                                                       *
 * Online manual                                                         *
 *                                                                       *
 *************************************************************************/
void usage()
{
    fprintf(stderr,"Usage: SortGen <#rows> <filename> \n");
    exit(1);
}

/*************************************************************************
 *                                                                       *
 * Main routine of data generator                                        *
 *                                                                       *
 *************************************************************************/
 
int main (int argc, char * argv[])
{
    long        GeneratedRecords = 0;       // Records generated so far
    long        DesiredRecords   = 0;       // Target number of records
    long        BufferRecords    = 0;       // Records in buffer
    char        *Buffer  ;                  // output buffer    
    long        WriteCount       = 0;       // bytes written by this IO  
    long        error            = 0;       // error flag
    FILE        * OutFile ;                 // Output file
    
    /*********************************************************************
     *                                                                   *
     * Decode parameters (record count and file name)                    *
     *                                                                   *
     *********************************************************************/
    srand(time(NULL));
    if (argc < 2)
        { usage(); error = 1; goto common_exit; }
    argc--; /* Don't count program name */
    argv++;
    if (argc > 0)
        {
        DesiredRecords = atoi(argv[0]);
        argc--;
        argv++;
        }
    if (argc > 0)
        {
        OutFileName = argv[0];
        argc--;
        argv++;
        }
    /************************************************************************
     *                                                                      *
     * Put out greeting                                                     *
     *                                                                      *
     ************************************************************************/
    fprintf(stderr,"SortGen - Sort Data Generator version %s\n", VERSION);
    fprintf(stderr,"\tGenerating %d sort test data records to file %s \n",
                                                DesiredRecords, OutFileName);

    /************************************************************************/
    /*                                                                      */
    /*      Open Output File and allocate output buffer                     */
    /*                                                                      */
    /************************************************************************/
    Buffer = (char *)malloc( BufferSize ) ;
    if( Buffer  == NULL  )
        {
        fprintf(stderr,"? Couldn't allocate the I/O buffer\n");
        error = 1;  goto common_exit;
        }

    OutFile = fopen( OutFileName, "w+b");
    if( OutFile == NULL )
        {
        fprintf(stderr,"? Error creating file %s\n", OutFileName) ;
        error = 1;  goto common_exit;
        }

    /************************************************************************/
    /*                                                                      */
    /*      Main Loop                                                       */
    /*      Fill the buffer write it out                                    */
    /*                                                                      */
    /************************************************************************/
    while( GeneratedRecords < DesiredRecords)
        {
        BufferRecords = FillBuffer(Buffer, BufferSize, GeneratedRecords, DesiredRecords);
        WriteCount = fwrite( Buffer, 1, (BufferRecords * RecordSize), OutFile);
        if( WriteCount != (BufferRecords * RecordSize))
            {fprintf(stderr,"? Error %d on write to outfile.\n",ferror(OutFile));
            error = 1; goto common_exit;
            }
        GeneratedRecords += BufferRecords ;
        }

    /************************************************************************/
    /*                                                                      */
    /*      All Done                                                        */
    /*      close output file; and exit                                     */
    /*                                                                      */
    /************************************************************************/
common_exit:
    fprintf(stderr, "\nCompleted writing %d Records to file %s\n",
                                                DesiredRecords,   OutFileName) ;
    fclose(OutFile) ;                   // close output file         
    free(Buffer);                       // free record buffer
    if (error)                          // delete output file on error
        remove( OutFileName);           //
    return(error);                      // return status code
}                                       // End of SortGen.c
