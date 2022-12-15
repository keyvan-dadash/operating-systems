






#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct btree_t{
    struct btree_t *left;
    struct btree_t *right;
    int value;
}btree_t;

/* Add value to the tree
 * return 1 if successful otherwise 0.
* */

btree_t* btree_create_node(int value, btree_t *right, btree_t *left) {
    btree_t *node = (btree_t *)malloc(sizeof(btree_t));

    node->right = right;
    node->left = left;
    node->value = value;

    return node;
}

int btree_add_node (btree_t *t, int value) {

    if (t == NULL) return 0;

    btree_t *temp = t;
    while (1) {

        if (value < temp->value) {
            if (temp->left == NULL) {
                temp->left = btree_create_node(value, NULL, NULL);
                break;
            }
            temp = temp->left;
        } else if (value > temp->value) {
            if (temp->right == NULL) {
                temp->right = btree_create_node(value, NULL, NULL);
                break;
            }
            temp = temp->right;
        }
            
    }

    return 1;
}

/* Remove a value from the tree
 * return 1 if successful otherwise 0.
* */
int btree_remove_node (btree_t *t, int value) {

    btree_t *temp = t;
    btree_t *prev = temp;
    int waye = 0;
    while (1) {
        if (temp == NULL) return 0;

        if (value > temp->value) {
            if (temp->right == NULL) {
                return 0;
            }
            prev = temp;
            temp = temp->right;
            waye = -1;
        } else if (value < temp->value) {
            if (temp->left == NULL) {
                return 0;
            }
            prev = temp;
            temp = temp->left;
            waye = 1;
        } else {
            //for delete this node we must find smallest element in right branch
            if (temp->left == NULL) {
                if (waye == 1) {
                    prev->left = temp->right;
                    free(temp);
                    break;
                }
                else if (waye == -1) {
                    prev->right = temp->right;
                    free(temp);
                    break;
                }
            } else if (temp->right == NULL) {
                 if (waye == 1) {
                    prev->left = temp->left;
                    free(temp);
                    break;
                }
                else if (waye == -1) {
                    prev->right = temp->left;
                    free(temp);
                    break;
                }
            }
            else {
                btree_t *par = temp;
                btree_t *maxMin = temp->right;
                int way = 1;


                while (maxMin && maxMin->left != NULL) {
                    par = maxMin;
                    maxMin = maxMin->left;
                    way = -1;
                }

                if (way == 1) {
                    par->right = maxMin->right;
                } else {
                    par->left = maxMin->left;
                }

                temp->value = maxMin->value;

                free(maxMin);
                break;
            }
        }    
    }
    return 1;
}

/* Look up a value in the tree
 * return 1 if found otherwise 0.
* */
int btree_search (btree_t *t, int value) {

    int index = 0;
    int size = 10;

    btree_t *temp = t;

    int *path;
    if ((path = (int *)malloc(size * sizeof(int))) == NULL) {
        printf("EROOR: unable to allocate memory \n");
        return -1;
    }

    while (1) {
        if (temp == NULL) return 0;

        if (value < temp->value) {
            temp = temp->left;
            if (index >= (size -1)) {
                size = size * 2;
                path = realloc(path, size);
                memset(&path[index+1], 0, (size - (index + 1)));
            }
            path[index] = -1;
            index++;
        } else if (value > temp->value) {
            temp = temp->right;
            if (index >= (size -1)) {
                size = size * 2;
                path = realloc(path, size);
                memset(&path[index+1], 0, (size - (index + 1)));
            }
            path[index] = 1;
            index++;
        } else {
            printf("given node->");
            for (int i=0; i < index; i++) {
                printf("%s->", (path[i] == 1 ? "right" : "left"));
            }
            printf("\n");
            return 1;
        }
    }
}

int main() {
    btree_t *root = btree_create_node(10, NULL, NULL);

    //left branch
    btree_add_node(root, 5);
    btree_add_node(root, 3);
    btree_add_node(root, 1);
    btree_add_node(root, 2);
    btree_add_node(root, 4);
    btree_add_node(root, 8);
    btree_add_node(root, 9);
    btree_add_node(root, 6);
    btree_add_node(root, 7);

    //right branch
    btree_add_node(root, 21);
    btree_add_node(root, 13);
    btree_add_node(root, 16);
    btree_add_node(root, 14);
    btree_add_node(root, 17);
    btree_add_node(root, 19);
    btree_add_node(root, 18);
    btree_add_node(root, 15);
    btree_add_node(root, 23);
    btree_add_node(root, 22);
    btree_add_node(root, 29);
    btree_add_node(root, 26);
    btree_add_node(root, 27);
    btree_add_node(root, 28);
    btree_add_node(root, 30);
    btree_add_node(root, 34);
    btree_add_node(root, 32);
    btree_add_node(root, 33);
    btree_add_node(root, 31);

    btree_remove_node(root, 30);
    btree_remove_node(root, 8);
    btree_remove_node(root, 26);

    btree_search(root, 19);
    btree_search(root, 31);
    btree_search(root, 28);
    btree_search(root, 6);
    btree_search(root, 2);
}
