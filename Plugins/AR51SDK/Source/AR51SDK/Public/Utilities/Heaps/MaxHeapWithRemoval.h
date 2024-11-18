#pragma once

#include "CoreMinimal.h"

template<typename T>
class TMaxHeapWithRemoval
{
public:

    /// <summary>
    /// Inserts the given element into the heap.
    /// </summary>
    /// <param name="Value">The value to be inserted.</param>
    void Insert(T Value)
    {
        Heap.Add(Value);
        ValueToIndexMap.Add(Value, Heap.Num() - 1);
        BubbleUp(Heap.Num() - 1);
    }

    /// <summary>
    /// Gets the maximum element from the heap without removing it.
    /// </summary>
    T Peek() const { return Heap[0]; }

    /// <summary>
    /// Removes and returns the maximum element from the heap.
    /// </summary>
    /// <returns>The maximum element in the heap.</returns>
    T Pop()
    {
        T MaxValue = Heap[0];
        Swap(0, Heap.Num() - 1);
        Heap.RemoveAt(Heap.Num() - 1);
        SinkDown(0);
        ValueToIndexMap.Remove(MaxValue);
        return MaxValue;
    }

    /// <summary>
    /// Removes a specific element from the heap.
    /// </summary>
    /// <param name="Value">The value to be removed.</param>
    /// <returns>Whether the removal was successful or not.</returns>
    bool Remove(T Value)
    {
        if (ValueToIndexMap.Contains(Value))
        {
            int32 Index = ValueToIndexMap[Value];
            Swap(Index, Heap.Num() - 1);
            Heap.RemoveAt(Heap.Num() - 1);
            BubbleUp(Index);
            SinkDown(Index);
            ValueToIndexMap.Remove(Value);
            return true;
        }
        return false;
    }

private:

    /// The heap, represented as an array.
    TArray<T> Heap;

    /// Map of values to their indices in the heap.
    TMap<T, int32> ValueToIndexMap;

    /// <summary>
    /// Bubbles up the element at the given index to its correct position in the heap.
    /// </summary>
    /// <param name="Index">The index of the element to be bubbled up.</param>
    void BubbleUp(int32 Index)
    {
        while (Index > 0 && Heap[Index] > Heap[(Index - 1) / 2])
        {
            Swap(Index, (Index - 1) / 2);
            Index = (Index - 1) / 2;
        }
    }

    /// <summary>
    /// Sinks down the element at the given index to its correct position in the heap.
    /// </summary>
    /// <param name="Index">The index of the element to be sinked down.</param>
    void SinkDown(int32 Index)
    {
        int32 LeftChild, RightChild, MaxChild;
        while (true)
        {
            LeftChild = Index * 2 + 1;
            RightChild = Index * 2 + 2;
            MaxChild = Index;

            if (LeftChild < Heap.Num() && Heap[LeftChild] > Heap[MaxChild])
            {
                MaxChild = LeftChild;
            }

            if (RightChild < Heap.Num() && Heap[RightChild] > Heap[MaxChild])
            {
                MaxChild = RightChild;
            }

            if (MaxChild == Index)
            {
                break;
            }

            Swap(Index, MaxChild);
            Index = MaxChild;
        }
    }

    /// <summary>
    /// Swaps two elements in the heap.
    /// </summary>
    /// <param name="IndexA">Index of the first element.</param>
    /// <param name="IndexB">Index of the second element.</param>
    void Swap(int32 IndexA, int32 IndexB)
    {
        T Temp = Heap[IndexA];
        Heap[IndexA] = Heap[IndexB];
        Heap[IndexB] = Temp;
        ValueToIndexMap[Heap[IndexA]] = IndexA;
        ValueToIndexMap[Heap[IndexB]] = IndexB;
    }
};
