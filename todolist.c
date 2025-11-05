#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void display_about();
void display_credit();
void display_current_time();
void add_task();
void view_task();
int search_task();
void delete_task();
void display_menu();

struct todolist
{
    int id;
    char task[1000];
};

int ID = 0;

int main()
{
    system("cls");

    // Load number of tasks (so IDs are consecutive and next ID = count + 1)
    FILE *todolist = fopen("todo.txt", "r");
    if (todolist)
    {
        struct todolist todo;
        int count = 0;
        while (fscanf(todolist, "%d", &todo.id) == 1)
        {
            fgets(todo.task, sizeof(todo.task), todolist);
            count++;
        }
        ID = count; // current number of tasks
        fclose(todolist);
    }

    while (1)
    {
        display_menu();
        int choice;
        printf("\n\n\n\t\t\t\t\t\tEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            // if invalid input, clear stdin and continue
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("\t\t\tInvalid input! Please enter a number.\n");
            continue;
        }

        printf("\n\n\n");

        switch (choice)
        {
        case 1:
            add_task();
            break;
        case 2:
            view_task();
            break;
        case 3:
            search_task();
            break;
        case 4:
            delete_task();
            break;
        case 5:
            display_about();
            break;
        case 6:
            display_credit();
            break;
        case 0:
            printf("Exiting....\n");
            return 0;
        default:
            printf("Invalid choice, please try again.\n");
        }

        display_current_time();
    }
}

// DELETE TASK with reindexing (remaining tasks get new consecutive IDs)
void delete_task()
{
    system("cls");
    int id = search_task();
    if (id != 0)
    {
        char del;
        printf("\n\t\t\tDelete? (y/n): ");
        scanf(" %c", &del);

        if (del == 'y' || del == 'Y')
        {
            struct todolist todo;
            FILE *temp = fopen("temp.txt", "w");
            FILE *todolist = fopen("todo.txt", "r");

            if (!temp || !todolist)
            {
                printf("\t\t\tError: Unable to open file!\n");
                if (temp) fclose(temp);
                if (todolist) fclose(todolist);
                return;
            }

            int index = 1;
            while (fscanf(todolist, "%d", &todo.id) == 1)
            {
                fgets(todo.task, sizeof(todo.task), todolist);
                if (todo.id != id) // keep others but reindex
                {
                    // write reindexed id and the task text (already contains newline from fgets)
                    fprintf(temp, "%d %s", index, todo.task);
                    index++;
                }
            }

            fclose(todolist);
            fclose(temp);

            // replace file
            remove("todo.txt");
            rename("temp.txt", "todo.txt");

            // update ID to number of remaining tasks
            ID = index - 1;

            system("cls");
            printf("\n\t\t\tDeleted Successfully!\n");
        }
        else
        {
            system("cls");
            printf("\t\t\tNot deleted!\n");
        }
    }
}

// SEARCH TASK (shows task and returns id if found)
int search_task()
{
    system("cls");
    printf("\t\t\t======================================================\n");
    printf("\t\t\t\t\tSEARCH TASK\n");
    printf("\t\t\t======================================================\n\n");

    int id;
    printf("\t\t\tEnter Task ID: ");
    if (scanf("%d", &id) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        system("cls");
        printf("\t\t\tInvalid ID input!\n");
        return 0;
    }

    struct todolist todo;
    FILE *todolist = fopen("todo.txt", "r");

    if (!todolist)
    {
        system("cls");
        printf("\t\t\tNo tasks available (File not found)!\n");
        printf("\t\t\t===========================================================\n");
        return 0;
    }

    while (fscanf(todolist, "%d", &todo.id) == 1)
    {
        fgets(todo.task, sizeof(todo.task), todolist);
        if (todo.id == id)
        {
            system("cls");
            printf("\n\n\t\t\tFound Task:\n");
            printf("\t\t\tID: %d\n", todo.id);
            printf("\t\t\tTask: %s", todo.task);
            printf("\n\t\t\t===========================================================\n");
            fclose(todolist);
            return id;
        }
    }

    fclose(todolist);
    system("cls");
    printf("\n\n\t\t\t\t\tNot Found!\n");
    printf("\t\t\t===========================================================\n");
    return 0;
}

// VIEW TASKS
void view_task()
{
    system("cls");
    printf("\t\t\t======================================================\n");
    printf("\t\t\t\t\tVIEW TASK\n");
    printf("\t\t\t======================================================\n\n");

    struct todolist todo;
    FILE *todolist = fopen("todo.txt", "r");

    if (!todolist)
    {
        printf("\t\t\tNo tasks available (File not found)!\n");
        printf("\t\t\t===========================================================\n");
        return;
    }

    int task_found = 0;

    printf("\t\t\tTasks:\n");
    printf("\t\t\t------\n\n");
    while (fscanf(todolist, "%d", &todo.id) == 1)
    {
        fgets(todo.task, sizeof(todo.task), todolist);
        if (strlen(todo.task) > 1)
        {
            task_found = 1;
            printf("\t\t\tID: %d\n", todo.id);
            printf("\t\t\tTask: %s\n", todo.task);
        }
    }

    fclose(todolist);

    if (!task_found)
    {
        printf("\t\t\tNo tasks found!\n");
    }

    char exit;
    printf("\n\n\t\t\tExit? (y/n): ");
    scanf(" %c", &exit);

    if (exit != 'y' && exit != 'Y')
    {
        view_task();
    }
    system("cls");
    printf("\t\t\t======================================================\n");
}

// ADD TASK (safe input + consistent write format)
void add_task()
{
    system("cls");
    printf("\t\t\t======================================================\n");
    printf("\t\t\t\t\tADD TASK\n");
    printf("\t\t\t======================================================\n\n");

    struct todolist todo;
    printf("\t\t\tAdd task description: ");
    getchar(); // clear leftover newline from previous input
    if (!fgets(todo.task, sizeof(todo.task), stdin)) {
        // input error
        system("cls");
        printf("\t\t\tInput error!\n");
        return;
    }
    // remove trailing newline if present
    todo.task[strcspn(todo.task, "\n")] = '\0';

    char save;
    printf("\t\t\tSave? (y/n): ");
    scanf(" %c", &save);

    if (save == 'y' || save == 'Y')
    {
        ID++; // next sequential ID (since we keep IDs consecutive)
        FILE *todolist = fopen("todo.txt", "a");
        if (!todolist) {
            printf("\t\t\tError: cannot open todo.txt for appending.\n");
            ID--; // rollback
            return;
        }
        fprintf(todolist, "%d %s\n", ID, todo.task);
        fclose(todolist);

        char more;
        printf("\t\t\tAdd more task? (y/n): ");
        scanf(" %c", &more);

        if (more == 'y' || more == 'Y')
        {
            add_task();
        }
        else
        {
            system("cls");
            printf("\t\t\tTask added successfully!\n");
            printf("\t\t\t===========================================================\n");
            return;
        }
    }
    else
    {
        system("cls");
        printf("\t\t\tTask not saved!\n");
    }
}

// DISPLAY CURRENT TIME
void display_current_time()
{
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    printf("\r");
    printf("\t\t\tCurrent Time: %02d:%02d:%02d\n", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    fflush(stdout);
}

// MENU
void display_menu()
{
    printf("\n\t\t\t\t\tTO-DO LIST MENU\n");
    printf("\t\t\t\t\t----------------\n");
    printf("\t\t\t\t\t1. Add Task\n");
    printf("\t\t\t\t\t2. View Task\n");
    printf("\t\t\t\t\t3. Search Task\n");
    printf("\t\t\t\t\t4. Delete Task\n");
    printf("\t\t\t\t\t5. About\n");
    printf("\t\t\t\t\t6. Credits\n");
    printf("\t\t\t\t\t0. Exit\n");
}

// ABOUT
void display_about()
{
    system("cls");
    printf("\t\t\t===========================================================\n");
    printf("\t\t\t\t\tTO-DO LIST\n");
    printf("\t\t\t===========================================================\n\n\n\n");
    printf("\t\t\tDESCRIPTION:\n");
    printf("\t\t\t------------\n\n");
    printf("\t\t\tThis project is a COMMAND-LINE based TO-DO LIST application\n");
    printf("\t\t\tdeveloped in C. It allows users to manage tasks effectively by\n");
    printf("\t\t\tadding, viewing, deleting, and searching tasks, while also\n");
    printf("\t\t\tdisplaying the current time during user interaction.\n\n");
    printf("\t\t\tFEATURES:\n");
    printf("\t\t\t---------\n\n");
    printf("\t\t\t> Add tasks to a TO-DO LIST.\n");
    printf("\t\t\t> View all tasks.\n");
    printf("\t\t\t> Search specific tasks.\n");
    printf("\t\t\t> Delete specific tasks.\n\n");
    printf("\t\t\tPURPOSE:\n");
    printf("\t\t\t--------\n\n");
    printf("\t\t\t> Demonstrate file handling in C.\n");
    printf("\t\t\t> Provide a simple user interface for task management.\n\n");
    printf("\t\t\t===========================================================\n");
}

// CREDITS
void display_credit()
{
    system("cls");
    printf("\t\t\t===========================================================\n");
    printf("\t\t\t\tPROJECT DEVELOPER INFORMATION\n");
    printf("\t\t\t===========================================================\n\n");
    printf("\t\t\tProject Name: TO-DO List Application (C)\n");
    printf("\t\t\t----------------------------------------\n\n");
    printf("\t\t\tDeveloped by:\n");
    printf("\t\t\t-------------\n\n");
    printf("\t\t\tName: Sanjida Ali\n");
    printf("\t\t\tID: C243454\n");
    printf("\t\t\tSection: 1AF (Autumn-24)\n");
    printf("\t\t\tDepartment of CSE (IIUC)\n\n");
    printf("\t\t\t===========================================================\n");
}
