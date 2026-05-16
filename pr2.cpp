// Practical 2: Parallel Bubble Sort and Parallel Merge Sort using OpenMP in C++.
// This program compares sequential and parallel versions using execution time.
// Compile command: g++ -fopenmp 02_parallel_bubble_merge_sort.cpp -o 02_parallel_bubble_merge_sort
// Run command: ./02_parallel_bubble_merge_sort

#include <iostream> // Provides cin and cout for input and output.
#include <vector> // Provides vector for storing the array.
#include <algorithm> // Provides swap and is_sorted helper functions.
#include <cstdlib> // Provides rand and srand functions.
#include <ctime> // Provides time function for random seed.
#include <omp.h> // Provides OpenMP functions and directives.

using namespace std; // Avoids writing std:: before standard library names.

void printArray(const vector<int>& arr) // Prints array elements.
{ // Starts the printArray function body.
    for (int value : arr) // Loops through every array value.
    { // Starts the loop body.
        cout << value << " "; // Prints one value followed by a space.
    } // Ends the loop body.
    cout << endl; // Prints a newline after the array.
} // Ends the printArray function body.

void sequentialBubbleSort(vector<int>& arr) // Sorts array using normal sequential bubble sort.
{ // Starts the sequentialBubbleSort function body.
    int n = arr.size(); // Stores number of elements in the array.
    for (int i = 0; i < n - 1; i++) // Runs n - 1 passes.
    { // Starts outer loop body.
        for (int j = 0; j < n - i - 1; j++) // Compares adjacent elements in unsorted part.
        { // Starts inner loop body.
            if (arr[j] > arr[j + 1]) // Checks if adjacent elements are in wrong order.
            { // Starts if body.
                swap(arr[j], arr[j + 1]); // Swaps the two adjacent elements.
            } // Ends if body.
        } // Ends inner loop body.
    } // Ends outer loop body.
} // Ends the sequentialBubbleSort function body.

void parallelBubbleSort(vector<int>& arr) // Sorts array using odd-even parallel bubble sort.
{ // Starts the parallelBubbleSort function body.
    int n = arr.size(); // Stores number of elements in the array.
    for (int phase = 0; phase < n; phase++) // Runs n odd-even phases.
    { // Starts phase loop body.
        if (phase % 2 == 0) // Checks if this is an even phase.
        { // Starts even phase body.
            #pragma omp parallel for // Compares independent even-indexed pairs in parallel.
            for (int i = 0; i < n - 1; i += 2) // Visits pairs (0,1), (2,3), and so on.
            { // Starts parallel loop body.
                if (arr[i] > arr[i + 1]) // Checks if the pair is in wrong order.
                { // Starts if body.
                    swap(arr[i], arr[i + 1]); // Swaps the pair.
                } // Ends if body.
            } // Ends parallel loop body.
        } // Ends even phase body.
        else // Runs when this is an odd phase.
        { // Starts odd phase body.
            #pragma omp parallel for // Compares independent odd-indexed pairs in parallel.
            for (int i = 1; i < n - 1; i += 2) // Visits pairs (1,2), (3,4), and so on.
            { // Starts parallel loop body.
                if (arr[i] > arr[i + 1]) // Checks if the pair is in wrong order.
                { // Starts if body.
                    swap(arr[i], arr[i + 1]); // Swaps the pair.
                } // Ends if body.
            } // Ends parallel loop body.
        } // Ends odd phase body.
    } // Ends phase loop body.
} // Ends the parallelBubbleSort function body.

void merge(vector<int>& arr, int left, int middle, int right) // Merges two sorted subarrays.
{ // Starts the merge function body.
    int n1 = middle - left + 1; // Stores size of the left subarray.
    int n2 = right - middle; // Stores size of the right subarray.
    vector<int> leftArray(n1); // Creates temporary left subarray.
    vector<int> rightArray(n2); // Creates temporary right subarray.

    for (int i = 0; i < n1; i++) // Copies data into left temporary array.
    { // Starts loop body.
        leftArray[i] = arr[left + i]; // Copies one left-side element.
    } // Ends loop body.

    for (int j = 0; j < n2; j++) // Copies data into right temporary array.
    { // Starts loop body.
        rightArray[j] = arr[middle + 1 + j]; // Copies one right-side element.
    } // Ends loop body.

    int i = 0; // Index for leftArray.
    int j = 0; // Index for rightArray.
    int k = left; // Index for original array.

    while (i < n1 && j < n2) // Runs while both temporary arrays have elements.
    { // Starts merge loop body.
        if (leftArray[i] <= rightArray[j]) // Checks if left element is smaller or equal.
        { // Starts if body.
            arr[k] = leftArray[i]; // Places left element into original array.
            i++; // Moves to next left element.
        } // Ends if body.
        else // Runs when right element is smaller.
        { // Starts else body.
            arr[k] = rightArray[j]; // Places right element into original array.
            j++; // Moves to next right element.
        } // Ends else body.
        k++; // Moves to next original array position.
    } // Ends merge loop body.

    while (i < n1) // Copies remaining left elements if any.
    { // Starts loop body.
        arr[k] = leftArray[i]; // Copies one remaining left element.
        i++; // Moves to next left element.
        k++; // Moves to next original array position.
    } // Ends loop body.

    while (j < n2) // Copies remaining right elements if any.
    { // Starts loop body.
        arr[k] = rightArray[j]; // Copies one remaining right element.
        j++; // Moves to next right element.
        k++; // Moves to next original array position.
    } // Ends loop body.
} // Ends the merge function body.

void sequentialMergeSort(vector<int>& arr, int left, int right) // Sorts array using normal recursive merge sort.
{ // Starts the sequentialMergeSort function body.
    if (left < right) // Checks whether the subarray has more than one element.
    { // Starts if body.
        int middle = left + (right - left) / 2; // Finds the middle index safely.
        sequentialMergeSort(arr, left, middle); // Sorts the left half.
        sequentialMergeSort(arr, middle + 1, right); // Sorts the right half.
        merge(arr, left, middle, right); // Merges both sorted halves.
    } // Ends if body.
} // Ends the sequentialMergeSort function body.

void parallelMergeSort(vector<int>& arr, int left, int right, int depth) // Sorts array using recursive parallel merge sort.
{ // Starts the parallelMergeSort function body.
    if (left < right) // Checks whether the subarray has more than one element.
    { // Starts if body.
        int middle = left + (right - left) / 2; // Finds the middle index safely.

        if (depth <= 0) // Stops creating new tasks after a fixed depth.
        { // Starts sequential fallback body.
            sequentialMergeSort(arr, left, middle); // Sorts left half sequentially.
            sequentialMergeSort(arr, middle + 1, right); // Sorts right half sequentially.
        } // Ends sequential fallback body.
        else // Runs when more parallel task depth is allowed.
        { // Starts parallel task body.
            #pragma omp task shared(arr) firstprivate(left, middle, depth) // Creates a task with safe copied boundary values for the left half.
            { // Starts the left task body.
                parallelMergeSort(arr, left, middle, depth - 1); // Sorts left half in a task.
            } // Ends the left task body.

            #pragma omp task shared(arr) firstprivate(middle, right, depth) // Creates a task with safe copied boundary values for the right half.
            { // Starts the right task body.
                parallelMergeSort(arr, middle + 1, right, depth - 1); // Sorts right half in a task.
            } // Ends the right task body.

            #pragma omp taskwait // Waits until both sorting tasks finish.
        } // Ends parallel task body.

        merge(arr, left, middle, right); // Merges the two sorted halves.
    } // Ends if body.
} // Ends the parallelMergeSort function body.

int main() // Program execution starts from main.
{ // Starts the main function body.
    int n; // Stores the number of elements.
    cout << "Enter number of elements: "; // Asks the user for array size.
    cin >> n; // Reads array size.

    vector<int> original(n); // Creates the original array.
    srand(time(0)); // Sets a random seed using current time.

    for (int i = 0; i < n; i++) // Fills the array with random values.
    { // Starts loop body.
        original[i] = rand() % 10000; // Stores a random number from 0 to 9999.
    } // Ends loop body.

    vector<int> seqBubble = original; // Copies original array for sequential bubble sort.
    vector<int> parBubble = original; // Copies original array for parallel bubble sort.
    vector<int> seqMerge = original; // Copies original array for sequential merge sort.
    vector<int> parMerge = original; // Copies original array for parallel merge sort.

    double startTime = omp_get_wtime(); // Records start time for sequential bubble sort.
    sequentialBubbleSort(seqBubble); // Runs sequential bubble sort.
    double endTime = omp_get_wtime(); // Records end time for sequential bubble sort.
    cout << "Sequential Bubble Sort Time: " << endTime - startTime << " seconds" << endl; // Prints sequential bubble time.

    startTime = omp_get_wtime(); // Records start time for parallel bubble sort.
    parallelBubbleSort(parBubble); // Runs parallel bubble sort.
    endTime = omp_get_wtime(); // Records end time for parallel bubble sort.
    cout << "Parallel Bubble Sort Time: " << endTime - startTime << " seconds" << endl; // Prints parallel bubble time.

    startTime = omp_get_wtime(); // Records start time for sequential merge sort.
    sequentialMergeSort(seqMerge, 0, n - 1); // Runs sequential merge sort.
    endTime = omp_get_wtime(); // Records end time for sequential merge sort.
    cout << "Sequential Merge Sort Time: " << endTime - startTime << " seconds" << endl; // Prints sequential merge time.

    startTime = omp_get_wtime(); // Records start time for parallel merge sort.
    #pragma omp parallel // Starts a parallel region for OpenMP tasks.
    { // Starts parallel region body.
        #pragma omp single // Allows only one thread to create the first recursive task tree.
        { // Starts single region body.
            parallelMergeSort(parMerge, 0, n - 1, 4); // Runs parallel merge sort with depth 4.
        } // Ends single region body.
    } // Ends parallel region body.
    endTime = omp_get_wtime(); // Records end time for parallel merge sort.
    cout << "Parallel Merge Sort Time: " << endTime - startTime << " seconds" << endl; // Prints parallel merge time.

    cout << "Sequential Bubble Sorted Correctly: " << (is_sorted(seqBubble.begin(), seqBubble.end()) ? "Yes" : "No") << endl; // Verifies sequential bubble sort.
    cout << "Parallel Bubble Sorted Correctly: " << (is_sorted(parBubble.begin(), parBubble.end()) ? "Yes" : "No") << endl; // Verifies parallel bubble sort.
    cout << "Sequential Merge Sorted Correctly: " << (is_sorted(seqMerge.begin(), seqMerge.end()) ? "Yes" : "No") << endl; // Verifies sequential merge sort.
    cout << "Parallel Merge Sorted Correctly: " << (is_sorted(parMerge.begin(), parMerge.end()) ? "Yes" : "No") << endl; // Verifies parallel merge sort.

    if (n <= 30) // Prints arrays only for small input sizes.
    { // Starts if body.
        cout << "Original Array: "; // Prints label for original array.
        printArray(original); // Prints original array.
        cout << "Parallel Merge Sorted Array: "; // Prints label for sorted array.
        printArray(parMerge); // Prints one sorted array result.
    } // Ends if body.

    return 0; // Returns 0 to show successful program execution.
} // Ends the main function body.
