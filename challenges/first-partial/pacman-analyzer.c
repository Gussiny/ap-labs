#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REPORT_FILE "packages_report.txt"

struct Node
{
    char *packageName;
    char *installDate;
    char *lastUpdateDate;
    int numUpdates;
    char *removalDate;
    struct Node *next;
};

void analizeLog(char *logFile, char *report);
char *readLine(int file, int b_size);
char *getWord(char *str, int start, int end);
void addElement(struct Node **head, char *name, char *date, int type);
void printList(struct Node *node);
void writeList(int writeFile, struct Node *node);

size_t read(int fd, void *buf, size_t cnt);
size_t write (int fd, void* buf, size_t cnt); 
int close(int fd);

int main(int argc, char **argv)
{

    if (argc < 4)
    {
        printf("Usage:./pacman-analizer.o \n");
        return 0;
    }

    analizeLog(argv[2], argv[4]);

    return 0;
}

void analizeLog(char *logFile, char *report)
{
    printf("Generating Report from: [%s] log file\n", logFile);

    // Implement your solution here.

    //  LinkedList head initializer
    struct Node *head = NULL;

    //  Packages Counters
    int installedPackages = 0, removedPackages = 0, upgradePackages = 0, currentInstalled = 0;

    //  Open file to readOnly
    int fileOpen = open(logFile, O_RDONLY);
    if (fileOpen == -1)
    {
        printf("Error while opening log file\n");
        return;
    }

    char *line;
    int b_size = 2;

    //  READ LINE BY LINE
    while ((line = readLine(fileOpen, b_size)) != NULL && (strcmp("\0", line)) != 0)
    {
        //  INITIALIZE POINTERS AND FLAGS
        int dateIn = -1, dateOut = -1;
        int instIn = -1, instOut = -1;
        int nameIn = -1, nameOut = -1;
        int dateCreated = 0;
        int instCreated = 0;
        int typeChecked = 0;
        int nameChecked = 0;
        int type = 0;
        char *date;
        char *inst;
        char *name;

        for (int x = 0; x < strlen(line); x++)
        {
            //   GET DATE
            if (line[x] == '[' && dateIn == -1)
            {
                //  DATE BEGIN
                dateIn = x + 1;
            }
            else if (line[x] == ']' && dateIn != 0 && dateOut == -1)
            {
                //  DATE END
                dateOut = x;
                date = getWord(line, dateIn, dateOut);
                dateCreated = 1;
            }

            //   GET INSTRUCTION
            else if (line[x] == '[' && instIn == -1 && dateCreated == 1)
            {
                //  INSTRUCION BEGIN
                instIn = x + 1;
            }
            else if (line[x] == ']' && instIn != 0 && instOut == -1)
            {
                //  INSTRUCTION END
                instOut = x;
                inst = getWord(line, instIn, instOut);
                instCreated = 1;
            }

            //  EXCLUDE OTHERS THAN ALPM
            else if (line[x] != ' ' && dateCreated == 1 && instCreated == 1 && typeChecked == 0)
            {
                if (strcmp(inst, "ALPM") != 0)
                {
                    continue;
                }

                typeChecked = 1;
                //   GET OPERATION TYPE
                if (line[x] == 'i')
                {
                    //  INSTALL
                    installedPackages++;
                    type = 1;
                }
                else if (line[x] == 'u')
                {
                    //  UPGRADE
                    upgradePackages++;
                    type = 2;
                }
                else if (line[x] == 'r' && line[x + 1] == 'e' && line[x + 2] == 'm')
                {
                    //  REMOVE
                    removedPackages++;
                    type = 3;
                }
                else
                {
                    //  IGNORE LINE
                    continue;
                }
            }

            // GET PACKAGE NAME
            else if (line[x] == ' ' && nameIn == -1 && dateCreated == 1 && instCreated && typeChecked != 0)
            {
                //  NAME BEGIN
                nameIn = x + 1;
            }
            else if (line[x] == ' ' && nameIn != 0 && nameOut == -1 && dateCreated == 1 && instCreated && typeChecked != 0)
            {
                //  NAME END
                nameOut = x;
                name = getWord(line, nameIn, nameOut);
                nameChecked = 1;

                //  REGISTER PACKAGE
                addElement(&head, name, date, type);
                continue;
            }
        }
    }

    //  CLOSE LOG FILE
    if(close(fileOpen) < 0){
        printf("Error while closing log file\n");
        return;
    }

    //  OPEN REPORT FILE TO WRITEONLY
    int reportFile = open(report, O_WRONLY | O_CREAT , 0644);
    if(reportFile < 0)
    {
        printf("Error while opening report file\n");
        return;
    }


    currentInstalled = installedPackages - removedPackages;    

    //  WRITE DATA ON FILE
    write(reportFile, "Pacman Packages Report\n", strlen("Pacman Packages Report\n"));
    write(reportFile, "----------------------\n", strlen("----------------------\n"));
    
    //  WRITE PACMAN PACKAGES REPORT
    int size_b = snprintf(NULL, 0, 
    "- Installed packages \t: %d\n"
    "- Removed packages \t: %d\n"
    "- Upgraded packages \t: %d\n"
    "- Current installed \t: %d\n\n", 
    installedPackages, removedPackages, upgradePackages, currentInstalled);

    char *buf = (char *)malloc(size_b + 1);
    if(buf==NULL){
        printf("Error while writing, out of memory");
        return;
    }

    snprintf(buf, size_b + 1, 
    "- Installed packages \t: %d\n"
    "- Removed packages \t: %d\n"
    "- Upgraded packages \t: %d\n"
    "- Current installed \t: %d\n\n", 
    installedPackages, removedPackages, upgradePackages, currentInstalled);
    write(reportFile, buf, size_b);
    free(buf);

    //  WRITE LIST OF PACKAGES
    write(reportFile, "List of packages\n", strlen("List of packages\n"));
    write(reportFile, "----------------\n", strlen("----------------\n"));
    writeList(reportFile, head);
       

    //  CLOSE LOG FILE
    if(close(reportFile) < 0){
        printf("Error while closing report file\n");
        return;
    }

    //  PRINT TOTALS
    printf("Pacman Packages Report\n");
    printf("----------------------\n");
    printf("- Installed packages : %d\n", installedPackages);
    printf("- Removed packages   : %d\n", removedPackages);
    printf("- Upgraded packages  : %d\n", upgradePackages);
    printf("- Current installed  : %d\n\n", currentInstalled);

    printf("List of packages\n");
    printf("----------------\n");
    printList(head);
}

char *readLine(int file, int b_size)
{
    char buffer[b_size];
    int bytesReaded;
    int size = 1;
    char *lineReaded = malloc(size);
    strcpy(lineReaded, "");
    if (lineReaded == NULL)
    {
        return NULL;
    }
    while ((bytesReaded = read(file, buffer, 1)) > 0)
    {
        buffer[1] = '\0';
        if (buffer[0] != '\n' && buffer[0] != '\r')
        {
            char *readed = malloc(size + 1);
            strcpy(readed, lineReaded);
            free(lineReaded);
            lineReaded = readed;
            strcat(lineReaded, buffer);
            size++;
        }
        else
        {
            strcat(lineReaded, "\0");
            return lineReaded;
        }
    }
    if (bytesReaded == -1)
    {
        return NULL;
    }

    return "\0";
}

char *getWord(char *str, int start, int end)
{
    //  RESERVE MEMORY
    char *data = malloc(end - start + 1);
    if (data == NULL)
    {
        return NULL;
    }

    //  SPLIT LINE STRING
    for (int i = 0; i < end - start; i++)
    {
        data[i] = str[i + start];
    }

    //  INSERT END OF LINE
    data[end - start] = '\0';

    //  RETURN WORD SPLITTED
    return data;
}

void addElement(struct Node **head, char *name, char *date, int type)
{
    //  RESERVE MEMORY FOR A NEW NODE
    struct Node *newElement = (struct Node *)malloc(sizeof(struct Node));

    //  POINT LAST NODE TO THE HEAD
    struct Node *last = *head;

    //  SAVE VALUES INTO THE NEW NODE
    newElement->packageName = name;
    newElement->installDate = date;
    newElement->lastUpdateDate = "-";
    newElement->removalDate = "-";
    newElement->numUpdates = 0;
    newElement->next = NULL;


    //  EMPTY LIST  
    if (*head == NULL)
    {
        //  CREATE NEW HEAD
        *head = newElement;
        return;
    }

    //  MOVE THROUG THE LINKED LIST
    while (last->next != NULL)
    {

        //  IF PACKAGE EXISTS
        if (strcmp(last->packageName, name) == 0)
        {
            //  INSTALL
            if (type == 1)
            {
                last->installDate = date;
            }
            //  UPDATE
            else if (type == 2)
            {
                last->lastUpdateDate = date;
                last->numUpdates++;
            }
            //  REMOVE
            else if (type == 3)
            {
                last->removalDate = date;
            }
            return;
        }
        last = last->next;
    }

    //  IF PACKAGE DOESN'T EXISTS
    last->next = newElement;
    return;
}

void writeList(int writeFile, struct Node *node){
    //  TRAVERSE TILL LAST NODE
    while (node != NULL)
    {
        int size = snprintf(NULL, 0, 
        "- Package Name \t: %s\n"
        "  - Install date \t: %s\n"
        "  - Last update date \t: %s\n"
        "  - How many updates \t: %d\n"
        "  - Removal date \t: %s\n", 
        node->packageName, node->installDate, node->lastUpdateDate, node->numUpdates, node->removalDate);

        char *buf = (char *)malloc(size + 1);
        snprintf(buf, size + 1, 
        "- Package Name \t: %s\n"
        "  - Install date \t: %s\n"
        "  - Last update date \t: %s\n"
        "  - How many updates \t: %d\n"
        "  - Removal date \t: %s\n", 
        node->packageName, node->installDate, node->lastUpdateDate, node->numUpdates, node->removalDate);

        write(writeFile, buf, size);
        free(buf);
        node = node->next;
    }
}

void printList(struct Node *node)
{
    //  TRAVERSE TILL LAST NODE
    while (node != NULL)
    {
        printf("- Package Name\t: %s\n", node->packageName);
        printf("  - Install date\t: %s\n", node->installDate);
        printf("  - Last update date\t: %s\n", node->lastUpdateDate);
        printf("  - How many updates\t: %d\n", node->numUpdates);
        printf("  - Removal date\t: %s\n", node->removalDate);
        node = node->next;
    }
}
