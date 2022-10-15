#include "constants.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int parent(int i) { return (i - 1) / 2; }

int left_child(int i) { return 2 * i + 1; }

int right_child(int i) { return 2 * i + 2; }

void swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void insert(int* keys, int* values, int key, int value, int* n)
{
    assert(*n < MAX_HEAP_SIZE);

    keys[*n] = key;
    values[*n] = value;
    *n = *n + 1;

    int i = *n - 1;
    while (i != 0 && keys[parent(i)] < keys[i]) {
        swap(&keys[parent(i)], &keys[i]);
        swap(&values[parent(i)], &values[i]);
        i = parent(i);
    }
}

void max_heapify(int* keys, int* values, int i, int n)
{
    int left = left_child(i);
    int right = right_child(i);
    int largest = i;

    if (left <= n && keys[left] > keys[largest]) {
        largest = left;
    }

    if (right <= n && keys[right] > keys[largest]) {
        largest = right;
    }

    if (largest != i) {
        swap(&keys[largest], &keys[i]);
        swap(&values[largest], &values[i]);
        max_heapify(keys, values, largest, n);
    }
}

int extract_max(int* keys, int* values, int* n)
{
    assert(*n > 0);
    int max_item = values[0];

    keys[0] = keys[*n - 1];
    values[0] = values[*n - 1];
    *n = *n - 1;

    max_heapify(keys, values, 0, *n);
    return max_item;
}
