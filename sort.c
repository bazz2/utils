#include <stdio.h>

void quick_sort(int a[], int left, int right)
{
    int i = left;
    int j = right;
    int pivot = a[left];

    if (left >= right)
        return;

    while (i != j) {
        /* process right zone */
        while (i < j && a[j] >= pivot)
            j--;
        if (i < j) {
            a[i] = a[j];
            printf("a[%d] = a[%d]\n", i, j);
        }

        /* process left zone */
        while (i < j && a[i] <= pivot)
            i++;
        if (i < j) {
            a[j] = a[i];
            printf("a[%d] = a[%d]\n", j, i);
        }
    }
    a[i] = pivot;
    printf("a[%d] = %d\n", i, pivot);

    int idx;
    for (idx = 0; idx < 9; idx++)
        printf("%d ", a[idx]);
    printf("\n");

    quick_sort(a, left, i-1);
    quick_sort(a, i+1, right);
}

void main()
{
    int i;
    int a[9] = {2,4,6,8,9,7,5,3,1};
    for (i = 0; i < 9; i++)
        printf("%d ", a[i]);
    printf("\n");
    quick_sort(a, 0, 8);
    for (i = 0; i < 9; i++)
        printf("%d ", a[i]);
    printf("\n");
}
