Chris Keefe
Quiz 3 - pre

1. 1, 2, 4: lots of communication, memory-bound, poor parallel efficiency

2. 2: Rank 0 should handle fewer buckets, because each bucket will be distributed more data points

3. 2, 3: Good scalability if 20 cores, poor if 100

4. 1 2 4 5: The code shows better than 1:1 speedup, which seems only to be possible in cases where increasing the number of ranks decreases the overal number of calculations required. I'm still not sure whether that is actually within the definition of speedup, or if that example only makes sense in terms of response time. :shrug:
