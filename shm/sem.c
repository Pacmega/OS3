    /*
    ** These examples use semaphores to ensure that writer and reader
    ** processes have exclusive, alternating access to the shared-memory region.
    */

    /**********  writer.c  ***********/

    #include <stdio.h>
    #include <string.h>
    #include <unistd.h>
    #include <errno.h>
    #include <semaphore.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <sys/fcntl.h>

    // removes newline from char * s
    static char * 
    remove_nl (char * s)
    {
        int len;
        
        len = strlen (s); // read the length of the string
        if (s [len - 1] == '\n') //If last char was \n, change it into \0
        {
            s [len - 1] = '\0';
        }
        
        return (s);
    }


    int main(void)
    {
        sem_t *     semdes = SEM_FAILED;    // Create semaphore
        char        line[80];
        char        sem_name[80];           // This is going to be the name of the semaphore
        int         value;
        int         permissions;
        int         choice = ' ';
        int         rtnval;

        while (choice != 'q')
        {
            printf ("\nMenu\n"
                    "========\n"
                    "   [n]    create new semaphore\n"
                    "   [o]    open existing semaphore\n"
                    "   [p]    post\n"
                    "   [t]    trywait\n"
                    "   [w]    wait\n"
                    "   [g]    getvalue\n"
                    "   [c]    close\n"
                    "   [u]    unlink\n"
                    "   [q]    quit\n"
                    "Enter choice: "
                   );
            fgets (line, sizeof (line), stdin);                                 // read user input for choice
            choice = line[0];
            printf ("\n");
            
            switch (choice)
            {
                case 'n':                                                       // make new semaphore
                    if (semdes != SEM_FAILED)                                   // If a semaphore is already opened, the value of semdes is not SEM_FAILED
                    {
                        printf ("ERROR: another semaphore already opened\n");
                        break;
                    }
                    printf ("Enter name: ");
                    fgets  (sem_name, sizeof (sem_name), stdin);                // Get a name for a new semaphore
                    remove_nl (sem_name);                                       // Remove the newline from the name value

                    printf ("Calling sem_open('%s', O_CREAT | O_EXCL)\n", sem_name);
                    semdes = sem_open (sem_name, O_CREAT | O_EXCL, 0600, 1);    // Semaphore is the value of sem_open
                    
                    if (semdes == SEM_FAILED)                                   
                    {
                        perror ("ERROR: sem_open() failed");
                    }
                    printf ("sem_open() returned %p\n", semdes);                // semaphore succesfully opened, return the value
                    break;
                case 'o':
                    if (semdes != SEM_FAILED)                                   // If the semaphore is already opened, break
                    {
                        printf ("ERROR: another semaphore already opened\n");
                        break;
                    }
                    printf ("Enter name: ");
                    fgets  (sem_name, sizeof (sem_name), stdin);                // Get the userinput for the name of the semaphore
                    remove_nl (sem_name);                                       // Remove the newline from the name parameter

                    printf ("Calling sem_open('%s',%#o)\n", sem_name,0);        
                    semdes = sem_open (sem_name, 0);                            // Use sem_open to open the semaphore with that name
                    
                    if (semdes == SEM_FAILED)                                   // If it fails to open, notify the user
                    {
                        perror ("ERROR: sem_open() failed");
                    }
                    printf ("sem_open() returned %p\n", semdes);
                    break;
                case 'c':
                    if (semdes == SEM_FAILED)                                   // If there is no open semaphore, do nothing
                    {
                        printf ("ERROR: no open semaphore\n");
                        break;
                    }
                    
                    printf ("Calling sem_close(%p)\n", semdes);
                    rtnval = sem_close (semdes);                                // Return value of the sem_close: on succes 0, on error -1
                    if (rtnval != 0)
                    {
                        perror ("ERROR: sem_close() failed");
                    }
                    printf ("sem_close() returned %d\n", rtnval);
                    semdes = SEM_FAILED;                                        // edit the semaphore to be able to create again
                    break;
                case 'p':                                                       // unlock the semaphore so others can write/read
                    if (semdes == SEM_FAILED)
                    {
                        printf ("ERROR: no open semaphore\n");
                        break;
                    }
                    
                    printf ("Calling sem_post(%p)\n", semdes);                  
                    rtnval = sem_post (semdes);                                 // Call sem_post to open up the semaphore: on succes 0, on error -1
                    if (rtnval != 0)
                    {
                        perror ("ERROR: sem_post() failed");
                    }
                    printf ("sem_post() returned %d\n", rtnval);
                    break;
                case 't':                                                       // Try to claim the semaphore. If it isnt currently available, return an error
                    if (semdes == SEM_FAILED)
                    {
                        printf ("ERROR: no open semaphore\n");
                        break;
                    }
                    
                    printf ("Calling sem_trywait(%p)\n", semdes);
                    rtnval = sem_trywait (semdes);                              // If the semaphore is > 0, decrement so you can use it. If it is 0, return an error
                    if (rtnval != 0)
                    {
                        perror ("ERROR/WARNING: sem_trywait() failed");
                    }
                    printf ("sem_trywait() returned %d\n", rtnval);
                    break;  
                case 'w':                                                       // Waits till the semaphore is available to claim
                    if (semdes == SEM_FAILED)
                    {
                        printf ("ERROR: no open semaphore\n");
                        break;
                    }
                    
                    printf ("Calling sem_wait(%p)\n", semdes);
                    rtnval = sem_wait (semdes);                                 
                    if (rtnval != 0)
                    {
                        perror ("ERROR: sem_wait() failed");
                    }
                    printf ("sem_wait() returned %d\n", rtnval);
                    break;
                case 'g':                                                       // If the semaphore isn' t blocked, read the value from the semaphore
                    if (semdes == SEM_FAILED)
                    {
                        printf ("ERROR: no open semaphore\n");
                        break;
                    }
                    printf ("Calling sem_getvalue(%p)\n", semdes);
                    rtnval = sem_getvalue (semdes, &value);
                    if (rtnval != 0)
                    {
                        perror ("ERROR: sem_getvalue() failed");
                    }
                    printf ("sem_getvalue() returned %d; value:%d\n", rtnval, value);
                    break;
                case 'u':                                                       // Removes the semaphore. If other processes/threads are using it, it'll remove itself after the others have closed it.
                    printf ("Enter name: ");
                    fgets  (sem_name, sizeof (sem_name), stdin);                
                    remove_nl (sem_name);
                    printf ("Calling sem_unlink('%s')\n", sem_name);
                    rtnval = sem_unlink (sem_name);
                    if (rtnval != 0)
                    {
                        perror ("ERROR: sem_unlink() failed");
                    }
                    printf ("sem_unlink() returned %d\n", rtnval);
                    break;
                case 'q':                                                       // If the semaphore is removed, close the program
                    if (semdes != SEM_FAILED)
                    {
                        printf ("WARNING: a semaphore still opened\n");
                    }
                    break;
                default:
                    printf ("Invalid option '%c' (%#x)\n", choice, choice);
                    break;
            }
        }
    	return (0);
    }
