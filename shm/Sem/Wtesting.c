#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <semaphore.h>

#include "number.h"

#define NUMBERARRAYLENGTH 9 

static int  shm_fd = -1;
char* shm_fixedName = "testSHM";
char* sem_fixedName = "testSEM";

// static char * 
// remove_nl (char * s)
// {
//     int len;
    
//     len = strlen (s);
//     if (s [len - 1] == '\n')
//     {
//         s [len - 1] = '\0';
//     }
    
//     return (s);
// }

number createNr(int value, char* pronunciation)
{
    number newNr;
    newNr.value = value;
    newNr.pronunciation = pronunciation;
    newNr.structSem = SEM_FAILED;

    return newNr;
}

void createStructs(number numberArray[])
{
    numberArray[0] = createNr(1, "Ace");
    numberArray[1] = createNr(2, "Deuce");
    numberArray[2] = createNr(3, "Trey");
    numberArray[3] = createNr(4, "Cater");
    numberArray[4] = createNr(5, "Cinque");
    numberArray[5] = createNr(6, "Sice");
    numberArray[6] = createNr(7, "Seven");
    numberArray[7] = createNr(8, "Eight");
    numberArray[8] = createNr(9, "Nine");
}

static char *
my_shm_create (char * shm_name, int size)
{
    int     rtnval;
    char *  shm_addr;
    
    printf ("Calling shm_open('%s')\n", shm_name);
    shm_fd = shm_open (shm_name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd == -1)
    {
        perror ("ERROR: shm_open() failed");
    }
    printf ("shm_open() returned %d\n", shm_fd);
                
    printf ("Calling ftrucate(%d,%d)\n", shm_fd, size);
    rtnval = ftruncate (shm_fd, size);
    if (rtnval != 0)
    {
        perror ("ERROR: ftruncate() failed");
    }
    printf ("ftruncate() returned %d\n", rtnval);
                
    printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }
    printf ("mmap() returned %p\n", shm_addr);

    return (shm_addr);
}

static char *
my_shm_open (char * shm_name)
{
    int     size;
    char *  shm_addr;
    
    printf ("Calling shm_open('%s')\n", shm_name);
    shm_fd = shm_open (shm_name, O_RDWR, 0600);
    if (shm_fd == -1)
    {
        perror ("ERROR: shm_open() failed");
    }
    printf ("shm_open() returned %d\n", shm_fd);
                
    printf ("Calling lseek(fd=%d,SEEK_END)\n", shm_fd);
    size = lseek (shm_fd, 0, SEEK_END);
    printf ("lseek() returned %d\n", size);
                
    printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }
    printf ("mmap() returned %p\n", shm_addr);

    return (shm_addr);
}

void
my_sem_open (number receivedNr)
{
    if (receivedNr.structSem != SEM_FAILED)
    {
        printf ("ERROR: another semaphore already opened\n");
        break;
    }

    printf("Calling sem_open on semaphore with name %s", sem_fixedName);
    receivedNr.structSem = sem_open(sem_fixedName, O_CREAT | O_EXCL, 0600, 1);

    if (receivedNr.structSem == SEM_FAILED)
    {
        perror("ERROR: sem_open() failed");
    }
    printf("sem_open() returned %p\n", receivedNr.structSem);
    break;
}

int main(void)
{
    char *      shm_addr = (char *) MAP_FAILED;
    char        line[80];
    int         choice = ' ';
    int         numberStructSize = sizeof(number);
    int         numberArraySize = NUMBERARRAYLENGTH * numberStructSize;
    int         rtnval;

    while (choice != 'q')
    {
        printf ("\nEnter choice (n,o,r,w,c,u,h=help,q): ");
        fgets (line, sizeof (line), stdin);
        choice = line[0];
        printf ("\n");
        
        switch (choice)
        {
            case 'n':
                printf("Using name: %s\n", shm_fixedName);
                printf("Using size of array of numbers, which is: %d\n", numberArraySize);
                
                shm_addr = my_shm_create (shm_fixedName, numberArraySize);

                break;
            case 'o':
                printf("Using name: %s\n", shm_fixedName);

                shm_addr = my_shm_open (shm_fixedName);

                printf("Opening number's semaphore.\n");
                my_shm_open()

                break;
            case 'w':
                printf("Commencing endless write.\n");

                number numberArray[9];
                createStructs(numberArray);
                number* shm_loc = (number*)shm_addr;

                while(1)
                {
                    // This continues forever.
                    for(int i = 0; i < NUMBERARRAYLENGTH; i++)
                    {
                        // Write the numbers in the struct to the shared memory one by one.
                        // fgets  (shm_addr, numberStructSize, numberArray[i]);
                        *shm_loc = numberArray[i];
                    }
                }
                
            case 'r':
                printf("Commencing endless read.\n");
                printf("ERROR: READ NOT IMPLEMENTED, NEED SEMAPHORES!\n");
                break;
            case 'c':
                printf ("Calling close(%#x)\n", shm_fd);
                rtnval = close (shm_fd);
                if (rtnval != 0)
                {
                    perror ("ERROR: close() failed");
                }
                printf ("close() returned %d\n", rtnval);
                shm_fd = -1;
                break;
            case 'u':
                printf("Using name: %s\n", shm_fixedName);
                printf ("Calling shm_unlink('%s')\n", shm_fixedName);
                rtnval = shm_unlink (shm_fixedName);
                if (rtnval != 0)
                {
                    perror ("ERROR: shm_unlink() failed");
                }
                printf ("shm_unlink() returned %d\n", rtnval);
                break;
            case 'q':
                break;
            case 'h':
            case '?':
                printf ("Options:\n"
                        "========\n"
                        "    [n]  create new shm\n"
                        "    [o]  open existing shm\n"
                        "    [w]  write\n"
                        "    [r]  read\n"
                        "    [c]  close\n"
                        "    [u]  unlink\n"
                        "    [h]  help\n"
                        "    [q]  quit\n");
                break;
            default:
                printf ("Invalid option '%c' (%#x)\n", choice, choice);
                break;
        }
    }
    return (0);
}