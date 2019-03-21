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

int path[200];//memoreaza calea pana la obiectul care a fost gasit
int path_length;//memoreaza lungimea caii


void getchildren(struct element *aux, int tab, FILE *g) {
    int j;
    int i;
    for(j = 0; j < tab; j++) {
        fprintf(g, "    ");
    }
    fprintf(g, "<%s", aux->name);
    if(aux->numberOfAttributes)
    {
        for(i = 0; i < aux->numberOfAttributes; i++) {
            fprintf(g, " %s=\"%s\"", aux->attr[i].name, aux->attr[i].value);
        }
    }

    fprintf(g, ">\n");
    i = 0;
    tab++;
    while(i < aux->numberOfChildren) {
        getchildren(&aux->child[i], tab, g);
        i++;
    }
    if(aux -> value)
    {
        for(j = 0;j < tab; j++) {
            fprintf(g, "    ");
        }
        fprintf(g, "%s\n", aux->value);
    }
    for(j = 0; j < tab-1; j++) {
        fprintf(g, "    ");
    }
    fprintf(g, "</%s>\n", aux->name);
}

void save_in_memory(struct element *el,FILE *f)
{
    //citeste si memoreaza numele elementului si atributele---

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
        //printf("%d\n",nr);

        //for(i=0;i<nr-1;i++)
        //printf("%s\n",  (*((*el).attr+i)).name  );
        //printf("\n\n");

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

void recDelete(struct element *El){
    int i;
    if(El->numberOfChildren != 0){
        for(i = 0; i < El->numberOfChildren; i++){
            recDelete(&El->child[i]);
        }
    }
    free(El);
    El = NULL;
}

void deleteElement(char *elName, int numberOfElement){
    int i;
    findXMLByTagName(&root, elName, numberOfElement);
    fatherSearchedElement = *(stiva + poz - 2);
    for(i = 0; i <= fatherSearchedElement->numberOfChildren - 1; i++){
        if(&fatherSearchedElement->child[i] == searchedElement){
            break;
        }
    }
    if(searchedElement->numberOfChildren != 0){
        recDelete(searchedElement);
    }
    for(i=0; i < fatherSearchedElement->numberOfChildren - 1; i++){
        fatherSearchedElement->child[i] = fatherSearchedElement->child[i + 1];
    }
    fatherSearchedElement->numberOfChildren--;
    fatherSearchedElement->child = realloc(fatherSearchedElement->child, fatherSearchedElement->numberOfChildren * sizeof(struct element));
}

void addElement(char *elName, int numberOfElement, struct element add, int pos){
    int i;
    findXMLByTagName(&root, elName, numberOfElement);
    if(searchedElement->numberOfChildren == 0){
        searchedElement->child = malloc(1 * sizeof(struct element));
        searchedElement->child[0].name = strdup(add.name);
        searchedElement->child[0].value = strdup(add.value);
        searchedElement->numberOfChildren++;
        if(add.attr){
            searchedElement->child[0].numberOfAttributes=1;
            searchedElement->child[0].attr=malloc(sizeof(struct attribute));
            searchedElement->child[0].attr->name=malloc(255);
            searchedElement->child[0].attr->value=malloc(255);
            strcpy(searchedElement->child[0].attr->name , strdup(add.attr->name));
            strcpy(searchedElement->child[0].attr->value , strdup(add.attr->value));
        }
    }
    else{
        searchedElement->numberOfChildren++;
        searchedElement->child = realloc(searchedElement->child, searchedElement->numberOfChildren * sizeof(struct element));
        int nr=searchedElement->numberOfChildren;
        for(i = nr-1; i >pos-1; i--){
            searchedElement->child[i] = searchedElement->child[i-1];
        }
        searchedElement->child[pos-1].child = NULL;
        searchedElement->child[pos-1].numberOfChildren = 0;
        searchedElement->child[pos-1].name = strdup(add.name);
        searchedElement->child[pos-1].value = strdup(add.value);
        if(add.attr){
            searchedElement->child[pos-1].numberOfAttributes=1;
            searchedElement->child[pos-1].attr=malloc(sizeof(struct attribute));
            searchedElement->child[pos-1].attr->name=malloc(255);
            searchedElement->child[pos-1].attr->value=malloc(255);
            strcpy(searchedElement->child[pos-1].attr->name , strdup(add.attr->name));
            strcpy(searchedElement->child[pos-1].attr->value , strdup(add.attr->value));
        }
    }
}

void editElement(char *elName, int numberOfElement, struct element add){
    findXMLByTagName(&root, elName, numberOfElement);
    free(searchedElement->name);
    searchedElement->name = strdup(add.name);
    free(searchedElement->value);
    searchedElement->value = strdup(add.value);
    if(add.attr){
        if(searchedElement->attr==NULL)
        {
            searchedElement->numberOfAttributes=1;
            searchedElement->attr=malloc(sizeof(struct attribute));
            searchedElement->attr->name=malloc(255);
            searchedElement->attr->value=malloc(255);
        }
        strcpy(searchedElement->attr->name , add.attr->name);
        strcpy(searchedElement->attr->value , add.attr->value);
    }
}

void commandPrompt() {
    int pos;
    struct element add;
    add.name=malloc(255);
    add.value=malloc(255);
    add.numberOfAttributes=0;
    add.attr=NULL;
    char str1[255], str2[255], aux[255];
    char c;
    do{
        printf("List of commands : parse, write, find, delete, add, edit, findByValue, path, restoftree, exit\nEnter command\n");
        printf(">>");
        scanf("%s", str1);
        if(strcmp(str1, "exit") == 0){
            printf("   Shutting down...\n");
            break;
        }
        else if(strcmp(str1, "parse") == 0){
            printf("   Enter XML file name: ");
            scanf("%s", str2);
            FILE *f;
            f = fopen(str2, "r");
            if(!f)
            {
                do
                {
                    printf("   XML file not found (specify .xml)\n");
                    printf("   Enter XML file name: ");
                    scanf("%s", str2);
                    f = fopen(str2, "r");
                }while(!f);
            }
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
                fclose(g);
            }
            else
                printf("You have nothing in memory\n");
        }
        else if(strcmp(str1, "find") == 0) {
            printf("   Enter element to be searched and its number: ");
            scanf("%s %d", str2, &numberOfElementToSearch);
            findXMLByTagName(&root, str2, numberOfElementToSearch);
            printf("%s %s\n", searchedElement->value, searchedElement->name);
            numberOfElementToSearch = 1;
            fatherSearchedElement = *(stiva + poz - 2);
            printf("%s %d", fatherSearchedElement->name, fatherSearchedElement->numberOfChildren);
        }
        else if(strcmp(str1, "delete") == 0){
            printf("   Enter element name to be deleted: ");
            scanf("%s", str1);
            printf("\n   Enter element number: ");
            scanf("%d", &numberOfElementToSearch);
            deleteElement(str1, numberOfElementToSearch);
            printf("   Deleting...\n");
            printf("%s", fatherSearchedElement->name);
        }
        else if(strcmp(str1, "add") == 0){
            printf("   Enter element father: ");
            scanf("%s", aux);
            printf("\n   Enter element number: ");
            scanf("%d", &numberOfElementToSearch);
            printf("   Enter element name to be added: ");
            scanf("%s", str2);
            add.name = strdup(str2);
            printf("   Enter element value: ");
            scanf("%s", str2);
            add.value = strdup(str2);
            printf("   Do you want to add an attribute to this element? (Y/N)");
            scanf("%c",&c);
            scanf("%c",&c);
            while(c!='Y' && c!='N')
            {
               printf("Do you want to add an attribute to this element? (Y/N)");
               scanf("%c",&c);
            }
            if(c=='Y')
            {
                add.attr=malloc(sizeof(struct attribute));
                add.attr->name=malloc(255);
                add.attr->value=malloc(255);
                printf("   Enter element attribute name: ");
                scanf("%s", add.attr->name);
                printf("   Enter element attribute value");
                scanf("%s",add.attr->value);
            }

            printf("\n   Enter element position: ");
            scanf("%d", &pos);
            addElement(strdup(aux), numberOfElementToSearch, add, pos);
            if(c=='Y')
            {
            free(add.attr->name);
            free(add.attr->value);
            free(add.attr);
            }
            printf("   Adding...\n");
        }
        else if(strcmp(str1, "edit") == 0){
            printf("   Enter element name to be edited: ");
            scanf("%s", str2);
            printf("\n   Enter element number: ");
            scanf("%d", &numberOfElementToSearch);
            printf("   Enter new element name: ");
            scanf("%s", aux);
            add.name = strdup(aux);
            printf("   Enter new element value: ");
            scanf("%s", aux);
            add.value = strdup(aux);
            printf("   Do you want to edit the attribute? (Y/N)");
            scanf("%c",&c);
            scanf("%c",&c);
            while(c!='Y' && c!='N')
            {
                printf("   Do you want to edit the attribute? (Y/N)");
                scanf("%c",&c);
            }
            if(c=='Y')
            {
                add.attr=malloc(sizeof(struct attribute));
                add.attr->name=malloc(255);
                add.attr->value=malloc(255);
                printf("   Enter new attribute's name: ");
                scanf("%s",add.attr->name);
                printf("   Enter new attribute's value:");
                scanf("%s",add.attr->value);
            }
            editElement(str2, numberOfElementToSearch, add);
            if(c=='Y')
            {

            free(add.attr->name);
            free(add.attr->value);
            free(add.attr);
            }
            printf("   Editing...\n");

        }
        else if(strcmp(str1,"findByValue")==0){
            printf("Enter the value you are looking for:");
            char s[100];
            scanf("%s",s);
            path_length=0;
            int ok=1;
            findByValue(&root,&ok,s);
            if(ok==1)
                {
                    printf("Nothing found\n");
                }
                else
                {
                    find_numberOfElementToSearch(&root,&ok);
                    printf("Value found\n");
                    printf("The name of the element and it's number of order are: %s , %d\n",searchedElement->name,numberOfElementToSearch);
                    printf("The name of the father is %s\n",fatherSearchedElement->name);
                    if(searchedElement->numberOfChildren==0)
                        printf("The element has no child\n");
                    else
                    {
                        printf("The children of the element are:");
                        int i;
                        for(i=0;i<searchedElement->numberOfChildren;i++)
                            printf("%d ,",(searchedElement->child+i)->name);
                    }
                    printf("\n");
                    ok=0;
                    printf("\n");


                    printf("What about this element?\n");
                }
        }
        else if(strcmp(str1,"path") == 0){
                    show_path();
                }
        else if(strcmp(str1,"restoftree") == 0){
            show_restoftree(searchedElement,0);
        }
        else {
            printf("Invalid command!\n");
        }
    }while(strcmp(str1, "exit") != 0);
}


void findByValue(struct element* el,int *OK,char *str)
{
    if( el->value!=NULL && strcmp((*el).value,str)==0)
        {
            (*OK)=0;
        }
    if(*OK==1)
        path_length++;
    int i;
    int nr=el->numberOfChildren;
    for(i=0;i<nr && (*OK)==1;i++)
    {
        fatherSearchedElement=el;
        path[path_length]=i;
        searchedElement=(*el).child+i;
        findByValue((*el).child+i,OK,str);
    }
    if(*OK==1)
        path_length--;
}

void show_path()
{
    int i;
    struct element *el=&root;
    printf("%s",el->name);
    for(i=1;i<=path_length;i++)
    {
        el=(*el).child+path[i];
       printf("->The %d child:%s",path[i]+1,el->name);
    }
    printf("\n");
}

void find_numberOfElementToSearch(struct element* el,int *nr)
{
    if(strcmp(searchedElement->name,el->name)==0)
    {
        (*nr)++;
        if(strcmp(searchedElement->value,el->value)==0)
            numberOfElementToSearch=(*nr);
    }
    int i;
    for(i=0;i<el->numberOfChildren;i++)
        find_numberOfElementToSearch(el->child+i,nr);
}

void show_restoftree(struct element *aux, int tab) {
    int j;
    int i;
    for(j = 0; j < tab; j++) {
        printf("    ");
    }
    printf("<%s", aux->name);
    if(aux->numberOfAttributes)
    {
        for(i = 0; i < aux->numberOfAttributes; i++) {
            printf(" %s=\"%s\"", aux->attr[i].name, aux->attr[i].value);
        }
    }

    printf(">\n");
    i = 0;
    tab++;
    while(i < aux->numberOfChildren) {
        show_restoftree(&aux->child[i], tab);
        i++;
    }
    if(aux -> value)
    {
        for(j = 0;j < tab; j++) {
            printf("    ");
        }
        printf("%s\n", aux->value);
    }
    for(j = 0; j < tab-1; j++) {
        printf("    ");
    }
    printf("</%s>\n", aux->name);
}


int main()
{
    commandPrompt();

    return 0;
}
