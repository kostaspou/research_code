PREPROCESSING
1. Assign order number for each DFF
2. Assign weight number for each DFF
3. The grade for each fault is the summation of the fault appearance. Whenever we detect a fault at the output we add the value 1/2(the order of the output) to the current fault grade. The order for the PO is 1 and for the dffs depends on the position at the chain. The weight is the reverse order of the chain
4. Generate random vectors to determine the grade of each fault
5. When we have the grades for all faults we evaluate each vector. The value for each vector is the summation of the (weight of output)/(fault grade). The weight of PO is the highest. While we are going towards the end of the chain the weight increases.
MAIN ALGORITHM
6. From the random vectors I select the one with the biggest value and apply it.
1. I check the PO and remove from the fault list the detected faults at the PO.
2. I call the two functions to estimate the new grade of the vector and decide if I will shift by one bit or apply again with the updated values in the dffs and new Pis.
7. APPLY function
1. apply the previous vector and update the dff values.
2. Go though the fault list and for every undetected fault call PODEM to find a new input vector 
3. Evaluate the grade of this new vector by checking ONLY the PO and add the grade of detected faults by this vector, only for those faults that have not detected by any previous vector.
8. SHIFT function
1. Basically the shift function returns the grade of the vector if we shift out one bit from the chain. So basically we estimate the grade by adding the grade of detected faults at the corresponding shifted out dff. So if we shift out we actually detect that faults.
9. We compare the results from the two functions and select the operation with the highest returning grade.
The selection is based on the total grade of the faults that will be immediately detected by the operation.
OBSERVATIONS:
1. after running the algorithm for few circuits I see that the algorithm prefers to shift out the scan chain especially when the PI and PO are few
2. Also the way we grade the faults, we give more priority to faults detected in the scan chain so every shift operation returns higher grade  

