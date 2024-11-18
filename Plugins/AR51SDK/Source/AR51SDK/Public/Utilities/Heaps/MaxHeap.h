#pragma once

#include "CoreMinimal.h"

/// <summary>
/// TMaxHeap is a simple binary heap implementation using TArray.
/// </summary>
template<typename T>
class TMaxHeap
{
public:
    TMaxHeap()
    {}

    /// <summary>
    /// Insert a new element to the heap.
    /// </summary>
    void Insert(const T& Elem)
    {
        // Add the element to the end of the array.
        Data.Add(Elem);

        // Reheapify from the last index upwards.
        HeapifyUp(Data.Num() - 1);
    }

    /// <summary>
    /// Remove and return the max element from the heap.
    /// </summary>
    T Pop()
    {
        // Swap the first element with the last element.
        Data.Swap(0, Data.Num() - 1);

        // Remove the last element (which was the max).
        T Max = Data.Pop();

        // Reheapify from the root downwards.
        HeapifyDown(0);

        return Max;
    }

    /// <summary>
    /// Get the max element from the heap without removing it.
    /// </summary>
    const T& Peek() const { return Data[0]; }

private:
    TArray<T> Data;

    void HeapifyUp(int32 Index)
    {
        while (Index > 0)
        {
            int32 Parent = (Index - 1) / 2;
            if (Data[Index] <= Data[Parent]) break;

            // Swap the element with its parent.
            Data.Swap(Index, Parent);
            Index = Parent;
        }
    }

    void HeapifyDown(int32 Index)
    {
        while (Index < Data.Num())
        {
            int32 LeftChild = 2 * Index + 1;
            int32 RightChild = LeftChild + 1;

            // Find the index of the largest child.
            int32 LargestChild = Index;
            if (LeftChild < Data.Num() && Data[LeftChild] > Data[LargestChild]) LargestChild = LeftChild;
            if (RightChild < Data.Num() && Data[RightChild] > Data[LargestChild]) LargestChild = RightChild;

            if (LargestChild == Index) break;

            // Swap the element with the larger of its two children.
            Data.Swap(Index, LargestChild);
            Index = LargestChild;
        }
    }
};
