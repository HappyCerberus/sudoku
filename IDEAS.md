# Killer sudoku research

- Rule of sum
  - there are only certain combinations of numbers that are possible in this block
- Rule of the intersection
  - if a killer block forces a number into another block, then we can safely remove the number of the rest of the block
- Rule of 45
  - each sudoku block sums up to 45, so we can create a new small killer block, alternatively if the block is fully
   contained within a sudoku block (row, column, 3x3) we can make it a killer block

## Observations

- if we have an intersection of blocks and we know the sum in the intersection, then we talk about the remainder, and
 vice versa
   
## Solving techniques

- only certain combinations are possible
- rule of the intersection

- start with a puzzle that's pre-filled with all the combinations of adjacent sudoku blocks (rows, columns, 3x3) and
 their sums
- if there is a block fully contained within another one, remove the outer block and insert the complement


- looking at any combination of adjacent blocks
  - if the number of squares within this area is one less then the number of squares contained within the killer
   blocks that intersect with this area, then we know the single remaining square value
 - if the number of squares within this area is one more than the number of squares contained within the killer
  blocks fully contained within this area, then we know the single remaining square value