#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

static int  shm_fd = -1;

static char * 
remove_nl (char * s)
{
    int len;
    
    len = strlen (s);
    if (s [len - 1] == '\n')
    {
        s [len - 1] = '\0';
    }
    
    return (s);
}

static char *
my_shm_create (char * shm_name, int size)                                                       // Creates and returns a shared memory segment
{
    int     rtnval;
    char *  shm_addr;                                                                           // The adress of the shared memory
    
    printf ("Calling shm_open('%s')\n", shm_name);
    shm_fd = shm_open (shm_name, O_CREAT | O_EXCL | O_RDWR, 0600);                              // Open an new shared memory segment with name shm_name and size size
    if (shm_fd == -1)                                                                           // On succes return 0, error -1
    {
        perror ("ERROR: shm_open() failed");
    }
    printf ("shm_open() returned %d\n", shm_fd);
                
    printf ("Calling ftrucate(%d,%d)\n", shm_fd, size);
    rtnval = ftruncate (shm_fd, size);                                                          // Gives size size to the file directory
    if (rtnval != 0)
    {
        perror ("ERROR: ftruncate() failed");
    }
    printf ("ftruncate() returned %d\n", rtnval);
                
    printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);       // locates the address of the file and puts it into shm_addr
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
    shm_fd = shm_open (shm_name, O_RDWR, 0600);                                                 // Claims the memory and returns 0 on succes, -1 on error
    if (shm_fd == -1)
    {
        perror ("ERROR: shm_open() failed");
    }
    printf ("shm_open() returned %d\n", shm_fd);
                
    printf ("Calling lseek(fd=%d,SEEK_END)\n", shm_fd);
    size = lseek (shm_fd, 0, SEEK_END);                                                         // Seeks the size of the segment
    printf ("lseek() returned %d\n", size);
                
    printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);       // Retreives the address of the memory segment
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }
    printf ("mmap() returned %p\n", shm_addr);

    return (shm_addr);
}

int main(void)
{
    char *      shm_addr = (char *) MAP_FAILED;
    char        line[80];
    char        shm_name[80];
    int         size = -1;
    int         choice = ' ';
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
                printf ("Enter name: ");
                fgets  (shm_name, sizeof (shm_name), stdin);
                remove_nl (shm_name);
                printf ("Enter size: ");
                fgets  (line, sizeof (line), stdin);
                sscanf (line, "%i", &size);
                
                shm_addr = my_shm_create (shm_name, size);                                      // Creates a shared memory segment with shm_name as name and size as size
                break;
            case 'o':
                printf ("Enter name: ");
                fgets  (shm_name, sizeof (shm_name), stdin);
                remove_nl (shm_name);
                
                shm_addr = my_shm_open (shm_name);                                              // Opens the segment witth name shm_name
                break;
            case 'w':
                printf ("Enter date to write @ %#x:\n", (unsigned int) shm_addr);
                fgets  (shm_addr, size, stdin);
                //break; (no break: check written data by reading it again...)
            case 'r':
                printf ("data (@ %#x): '%s'\n", (unsigned int) shm_addr, shm_addr);
                break;
            case 'c':
                printf ("Calling close(%#x)\n", shm_fd);
                rtnval = close (shm_fd);                                                        // Closes the shared memory segment at shm_fd
                if (rtnval != 0)
                {
                    perror ("ERROR: close() failed");
                }
                printf ("close() returned %d\n", rtnval);
                shm_fd = -1;
                break;
            case 'u':
                printf ("Enter name: ");
                fgets  (shm_name, sizeof (shm_name), stdin);
                remove_nl (shm_name);
                printf ("Calling shm_unlink('%s')\n", shm_name);
                rtnval = shm_unlink (shm_name);
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
