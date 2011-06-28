/**************************************************************/
/* File: varset.c                                             */
/* Implementation of the data structure of free variable set. */
/* Author: Minjie Zha                                         */
/**************************************************************/

#include "globals.h"
#include "util.h"
#include "varset.h"

#define SHIFT 4

/* Hash function. */
static int hash(const char * str) {
    int temp = 0;
    int i;
    for(i=0;str[i]!='\0';i++) {
        temp = (temp << (SHIFT + str[i])) % SIZE;
    }
    return temp;
}

VarSet * newVarSet(void) {
    VarSet * set = (VarSet *) malloc(sizeof(VarSet));
    if(set==NULL) {
        fprintf(errOut,"Out of memory.\n");
    }else {
        int i;
        for(i=0;i<SIZE;i++) {
            set->hashset[i] = NULL;
        }
    }
    return set;
}

void deleteVarSet(VarSet * set) {
    if(set!=NULL) {
        int i;
        // free the bucket list
        for(i=0;i<SIZE;i++) {
            BucketList bucket = set->hashset[i];
            while(bucket!=NULL) {
                BucketList tmp = bucket;
                bucket = bucket->next;
                free(tmp->name);
                tmp->name = NULL;
                free(tmp);
                tmp = NULL;
            }
            set->hashset[i] = NULL;
        }
        // free the set
        free(set);
        set = NULL;
    }
}

void addVar(VarSet* set, const char *var) {
    int h = hash(var);
    BucketList bucket = set->hashset[h];
    if(bucket==NULL) {
        bucket = (BucketList) malloc(sizeof(struct bucketList));
        if(bucket==NULL) {
            fprintf(errOut,"Out of memory.\n");
        }else {
            bucket->name = stringCopy(var);
            bucket->next = NULL;
            set->hashset[h] = bucket;
        }
    }else {
        while(bucket!=NULL) {
            if(strcmp(bucket->name,var)==0) {
                break;
            }
            bucket = bucket->next;
        }
        if(bucket==NULL) {  // var not exists
            bucket = (BucketList) malloc(sizeof(struct bucketList));
            if(bucket==NULL) {
                fprintf(errOut,"Out of memory.\n");
            }else {
                bucket->name = stringCopy(var);
                bucket->next = set->hashset[h];
                set->hashset[h] = bucket;
            }
        }
    }
}

void deleteVar(VarSet* set, const char *var) {
    int h = hash(var);
    BucketList bucket = set->hashset[h];
    if(bucket!=NULL) {
        BucketList pre = NULL;
        while(bucket!=NULL) {
            if(strcmp(bucket->name,var)==0) {
                break;
            }
            pre = bucket;
            bucket = bucket->next;
        }
        if(bucket!=NULL) {  // var found
            if(pre==NULL) {
                set->hashset[h] = bucket->next;
            }else {
                pre->next = bucket->next;
                bucket->next = NULL;
            }
            free(bucket->name);
            free(bucket);
        }
    }
}

static void copyVarSet(VarSet* des, VarSet* source) {
    int i;
    for(i=0;i<SIZE;i++) {
        BucketList bucket = source->hashset[i];
        while(bucket!=NULL) {
            addVar(des,bucket->name);
            bucket = bucket->next;
        }
    }
}

void unionVarSet(VarSet* newSet, VarSet* set1, VarSet* set2) {
    copyVarSet(newSet,set1);
    copyVarSet(newSet,set2);
}

int contains(VarSet* set, const char *var) {
    int h = hash(var);
    BucketList bucket = set->hashset[h];
    if(bucket!=NULL) {
        while(bucket!=NULL) {
            if(strcmp(bucket->name,var)==0) {
                return 1;
            }
            bucket = bucket->next;
        }
    }
    return 0;
}

int vs_empty(VarSet* set) {
    int i;
    for(i=0;i<SIZE;i++) {
        if(set->hashset[i]!=NULL) {
            return 0;
        }
    }
    return 1;
}

VarSetList* vs_asList(VarSet* set) {
    VarSetList *list = NULL;
    VarSetList *cur = NULL;
    int i;
    for(i=0;i<SIZE;i++) {
        if(set->hashset[i]!=NULL) {
            BucketList bucket = set->hashset[i];
            while(bucket!=NULL) {
                VarSetList *l = malloc(sizeof(VarSetList));
                l->name = stringCopy(bucket->name);
                l->next = NULL;
                if(list==NULL) {
                    cur = list = l;
                }else {
                    cur->next = l;
                    cur = cur->next;
                }
                bucket = bucket->next;
            }
        }
    }
    return list;
}

void vs_deleteVarSetList(VarSetList *list) {
    VarSetList *l = NULL;
    while(list!=NULL) {
        l = list;
        list = list->next;
        free(l->name);
        free(l);
    }
}
