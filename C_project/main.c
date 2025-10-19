#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_UNIVERSITIES 5
#define MAX_FACULTIES 5
#define MAX_SPECIALIZATIONS 5
#define MAX_APPLICATIONS 100


/// Structures for University, Faculty, Specialization, and Application
typedef struct {
    char first_name[100];
    char last_name[100];
    int age;
    char highschool[100];
    float bac_avg;
} Application;


typedef struct {
    char name[100];
    Application applications[MAX_APPLICATIONS];
    int app_count;
} Specialization;


typedef struct {
    char name[100];
    Specialization specializations[MAX_SPECIALIZATIONS];
    int spec_nr, nr_app_faculty;
} Faculty;


typedef struct {
    char name[100];
    Faculty faculties[MAX_FACULTIES];
    int faculty_nr, nr_app_univ;
} University;


University univ[MAX_UNIVERSITIES];
int uni_nr;


int university_app_count[MAX_UNIVERSITIES];
int faculty_app_count[MAX_UNIVERSITIES][MAX_FACULTIES];
int specialization_app_count[MAX_UNIVERSITIES][MAX_FACULTIES][MAX_SPECIALIZATIONS];

void clear_screen()
{
    system("cls");
}

void header()
{
    clear_screen();
    printf("----- University Admission System -----\n");
    printf("1. Apply to a University\n");
    printf("2. View Applications\n");
    printf("3. View Statistics\n");
    printf("4. Exit\n");
    printf("----------------------------------------\n");

}


void University_file(University *univ, int *uni_nr, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open the file %s\n", filename);
        return;
    }

    char line[256];
    University *current_university = NULL;
    Faculty *current_faculty = NULL;

    *uni_nr = 0;

    while (fgets(line, sizeof(line), file))
    {
        /// Remove the newline character from the line
        line[strcspn(line, "\n")] = '\0';

        /// Count the indentation level
        int indent_level = 0;

        while (line[indent_level] == ' ')
        {
            indent_level++;
        }
        /// Handle different levels of indentation
        if (indent_level==0) {
            /// New university
            current_university = &univ[*uni_nr];
            strcpy(current_university->name, line);
            current_university->nr_app_univ = 0;
            current_university->faculty_nr = 0;  // Ensure faculty_nr is reset for each university
            (*uni_nr)++;
        }
        if (indent_level==4)
        {
            /// New faculty
            current_faculty = &current_university->faculties[current_university->faculty_nr];
            strcpy(current_faculty->name, line+4);
            current_faculty->nr_app_faculty = 0;
            current_faculty->spec_nr = 0;
            current_university->faculty_nr++;
        }
        if (indent_level == 8)
        {
            /// New specialization
            Specialization *new_spec = &current_faculty->specializations[current_faculty->spec_nr];
            strcpy(new_spec->name, line+8);
            new_spec->app_count = 0;
            current_faculty->spec_nr++;
        }
    }

    fclose(file);
}



void uni()
{
    ///Getting the information about universities, faculties and specializations from the universities.txt file
    University_file(univ, &uni_nr, "universities.txt");


    getchar();
    printf("-----Applying to a University----- \n\n");

    ///User can apply to univeristy
    printf("To what university would you like to apply to? \n");
    for (int i=0; i<uni_nr; i++)
    {
        printf("%d. %s\n", i+1, univ[i].name);
    }
    int uni_choice;
    printf("     Select the university number: ");
    scanf("%d", &uni_choice);
    if (uni_choice<1 || uni_choice>uni_nr)
    {
        do
        {
            printf("     The university choice is invalid!\n");
            printf("     Select a valid university number: ");
            scanf("%d", &uni_choice);
        }while(uni_choice<1 && uni_choice>uni_nr);
    }

    University *selected_uni=&univ[uni_choice-1];
    printf("\n Select a faculty from %s:\n", selected_uni->name);
    for (int i=0; i<selected_uni->faculty_nr; i++)
    {
        printf("%d. %s\n", i+1, selected_uni->faculties[i].name);
    }


    int faculty_choice;
    printf("     Enter the faculty number: ");
    scanf("%d", &faculty_choice);
    if (faculty_choice<1 || faculty_choice>selected_uni->faculty_nr)
    {
        do
        {
            printf("     The faculty choice is invalid!\n");
            printf("     Select a valid faculty number: ");
            scanf("%d", &faculty_choice);
        }while(faculty_choice<1 && faculty_choice>selected_uni->faculty_nr);
    }

    Faculty *selected_faculty = &selected_uni->faculties[faculty_choice-1];
    printf("\n Select a specialization from %s\n", selected_faculty->name);
    for (int i=0; i<selected_faculty->spec_nr; i++)
    {
        printf("%d. %s\n", i+1, selected_faculty->specializations[i].name);
    }


    int spec_choice;
    printf("     What is the specialization you would like to follow? ");
    scanf("%d", &spec_choice);
    if (spec_choice<1 || spec_choice>selected_faculty->spec_nr)
    {
        do
        {
            printf("     The specialization choice is invalid!\n");
            printf("     Select a valid specialization number: ");
            scanf("%d", &spec_choice);

        }while (spec_choice<1 && spec_choice>selected_faculty->spec_nr);
    }
    Specialization *selected_spec = &selected_faculty->specializations[spec_choice-1];


    getchar();

    ///Function form is called so that the user can introduce inforation about themselves
    form(selected_uni, selected_faculty, selected_spec);



    ///Adding the application to the file
    const char *filename = "view_applications.csv";
    append_application(univ, filename);


    printf("\n Press ENTER to exit..." );
    getchar();

}



void form(University *u, Faculty *faculty, Specialization *spec)
{
    ///Create the form where the user can introduce data about themselves
    if (spec->app_count>=MAX_APPLICATIONS)
    {
        printf("Sorry, no more applications can be accepted for this specialization.\n");
        return;
    }

    char s[201]="";
    Application a;
    printf("\n-----Enter the information about yourself-----\n");
    printf("Name: ");
    scanf("%s", a.first_name);

    printf("Last name: ");
    scanf("%s", a.last_name);

    printf("Age: ");
    scanf("%d", &a.age);

    printf("The graduated High School: ");
    scanf("%s", a.highschool);

    printf("The average of the bacalaureat exam: ");
    scanf("%f", &a.bac_avg);

    /// Add the application to the specialization
    spec->applications[spec->app_count] = a;
    spec->app_count++;


    printf("\n      Application successfully submitted to %s - %s - %s!\n", u->name, faculty->name, spec->name);

    getchar();
    char c;
    printf("\nWould you like to submit your application to another university? (y/n) ");
    scanf("%c", &c);
    if (c=='y')
    {
        while(c=='y')
        {
            clear_screen();

            printf("To what university would you like to apply to? \n");
            for (int i=0; i<uni_nr; i++)
            {
                printf("%d. %s\n", i+1, univ[i].name);
            }
            int uni_choice;
            printf("     Select the university number: ");
            scanf("%d", &uni_choice);
            if (uni_choice<1 || uni_choice>uni_nr)
            {
                do
                {
                    printf("     The university choice is invalid!\n");
                    printf("     Select a valid university number: ");
                    scanf("%d", &uni_choice);
                }while(uni_choice<1 && uni_choice>uni_nr);
            }

            University *selected_uni=&univ[uni_choice-1];
            printf("\n Select a faculty from %s:\n", selected_uni->name);
            for (int i=0; i<selected_uni->faculty_nr; i++)
            {
                printf("%d. %s\n", i+1, selected_uni->faculties[i].name);
            }


            int faculty_choice;
            printf("     Enter the faculty number: ");
            scanf("%d", &faculty_choice);
            if (faculty_choice<1 || faculty_choice>selected_uni->faculty_nr)
            {
                do
                {
                    printf("     The faculty choice is invalid!\n");
                    printf("     Select a valid faculty number: ");
                    scanf("%d", &faculty_choice);
                }while(faculty_choice<1 && faculty_choice>selected_uni->faculty_nr);
            }


            Faculty *selected_faculty = &selected_uni->faculties[faculty_choice - 1];
            printf("\n Select a specialization from %s\n", selected_faculty->name);
            for (int i=0; i<selected_faculty->spec_nr; i++)
            {
                printf("%d. %s\n", i+1, selected_faculty->specializations[i].name);
            }


            int spec_choice;
            printf("     What is the specialization you would like to follow? ");
            scanf("%d", &spec_choice);
            if (spec_choice<1 || spec_choice>selected_faculty->spec_nr)
            {
                do
                {
                    printf("     The specialization choice is invalid!\n");
                    printf("     Select a valid specialization number: ");
                    scanf("%d", &spec_choice);

                }while (spec_choice<1 && spec_choice>selected_faculty->spec_nr);
            }
            Specialization *selected_spec = &selected_faculty->specializations[spec_choice-1];

            ///Add each application to the corresponding university, faculty and specialization then increase the application count
            univ[uni_choice-1].faculties[faculty_choice-1].specializations[spec_choice-1].applications[univ[uni_choice-1].faculties[faculty_choice-1].specializations[spec_choice-1].app_count] = a;
            univ[uni_choice-1].faculties[faculty_choice-1].specializations[spec_choice-1].app_count++;

            printf("\n Application successfully submitted to %s - %s - %s!\n", univ[uni_choice-1].name, univ[uni_choice-1].faculties[faculty_choice-1].name, univ[uni_choice-1].faculties[faculty_choice-1].specializations[spec_choice-1].name);


            getchar();
            printf("\nWould you like to apply to another one? (y/n) ");
            scanf("%c", &c);
            if (c=='n')
                break;
        }
    }
    getchar();


}


void applications(char path[])
{
    getchar();
    FILE * filePointer;
    filePointer = fopen("view_applications.csv", "r");

    printf("-------------------------View applications-------------------------\n\n");

    if ( filePointer == NULL )
    {
        printf( "The file cannot be accessed." ) ;
    }
    else
    {
        char c = fgetc(filePointer);
        while (c != EOF)
        {
            printf ("%c", c);
            c = fgetc(filePointer);
        }

        fclose(filePointer);
    }

    printf("--------------------------------------------------------------------\n");
    getchar();
}



void append_application(University *univ, const char *filename)
{
    FILE *file = fopen(filename, "a"); /// Open the file in append mode ("a")
    if (!file) {
        printf("Could not open the file %s\n", filename);
        return;
    }


    /// Iterate through each university and its faculties, specializations, and applications
    for (int i=0; i<uni_nr; i++)
    {
        University *current_uni = &univ[i];
        for (int j=0; j<current_uni->faculty_nr; j++)
        {
            Faculty *current_faculty = &current_uni->faculties[j];

            for (int k=0; k<current_faculty->spec_nr; k++)
            {
                Specialization *current_spec = &current_faculty->specializations[k];
                /// Loop through each application in this specialization
                for (int l=0; l<current_spec->app_count; l++)
                {
                    Application *current_app = &current_spec->applications[l];

                    /// Write the application data to the file in CSV format
                    fprintf(file, "%s,%s,%d,%s,%.2f,%s,%s,%s\n",    current_app->first_name,
                                                                    current_app->last_name,
                                                                    current_app->age,
                                                                    current_app->highschool,
                                                                    current_app->bac_avg,
                                                                    current_uni->name,
                                                                    current_faculty->name,
                                                                    current_spec->name);
                }
            }
        }
    }
    fclose(file);
}


///Initializes the number of applications with 0
void reset_all_application_counts()
{
    University_file(univ, &uni_nr, "universities.txt");
    for (int i=0; i<uni_nr; i++)
    {
        univ[i].nr_app_univ=0;
        for (int j=0; j<univ[i].faculty_nr; j++)
        {
            univ[i].faculties[j].nr_app_faculty=0;
            for (int k=0; k<univ[i].faculties[j].spec_nr; k++)
            {
                univ[i].faculties[j].specializations[k].app_count=0;
            }
        }
    }
}


void count_applications_from_csv(const char *filename)
{

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening application file");
        return;
    }

    reset_all_application_counts();

    char line[512];
    while (fgets(line, sizeof(line), f))
    {
        char university[100]="", faculty[100]="", specialization[100]="";
        strtok(line, ","); // skip first name
        strtok(NULL, ","); // skip last name
        strtok(NULL, ","); // skip age
        strtok(NULL, ","); // skip high school
        strtok(NULL, ","); // skip bac avg

        ///is a safer way of parsing CSV lines.
        char *c;
        c=strtok(NULL, ",");
        if (c)
            strcpy(university, c);

        c=strtok(NULL, ",");
        if (c)
            strcpy(faculty, c);

        c=strtok(NULL, "\n");
        if (c)
            strcpy(specialization, c);


        for (int i=0; i<uni_nr; i++)
        {
            if (strcmp(univ[i].name, university)==0)
            {
                univ[i].nr_app_univ++;
                for (int j=0; j<univ[i].faculty_nr; j++)
                {
                    if (strcmp(univ[i].faculties[j].name, faculty)==0)
                    {
                        univ[i].faculties[j].nr_app_faculty++;
                        for (int k=0; k<univ[i].faculties[j].spec_nr; k++)
                        {
                            if (strcmp(univ[i].faculties[j].specializations[k].name, specialization)==0)
                            {
                                univ[i].faculties[j].specializations[k].app_count++;
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(f);
}

void write_statistics_to_file(const char *filename)
{

    count_applications_from_csv("view_applications.csv"); // Count from file
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        perror("Failed to open statistics file");
        return;
    }


    for (int i=0; i<uni_nr; i++)
    {
        University *univ_ptr = &univ[i];
        fprintf(f, "University: %s\n", univ_ptr->name);
        fprintf(f, "   Total applications: %d\n", univ_ptr->nr_app_univ);

        for (int j=0; j<univ_ptr->faculty_nr; j++)
        {
            Faculty *fac = &univ_ptr->faculties[j];
            fprintf(f, "    Faculty: %s\n", fac->name);
            fprintf(f, "       Applications: %d\n", fac->nr_app_faculty);

            for (int k=0; k<fac->spec_nr; k++)
            {
                Specialization *spec = &fac->specializations[k];
                fprintf(f, "        Specialization: %s\n", spec->name);
                fprintf(f, "           Applications: %d\n", spec->app_count);
            }
        }

        fprintf(f, "\n");
    }
    fclose(f);
}



void statistics(char path[]) {

    getchar();
    write_statistics_to_file(path);
    FILE * filePointer;
    filePointer = fopen("view_statistics.txt", "r");
    printf("-------------------------View statistics----------------------------\n\n");

    if ( filePointer == NULL )
    {
        printf( "The file cannot be accessed." ) ;
    }
    else
    {
        char c = fgetc(filePointer);
        while (c != EOF)
        {
            printf ("%c", c);
            c = fgetc(filePointer);
        }

        fclose(filePointer);
    }

    printf("--------------------------------------------------------------------\n");
    getchar();
}



void menu(int op)
{
    switch(op)
    {
        case 1:
        {
            clear_screen();
            uni();
            break;
        }
        case 2:
        {
            clear_screen();
            applications("view_applications.csv");
            break;
        }
        case 3:
        {
            clear_screen();
            statistics("view_statistics.txt");

            break;
        }
        default:
            printf("EXIT!\n");
    }
}

int main()
{

    int option;
    do{
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while(option > 0 && option <= 3);
    return 0;
}
