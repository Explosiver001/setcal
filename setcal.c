/*
    #Zaklady programovani
    #projekt 2
    #clenove:   Michal Novak        xnovak3g
                Vojtech Mothejzik   xmothe00
                Jakub Korinek       xkorin17
                Jan Kopecky         xkopec59 
    
    #datum:     5.12.2021 
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/***********\
*  STRUCTS  *
\***********/

enum Commands{ EMPTY, CARD, COMPLEMENT, UNION, INTERSECT, MINUS, SUBSETQ, SUBSET, EQUALS, REFLEXIVE, SYMMETRIC, ANTISYMMETRIC, TRANSITIVE, FUNCTION, DOMAIN, CODOMAIN, INJECTIVE, SURJECTIVE, BIJECTIVE };
enum Errors{MALLOC = -1, UNIERROR = 1, SETERROR = 2, RELERROR = 3, CMDERROR = 4, FORMATERROR = 5};

/*Datove struktury pro ukladani dat*/
typedef struct
{
    char *element;
    int lenght;
} element;

typedef struct
{
    element *elements;
    int elementsCount;
} set;

typedef struct
{
    char *first;
    char *second;

    int firstLenght;
    int secondLenght;
} pair;

typedef struct
{
    pair *pair;
    int elementsCount;
} relation;

typedef struct{
    int command;
    int relation;
    int setA;
    int setB;
} command;

/****************\
*  CONSTRUCTORS  *
\****************/

bool set_ctor(set *set)
{
    set->elements = malloc(set->elementsCount*sizeof(element));
    return (set->elements == NULL);
}

void set_dtor(set *set)
{
    free(set->elements);
    set->elements = NULL;
    set->elementsCount = 0;
}

bool element_ctor(set *set, int size, int num)
{
    set->elements[num].element = malloc((size+1)*sizeof(char));
    return (set->elements->element == NULL);
}

bool rel_ctor(relation *rel){
    rel->pair = malloc(rel->elementsCount*sizeof(pair));
    return (rel->pair == NULL);
}

bool pair_ctor(relation *rel, int element, int size, int num)
{
    switch(element){
        case 1: 
            rel->pair[num].first = malloc((size+1)*sizeof(char));
            return(rel->pair[num].first == NULL);
        case 2: 
            rel->pair[num].second = malloc((size+1)*sizeof(char));
            return(rel->pair[num].second == NULL);
        default:
            return true;
    }
}



/*Deconstractor na vsechna data*/
void freeAll(set *U, set *allSets, int setCount, relation *allRelations, int relationCount, int *setLines, int *relationLines)
{
    for (int i = 0; i < setCount; i++){
        for (int j = 0; j < allSets[i].elementsCount; j++){
            free(allSets[i].elements[j].element);
            allSets[i].elements[j].element = NULL;
        }
        free(allSets[i].elements);
        allSets[i].elements = NULL;
    }
    free(allSets);
    allSets = NULL;

    for (int i = 0; i < relationCount; i++){
        for(int j = 0; j<allRelations[i].elementsCount; j++){
            free(allRelations[i].pair[j].first);
            allRelations[i].pair[j].first = NULL;
            free(allRelations[i].pair[j].second);
            allRelations[i].pair[j].second = NULL;
        }
        free(allRelations[i].pair);
        allRelations[i].pair = NULL;
    }
    free(allRelations);
    allRelations = NULL;
    
    for(int i = 0; i < U->elementsCount; i++){
        if(U->elements[i].element != NULL){
            free(U->elements[i].element);
            U->elements[i].element = NULL;
        }
    }
    if(U->elements != NULL){
        free(U->elements);
        U->elements = NULL;
    }
    
    free(setLines);
    setLines = NULL;
    free(relationLines);
    relationLines = NULL;
}




/*************\
*  FUNCTIONS  *
\*************/

/*Prevod textu na cislo*/
int stringToInt(char *temp)
{
    int output = 0;
    for(int i = 0; temp[i] != '\0'; i++){
        if(temp[i] >= '0' && temp[i] <= '9'){
            output = output*10 + temp[i]-'0';
        }
        else return -1;
    }
    return output;
}

/*Vytisknuti mnoziny*/
void printSet(set toPrint, int type)
{
    switch(type){
        case 0:
            fprintf(stdout, "S");
            break;
        case 1:
            fprintf(stdout, "U");
            break;
    }
    if(toPrint.elementsCount != 0){
        for (int i = 0; i < toPrint.elementsCount; i++){
            fprintf(stdout, " %s", toPrint.elements[i].element);
        }
    }
    fprintf(stdout, "\n");
}

/*Vytisknuti relace*/
void printRelation(relation toPrint)
{ 
    fprintf(stdout, "R");
    for (int i = 0; i < toPrint.elementsCount; i++){
        fprintf(stdout, " (%s %s)", toPrint.pair[i].first, toPrint.pair[i].second);
    }
    fprintf(stdout, "\n");
}

/*Funkce pro urceni poctu prvku na radku*/
int getElementsCount(char *line)
{
    int count = 0;
    int left = 0;
    int right = 0;
    if(strlen(line)<=2){
        return 0;
    }

    switch (line[0]){
        /*U mnoziny a prikazu se rozhoduje na zaklade mezer mezi prvky*/
    case 'S':
    case 'U':
    case 'C':
        for (int j = 0; line[j] != '\0'; j++){ 
            if (line[j] == ' ' && line[j - 1] != ' '){
                count++;
            }
        }
        return count;
    case 'R':
        /*U relace se rozhoduje na zaklade zavorek*/
        for (int j = 0; line[j] != '\0'; j++){ 
            if (line[j] == '('){
                left++;
            }
            if (line[j] == ')'){
                right++;
            }
        }
        if (left == right){
            return left;
        }
        else
            return -1;

    default:
        return 0;
    }
}

/*Ziskani vsech prvku mnozinky z precteneho radku + ulozeni do pameti*/
int getSet(char *line, set *output)
{
    if(line[1] != ' '){
        return 1;
    }
    int position = 2; //cteni zacina od prvniho znaku prvniho prvku 
    while (line[position] == ' '){ //odstraneni prebytecnych mezer
        position++;
    }

    int elementNum = 0; //poradi ukladaneho prvku

    while (true){
        char temp[32] = {0}; //pro docasne ulozeni
        int i;
        for (i = 0; line[position] != '\0' && line[position] != ' ' && line[position] != 0 && i < 31; i++){
            temp[i] = line[position];

            position++;
        }
        /*v pripade prekroceni dovoleneho poctu se cteni ukonci*/
        if(i > 30){
            output->elementsCount = elementNum;
            return 1;
        }
        if (line[position] == ' ' || line[position] == '\0' || line[position] == 0){
            output->elements[elementNum].lenght = i;
            /*ukladani prvku*/
            if(element_ctor(output, i, elementNum)){
                printf("fail");
                for(int j = 0; j < elementNum - 0; j++){
                    free(output->elements[elementNum].element);
                    output->elements[elementNum].element = NULL;
                }
                if(output->elements != NULL){
                    free(output->elements);
                    output->elements = NULL;
                }
                return MALLOC;
            }
            strcpy(output->elements[elementNum].element, temp);
            output->elements[elementNum].lenght = i;//ulozeni delky prvku
            elementNum++;

            while (line[position] == ' '){ 
                position++;
            }
        }
        if (line[position] == '\0' || line[position] == 0){
            break;
        }
    }

    return 0;
}

/*Ziskani vsech prvku relace z precteneho radku + ulozeni do pameti*/
/*princip podobný jaku o mnozin*/
int getRelation(char *line, relation *output)
{
    if(line[1] != ' '){
        return 1;
    }
    int position = 3;
    while (line[position] == ' ' || line[position] == '('){ 
        position++;
    }

    int elementNum = 0; //poradi ukladane dvojice
    int pairElement = 0; //rozliseni pozice, na kterou se uklada

    while (true){
        char first[32] = {0};
        char second[32] = {0};

        int i;
        for (i = 0; line[position] != '\0' && line[position] != ')' && line[position] != ' ' && line[position] != 0 && i < 31; i++){
            switch (pairElement){
            case 0:
                first[i] = line[position];
                break;
            case 1:
                second[i] = line[position];
                break;
            }

            position++;
        }
        if(i > 30){
            output->pair[elementNum].first = realloc(output->pair[elementNum].first, sizeof(char));
            output->pair[elementNum].second = realloc(output->pair[elementNum].second, sizeof(char));
            output->elementsCount = elementNum + 1;
            return 1;
        }
        /*ukladani prvniho prvku relace*/
        if (line[position] == ' ' && line[position+1] != ')'){
            output->pair[elementNum].firstLenght = i; //ulozeni delky prvku
            if(pair_ctor(output, 1, i+1, elementNum)){
                return MALLOC;
            }

            strcpy(output->pair[elementNum].first, first);
            pairElement++;
            while (line[position] == ' '){
                position++;
            }
        } 
        /*ukladani druheho prvku relace*/
        if (line[position] == ')' || line[position] == '\0'|| line[position] == 0){
            output->pair[elementNum].secondLenght = i;
            if(pair_ctor(output, 2, i+1, elementNum)){
                return MALLOC;
            }
            strcpy(output->pair[elementNum].second, second);
            elementNum++;
            pairElement = 0;
            position++;
            while (line[position] == ' ' || line[position] == '('){
                position++;
            }
        }
        if (line[position] == '\0' || line[position] == 0)
        {
            break;
        }
    }

    return 0;
}

/*ziskani prikazu*/
int getCommand(char *line, command *C){
    if(line[1] != ' '){
        return CMDERROR;
    }
    enum Type{S, SPLUS, R, RPLUS}; //typ prikazu na zaklade potrebnych argumentu
    C->setA = -1;
    C->setB = -1;
    C->relation = -1;
    int onLineParam = getElementsCount(line); //pocet slov na radku
    char commandText[30] = {0}; //ukladani retezce prikazu pro nasledujici porovnani
    int type; //typ prikazu
    int position = 2;
    while (line[position] == ' '){
        position++;
    }
    for(int i = 0; line[position] != ' ' && line[position] != '\0'; i++){
        commandText[i] = line[position];
        position++;
    }
    /*retezce prikazu, se kterymi se srovnava*/
    char commandStrings[19][30] = {"empty", "card", "complement", "union", "intersect", "minus", "subseteq", "subset", "equals", "reflexive", "symmetric", "antisymmetric", "transitive", "function", "domain", "codomain", "injective", "surjective", "bijective"};
    
    for(int i = 0; i<20; i++){
        if(i==19){
            C->command = -1;
            return CMDERROR;
        }
        if(strcmp(commandText, commandStrings[i]) == 0){
            C->command = i;
            if(i<UNION) type = S;
            if(i>COMPLEMENT && i<REFLEXIVE) type = SPLUS;
            if(i>EQUALS && i<INJECTIVE) type = R;
            if(i>CODOMAIN) type = RPLUS;
            break;
        }
    }

    int counter = 0;
    while(line[position] != '\0'){
        while (line[position] == ' '){
        position++;
        }
        char *temp;
        temp = malloc(sizeof(char));
        if(temp == NULL){
            return MALLOC;
        }
        for(int i = 0; line[position] != '\0' && line[position] != ' ';i++){
            temp = realloc(temp, (i+2)*sizeof(char));
            if(temp == NULL){
                return MALLOC;
            }
            temp[i] = line[position];
            temp[i+1] = '\0';
            position++;
        }
        /*ukladani dalsich potrebnych argumentu*/
        switch(type){
            case S:
                if(counter < 1 && onLineParam == 2){
                    C->setA = stringToInt(temp);
                    counter++;
                }
                else{
                    free(temp);
                    return CMDERROR;
                } 
                break;
            case SPLUS:
                if(counter < 2 && onLineParam == 3){
                    switch (counter){
                        case 0: 
                            C->setA = stringToInt(temp);
                            break;
                        case 1:
                            C->setB = stringToInt(temp);
                            break;
                    }
                    counter++;
                }
                else{
                    free(temp);
                    return CMDERROR;
                }
                break;
            case R:
                if(counter < 1 && onLineParam == 2){
                    C->relation = stringToInt(temp);
                    counter++;
                }
                else{
                    free(temp);
                    return CMDERROR;
                }
                break;
            case RPLUS:
                if(counter < 3 && onLineParam == 4){
                    switch (counter){
                        case 1: 
                            C->setA = stringToInt(temp);
                            break;
                        case 2:
                            C->setB = stringToInt(temp);
                            break;
                        case 0:
                            C->relation = stringToInt(temp);
                            break;
                    }
                    counter++;
                }
                else{
                    free(temp);
                    return CMDERROR;
                }
                break;
        }
        free(temp);
    }
        return 0;
}


/*FUNKCE PRO PRIKAZY*/

int setUnion(set A, set B)
{
    int sameElements = 0; //pocet shodnych prvku
    bool same[B.elementsCount];
    for(int i = 0; i < B.elementsCount; i++){ //rozpoznavani shodnych prvku
        same[i] = false;
    }

    for (int i = 0; i < A.elementsCount; i++){
        for (int j = 0; j < B.elementsCount; j++){
            if (strcmp(A.elements[i].element, B.elements[j].element) == 0){
                sameElements++;
                same[j] = true;
            }
        }
    }

    set united; //tvoreni nove mnoziny
    united.elementsCount = (A.elementsCount + B.elementsCount - sameElements);
    if(set_ctor(&united)){
        return MALLOC;
    }
    /*ulozi se vsechny prvky z mnoziny A*/
    for (int i = 0; i < A.elementsCount; i++){
        if(element_ctor(&united, A.elements[i].lenght, i)){
            for(int j = i - 1; j>=0; j--){
                free(united.elements[i].element);
            }
            return MALLOC;
        }
        strcpy(united.elements[i].element, A.elements[i].element);
    }
    /*ulozi se jen neshodne prvky z mnoziny B*/
    int countB = 0;
    for (int i = A.elementsCount; i < A.elementsCount + B.elementsCount - sameElements; i++){
        if (!same[countB]){
            if(element_ctor(&united, B.elements[countB].lenght, i)){
                for(int j = i; j>=0; j--){
                    free(united.elements[i].element);
                }
                return MALLOC;
            }
            strcpy(united.elements[i].element, B.elements[countB].element);
            countB++;
        }
        else{
            i--;
            countB++;
        }
    }

    printSet(united,0);
    for(int i = 0; i<united.elementsCount; i++){
        free(united.elements[i].element);
    }
    free(united.elements);
    return 0;
}

int setIntersect(set A, set B){
    int sameElements = 0; //pocet stejnych prvku
    bool same[A.elementsCount];
    for(int i = 0; i < A.elementsCount; i++){
        same[i] = false;
    }

    for (int i = 0; i < A.elementsCount; i++){
        for (int j = 0; j < B.elementsCount; j++){
            if (strcmp(A.elements[i].element, B.elements[j].element) == 0){
                sameElements++;
                same[i] = true;
            }
        }
    }
    set intersect; //tvoreni nove mnoziny
    intersect.elementsCount = sameElements;
    if(set_ctor(&intersect)){
        return MALLOC;
    }

    int countA = 0;
    /*ulozi se jen shodne prvky*/
    for(int i = 0; i < sameElements; i++){
        if (same[countA]){ 
            if(element_ctor(&intersect, A.elements[countA].lenght, i)){
                for(int j = i; j>=0; j--){
                    free(intersect.elements[i].element);
                }
                return MALLOC;
            }
            strcpy(intersect.elements[i].element, A.elements[countA].element);
            countA++;
        }
        else{
            i--;
            countA++;
        }
    }

    printSet(intersect,0);
    for(int i = 0; i < intersect.elementsCount; i++){
        free(intersect.elements[i].element);
    }
    free(intersect.elements);

    return 0;
}

/*overeni na zaklade poctu prvku*/
void setEmpty(set set){
    if (set.elementsCount == 0){
        fprintf(stdout, "true\n");
    }
    else{
        fprintf(stdout, "false\n");
    }
}

/*vypis poctu prvku*/
void setCard(set set){
    fprintf(stdout, "%d\n", set.elementsCount);
}

int setMinus(set A, set B){
    int sameElements = 0; //pocet shodnych prvku
    bool same[A.elementsCount];
    for(int i = 0; i < A.elementsCount; i++){
        same[i] = false;
    }

    for (int i = 0; i < A.elementsCount; i++){
        for (int j = 0; j < B.elementsCount; j++){
            if (strcmp(A.elements[i].element, B.elements[j].element) == 0){
                sameElements++;
                same[i] = true;
            }
        }
    }

    set minus; //vytvoreni nove mnoziny
    minus.elementsCount = A.elementsCount-sameElements;
    set_ctor(&minus);

    int countA = 0;
    /*ukladaji se neshodne prvky z prvni mnoziny*/
    for(int i = 0; i < minus.elementsCount; i++){
        if (!same[countA]){ 
            if(element_ctor(&minus, A.elements[countA].lenght, i)){
                for(int j = i; j >= 0; j--){
                    free(minus.elements[i].element);
                }
                return -1;
            }
            strcpy(minus.elements[i].element, A.elements[countA].element);
            
            countA++;
        }
        else{
            i--;
            countA++;
        }
    }

    printSet(minus,0);
    for(int i = 0; i<minus.elementsCount; i++){
        free(minus.elements[i].element);
    }
    free(minus.elements);

    return 0;
}

void setSubsetEq(set A, set B){
    int sameElements = 0;
    /*hledani poctu shodnych prvku*/
    for (int i = 0; i < A.elementsCount; i++){
        for (int j = 0; j < B.elementsCount; j++){
            if (strcmp(A.elements[i].element, B.elements[j].element) == 0){
                sameElements++;
            }
        }
    }
    /*pokud se pocet shodnych prvku shoduje s poctem prvku celkove == je podmnozina*/
    if(sameElements == A.elementsCount){
        fprintf(stdout, "true\n");
    }
    else{
        fprintf(stdout, "false\n");
    }
}

/*podmnozina s mensim poctem prvku*/
void setSubset(set A, set B){
    if(A.elementsCount < B.elementsCount){
        setSubsetEq(A, B);
    }
    else{
        fprintf(stdout, "false\n");
    }
}

/*specialni pripad podmnoziny*/
void setEquals(set A, set B){
    if(A.elementsCount == B.elementsCount){
        setSubsetEq(A, B);
    }
    else{
        fprintf(stdout, "false\n");
    }
}

/*hleda se poruseni reflexivity*/
void relReflexive(set U, relation rel){
    int sameRelationsCount = 0;

    for (int i = 0; i < rel.elementsCount; i++){

        if(strcmp(rel.pair[i].first,rel.pair[i].second) == 0){
            sameRelationsCount++;
        }

    }

    if(sameRelationsCount == U.elementsCount){
        fprintf(stdout, "true\n");
        return;
    }

    fprintf(stdout, "false\n");
}

//pro každou dvojici hledá symterické partnera, v případě nenalezení není symetrická
void relSymmetric(relation rel){
    bool symfound = false;
    if(rel.elementsCount != 0){
        for (int i = 0; i < rel.elementsCount; i++){
            symfound = false;

            if ((strcmp(rel.pair[i].first, rel.pair[i].second)) == 0) continue;

            for (int j = 0; j < rel.elementsCount; j++){
                symfound = false;

                if ((strcmp(rel.pair[j].first, rel.pair[j].second)) == 0) continue;

                if (strcmp(rel.pair[i].second, rel.pair[j].first) == 0 && strcmp(rel.pair[i].first, rel.pair[j].second) == 0){
                    symfound = true;
                    break;
                }
            }
            if (!symfound){
                fprintf(stdout, "false\n");
                return;
            }
        }
    }
    fprintf(stdout, "true\n");
}

//kontroluje antisymetrii za pomoci symetrie dvou dvojic, pokud taková existuje, není antisymterická
void relAntisymmetric(relation rel){
    bool symfound = false;
    bool allsame = true;
    if(rel.elementsCount != 0){
        for(int i = 0; i < rel.elementsCount; i++){
            if ((strcmp(rel.pair[i].first, rel.pair[i].second)) == 0) continue;
            symfound = false;
            allsame = false;
            for(int j = 0; j<rel.elementsCount; j++){
                if (i == j) continue;
                symfound = false;
                if (strcmp(rel.pair[i].second, rel.pair[j].first) == 0 && strcmp(rel.pair[i].first, rel.pair[j].second) == 0){
                    symfound = true;
                    break;
                }
            }
            if (symfound){
               fprintf(stdout, "false\n");
               return;
            }
        }
        if (allsame) fprintf(stdout, "false\n");
        else fprintf(stdout, "true\n");
    }
    else fprintf(stdout, "true\n");
    
}

// kontroluje tranzivitu pomocí "náhradních" relací, které porovnává se zbytkem, v případě nenalezení relace není tranzitivní 
void relTransitive(relation rel){
    bool transfound = true;
    if(rel.elementsCount != 0){
        for (int i = 0; i < rel.elementsCount; i++){

            if ((strcmp(rel.pair[i].first, rel.pair[i].second)) == 0) continue;

            for (int j = 0; j < rel.elementsCount; j++){
                if (strcmp(rel.pair[i].second, rel.pair[j].first) == 0){
                    transfound = false;
                    pair tmp;
                    tmp.first = rel.pair[i].first;
                    tmp.second = rel.pair[j].second; 
                    for (int k = 0; k < rel.elementsCount; k++){
                        if (strcmp(tmp.first, rel.pair[k].first) == 0 && strcmp(tmp.second, rel.pair[k].second) == 0){
                            transfound = true;
                            break;
                        }
                    }
                    if (!transfound){
                        fprintf(stdout, "false\n");
                        return;
                    }
                }
            }       
        }
    }
    fprintf(stdout, "true\n");
}

void relFunction(relation rel){
    if(rel.elementsCount != 0){ 
        /*kontola, jestli je x z R(x,y) pouzito vickrat*/
        for(int i = 0; i< rel.elementsCount-1; i++){
            for(int j = i+1; j<rel.elementsCount; j++){
                if(strcmp(rel.pair[i].first, rel.pair[j].first) == 0){
                    fprintf(stdout, "false\n");
                    return;
                }
            }
        }
    }
    fprintf(stdout, "true\n");
}


int relDomain(relation rel){
    fprintf(stdout, "S");
    bool *same; /*ukladani pozic shodnych prvku*/
    same = malloc(rel.elementsCount*sizeof(bool));
    if(same == NULL){
        return MALLOC;
    }
    for(int i = 0; i < rel.elementsCount; i++){
        same[i] = false;
    }
    if(rel.elementsCount != 0){
        for(int i = 0; i < rel.elementsCount; i++){
            for(int j = i+1; j < rel.elementsCount; j++){
                if(strcmp(rel.pair[i].first, rel.pair[j].first) == 0){
                    same[i] = true;
                    break;
                }
            }
        }
        /*vypis pouze neshodnych prvku*/
        for(int i = 0; i < rel.elementsCount; i++){
            if(same[i] != true){
                fprintf(stdout, " %s", rel.pair[i].first);
            }
        }
    }
    fprintf(stdout, "\n");
    free(same);
    return 0;
}

int relCodomain(relation rel){
    fprintf(stdout, "S");
    bool *same; /*ukladani pozic shodnych prvku*/
    same = malloc(rel.elementsCount*sizeof(bool));
    if(same == NULL){
        return MALLOC;
    }
    for(int i = 0; i < rel.elementsCount; i++){
        same[i] = false;
    }
    if(rel.elementsCount != 0){
        for(int i = 0; i < rel.elementsCount; i++){
            for(int j = i+1; j < rel.elementsCount; j++){
                if(strcmp(rel.pair[i].second, rel.pair[j].second) == 0){
                    same[i] = true;
                    break;
                }
            }
        }
        /*vypis pouze neshodnych prvku*/
        for(int i = 0; i < rel.elementsCount; i++){
            if(same[i] != true){
                fprintf(stdout, " %s", rel.pair[i].second);
            }
        }
    }
    fprintf(stdout, "\n");
    free(same);
    return 0;
}


bool hasSomeReflex(relation rel){

    for (int i = 0; i < rel.elementsCount; i++){

        if(strcmp(rel.pair[i].first, rel.pair[i].second) == 0){
            return true;
        }

    }

    return false;
}

bool isInjective(relation rel, set A, set B){
    //injekce = k jednomu A jedno B

    //zjisti jestli se jedna o prazdnou relaci / mnozinu
    if(rel.elementsCount == 0 || A.elementsCount == 0 || B.elementsCount == 0){
        //jestli jsou vsechny mnozina a relace nulove => je to injekce
        if(rel.elementsCount == 0 && A.elementsCount == 0 && B.elementsCount == 0){
            return true;
        }
        else return false;
    }


    char *APOM[A.elementsCount];
    char *BPOM[B.elementsCount];

    //vytvori se nove pole pro mnozinu A
    for(int i = 0; i < A.elementsCount; i++){
        APOM[i] = A.elements[i].element;
    }

    //vytvori se nove pole pro mnozinu B
    for(int i = 0; i < B.elementsCount; i++){
        BPOM[i] = B.elements[i].element;
    }
    
    int ACount = 0,BCount = 0;
    int setCountA = 0,setCountB = 0;

    //jestli je relace reflexivni nejedna se o injekci
    if(hasSomeReflex(rel)){
        return false;
    }
    
    //pro kazdy prvek v relaci (x y) se zavola zda se prvni prvek relace (x) rovna prvku z mnoziny A
    //pokud se rovnaji, vymaze se prvek z pomocne mnoziny A a pricte se promena
    for (int i = 0; i < rel.elementsCount; i++){
        setCountA = 0;

        for (int y = 0; y < A.elementsCount; y++){
            
            if(strcmp(rel.pair[i].first, APOM[y]) == 0){
                APOM[i] = "";
                setCountA++;
            }

        }

        if(setCountA == 1){
            ACount++;
        }
    }

    // po pricteni promene se zkontroluje zda se rovna pocet (x) poctu prvku v relaci
    // kdyby na pozici X v relaci byl dvakrat stejny prvek => nerovnala by se promena s poctem prvku v relaci
    // zaroven se kontroluje zda vsechny prvky z mnoziny A maji jeden prvek z mnoziny B
    if(ACount != rel.elementsCount || ACount < A.elementsCount){
        return false;
    }

    //pro kazdy prvek v relaci (x y) se zavola zda se druhy prvek relace (y) rovna prvku z mnoziny B
    //pokud se rovnaji, vymaze se prvek z pomocne mnoziny B a pricte se promena
    for (int i = 0; i < rel.elementsCount; i++){
        setCountB = 0;

        for (int y = 0; y < B.elementsCount; y++){
            
            if(strcmp(rel.pair[i].second, BPOM[y]) == 0){
                BPOM[i] = "";
                setCountB++;
            }

        }

        if(setCountB == 1){
            BCount++;
        }
    }

    // po pricteni promene se zkontroluje zda se rovna pocet (y) poctu prvku v relaci
    // kdyby na pozici Y v relaci byl dvakrat stejny prvek => nerovnala by se promena s poctem prvku v relaci
    if(BCount != rel.elementsCount){
        return false;
    }

    return true;
}


bool isSurjective(relation rel, set A, set B){
    //surjekce = v B neni prazdny prvek

    //zjisti jestli se jedna o prazdnou relaci / mnozinu
    if(rel.elementsCount == 0 || A.elementsCount == 0 || B.elementsCount == 0){
        //jestli jsou vsechny mnozina a relace nulove => je to surjekce
        if(rel.elementsCount == 0 && A.elementsCount == 0 && B.elementsCount == 0){
            return true;
        }
        else return false;
    }
    
    int BCount = 0;
    //zjistuje se, zda A je podmnozina relace
    int ACount = 0,setCountA = 0;

    char *APOM[A.elementsCount];

    for(int i = 0; i < A.elementsCount; i++){
        APOM[i] = A.elements[i].element;
    }
    
    for (int i = 0; i < rel.elementsCount; i++){
        setCountA = 0;

        for (int y = 0; y < A.elementsCount; y++){
            
            if(strcmp(rel.pair[i].first, APOM[y]) == 0){
                APOM[i] = "";
                setCountA++;
            }

        }

        if(setCountA == 1){
            ACount++;
        }
    }

    if(ACount != rel.elementsCount){
        return false;
    }
    //---

    //pokud relace ma mene prvku nez mnozina B => neni to surjekce
    if(rel.elementsCount < B.elementsCount){
        return false;
    }

    //zjsti se, kolikrat se prvky z B shoduji s prvky z relace na pozici Y
    for (int i = 0; i < rel.elementsCount; i++){

        for (int y = 0; y < B.elementsCount; y++){
            if(strcmp(rel.pair[i].second, B.elements[y].element) == 0){
                BCount++;
            }
        }

    }

    //pokud je shoda s vice prvky nez je v mnozine B => surjekce
    if(BCount >= B.elementsCount) {
        return true;
    }

    return false;


}

void relInjective(relation rel, set A, set B){  
    if(isInjective(rel, A, B)){
        fprintf(stdout, "true\n");
        return;
    }

    fprintf(stdout, "false\n");
    return;
}

void relSurjective(relation rel, set A, set B){
    if(isSurjective(rel, A, B)){
        fprintf(stdout, "true\n");
        return;
    }

    fprintf(stdout, "false\n");
    return;
}

void relBijective(relation rel, set A, set B){
    //bijekce = surjektivni a zaroven injektivni

    if( isInjective(rel, A, B) && isSurjective(rel, A, B) ){
        fprintf(stdout, "true\n");
        return;
    }

    fprintf(stdout, "false\n");
    return;
}


/*hledani radku, na kterem je mnozina nebo relace ulozena*/

int findIndex(int *array, int num, int lenght){
    for(int i = 0; i<lenght; i++){
        if(array[i] == num) return i;
    }
    return -1;
}

/*Vykonani prikazu*/
int execute(set U, set *allSets, int setCount, int *setLines, relation *allRelations, int relationCount, int *relationLines, command *C){
    int indexA, indexB, indexR;
    indexA = findIndex(setLines, C->setA, setCount); //index mnoziny A
    indexB = findIndex(setLines, C->setB, setCount); //index mnoziny B
    indexR= findIndex(relationLines, C->relation, relationCount); //index relace
 
    /*spousteni funkci dle yadaneho prikazu*/
    switch(C->command){
        case EMPTY:
            if(indexA != -1){
                setEmpty(allSets[indexA]);
            }
            else if(C->setA == 1){
                setEmpty(U);
            }
            else return 1;
            break;
        case CARD:
            if(indexA != -1){
                setCard(allSets[indexA]);
            }
            else if(C->setA == 1){
                setCard(U);
            }
            else return 1;
            break;
        case COMPLEMENT:
            if(indexA != -1){
                return(setMinus(U, allSets[indexA]));
            }
            else if(C->setA == 1){
                return(setMinus(U, U));
            }
            else return 1;
            break;  
        case UNION:
            if(indexA != -1 && indexB != -1){
                return(setUnion(allSets[indexA], allSets[indexB]));
            }
            else if(C->setA == 1 && indexB != -1){
                return(setUnion(U, allSets[indexB]));
            }
            else if(C->setB == 1 && indexA != -1){
                return(setUnion(U, allSets[indexA]));
            }
            else if(C->setA == 1 && C->setB == 1){
                return(setUnion(U, U));
            }
            else return 1;
            break; 
        case INTERSECT:
            if(indexA != -1 && indexB != -1){
                return(setIntersect(allSets[indexA], allSets[indexB]));
            }
            else if(C->setA == 1 && indexB != -1){
                return(setIntersect(U, allSets[indexB]));
            }
            else if(C->setB == 1 && indexA != -1){
                return(setIntersect(allSets[indexA], U));
            }
            else if(C->setA == 1 && C->setB == 1){
                printSet(U, 0);
            }
            else return 1;
            break;
        case MINUS:
            if(indexA != -1 && indexB != -1){
                return(setMinus(allSets[indexA], allSets[indexB]));
            }
            else if(C->setA == 1 && indexB != -1){
                return(setMinus(U, allSets[indexB]));
            }
            else if(C->setB == 1 && indexA != -1){
                return(setMinus(allSets[indexA], U));
            }
            else if(C->setA == 1 && C->setB == 1){
                return(setMinus(U, U));
            }
            else return 1;
            break; 
        case SUBSETQ:
            if(indexA != -1 && indexB != -1){
                setSubsetEq(allSets[indexA], allSets[indexB]);
            }
            else if(C->setA == 1 && indexB != -1){
                setSubsetEq(U, allSets[indexB]);
            }
            else if(C->setB == 1 && indexA != -1){
                setSubsetEq(allSets[indexA], U);
            }
            else if(C->setA == 1 && C->setB == 1){
                setSubsetEq(U, U);
            }
            else return 1;
            break;
        case SUBSET:
            if(indexA != -1 && indexB != -1){
                setSubset(allSets[indexA], allSets[indexB]);
            }
            else if(C->setA == 1 && indexB != -1){
                setSubset(U, allSets[indexB]);
            }
            else if(C->setB == 1 && indexA != -1){
                setSubset(allSets[indexA], U);
            }
            else if(C->setA == 1 && C->setB == 1){
                setSubset(U, U);
            }
            else return 1;
            break;
        case EQUALS:
            if(indexA != -1 && indexB != -1){
                setEquals(allSets[indexA], allSets[indexB]);
            }
            else if(C->setA == 1 && indexB != -1){
                setEquals(U, allSets[indexB]);
            }
            else if(C->setB == 1 && indexA != -1){
                setEquals(allSets[indexA], U);
            }
            else if(C->setA == 1 && C->setB == 1){
                setEquals(U, U);
            }
            else return 1;
            break;
        case REFLEXIVE:
            if(indexR != -1){
                relReflexive(U, allRelations[indexR]);
            }
            else return 2;
            break;
        case SYMMETRIC:
            if(indexR != -1){
                relSymmetric(allRelations[indexR]);
            }
            else return 2;
            break;
        case ANTISYMMETRIC:
            if(indexR != -1){
                relAntisymmetric(allRelations[indexR]);
            }
            else return 2;
            break;
        case TRANSITIVE:
            if(indexR != -1){
                relTransitive(allRelations[indexR]);
            }
            else return 2;
            break;
        case FUNCTION:
            if(indexR != -1){
                relFunction(allRelations[indexR]);
            }
            else return 2;
            break;   
        case DOMAIN:
            if(indexR != -1){
                relDomain(allRelations[indexR]);
            }
            else return 2;
            break; 
        case CODOMAIN:
            if(indexR != -1){
                relCodomain(allRelations[indexR]);
            }
            else{
                return 2;
            }
            break;
        case INJECTIVE:
            if(indexR != -1 && indexA != -1 && indexB != -1){
                (void)(relInjective(allRelations[indexR], allSets[indexA], allSets[indexB]));
            }
            else return 2;
            break;
        case SURJECTIVE:
            if(indexR != -1 && indexA != -1 && indexB != -1){
                (void)relSurjective(allRelations[indexR], allSets[indexA], allSets[indexB]);
            }
            else return 2;
            break;
        case BIJECTIVE:
            if(indexR != -1 && indexA != -1 && indexB != -1){
                (void)relBijective(allRelations[indexR], allSets[indexA], allSets[indexB]);
            }
            else return 2;
            break;
    }
    return 0;
}

/*Funkce pro kontrolu zadanych dat*/

bool universumCheck(set U){
    /*porovnani se zakazanymi retezci*/
    char banned[21][30] = {"true", "false", "empty", "card", "complement", "union", "entersect", "minus", "subsetq", "subset", "equals", "reflexive", "symmetric", "antisymmetric", "transitive", "function", "domain", "codomain", "injective", "surjective", "bijective"};
    for(int i = 0; i < U.elementsCount; i++){
        for(int j = 0; j < U.elements[i].lenght; j++){
            /*kontola, jesli prvky obsahuji jen pismena anglicke abecedy*/
            if(!((U.elements[i].element[j] >= 'a' && U.elements[i].element[j] <= 'z')||(U.elements[i].element[j] >= 'A' && U.elements[i].element[j] <= 'Z'))){
                return false;
            }
        }

        for(int j = 0; j<21; j++){
            if(strcmp(U.elements[i].element, banned[j]) == 0){
                return false;
            }
        }

        /*kontrola duplicitnich prvku*/
        for(int j = i+1; j < U.elementsCount; j++){
            if(strcmp(U.elements[i].element, U.elements[j].element) == 0){
                return false;
            }
        }
    }
    return true;
}

bool setCheck(set A, set U){
    if(A.elementsCount == 0){
        return true;
    }

    int sameElements = 0;
    for (int i = 0; i < A.elementsCount; i++){
        /*kontrola, zda je mnozina podmnozinou univerza*/
        for (int j = 0; j < U.elementsCount; j++){
            if (strcmp(A.elements[i].element, U.elements[j].element) == 0){
                sameElements++;
            }
        }
        /*hledani duplicitnich prvku*/
        for(int j = i + 1; j < A.elementsCount; j++){
            if(strcmp(A.elements[i].element, A.elements[j].element) == 0){
                return false;
            }
        }
    }
    
    if(sameElements == A.elementsCount) return true;
    else return false;
}


bool relCheck(relation rel, set U){
    if(rel.elementsCount == 0 || U.elementsCount == 0){
        return true;
    }

    for(int i = 0; i < rel.elementsCount; i++){

        for(int j = i+1; j < rel.elementsCount; j++){
            /*kontrola duplicitnich prvku*/
            if(strcmp(rel.pair[i].first, rel.pair[j].first) == 0 && strcmp(rel.pair[i].second, rel.pair[j].second) == 0){
                return false;
            }
        }
        bool containsFirst = false;
        bool containsSecond = false;
        /*kontrola, zda jsou vsechny prvky obsazeny i v univerzu*/
        for(int j = 0; j < U.elementsCount; j++){
            if(strcmp(rel.pair[i].first, U.elements[j].element) == 0){
                
                containsFirst = true;
                break;
            }
        }
        for(int j = 0; j < U.elementsCount; j++){
            if(strcmp(rel.pair[i].second, U.elements[j].element) == 0){
                containsSecond = true;
                break;
            }
        }
        if(containsFirst != true || containsSecond != true){
            return false;
        }
    }

    return true;
}

/********\
*  MAIN  *
\********/

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Neni zadan textovy soubor!\n");
        return -1;
    }

    FILE *source = fopen(argv[1], "r"); //otevreni souboru

    if (source == NULL)
    {
        fprintf(stderr, "Zadany textovy soubor neexistuje! \n");
        return -1;
    }

    int error = 0; //finalni chyba

    /*konstrukce promennych pro ukladani dat*/
    set universum;
    universum.elementsCount = 0;
    universum.elements = malloc(sizeof(element));
    if(universum.elements == NULL) error = MALLOC;
    bool universumFound = false;

    set *allSets;
    allSets = malloc(sizeof(set));
    if(allSets == NULL) error = MALLOC;
    allSets[0].elementsCount = 0;
    int setCount = 0;

    relation *allRelations;
    allRelations = malloc(sizeof(relation));
    if(allRelations == NULL) error = MALLOC;
    allRelations[0].elementsCount = 0;
    int relationCount = 0;
    
    
    int *setLines; //ukladani radku, na kterem je definovana mnozina
    setLines = malloc(sizeof(int));
    int *relationLines; //ukladani radku, na kterem je definovana relace
    relationLines = malloc(sizeof(int));
    if(setLines == NULL || relationLines == NULL) error = MALLOC;

    
    bool commandUsed = false; //kontrola, zda je pouzit prikaz

    /*cteni souboru*/
    for(int lineCount = 1; true && error == 0; lineCount++){
        if(lineCount>1001){
            error = FORMATERROR;
        }
        char *line; //ukladani celeho radku pro dalsi zpracovani
        line = malloc(2*sizeof(char));
        if(line == 0) error = MALLOC;
        line[0] = 0;
        
        /*cteni po jednotlivych charakterech*/
        char temp = fgetc(source);
        for(int i = 0; temp != '\n' && temp != EOF; i++){
            line = realloc(line, (i+2)*sizeof(char));
            line[i] = temp;
            line[i+1] = '\0';
            temp = fgetc(source);
        }
        if(line[0] == 0 && temp != EOF){
            fprintf(stderr, "line");
            error = 1;
        } 

        /*overeni univerza na prvnim radku*/
        if(lineCount == 1 && line[0] != 'U'){
            fprintf(stderr, "Chybi Universum na prvnim radku!\n");
            freeAll(&universum, allSets, setCount, allRelations, relationCount, setLines, relationLines);
            fclose(source);
            free(line);
            return UNIERROR;
        }
        /*overeni spravneho zadani*/
        if(!(line[0] == 'S' || line[0] == 'R' || line[0] == 'U' || line[0] == 'C') && temp != EOF){
            error = FORMATERROR;
        }
        if(error == 0){
            /*prepinac na jednotliva data*/
            switch (line[0]){

                /*ukladani mnozin*/
            case 'S': 
                allSets = realloc(allSets, (setCount+1)*sizeof(set));
                setLines = realloc(setLines, (setCount+1)*sizeof(int));
                if(allSets == NULL || setLines == NULL){
                    error = MALLOC;
                }
                if(error == 0 && commandUsed){
                    error = FORMATERROR;
                } 
                if(error == 0){
                    allSets[setCount].elementsCount = getElementsCount(line);
                }

                if(allSets[setCount].elementsCount != 0){
                    allSets[setCount].elements = malloc((allSets[setCount].elementsCount) * sizeof(element));
                    if(allSets[setCount].elements == NULL){
                        error = MALLOC;
                    }
                    else{
                        error = getSet(line, &allSets[setCount]);
                    }
                    if(error == 0){
                        if(!setCheck(allSets[setCount], universum)){
                            error = SETERROR;
                        }
                    }
                    if(error != 0){
                        setCount++;
                        break;
                    }
                }

                setLines[setCount] = lineCount;
                printSet(allSets[setCount],0);
                setCount++;
                
                break;


                /*ukladani univerza*/
            case 'U': 
                    if(!universumFound){
                        universumFound = true;
                        universum.elementsCount = getElementsCount(line);
                        if(universum.elementsCount != 0){
                            universum.elements = realloc(universum.elements, (universum.elementsCount) * sizeof(element));
                            if(universum.elements == NULL){
                                error = MALLOC;
                            }
                            else{
                                error = getSet(line, &universum);
                            }
                            if(error == 0){
                                if(!universumCheck(universum)){
                                    error = UNIERROR;
                                }
                            }
                            if(error != 0){
                                setCount++;
                                relationCount++;
                                break;
                            }
                        }
                        printSet(universum, 1);
                    }
                    else error = UNIERROR;
                break;


                /*ukladani relaci*/
            case 'R':
                
                allRelations = realloc(allRelations, (relationCount+1)*sizeof(relation));
                relationLines = realloc(relationLines, (relationCount+1)*sizeof(int));
                if(allRelations == NULL || relationLines == NULL){
                    error = MALLOC;
                }
                if(error == 0 && commandUsed){
                    error = FORMATERROR;
                } 
                if(error == 0){
                    allRelations[relationCount].elementsCount = getElementsCount(line);
                }

                if(allRelations[relationCount].elementsCount != 0){
                    allRelations[relationCount].pair = malloc((allRelations[relationCount].elementsCount) * sizeof(pair));
                    if(allRelations[relationCount].pair == NULL){
                        error = MALLOC;
                    }
                    else{
                        error = getRelation(line, &allRelations[relationCount]);
                    }
                    if(error == 0){
                        if(!relCheck(allRelations[relationCount], universum)){
                            error = RELERROR;
                        }
                    }
                    if(error != 0){
                        relationCount++;
                        break;
                    }
                }

                relationLines[relationCount] = lineCount;
                printRelation(allRelations[relationCount]);
                relationCount++;
                break;


                /*cteni a vykonani prikazu*/
            case 'C': ;
                command *C;
                C = malloc(sizeof(command));
                if(C == NULL) error = MALLOC;
                else{
                    error = getCommand(line, C);
                }
                if(error == 0){
                    switch (execute(universum, allSets, setCount, setLines, allRelations, relationCount, relationLines, C)){
                    case MALLOC:
                        error = MALLOC;
                        break;
                    case 0:
                        error = 0;
                        break;
                    case 1:
                        fprintf(stderr, "Mnozina nebyla definovana!\n");
                        error = CMDERROR;
                        break;
                    case 2:
                        fprintf(stderr, "Relace nebyla definovana!\n");
                        error = CMDERROR;
                        break;
                    }
                }
                commandUsed = true;
                free(C);
                break;
            }
        }
        
        free(line);
        line = NULL;
        if(error!=0) break;
        if(temp == EOF) break;
    }
    if(error == 0 && (!commandUsed || !(relationCount != 0 || setCount != 0))){
        error = FORMATERROR;
    }
    /*pro pripad vyskytu chyby - chybove ukonceni*/
    if(error != 0){
        switch(error){
            case MALLOC:
                fprintf(stderr, "Chyba pri allocaci pameti!\n");
                break;
            case SETERROR:
                fprintf(stderr, "Jedna z mnozin je spatne zadana!\n");
                break;
            case RELERROR:
                fprintf(stderr, "Jedna z relaci je spatne zadana!\n");
                break;
            case FORMATERROR:
                fprintf(stderr, "Chyba ve formatu vstupu!\n");
                break;
            case CMDERROR:
                fprintf(stderr, "Chyba pri vykonani prikazu!\n");
                break;
            case UNIERROR:
                fprintf(stderr, "Chyba pri zadavani universa!\n");
                break;
        }
        freeAll(&universum, allSets, setCount, allRelations, relationCount, setLines, relationLines);
        fclose(source);
        return -1;
    }
    freeAll(&universum, allSets, setCount, allRelations, relationCount, setLines, relationLines);
    fclose(source);
    return 0;
}
