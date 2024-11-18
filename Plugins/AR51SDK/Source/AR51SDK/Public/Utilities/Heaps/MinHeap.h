#pragma once

#include "CoreMinimal.h"

/// <summary>
/// TMinHeap is a simple binary heap implementation using TArray.
/// </summary>
template<typename T>
class TMinHeap
{
public:
    TMinHeap() {}

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
    /// Remove and return the min element from the heap.
    /// </summary>
    T Pop()
    {
        // Swap the first element with the last element.
        Data.Swap(0, Data.Num() - 1);

        // Remove the last element (which was the min).
        T Min = Data.Pop();

        // Reheapify from the root downwards.
        HeapifyDown(0);

        return Min;
    }

    /// <summary>
    /// Gets the min element from the the heap without removing it.
    /// </summary>
    const T& Peek() const { return Data[0]; }

private:
    TArray<T> Data;

    void HeapifyUp(int32 Index)
    {
        while (Index > 0)
        {
            int32 Parent = (Index - 1) / 2;
            if (Data[Index] >= Data[Parent]) break;

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

            // Find the index of the smallest child.
            int32 SmallestChild = Index;
            if (LeftChild < Data.Num() && Data[LeftChild] < Data[SmallestChild]) SmallestChild = LeftChild;
            if (RightChild < Data.Num() && Data[RightChild] < Data[SmallestChild]) SmallestChild = RightChild;

            if (SmallestChild == Index) break;

            // Swap the element with the smaller of its two children.
            Data.Swap(Index, SmallestChild);
            Index = SmallestChild;
        }
    }
};
