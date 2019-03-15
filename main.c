#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct attribute {
    char* name;
    char* value;
};

struct element {
    char* name;
    struct attribute* attr;
    struct element* child;
    char* value;
    int numberOfChildren;
    int numberOfAttributes;
}root;

int numberOfElementToSearch;
struct element *searchedElement;
struct element *fatherSearchedElement;
struct element *stiva[100];
int poz = 0;
int stop = 0;

void getchildren(struct element *aux, int tab, FILE *g) {
    int j;
    int i;
    for(j=0;j<tab;j++) {
        fprintf(g,"    ");
    }
    fprintf(g,"<%s", aux->name);
    if(aux->numberOfAttributes)
    {
           for(i=0;i<aux->numberOfAttributes;i++) {
        fprintf(g," %s=\"%s\"", aux->attr[i].name, aux->attr[i].value);
    }
    }

    fprintf(g,">\n");
    i=0;
    tab++;
    while(i<aux->numberOfChildren) {
        getchildren(&aux->child[i],tab,g);
        i++;
    }
    if(aux->value)
    {
        for(j=0;j<tab;j++) {
            fprintf(g,"    ");
        }
        fprintf(g,"%s\n",aux->value);
    }
    for(j=0;j<tab-1;j++) {
        fprintf(g,"    ");
    }
    fprintf(g,"</%s>\n", aux->name);
}

void getchildren2(struct element *aux, int tab) {
    int j;
    int i;
    for(j=0;j<tab;j++) {
        printf("    ");
    }
    printf("<%s", aux->name);
    if(aux->numberOfAttributes)
    {
           for(i=0;i<aux->numberOfAttributes;i++) {
        printf(" %s=%s", aux->attr[i].name, aux->attr[i].value);
    }
    }

    printf(">\n");
    i=0;
    tab++;
    while(i<aux->numberOfChildren) {
        getchildren2(&aux->child[i],tab);
        i++;
    }
    if(aux->value)
    {
        for(j=0;j<tab;j++) {
            printf("    ");
        }
        printf("%s\n",aux->value);
    }
    for(j=0;j<tab-1;j++) {
        printf("    ");
    }
    printf("</%s>\n", aux->name);
}



void save_in_memory(struct element *el,FILE *f)
{
    ///---citeste si memoreaza numele elementului si atributele---

    //initializeaza vectorii attr si child
    (*el).attr=NULL;
    (*el).numberOfAttributes=0;
    (*el).child=NULL;
    (*el).numberOfChildren=0;

    int i,k; // contoare
    getc(f);//trece de caracterul '<'
    char *s=malloc(255);
    char c=getc(f);
    i=0;
    while(c!=' ' && c!='>')
        {//citeste numele
            s[i]=c;
            i++;
            c=getc(f);
        }
    (*el).name=malloc(i+1);
    for(k=0;k<i;k++)//memoreaza numele
        (*el).name[k]=s[k];
    (*el).name[k]='\0';

    int nr=0;//contor pentru numarul de atribute
    if(c==' ')//daca elementul are atribute
        do
    {//citeste si memoreaza atributele
        nr++;//creste numarul de atribute

        (*el).attr=realloc((*el).attr,nr*sizeof(struct attribute) );//adauga memorie pentru un nou element
        (*((*el).attr+nr-1)).name=malloc(255); //adauga memorie pentru numele noului element
        (*((*el).attr+nr-1)).value=malloc(255);//adauga memorie pentru valoarea noului element



        i=0;
        c=getc(f);//sare peste ' '
        while(c != '=')
        {//citeste numele atributului
            s[i]=c;
            i++;
            c=getc(f);
        }

        for(k=0;k<i;k++)
        {//memoreaza numele atributului
            (*((*el).attr+nr-1)).name[k]=s[k];
        }
        (*((*el).attr+nr-1)).name[k]='\0';

        i=0;
        c=getc(f);//sare peste caracterul '='
        c=getc(f);//sare peste caracterul '"'
        while(c!='"')//pana cand se inchid ghilimelele
        {//citeste valoarea atributului
            s[i]=c;
            i++;
            c=getc(f);
        }
        for(k=0;k<i;k++)//memoreaza valoarea atributului
            {
                (*((*el).attr+nr-1)).value[k]=s[k];
            }
        (*((*el).attr+nr-1)).value[k]='\0';



        c=getc(f);//sare peste ghilimele
    }while(c==' ');

    (*el).numberOfAttributes=nr;//actualizeaza numarul de atribute

    int nr_copii=0;//contor pentru numarul copiilor


    ///---citeste valoarea sau fiul pana cand se incheie tagul---

    (*el).value=malloc(255);
    (*el).value[0]='\0';

    char c1=getc(f);
    if(c1=='\n')
        c1=getc(f);//trece pe randul urmator
    while(c1==' ')//sare peste spatii
        c1=getc(f);
    char c2=getc(f);
    //printf("%c %c\n",c1,c2);
    while(!(c1=='<' && c2=='/'))
    {
        if(c1!='<')//daca nu este tag
        {
            (*el).value[0]=c1;
            if(c2!='\n' && c2!='<')//daca valoarea nu a fost citita complet
                {
                    (*el).value[1]=c2;
                    i=2;
                    c=getc(f);
                    while(c!='\n' && c!='<')
                    {
                        (*el).value[i]=c;
                        i++;
                        c=getc(f);
                    }
                    (*el).value[i]='\0';
                    if(c=='\n')
                    {
                        c1=getc(f);
                        while(c1==' ')//sare peste spatii
                            c1=getc(f);
                        c2=getc(f);
                    }
                    else
                    {
                        c1=c;
                        c2=getc(f);
                    }
                }
                else
                {
                    (*el).value[1]='\0';
                    if(c2=='\n')
                    {
                        c1=getc(f);
                        while(c1==' ')//sare peste spatii
                            c1=getc(f);
                        c2=getc(f);
                    }
                    else
                        {
                            c1=c2;
                            c2=getc(f);
                        }
                }
        }
        else
        {//stim ca am gasit un tag deschis
            fseek(f,-2,SEEK_CUR);//ne intoarcem la inceputul tagului
            nr_copii++;
            (*el).child=realloc((*el).child,nr_copii*sizeof(struct element));
            save_in_memory((*el).child+nr_copii-1,f);
            c1=getc(f);
            c2=getc(f);
        }
    }
    while(c2!='>')
        c2=getc(f);
    c2=getc(f);
    while(c2=='\n' && c2!=EOF)
    {
        c2=getc(f);
        while(c2==' ')
            c2=getc(f);
    }
    fseek(f,-1,SEEK_CUR);

    (*el).numberOfChildren=nr_copii;//actualizare nr copii

}

void findXMLByTagName(struct element *el, char *tagNameForSearch, int numberOfElement)
{

    if(stop==0)
    {
        *(stiva + poz) = el;
        poz++;
    }

    numberOfElementToSearch = numberOfElement;
    if(numberOfElementToSearch==0)
        return;
    if(strcmp(el->name, tagNameForSearch)==0)
    {

        numberOfElementToSearch--;
        searchedElement = el;
        if(numberOfElementToSearch==0) {
            stop=1;
        return;
        }

    }
    int i;
    for(i=0;i<el->numberOfChildren;i++)
    {

        findXMLByTagName(el->child+i, tagNameForSearch, numberOfElementToSearch);
    }
    if(stop==0)
        poz--;
}

void deleteElement(char *elName, int numberOfElementToSearch){
    int i;
    findXMLByTagName(&root, elName, numberOfElementToSearch);
    if(searchedElement->numberOfChildren != 0){
        free(searchedElement->child);
        searchedElement->child = NULL;
    }
    for(i = fatherSearchedElement->numberOfChildren; i < numberOfElementToSearch; i--){
        fatherSearchedElement->child[i] = fatherSearchedElement->child[i - 1];
    }
    fatherSearchedElement->numberOfChildren--;
    fatherSearchedElement->child = realloc(fatherSearchedElement->child, fatherSearchedElement->numberOfChildren * sizeof(struct element));
}

void addElement(){

}

void modifyElement(){

}

void commandPrompt() {
    char str1[255], str2[255];
    do{
        printf("\nPARSE for reading a xml file , WRITE for writing it in a xml file and showing in console , findByTagName finds a tag by it's name and number of order , EXIT.\n");
        printf(">>");
        scanf("%s", str1);
        if(strcmp(str1, "exit") == 0){
            printf("Shutting down...\n");
            break;
        }
        else if(strcmp(str1, "parse") == 0){
            printf("   Enter XML file name: ");
            scanf("%s", str2);
            FILE *f;
            f = fopen(str2, "r");
            save_in_memory(&root, f);
            fclose(f);
            printf("   Parsing...\n");
        }
        else if(strcmp(str1, "write") == 0) {
            if(strcmp(root.name, "(null)") != 0){
                printf("   Enter output file name: ");
                scanf("%s", str2);
                FILE *g;
                g = fopen(str2,"w");
                getchildren(&root, 0, g);
                getchildren2(&root,0);
                fclose(g);
            }
            else
                printf("You don't have anything in memory\n");
        }
        else if(strcmp(str1, "findByTagName") == 0) {
            printf("   Enter element to be searched and its number: ");
            scanf("%s %d", str2, &numberOfElementToSearch);
            findXMLByTagName(&root, str2, numberOfElementToSearch);
            printf("%s %s\n", searchedElement->value, searchedElement->name);
            numberOfElementToSearch = 1;
            fatherSearchedElement = *(stiva + poz - 2);
            printf("%s %d", fatherSearchedElement->name, fatherSearchedElement->numberOfChildren);
        }
        /*else if(strcmp(str1, "delete") == 0){
            printf("   Enter element name to be deleted: ");
            scanf("%s", str1);
            printf("\n   Enter element number: ");
            scanf("%d", &numberOfElementToSearch);
            deleteElement(str1, numberOfElementToSearch);
        }*/
        else {
            printf("Invalid command!\n");
        }
    }while(strcmp(str1, "exit") != 0);
}

int main()
{
    commandPrompt();
    return 0;
}
