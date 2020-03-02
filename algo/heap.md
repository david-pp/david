
堆（Heap）
----

- 最大堆（max-heap）：A[PARENT(i)] >= A[i]
- 最小堆（min-heap）：A[PARENT(i)] <= A[i]

``` python

# 【父节点】
PARENT(i)
    return i/2

# 【左子节点】
LEFT(i)
    return 2*i

# 【右子节点】
RIGHT(i)
    return 2*i + 1

# 【维持最大堆的性质】
# 最大堆节点i若不满足堆性质，就沿着子节点依次下移，直到合适的位置
# 复杂度：O(lgn)
MAX-HEAPIFY(A, i)
    # 找到节点i及其左右子节点中最大的
    l = LEFT(i)
    r = RIGHT(i)
    if l <= A.heap-size and A[l] > A[i]
        largest = l
    else
        largest = i
    if r <= A.heap-size and A[r] > largest
        largest = r
    
    # 若节点i不是最大的，则下移，下移可能又回破坏之前节点的堆性质，继续处理
    if largest != i
        exchange A[i] with A[largest]
        MAX_HEAPIFY(A, largest)   
        
        
# 【构建一个最大堆】
# 对所有非叶子节点让其满足最大堆的性质
# 复杂度：O(n)
BUILD-MAX-HEAP(A)
    A.heap-size = A.length
    # 所有非叶子节点
    for i = [A.length/2] dowto 1
        MAX-HEAPFIFY(A, i)
```

堆排序（Heapsort）
---

```python
# 堆排序，依次移除最大元素，并缩小堆。结果A从小到大排列。
# 复杂度：O(nlgn)
HEAPSORT(A)
    BUILD-MAX-HEAP(A)
    for i = A.length downto 2
       # 最大的元素和最后一个元素交换，并从堆中移除
       exchange A[1] with A[i]
       A.heap-size = A.heap-size - 1
       # 第一个元素堆特性被破坏，维持该节点的堆特性
       MAX-HEAPIFY(A, 1)
```

优先级队列（Priority Queue）
------

- 最高优先级队列（max-priority queue）：任务调度
- 最低优先级队列（min-priority queue）：模拟事件处理队列，事件时间为key，

```python
# 【最高优先级元素】
HEAP-MAXIMUMU(A)
    reutrn A[1]
 
# 【删除优先级最高的元素】
# 复杂度：O(lgn)
HEAP-EXTRACT-MAX(A)
    if A.heap-size < 1
        error "heap underflow"
    max = A[1]
    A[1] = A[A.heap-size]
    A.heap-size = A.heap-size - 1
    MAX-HEAPIFY(A, 1)
    return max
    
# 【提升优先级】
# 复杂度：O(lgn)
HEAP-INCREASE-KEY(A, i, key)
    if key < A[i]
        error "new key is smaller than current key"
    A[i] = key
    # 优先级提升后，依次沿着父节点比较，直到合适的位置
    while i > 1 and A[PARENT(i)] < A[i]
        exchange A[i] with A[PARENT(i)]
        i = PARENT(i)
               
# 【插入元素】
# 复杂度：O(lgn)
MAX-HEAP-INSERT(A, key)
    A.heap-size = A.heap-size + 1
    A[A.heap-size] = -Infinity
    HEAP-INCREASE-KEY(A, A.heap-size, key)
```


