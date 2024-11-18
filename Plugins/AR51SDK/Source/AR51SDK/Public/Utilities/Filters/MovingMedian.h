#pragma once

#include "CoreMinimal.h"
#include "MinHeapWithRemoval.h"
#include "MaxHeapWithRemoval.h"

class AR51SDK_API TMovingMedian : public IFilterInterface
{
private:
    // The cap on the number of elements that can be stored.
    uint32 Cap;

    // The minimum heap, which stores the largest half of the elements.
    TMinHeapWithRemoval<float> MinHeap;

    // The maximum heap, which stores the smallest half of the elements.
    TMaxHeapWithRemoval<float> MaxHeap;

    // The queue to maintain the order of the elements.
    TCircularBuffer<TPair<float, bool>> Order;

public:
    TMovingMedian(uint32 InCap);

    /// <summary>
    /// Updates the estimated state based on the measurement.
    /// </summary>
    /// <param name="measurement">The current measurement of the system state.</param>
    /// <returns>The estimated state after correction.</returns>
    virtual float Filter(float Measurement) override;

private:
    /// <summary>
    /// Removes the oldest element from the relevant heap.
    /// </summary>
    void RemoveOldestElement();

    /// <summary>
    /// Balances the heaps if necessary.
    /// </summary>
    void BalanceHeaps();

    /// <summary>
    /// Gets the median of the current elements.
    /// </summary>
    /// <returns>The median value.</returns>
    float GetMedian() const;
};


TMovingMedian::TMovingMedian(uint32 InCap)
    : Cap(InCap), MinHeap(), MaxHeap(), Order(InCap)
{
}

float TMovingMedian::Filter(float Measurement)
{
    // Add the new measurement to the appropriate heap.
    if (MinHeap.IsEmpty() || Measurement <= MinHeap.Peek())
    {
        MinHeap.Push(Measurement);
    }
    else
    {
        MaxHeap.Push(Measurement);
    }

    // Add the measurement to the order queue.
    Order.Enqueue(TPair<float, bool>(Measurement, Measurement <= MinHeap.Peek()));

    BalanceHeaps();

    // If the total number of elements exceeds the cap, remove the oldest element.
    if (MinHeap.GetSize() + MaxHeap.GetSize() > Cap)
    {
        RemoveOldestElement();
        BalanceHeaps();
    }

    return GetMedian();
}

void TMovingMedian::RemoveOldestElement()
{
    TPair<float, bool> OldestElement = Order.Dequeue();
    if (OldestElement.Value)
    {
        MinHeap.Remove(OldestElement.Key);
    }
    else
    {
        MaxHeap.Remove(OldestElement.Key);
    }
}

void TMovingMedian::BalanceHeaps()
{
    // If the minimum heap has more than one element more than the maximum heap, move the root of the minimum heap to the maximum heap.
    while (MinHeap.GetSize() > MaxHeap.GetSize() + 1)
    {
        float Root = MinHeap.Pop();
        MaxHeap.Push(Root);
        Order.Enqueue(TPair<float, bool>(Root, false));
    }

    // If the maximum heap has more elements than the minimum heap, move the root of the maximum heap to the minimum heap.
    while (MaxHeap.GetSize() > MinHeap.GetSize())
    {
        float Root = MaxHeap.Pop();
        MinHeap.Push(Root);
        Order.Enqueue(TPair<float, bool>(Root, true));
    }
}

float TMovingMedian::GetMedian() const
{
    // If the total number of elements is odd, the median is the root of the minimum heap.
    // Otherwise, the median is the average of the roots of the two heaps.
    if ((MinHeap.GetSize() + MaxHeap.GetSize()) % 2 == 1)
    {
        return MinHeap.Peek();
    }
    else
    {
        return (MinHeap.Peek() + MaxHeap.Peek()) / 2.0f;
    }
}
