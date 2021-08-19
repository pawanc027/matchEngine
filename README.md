Name: Pavan Kumar Chilamakuri

Build and Run Instructions:

   1) git clone git@github.com:pawanc027/matchEngine.git
   2) cd matchEngine
   3) run ./test.sh

Problem Approach:

 1) feed all the orders to a queue from stdin.
 2) spun-up a worker thread , that will process all the elements in the queue.
 3) The worker thread , process all the elements of the queue as below
     a) Starting front element of the queue and check to see if we can fully or partially find a trade match for this order
     b) If the order is BUY order ,check the SELL Book for a match
     c) If the order is SELL order, check the BUY book for a match 
     d) Deduct the SELL and BUY side orders quantities , if a trade is partial match
     d) erase the entry from the book if either the SELL and BUY side orders quantitiea becomes zero , due to full or partial trade match
     d) If found a match either full or partial trade , stdout the trade results
 4) reload the orders to a temporary map to sort the remaing orders as per their timestamp
 5) post the unprocessed order back to stdout as per the order they arrived , where SELL orders will be printed first followed by BUY orders. 

Time Spent: 
  
  Around 5 Hours split as below
  a) 2 Hours for the Design and Implementation 
  b) 1 Hour for CMake and Docker configuration.
  c) 2 Hours for debug and test 
